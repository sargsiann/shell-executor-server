#include "server.h"

// member lasts for command or file 0 -> command 1 -> files
char	*get_token(char *request, short member) 
{
	char	*command_idx = strstr(request, "<|COMMAND|>");
	char	*file_idx = strstr(request, "<|FILES|>");
	char	*end_idx = strstr(request, "<|END|>");

	// For commands starting from one char affter COMMAND so dy for FILE if file
	char *start_index = member == 0 ? command_idx + 11 : file_idx + 9; 
	char *end_index = member == 0 ? (file_idx != NULL ? file_idx : end_idx) : end_idx;
	
	// Getting token for that size

	size_t len = end_index - start_index;

	char *token = malloc(end_index - start_index + 1);
	memset(token,0,len + 1);
	strncpy(token,start_index,len);
	return token;
}

char	**parser_ses(char *message) 
{
	if (!message) 
		return NULL;
	
	char	**tokens = malloc(3 * sizeof(char *));
	char	*commands;
	char	*files;
	tokens[2] = NULL;
	
	// Getting the index for splitting to command part and to file part

	// Message comes right
	// printf("%s\n",message);

	commands = get_token(message, 0);
	if (!strstr(message,"<|FILES|>"))
		files = NULL;
	else
		files = get_token(message,1);
	

	tokens[0] = commands;
	tokens[1] = files;
	return tokens;
}