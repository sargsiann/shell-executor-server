#include "server.h"

void	 print_pool(t_queue **head) 
{
	t_queue *tmp;

	tmp = *head;

	for (;tmp;tmp = tmp->next) {
		printf("%d\n",*(tmp->connection_fd));
	}
}

void	join_threads(pthread_t threads[]) 
{

}

char	*string_reallocator(char *old_message,char *buffer) 
{
	// Getting the lenght of previous message
	size_t old_message_len = 0; 
	if (!old_message)
		old_message_len = 0; 
	else
		old_message_len = strlen(old_message);
	
	// Getting the total lenght
	size_t len = old_message_len  + strlen(buffer);
	
	
	char *new_message = malloc(old_message_len + strlen(buffer));

	// Copying the content from all to one and returning it
	if (old_message)
		strcpy(new_message,old_message);

	strcpy(new_message + old_message_len, buffer);
	return new_message;
}