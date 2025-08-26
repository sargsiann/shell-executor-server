#include "initials.h"

void	free_server(t_server *serv) {
	close(serv->listen_fd);
}

void	exit_error(const char *msg,t_server *to_free) 
{
	perror(msg);
	if (to_free) {
		free_server(to_free);
	}
	exit(1);
}

// Compareing first 12 bytes of addres is its mapped it would be 00 00 00 00 00 00 00 00 00 00 00 ff ff by hex
bool	is_ipv6_mapped(struct in6_addr sin6_addr) 
{
	return (memcmp(&sin6_addr, "\0\0\0\0\0\0\0\0\0\0\xff\xff", 12) == 0);
}