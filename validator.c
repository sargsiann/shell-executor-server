#include "server.h"

bool	check_for_safe_words() 
{

}

// Checking the commands that are not safe and check it ouf brackets
bool	validate_commands(char *commands) 
{
	size_t	i = 0;
	bool	in_quote; 
	char	quote_type = 0;
	size_t	len = strlen(commands);

	while (i < len)
	{
		// Start the detection of bracket
		if (quote_type == 0 && (commands[i] == 39 || commands[i] == 34)) {
			quote_type = commands[i];
			in_quote = true;
			// forwarding
			continue;
		}
		// If we got end to the brecket the closeing one
		if (quote_type != 0 && (commands[i] == quote_type)) {
			quote_type = 0;
			in_quote = false;
		}
		if (strcmp(commands + i,""))
		i++;
	}	
}

bool	validate_files(char	*files) 
{

}

bool	validator(char **tokens) 
{
	return (validate_commands(tokens[0]) && validate_files(tokens[1]));
}