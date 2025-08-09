#include "server.h"

// Flags for threads to stop working
static bool	shutdown_threads = false;

t_cinfo	**clients;
pthread_mutex_t queue_lock;

pthread_mutex_t atomic_lock;
// Our threads for serving array
pthread_t	threads[THREADS_FOR_SERVE];

char	*substr(char *start, char *end);

void	exec_command(char *command, int sock_fd) 
{
	// Locking file descriptors to avoid overwriting each results
	time_t start_time = time(NULL);
	int	stdout_copy = dup(STDOUT_FILENO);
	dup2(sock_fd,STDOUT_FILENO);
	int status = system(command);
	while (1) {
		if (difftime(time(NULL), start_time) > 5) {
			kill(0, SIGTERM);
		}
	}
	dup2(stdout_copy,STDOUT_FILENO);
	close(stdout_copy);
}

void	handle_sigint(int sigint) {
	// All threads got to end
	shutdown_threads = true;
	for (size_t i = 0; i < THREADS_FOR_SERVE; i++) {
		pthread_join(threads[i],NULL);
	}
	// Destroying mutexes
	pthread_mutex_destroy(&queue_lock);
	pthread_mutex_destroy(&atomic_lock);
	free_queue(clients);
	exit(0);
}

void setup_signal_handler() {
	// Struct for handler
    struct sigaction sa;
	// Handler function name
    sa.sa_handler = handle_sigint;
	// Getting other signals too not blocking it
    sigemptyset(&sa.sa_mask);
	// For breaking all sys cakks (in our case accept) for right handleing of closeing our programm
	sa.sa_flags = 0; 
	// Putting struct to SIGINT for handleing
    sigaction(SIGINT, &sa, NULL);
}

void	create_file(char *file_name,char *content) 
{
	int	fd = open(file_name,O_CREAT | O_RDWR | O_TRUNC,0644);
	if (fd == -1) {
		perror("File creating error :");
		return ;
	}
	// Writing content to file
	if (write(fd,content,strlen(content)) == -1) {
		perror("Writing to file error :");
		close(fd);
		return ;
	}
	
	close(fd);
}

// File creating
t_files	**create_files(char *files) 
{
	// ALWAYS WHEN DOUBLE POINTER INIT NULL ITS VALUE ALWAYS TWO DAYS ERRORORORRS
	t_files **files_list = malloc(sizeof(t_files *));
	files_list[0] = NULL; // Initialize the head of the list to NULL
	if (!files)
		return NULL;

	// Files for lock one for strtok and also file creating and writing
	char	*token = strtok(files,"<>");
	char	*file_name = NULL;
	char	*file_content = NULL;
	char	*coma = NULL;

	while (token != NULL)
	{	
		if (token[0] == 0) {
			token = strtok(NULL,"<>");
			continue;
		}
		coma = strchr(token,',');
		if (coma) {
			file_name = substr(token,coma);
			file_content = substr(coma + 1,token + strlen(token));
			create_file(file_name,file_content);
			add_file(files_list,file_name);
			if (file_content)
				free(file_content);
			file_content = NULL;
		}
        token = strtok(NULL,"<>");
	}
	return files_list;
}


char	*str_realloc(char *old, char *new) 
{
	char	*final;

	size_t	ol = 0;
	size_t	nl = 0;
	size_t	len = 0;
	if (old)
		ol = strlen(old);
	if (new)
		nl = strlen(new);
	len = ol + nl + 1;
	final = malloc(len);
	memset(final,0,len);
	if (old)
		strcpy(final,old);
	if (new)
		strcpy(final + ol,new);
	free(old);
	old = NULL;
	return final;
}

char	*substr(char *start, char *end) 
{
	if (!start || ! end || start >= end)
		return NULL;
	size_t len = end - start + 1;

	char *sub = malloc(len);
	memset(sub,0,len);
	strncpy(sub,start,len - 1);
	return sub;
}

char	*get_command(char	*all_message) 
{
	// If the command tag is not in first
	if (strstr(all_message,"<COMMANDS>") - all_message != 0)
		return NULL;
	char	*files = strstr(all_message,"<FILES>");
	char	*end = strstr(all_message,"<END>");
	if (!end && !files)
		return NULL;
	char	*end_of_commands = files != NULL ? files : end;
	char	*command = substr(all_message + 10, end_of_commands);
	return command;
}

char	*get_files_section(char *all_message) 
{
	char	*files = NULL;
	char	*start = strstr(all_message,"<FILES>");

	if (!start)
		return NULL;
	char	*end = strstr(all_message,"<END>");

	files = substr(start + 8,end);
	return files;
}

