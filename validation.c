#include "server.h"

// no redirections are allowed or here docs
// not allowed creating variables and execute it in this way $()

// Ls - shows the files in dir
// cat prints the content of file
// grep finds some patterns in text
// ps shows running processes
// pwd prints current working dir
// sort sorts the text by some rule
// head shows first lines of file tail -> last
// wc word counter in file or in text
// top is ps but also shows resources usaed
// df shows usage of disk
// prints the text to file or to stdout (in our case no redirects so in stdout)
// whoami prints user name current
// id prints ids of users groups
// uptime shows time of working of our system
// date prints date
// file shows type of file 
// stat shows all permissions and info about file
// du shows all directories and usage resources from disk
// more less pageing



bool	is_allowed(char	*command) {
	char *allowed[] = {"ls", "cat", "grep", "ps", "pwd", "sort", "head", "tail", "wc", "top", "df", "echo",
			"whoami", "id", "uptime", "date", "file", "stat", "du", "more", "yes", "less", NULL};

	int	i = 0;

	if (!command)
		return false;
	while (allowed[i])
	{

		if (strcmp(allowed[i],command) == 0) 
			return true;
		i++;
	}
	return false;
}

bool	is_allowed_operand(char *op) 
{
	if (!op)
		return false;
	char *allowed_operators[] = {"|", "||", "&&", ";", NULL};

	int i = 0;
	while (allowed_operators[i])
	{
		if (strcmp(allowed_operators[i],op) == 0) {
			return true;
		}
		i++;
	}
	return false;
}

bool	is_not_allowed_operand(char *argument) 
{
	if (!argument)
		return false;
	char *operators[] = {">", ">>", "<<", "<","&", NULL};

	int i = 0;
	while (operators[i])
	{
		if (strcmp(operators[i], argument) == 0)
		{
			printf("Found not allowed operand: %s\n", argument);
			fflush(stdout);
			return false;
		}
		i++;
	}
	return true;
}

// Checking is there any hidden redirects and non allowed operands in big chunk of token 
bool	is_allowed_argument(char *argument) 
{
	bool	in_quotes = 0;
	char	quote_type = 0;

	int i = 0;

	if (!argument)
		return false;
	while (argument[i])
	{
		if (argument[i] == 39 || argument[i] == 34) {
			quote_type = argument[i];
			i++;
			while (argument[i] && argument[i] != quote_type)
				i++;
			// Non closed quotes we can return false
			if (argument[i] == 0)
				return false;
			i++;
			continue;
		}
		if (!is_not_allowed_operand(argument + i))
			return false;
		i++;
	}
	return true;
}

// Checking the commands that are not safe and check it ouf brackets
bool	validate_commands(char *commands) 
{
	if (!commands)
		return false;
	// Allowed commands
	t_token **command_tokens = get_token_commands(commands);
	t_token *tmp = *command_tokens;

	while (tmp)
	{
		if (tmp->token_type == 1)
		{
			if (is_allowed(tmp->token_name) == 0) {
				free_tokens(command_tokens);
				command_tokens = NULL;
				return false;
			}
		}
		else if (tmp->token_type == 2) {
			if (is_allowed_operand(tmp->token_name) == 0)
			{
				free_tokens(command_tokens);
				command_tokens = NULL;
				return false;
			}
		}
		else if (tmp->token_type == 3) {
			if (is_allowed_argument(tmp->token_name) == 0) {
				free_tokens(command_tokens);
				command_tokens = NULL;		
				return false;	
			}
		}
		tmp = tmp->next;
	}
	free_tokens(command_tokens);
	command_tokens = NULL;
	return true;
}


bool	validator(char *commands, char *files) 
{
	if (!validate_commands(commands))
		return false;
	return true;
}

bool	memory_checker() 
{
	
}