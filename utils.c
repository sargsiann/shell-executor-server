#include "server.h"

void	 print_pool(t_queue **head) 
{
	t_queue *tmp;

	tmp = *head;

	for (;tmp;tmp = tmp->next) {
		printf("%d\n",*(tmp->connection_fd));
	}
}