void	*connection_handle(void	*data)
{
	// Fucntion for signlas
	signal(SIGINT,handle_sigint);
	// Casting data to our type
	t_cinfo *client_data = (t_cinfo *)data;
	// Connection fd getting
	int		connection_fd = client_data->c_socket_fd;

	// Buffer for getting messages
	char	buff[BUFFER_SIZE];

	// Received bytes (returned by recv)
	int		receveid_bytes;

	char	*all_message = NULL;

	while (1)
	{
		// Setting our buffer to 0 s
		memset(&buff,0,sizeof(buff));

		receveid_bytes = recv(connection_fd,&buff,BUFFER_SIZE,0);
		// Error case
		if(receveid_bytes == -1) {
			free(all_message);
			perror("Recv error :");
			// Closeing connection due to error
			close(connection_fd);
			break;
		}
		if (receveid_bytes == 0) {
			// printf("Connection closed\n");
			// Closeing connection ALWAYS
			if (all_message)
				free(all_message);
			close(connection_fd);
			fflush(stdout);
			return NULL;
		}

		all_message = str_realloc(all_message,buff);

		// If we have got end to request
		if (strstr(all_message,"<END>"))
		{
			pthread_mutex_lock(&atomic_lock);
			t_files **files_list = NULL;
			char	*command = get_command(all_message);
			char	*files = get_files_section(all_message);

			if (validator(command,files))
				send(connection_fd,"inValid❌\n",12,0);
			else
				send(connection_fd,"Valid✅\n",10,0);
			// files_list = create_files(files);
			exec_command(command,connection_fd);
			if (files)
				free(files);
			if (command)
				free(command);
			if (all_message)
				free(all_message);
			// if (files_list && *files_list)
			// 	free_files_list(files_list);
			command = NULL;
			files = NULL;
			all_message = NULL;
			// files_list = NULL;
			pthread_mutex_unlock(&atomic_lock);
		}
		if (shutdown_threads)
			return NULL;
	}
}

void	*thread_logic(void	*arguments) 
{
	// At first no client to serve
	t_cinfo *client_to_serve = NULL;
	while (1)
	{
		// If we got shutdown variable changed so we exit the thread
		if (shutdown_threads)
		{
			if (client_to_serve) {
				free(client_to_serve);
				client_to_serve = NULL;
			}
			return NULL;
		}
		// If we have no one for serving
		if (!client_to_serve) {
			// Trying to get something from the queue maybe some appears
			pthread_mutex_lock(&queue_lock);

			client_to_serve = get_client_to_serve(clients);
			
			pthread_mutex_unlock(&queue_lock);

			// If we got not client to serve continueing
			if (!client_to_serve)
				continue;
		}
		// Else
		connection_handle(client_to_serve);
		// After putting our client to NULL so wee have no client to serve
		free(client_to_serve);
		client_to_serve = NULL;
		
	}
}

int main() 
{
	setup_signal_handler();	
	// Struct containing info about 
	struct sockaddr_in6 info;
	
	memset(&info,0,sizeof(info));

	// Ipv6
	info.sin6_family = AF_INET6;
	// Port
	info.sin6_port = htons(3490);
	// Use my local address
	info.sin6_addr = in6addr_any;

	// Creating socket for ipv6 handleing by tcp protocol (SOCK_STREAM)
	int sock_fd = socket(AF_INET6,SOCK_STREAM,0);
	
	if (sock_fd < 0) {
		perror("Socket Error :");
		exit(1);
	}

	// Reuse option
	int	enable_reuse = 1;

	// SOL_SOCKET -> only socket SO_REUSEADDR->reusing option
	if (setsockopt(sock_fd,SOL_SOCKET,SO_REUSEADDR,&enable_reuse,sizeof(enable_reuse)) == -1) {
		perror("Setting Reussability failed :");
		exit(1);
	}

	if (bind(sock_fd,(SA *)&info,sizeof(info)) == -1) {
		perror("Binding Error");
		exit(1);
	}

	// second opt of listen is for connections waiting in queue
	if (listen(sock_fd,20) == -1) {
		perror("Listening on my port failed :");
		exit(1);
	}

	// Initing mutex BEFORE THREAD INTING TO AVOID USING IT UNITILIAZED for locking threads while working with clients queue
	pthread_mutex_init(&queue_lock,NULL);
	pthread_mutex_init(&atomic_lock,NULL);
	// Initing our threads
	for (int i = 0; i < THREADS_FOR_SERVE;i++) 
		pthread_create(&threads[i],NULL,&thread_logic,NULL);
	// Initing our clients queue head
	clients = malloc(sizeof(t_cinfo *));
	*clients = NULL;
	
	// Initing conditional variable for not useing cpu too much (our thread will only wait)

	
	socklen_t	len = sizeof(struct sockaddr_in6);	
	int		client_fd;
	
	while (1)
	{
		// Accept stops programm until the connection,-1 if failed speccialy for errors		
		client_fd = accept(sock_fd,NULL,NULL);
		if (client_fd == -1) {
			perror("Accepting connection Error :");
			exit(1);
		}

		// Adding from acceptor thread to clients using mutex to prev race conditions
		pthread_mutex_lock(&queue_lock);
		add_client_to_serve(client_fd,clients);
		// Sending signal to one of waiting threads
		pthread_mutex_unlock(&queue_lock);

	}	
}