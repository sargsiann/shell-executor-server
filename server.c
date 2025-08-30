#include "initials.h"

int main() {

	// Our Ipv6 handler struct and socket fds
	struct sockaddr_in6 server_info;
	t_infos server;
	int		listen_fd;
	int		connection_fd;

	
	struct sockaddr_storage	client_info; // Structs for ipv6 client in any case we will get ipv6 struct


	// Setting params to our server
	memset(&server_info,0,sizeof(server_info));
	memset(&server,0,sizeof(server));
	server_info.sin6_addr = in6addr_any;
	server_info.sin6_family = AF_INET6;
	server_info.sin6_port = htons(PORT); // Using htons to convert host byte order to network

	listen_fd = socket(AF_INET6,SOCK_STREAM,0);
	if (listen_fd == -1)
		exit_error("Socket creation error",NULL);
	server.fd = listen_fd;

	// Allowing our socket for working with both ipv4 and ipv6
	// So our ip4 [oct1]:[oct2]:[oct3]:[oct4] will be seen mapped as ipv6 like ::ffff:[oct1]:[oct2]:[oct3]:[oct4]
	// It is done by kernel
	int handle_ipv4_also = 0; 
	if (setsockopt(listen_fd, IPPROTO_IPV6, IPV6_V6ONLY, &handle_ipv4_also, sizeof(int)) == -1)
		exit_error("Setting dual stack failed",&server);

	int	optval = 1; // Setting reusable same port to avoid TIME_WAIT issues
	if (setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) < 0) 
		exit_error("Error for reusing port",&server);

	if (bind(listen_fd,(SA *)(&server_info),sizeof(server_info)) == -1) 
		exit_error("Bind error",&server);
	
	if (listen(listen_fd,10) == -1) 
		exit_error("Listen error",&server);

	memset(&client_info,0,sizeof(client_info)); // Clearing the memory of client
	socklen_t cinfo_len = sizeof(client_info); // Getting the size
	while (1)
	{
		connection_fd = accept(listen_fd,(SA *)(&client_info),&cinfo_len);
		if (connection_fd == -1) 
			exit_error("Accept error",&server);
		log_client_ip_info(client_info);
		request_handler(&connection_fd);
	}	
}