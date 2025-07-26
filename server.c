#include "server.h"

int main(int ac, char **av) {

	// Struct for fields of connection
	struct addrinfo server_addr;

	// Struct for storeing c a i in each acception
	struct sockaddr_in client_address_info; 

	// Result struct for the connection result or results
	struct addrinfo *res;

	// Cleaning up the server address structure
	memset(&server_addr, 0, sizeof(struct addrinfo));

	server_addr.ai_family = AF_UNSPEC; // Use AF_INET or AF_INET6 as needed
	server_addr.ai_socktype = SOCK_STREAM; // Use SOCK_DGRAM for UDP
	server_addr.ai_flags = AI_PASSIVE; // use my address (so we are making an server)

	// Getting address information for the server
	if (getaddrinfo(NULL, "3490", &server_addr, &res) != 0) {
		perror("getaddrinfo failed");
		return 1;
	}

	// Part just for checking getting info about address

	// Address ipv4 or ipv6
	void *addr;

	// char for version of the IP
	char *ipver;

	// string to hold the IP address
	char ip_str[INET_ADDRSTRLEN];

	struct addrinfo *tmp = res;
	while (tmp) 
	{
		if (tmp->ai_family == AF_INET) {
			// IPv4
			struct sockaddr_in *ipv4 = (struct sockaddr_in *)tmp->ai_addr;
			addr = &(ipv4->sin_addr);
			inet_ntop(tmp->ai_family, addr, ip_str, sizeof(ip_str));
			ipver = "IPv4";
		} 
		// Convert the IP address to a string
		// moving through the tmps
		tmp = tmp->ai_next;
	}



		// File descriptor for the socket
	int sock_fd;

	// creating the socket by the result info got from res struct IP type, 
	// socket type (tcp or udp) and ai_protocol exact protocol
	sock_fd = socket(res->ai_family,res->ai_socktype,res->ai_protocol);

	if (sock_fd == -1) {
		perror("Socket creating failed : ");
		// cleanup for result
		freeaddrinfo(res);
		exit(1);
	}

	int yes = 1;	
	
	// makeing the port reusable
	if (setsockopt(sock_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) == -1) {
		perror("setsockopt(SO_REUSEADDR) failed");
		freeaddrinfo(res);
		exit(1);
	}

	// We mentioned the port number in initial part of res creating so we now binding it
	// The kernel remember that port for my process and if anything comes to mentioned port it will come to this process
	// ports < 1024 are more restricted needs root permision
	if (bind(sock_fd, res->ai_addr, res->ai_addrlen) == -1) {
		perror(("Binding socket failed"));
		freeaddrinfo(res);
		// Closeing that file descriptor of socket
		close(sock_fd);
		exit(1);
	}

	// The number 10 is BACKLOG SO HOW MANY CLIENTS CAN WAIT TO BE SERVED IF THE SERVER IS LOADED BIG I MEAN
	// 10 + CLIENTS WANT TO WAIT THE 11 TH WILL GET REFUSION SO THE CONNECTION WILL NOT WAIT TO SERVE HIM
	// ONLY AFTER WILL BE FREE SPACE FOR IT
	// the listen function starting to use binded port for listening so our process will become seenable 
	// Запомните: backlog - это размер "приёмной" перед вашим сервером,
	//  где могут подождать новые подключения, пока сервер занят обработкой текущих.
	if (listen(sock_fd, 10) == -1) {
		perror(("Cant listen"));
		freeaddrinfo(res);
		close(sock_fd);
		exit(1);
	}
	

	// Connection file descriptor for each client when accept starting to accept connection
	
	int connection_fd;
	socklen_t len = sizeof(client_address_info);

	// Buffer for storeing the messages
	char buffer[1024];

	// setting 0 
	memset(buffer,0,1024);

	

	char client_ip[INET6_ADDRSTRLEN];
	unsigned short client_port;

	char response[1024];
	// Infinite loops for getting 
	while (1)
	{

		// getting connection file description and storeing connected 
		// addres info to our struct by our listening socket fd
		// Our accept stops the programm before getting the connection
		connection_fd = accept(sock_fd,(struct sockaddr*)&client_address_info,&len);

		// network to local (changeing bytes order from network to local machines) 
		// Getting the ip and port of client
		inet_ntop(AF_INET, &client_address_info.sin_addr, client_ip, INET6_ADDRSTRLEN);
		client_port = htons(client_address_info.sin_port);
		
		if (connection_fd == -1) {
			perror("Accept Error :");
			freeaddrinfo(res);
			close(sock_fd);
			exit(1);
		}
		
		// FORCED TO SEND TO STDOUT 
		printf("Got connected to ip : %s port : %d \n",client_ip, client_port);	
		fflush(stdout);
		


		// Loop for accepting every message from client until it ends 
		while (1)
		{

			// Getting the message by connection to our buffer
			// the recv stops execution before getting all the message
			
			int bytes = recv(connection_fd,&buffer,1024,0);

			// Error case
			if (bytes == -1) {
				perror("Receveid error :");
				freeaddrinfo(res);
				close(sock_fd);
				exit(1);
			}
			
			else if (bytes == 0)
			{
				// The case when remote interupted connection
				printf("connection lost from %s %d \n",client_ip, client_port);
				fflush(stdout);
				break;
			}

			// Combining all info to one response string and after send (multiplie sends is not recomended)
	 		snprintf(response, sizeof(response), 
                    "Echo from server (IP: %s, Port: %s): %s\n", 
                    "127.0.0.1", "3490", buffer);
			
			 if (send(connection_fd, response, strlen(response), 0) == -1) {
                perror("send failed");
                break;
            }
			
			printf("%s is message from client \n",buffer);

			// Setting the buffer to 0 values
			memset(buffer,0,1024);
			memset(response,0,1024);
		}
		close(connection_fd);
	}
	return 0;
}