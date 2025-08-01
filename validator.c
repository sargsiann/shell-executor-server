#include "server.h"

// The allowed commands are ls,cat,grep,ps,pwd,sort,head,tail,wc,top,df,find,echo
// no redirections are allowed or here docs
// not allowed creating variables and execute it in this way $()


bool	validate_command(t_token *command) 
{

}

// Checking the commands that are not safe and check it ouf brackets
bool	validate_commands(char *commands) 
{
	t_token **command_tokens = get_token_commands(commands);
	return 1;
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
	// if (!validate_commands(tokens[0]))
	return NULL;
}