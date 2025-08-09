#include "server.h"

void	add_client_to_serve(int client_conn_fd, t_cinfo **head) 
{
	// Creating struct for client
	t_cinfo *new_client = malloc(sizeof(t_cinfo));
	new_client->c_socket_fd = client_conn_fd;
	new_client->next = NULL;

	if (!head)
		return ;
	// Case when tasks are empty
	if (!*head)
	{
		*head = new_client;
		return ;
	}

	t_cinfo *tmp = *head;

	// Going to last member
	while (tmp->next)
		tmp = tmp->next;
	tmp->next = new_client;
}

// QUEUE IS FIRST IN FIRST OUT SO WE MUST GET THE FIRST MEMBER
t_cinfo *get_client_to_serve(t_cinfo **head) 
{
	// Empty case
	if (!head)
		return NULL;
	if (!*head)
		return NULL;
	// Geting pointer to client
	t_cinfo *client_to_serve = *head;
	
	// Moveing head to next
	*head = (*head)->next;
	// "Disconecting" client from tasks
	client_to_serve->next = NULL;

	return client_to_serve;
}

void	free_queue(t_cinfo **head) 
{
	if (!head)
		return ;
	t_cinfo *tmp = *head;
	t_cinfo *to_free = tmp;

	while (tmp)
	{
		to_free = tmp;
		tmp = tmp->next;
		free(to_free);
	}
	free(head);
}