#include "server.h"

void	create_file(char *file_name,char *content) 
{
	int	fd = open(file_name,O_CREAT | O_RDWR | O_TRUNC,0644);
	if (fd == -1) {
		perror("File creating error :");
		return ;
	}
	// Writing content to file
	if (write(fd,content,strlen(content)) == -1) {
		perror("Writing to file error :");
		close(fd);
		return ;
	}
	
	close(fd);
}

void setup_signal_handler() {
	// Struct for handler
    struct sigaction sa;
	// Handler function name
    sa.sa_handler = handle_sigint;
	// Getting other signals too not blocking it
    sigemptyset(&sa.sa_mask);
	// For breaking all sys cakks (in our case accept) for right handleing of closeing our programm
	sa.sa_flags = 0; 
	// Putting struct to SIGINT for handleing
    sigaction(SIGINT, &sa, NULL);
}

// File creating
t_files	**create_files(char *files)  {
	if (!files)
		return NULL;
	// ALWAYS WHEN DOUBLE POINTER INIT NULL ITS VALUE ALWAYS TWO DAYS ERRORORORRS
	t_files **files_list = malloc(sizeof(t_files *));
	files_list[0] = NULL; // Initialize the head of the list to NULL

	// Files for lock one for strtok and also file creating and writing
	char	*token = strtok(files,"<>");
	char	*file_name = NULL;
	char	*file_content = NULL;
	char	*coma = NULL;

	while (token != NULL)
	{	
		if (token[0] == 0) {
			token = strtok(NULL,"<>");
			continue;
		}
		coma = strchr(token,',');
		if (coma) {
			file_name = substr(token,coma);
			file_content = substr(coma + 1,token + strlen(token));
			// Error case no file names
			if (!file_name)
			{
				free_files_list(files_list);
				files_list = NULL;
				return NULL;
			}
			create_file(file_name,file_content);
			add_file(files_list,file_name);
			if (file_content)
				free(file_content);
			file_content = NULL;
		}
		// Error case no comma in sending "\n" 
		if (!coma && strcmp(token,"\n"))
		{
			fprintf(stderr,"Error: No file name found in token: [%s]\n", token);
			free_files_list(files_list);
			files_list = NULL;
			return NULL;
		}
        token = strtok(NULL,"<>");
	}
	return files_list;
}


char	*str_realloc(char *old, char *new) 
{
	char	*final;

	size_t	ol = 0;
	size_t	nl = 0;
	size_t	len = 0;
	if (old)
		ol = strlen(old);
	if (new)
		nl = strlen(new);
	len = ol + nl + 1;
	final = malloc(len);
	memset(final,0,len);
	if (old)
		strcpy(final,old);
	if (new)
		strcpy(final + ol,new);
	free(old);
	old = NULL;
	return final;
}

char	*substr(char *start, char *end) 
{
	if (!start || ! end || start >= end)
		return NULL;
	size_t len = end - start + 1;

	char *sub = malloc(len);
	memset(sub,0,len);
	strncpy(sub,start,len - 1);
	return sub;
}

char	*get_command(char	*all_message) 
{
	// If the command tag is not in first
	if (strstr(all_message,"<COMMANDS>") - all_message != 0)
		return NULL;
	char	*files = strstr(all_message,"<FILES>");
	char	*end = strstr(all_message,"<END>");
	if (!end && !files)
		return NULL;
	char	*end_of_commands = files != NULL ? files : end;
	char	*command = substr(all_message + 10, end_of_commands);
	return command;
}

char	*get_files_section(char *all_message) 
{
	char	*files = NULL;
	char	*start = strstr(all_message,"<FILES>");

	if (!start)
		return NULL;
	char	*end = strstr(all_message,"<END>");

	files = substr(start + 8,end);
	return files;
}
