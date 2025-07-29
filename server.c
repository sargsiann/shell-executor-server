#include "server.h"

typedef struct sockaddr SA;
# define BUFF_SIZE 1024;
# define THREADS_TO_SERVE 2

// Mutex to prevent threads use members of pool
pthread_mutex_t lock;
pthread_cond_t cond;


// The head of pool
t_queue **pool_head;

// Wee need reallocator (will read by 1kb each time and if needed realloc our main message for that)


// Message : handler (buffer maybe realloced each time if the message is big)
/*
	Getting the part of command runing to pass it through validator
	And pass the file handler to get files and also maybe checking
*/

// File handler :: fucntions to handle file getting via tcp (we will write our simple protocol)
/*
	Find the liine for file information
	If there is read it creating and starting to listen file information
	after close fd s
*/

// Executer function 
/*
	Execute via the system()
	after call clean all files for cleaning client files from machine
*/

// Validator function 
/*
	to validate file (to discard such things as sh ...[filename]) or ./[filename]
	to handle cases of shell injection and prevent them
*/



void	heap_checker(char *message) 
{

}


void	message_handler(char buffer[]) 
{
	
}

bool	check_for_end() {
	
}

void *hanlde_echo(void *data) 
{
    char buffer[1024];
	t_queue *node_to_handle = NULL;
	char *message = NULL;

	// For further will be handled for http
	int protocol_type = 0;
    while (1)
    {
		// Dereferencing and getting (Locking with mutex because another thread may access at that time to avoid race conditions)
		// And dont work for example NULL
		if (!node_to_handle) {
			pthread_mutex_lock(&lock);
			node_to_handle = get_from_pool(pool_head);
			// ???????????????????? Why there ??
			if (!node_to_handle)
				pthread_cond_wait(&cond,&lock);	
			// Making the thread wait before get the signal that we got connection in case if our node is not free else starting doing our work
			pthread_mutex_unlock(&lock);
	
		}
		if (!node_to_handle)
			continue;
		// Setting 0 each time
        memset(buffer, 0, sizeof(buffer));
	
		// Getting it
        int bytes_received = recv(*(node_to_handle->connection_fd), buffer, sizeof(buffer), 0);
		if (bytes_received == -1) {
            perror("Recv error :");
            break;
		}
		if (bytes_received == 0) {
			// If we got disconected by client our thread frees that part no need for mutex because we onl
			// already have access to that pointer (we disconnected it from main tasks queue)
            printf("\nClient disconnected\n");

			// And cloesing that socket connection fd from out thread
		    close(*(node_to_handle->connection_fd));
			fflush(stdout);
			// Freeing node for mem managment and assinging NULL for further searching 
			free(node_to_handle);

			// And the whole message from client
			free(message);
			node_to_handle = NULL;
			// And continue to listen not break from the loop
			continue;
		}

		// Need to add checker for message size to not overload the heap
		// Re allocateing each time for getting the message by chunks
		message = string_reallocator(message,buffer);
		
		// Starting from new input
		// if (message != NULL && message[strlen(message) - 1] == '\n')
		// {
		// 	free(message);
		// 	message = NULL;
		// }

		printf("%s\n",message);
		fflush(stdout);

		// Sending back the message
        if (send(*(node_to_handle->connection_fd), buffer, strlen(buffer), 0) == -1) {
            perror("Send Error :");
            break;
        }
    }
}

int main(int ac, char **av) 
{
	int sock_fd;
	struct sockaddr_in6 info;

	// Initing our pool_head;
	pool_head = malloc(sizeof(t_queue *));

	// Creating max number threads (20 IN OUR CASE FOR POOL)
	// pthread_t threads[THREADS_TO_SERVE];

	pthread_t threads[THREADS_TO_SERVE];

	for (int i = 0; i < THREADS_TO_SERVE;i++)
		pthread_create(&threads[i],NULL,&hanlde_echo,NULL);

	// Initing our mutex
	pthread_mutex_init(&lock,NULL);
	
	// Initing our condition variable (our threads will wait and work depends on it)
	pthread_cond_init(&cond,NULL);

	// Clearing in case of junk
	memset(&info,0,sizeof(info));

	// THIS IS HOST TO NETWORK BYTES CASTING
	info.sin6_port = htons(3490);
	// Any local address
	info.sin6_addr = in6addr_any;
	// IP4 VERSION
	info.sin6_family = AF_INET6;

	// AF INET -> IPV4 SOCK_STREAM->TCP O->FOR ANY PROTOCOL TYPE
	if ((sock_fd = socket(AF_INET6,SOCK_STREAM, 0)) == -1) 
	{
		perror("Socket creatin error");
		exit(1);
	}

	// Making port reusable after each call
	int opt = 1;
    if (setsockopt(sock_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        perror("setsockopt failed");
        exit(EXIT_FAILURE);
    }

	socklen_t len = sizeof(info);
	// After socket wee need to bind it
	if (bind(sock_fd,(SA *)&info,len) == -1) {
		perror("Binding Error :");
		// Closeing fd to the socket
		close(sock_fd);
		exit(1);
	}

	if (listen(sock_fd,10) == -1) {
		perror("Listening Error");
		close(sock_fd);
		exit(1);
	}

	struct sockaddr_in client_info;
	socklen_t client_info_len = sizeof(client_info);
	
	char ip_str[INET_ADDRSTRLEN];
	unsigned short port;

	int connection_sock_fd;
	while (true)
	{
		if ((connection_sock_fd = accept(sock_fd,(SA *)&client_info,&client_info_len)) == -1) {
				perror("Accept Error");
				close(sock_fd);
				exit(1);
		}

		port = ntohs(client_info.sin_port);
		// Passing to inet_ntop main struct of IP address (there is field with s_addr(6) in Ipv6)
		inet_ntop(AF_INET,&client_info.sin_addr,ip_str,INET_ADDRSTRLEN);
		
		printf("-----------------------------\n");
		printf("Got connected by IP %s Port %d \n",ip_str,port);
		fflush(stdout);


		// The main socket binded uses for listening 
		// The accept returns the socket 

		// Creating the thread for each accepted connection (so the main thread go ahead and accpet new connections)

		int *c = malloc(sizeof(int));
		*c = connection_sock_fd;

		pthread_mutex_lock(&lock);
		// Adding to pool 
		// Preventing other threads changeing pull at this moment
		// For example one thread may be do get and add in one time
		add_to_pool(pool_head,c);
		// Sending the signal to start working
		pthread_cond_signal(&cond);
		// Unlocking
		pthread_mutex_unlock(&lock);

		printf("-----------------------------\n");
		fflush(stdout);

	}	
}