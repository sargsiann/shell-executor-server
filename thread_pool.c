#include "server.h"

// First in First out

// Getting the head of pool and connection fd to adding it to end of pool

void	add_to_pool(t_queue **pool_head,int *connection_fd) 
{
	// Initing the new member
	t_queue *member = malloc(sizeof(t_queue ));
	member->connection_fd = connection_fd;
	member->next = NULL;

	// In case of emptyness
	if (!*pool_head){
		*pool_head = member;
		return ;
	}

	t_queue *tmp = *pool_head;
	// Going to the last member
	while (tmp->next)
		tmp = tmp->next;
	tmp->next = member;

}

// Get the first member (means the first added member) and remove it from the queue

t_queue *get_from_pool(t_queue **head) 
{
	// If free got nothing although it will be not necessary we have checking in case of work for threads
	if (!*head)
		return NULL;
	// Getting the first added 
	t_queue *returning_member = *head;
	// Moveing head to the next member;
	*head = (*head)->next;

	// Disconnecting it from the pool;
	returning_member->next = NULL;

	return returning_member;
}