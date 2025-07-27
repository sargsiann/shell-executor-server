#include "server.h"

typedef struct sockaddr SA;
# define BUFF_SIZE 1024;

void *hanlde_echo(void *data) 
{
	// Casting the fd
	int *s = (int *)data;
	int connection_socket_fd = *s;

    char buffer[1024];    
    while (1)
    {
		// Setting 0 each time
        memset(buffer, 0, sizeof(buffer));

		// Getting it
        int bytes_received = recv(connection_socket_fd, buffer, sizeof(buffer), 0);
		if (bytes_received == -1) {
            perror("Recv error :");
            break;
        } else if (bytes_received == 0) {
            printf("\nClient disconnected\n");
			fflush(stdout);
            break;
        }

		// Printing message in server
		printf("Client : %s", buffer);
		fflush(stdout);

		// Sending back the message
        if (send(connection_socket_fd, buffer, strlen(buffer), 0) == -1) {
            perror("Send Error :");
            break;
        }
    }
	// After close the fd
    close(connection_socket_fd);
}

int main(int ac, char **av) 
{
	int sock_fd;
	struct sockaddr_in info;	
	
	// Clearing in case of junk
	memset(&info,0,sizeof(info));

	// THIS IS HOST TO NETWORK BYTES CASTING
	info.sin_port = htons(3490);
	// Any local address
	info.sin_addr.s_addr = INADDR_ANY;
	// IP4 VERSION
	info.sin_family = AF_INET;

	// AF INET -> IPV4 SOCK_STREAM->TCP O->FOR ANY PROTOCOL TYPE
	if ((sock_fd = socket(AF_INET,SOCK_STREAM, 0)) == -1) 
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
		pthread_t thread;

		int *c = malloc(sizeof(int));
		*c = connection_sock_fd;
		pthread_create(&thread,NULL,&hanlde_echo,c);

		// Detaching when thread is done so the function ends )
		pthread_detach(thread);
		printf("-----------------------------\n");
		fflush(stdout);

	}	
}