#include "server.h"

// The allowed commands are ls,cat,grep,ps,pwd,sort,head,tail,wc,top,df,find,echo
// no redirections are allowed or here docs
// not allowed creating variables and execute it in this way $()


bool	is_allowed(char	*command) {
	char *allowed[] = {"ls", "cat", "grep", "ps", "pwd", "sort", "head", "tail", "wc", "top", "df", "find", "echo", NULL};

	int	i = 0;

	while (allowed[i])
	{
		if (strcmp(allowed[i],command) == 0) 
		{
			return true;
		}
		i++;
	}
	return false;
}

bool	is_allowed_operand(char *op) 
{
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

// Checking the commands that are not safe and check it ouf brackets
bool	validate_commands(char *commands) 
{
	// Allowed commands
	t_token **command_tokens = get_token_commands(commands);
	t_token *tmp = *command_tokens;

	while (tmp)
	{
		if (tmp->token_type == 1)
		{
			if (is_allowed(tmp->token_name) == 0) {
				fflush(stdout);
				free_tokens(command_tokens);
				command_tokens = NULL;
				return false;
			}
		}
		if (tmp->token_type == 2) {
			if (is_allowed_operand(tmp->token_name) == 0)
			{
				printf("NOOO");
				free_tokens(command_tokens);
				command_tokens = NULL;
				fflush(stdout);
				return false;
			}
		}
		tmp = tmp->next;
	}
	free_tokens(command_tokens);
	command_tokens = NULL;
	printf("VALID");
	fflush(stdout);
	return true;
}

// Function to check files form right handleing
bool	validate_files(char	*files) 
{
	return 1;
}

char	*validator(char **tokens) 
{
	if (!validate_commands(tokens[0]))
		return "Command handleing error";
	return NULL;
}