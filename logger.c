#include "initials.h"

void	log_client_ip_info(struct sockaddr_storage client_info) 
{
	unsigned short port = 0;

	// In case if we got pure Ipv4 connection (rarely)
	if (client_info.ss_family == AF_INET) {
		char	str_ipv4[INET_ADDRSTRLEN];
		struct sockaddr_in *client_ipv4 = (struct sockaddr_in *)(&client_info);
		port = ntohs(client_ipv4->sin_port);
		inet_ntop(AF_INET,&client_ipv4->sin_addr,str_ipv4,INET_ADDRSTRLEN);
		printf("Got connected by original Ipv4 : %s ip %hu port\n",str_ipv4,port);
	}
	else if (client_info.ss_family == AF_INET6) {
		char	str_ipv6[INET6_ADDRSTRLEN];
		struct sockaddr_in6 *client_ipv6 = (struct sockaddr_in6 *)(&client_info);
		port = ntohs(client_ipv6->sin6_port);
		inet_ntop(AF_INET6,&client_ipv6->sin6_addr,str_ipv6,INET6_ADDRSTRLEN);
		
		bool	is_mapped_ipv6 = is_ipv6_mapped(client_ipv6->sin6_addr);
		// Checking for mapped ipv4 to ipv6
		if (is_mapped_ipv6 == true)
			printf("Got connected by mapped Ipv4 : %s ip %hu port\n",str_ipv6,port);
		else
			printf("Got connected by original Ipv6 : %s ip %hu port\n",str_ipv6,port);
	}
}