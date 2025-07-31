#include "server.h"


void	create_file(t_queue **files_head, char *file_name, char *file_content) 
{
	int	*fd_ptr = malloc(sizeof(int));

	int	fd = open(file_name,O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);

	if (fd == -1)
	{
		perror("open");
		fflush(stderr);
	}
	write(fd,file_content,strlen(file_content));

	*fd_ptr = fd;

	add_to_pool(files_head, fd_ptr, file_name);
	close(fd);
}


// Using the same queue for putting each fd to it after for deleting after handleing
t_queue **file_handler(char *files) 
{
	// Getting to
	char	*handler = strstr(files,"<|>");
	

	// If we dont got any files (error)
	if (!handler)
		return NULL;

	// Vars used
	bool	in_file = true;
	char	*tmp = NULL;
	char	*file_name;
	char	*content;
	int		token_len;

	// The head of queue
	t_queue **files_head = malloc(sizeof(t_queue *));
	while (handler)
	{
		if (in_file)
			handler += 3;
		tmp = handler;
		handler = strstr(handler,",");

		// // If we dont get separator it refers to be error
		if (!handler && in_file) {
			return files_head;
		}

		
		// Getting the file name
		file_name = get_substr(tmp,handler - 1);

		tmp = handler + 1;
		handler = strstr(handler,"<|>") - 1;

		// If we havent closeing <|> tag and we are in file info section so we got in it
		if (!handler && in_file)
		{
			free(file_name);
			free(content);
			free_pool(files_head,1);		
			return NULL;
		}
		// Getting the file content
		content = get_substr(tmp,handler);
		fflush(stdout);
		// Comes file creation phase and adding to queue
		pthread_mutex_lock(&lock);
		create_file(files_head,file_name,content);
		pthread_mutex_unlock(&lock);
		handler += 1;
		// Got out from file section;
		in_file = false;

		
		handler = strstr(handler,"<|>");
		// // No more files
		if (!handler)
			break;
		in_file = true;
		handler += 3;
	}
	return files_head;
}