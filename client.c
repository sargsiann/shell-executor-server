#include "server.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 3490
#define BUFF_SIZE 1024

int main(int ac, char **av) 
{
    // Create socket (IPv4 specifically since we're using 127.0.0.1)
    int sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (sock_fd == -1) {
        perror("Socket creation error");
        exit(1);
    }

    struct sockaddr_in server_addr_info;
    memset(&server_addr_info, 0, sizeof(server_addr_info));
    
    // Configure server address
    server_addr_info.sin_family = AF_INET;  // IPv4
    server_addr_info.sin_port = htons(PORT);
    
    // Convert IP address from text to binary form
    if (inet_pton(AF_INET, "127.0.0.1", &server_addr_info.sin_addr) <= 0) {
        perror("Invalid address/Address not supported");
        close(sock_fd);
        exit(1);
    }
    
    if (connect(sock_fd, (struct sockaddr *)&server_addr_info, sizeof(server_addr_info)) == -1) {
        perror("Connection failed");
        close(sock_fd);
        exit(1);
    }

	struct timeval tv;
	tv.tv_sec = 5;  // 5 second timeout
	tv.tv_usec = 0;
	setsockopt(sock_fd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));

    char buffer[BUFF_SIZE];
	char message_serve[BUFF_SIZE];
    while (1) {

        if (!fgets(buffer, BUFF_SIZE, stdin)) {
            break;  // Exit on EOF or error
        }
		// Going through enter
		if (strcmp(buffer,"\n") == 0) {
			continue;
		}
        
        // Remove newline if present
        buffer[strcspn(buffer, "\n")] = 0;
        
        if (send(sock_fd, buffer, strlen(buffer), 0) == -1) {
            perror("Send failed");
            break;
        }

		memset(&buffer,0,BUFF_SIZE);
		recv(sock_fd,&buffer,1024,0);	
		printf("%s",buffer);
		memset(&buffer,0,BUFF_SIZE);
	}

    close(sock_fd);
    return 0;
}