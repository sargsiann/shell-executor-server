#include "server.h"
void add_file(t_files **head, char *name)
{
	if (!head || !name || name[0] == '\0')
		return;

	t_files *new_file = malloc(sizeof(t_files));
	t_files *tmp = *head;

	if (!new_file)
		return;

	new_file->name = name;
	if (!new_file->name) {
		free(new_file);
		return;
	}
	new_file->next = NULL;

	if (!*head) {
		*head = new_file;
		return;
	}

	while (tmp->next)
		tmp = tmp->next;

	tmp->next = new_file;
}
 
void	free_files_list(t_files **head)
{
	if (!head)
		return ;
	t_files *tmp = *head;
	t_files *to_free;
	while (tmp)
	{
		to_free = tmp;
		tmp = tmp->next;
		unlink(to_free->name); // Assuming the file was created and needs to be deleted
		free(to_free->name);
		free(to_free);
	}
	free(head);
}

void	print_files_list(t_files *head)
{
	if (!head)
		return ;
	t_files *tmp = head;
	printf("----------------------------------------\n");
	while (tmp)
	{
		fprintf(stderr,"File: %s\n", tmp->name);
		tmp = tmp->next;
	}
	printf("----------------------------------------\n");
}