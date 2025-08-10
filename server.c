#include "server.h"

// Flags for threads to stop working
static bool	shutdown_threads = false;

t_cinfo	**clients;
pthread_mutex_t queue_lock;
pthread_cond_t  queue_cond;
pthread_mutex_t atomic_lock;
// Our threads for serving array
pthread_t	threads[THREADS_FOR_SERVE];

char	*substr(char *start, char *end);

void	handle_sigint(int sigint) {
	// All threads got to end
	shutdown_threads = true;
	pthread_cond_broadcast(&queue_cond);
	for (size_t i = 0; i < THREADS_FOR_SERVE; i++) {
		pthread_join(threads[i],NULL);
	}
	// Destroying mutexes
	pthread_mutex_destroy(&queue_lock);
	pthread_mutex_destroy(&atomic_lock);
	pthread_cond_destroy(&queue_cond);
	free_queue(clients);
	exit(0);
}

void	exec_command(char *command, int sock_fd) 
{
	// Start of execution
    time_t start = time(NULL);
	// Getting pipe for reading the result of execution from that 
    FILE *fp = popen(command, "r");
	// Reading to buffer
    char buffer[1024];
    
    if (!fp) return;
    
	// While we getting results
    while (fgets(buffer, sizeof(buffer), fp)) {
        // Timeout check
        if (time(NULL) - start > 5) {
            send(sock_fd, "Timeout❌\n", 11, 0);
            break;           // Done
        }
        
        if (send(sock_fd, buffer, strlen(buffer), 0) == -1) {
            break;  // Connection already closed by client
        }
    }
    pclose(fp);  // Command dies
}

void	*connection_handle(void	*data)
{
	// Casting data to our type
	t_cinfo *client_data = (t_cinfo *)data;
	// Connection fd getting
	int		connection_fd = client_data->c_socket_fd;

	// Buffer for getting messages
	char	buff[BUFFER_SIZE];

	// Received bytes (returned by recv)
	int		receveid_bytes;

	char	*all_message = NULL;
	char	*command = NULL;
	char	*files = NULL;
	t_files **files_list = NULL;
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
			files_list = NULL;
			command = get_command(all_message);
			files = get_files_section(all_message);
			files_list = create_files(files);
			
			// if (!validator(command,files) || (files !=  NULL && files_list == NULL) ) {
			// 	send(connection_fd,"inValid❌\n",12,0);
			// }
			// else {
				exec_command(command,connection_fd);
			// }
			// After freeing all
			if (files)
				free(files);
			if (command)
				free(command);
			if (all_message)
				free(all_message);
			if (files_list)
				free_files_list(files_list);
			command = NULL;
			files = NULL;
			all_message = NULL;
			files_list = NULL;
			pthread_mutex_unlock(&atomic_lock);
		}
		if (shutdown_threads) {
			pthread_t id = pthread_self();
			printf("Shutting down thread %lu\n",id);
			fflush(stdout);
			return NULL;
		}
	}
}

void	*thread_logic(void	*arguments) 
{
	// At first no client to serve
	t_cinfo *client_to_serve = NULL;
	while (1)
	{
		pthread_mutex_lock(&queue_lock);
		//  ?????????????????
		while (!shutdown_threads && !(client_to_serve = get_client_to_serve(clients))) {
			pthread_cond_wait(&queue_cond, &queue_lock);
		}
		pthread_mutex_unlock(&queue_lock);

		if (shutdown_threads) {
			// Clean up and exit thread
			if (client_to_serve) {
				free(client_to_serve);
				client_to_serve = NULL;
			}
			return NULL;
		}
		if (shutdown_threads)
		{
			if (client_to_serve) {
				free(client_to_serve);
				client_to_serve = NULL;
			}
			return NULL;
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
	pthread_cond_init(&queue_cond,NULL);
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
		pthread_cond_signal(&queue_cond);
		pthread_mutex_unlock(&queue_lock);

	}	
}