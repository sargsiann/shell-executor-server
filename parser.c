#include "server.h"

static int		space_or_newline(char c) {
	return((c >= 8 && c <= 13) || c == '\n');		
}

// The condition on the line in divider
static int		condition_for_divider(char *message, int i, bool in_braces) 
{
	// If we got that name FILE its not in braces its not on first and before it comes space or newline
	return ((strncmp("FILE",message + i,4) == 0) && !in_braces && i > 0 
	&& space_or_newline(message[i - 1]));
}

// Finding the index of separation or the end of command
// Because that can be no files
static int		divider_finder(char *message) 
{
	// If we got that message in braces
	unsigned char brace_id = 0;
	bool in_braces= 0;
	size_t len = strlen(message);
	
	for (int i = 0;i < len;i++) 
	{
		// Initing for first time
		if (!brace_id && (message[i] == 34 || message[i] == 39)) {
			if (message[i] == 34)
				brace_id = 34;
			else
				brace_id = 39;
			in_braces = 1;	
			continue;
		}
		// We gotting the second time to it
		if (message[i] == brace_id) {
			in_braces = 0;
			// If we got brace after another putting in_braces true
			if (message[i] == 34 || message[i] == 39) {
				if (message[i] == 34)
					brace_id = 34;
				else
					brace_id = 39;
				in_braces = 1;
			}
			// Else doing nothing
		}
		if (condition_for_divider(message,i,in_braces))
			return i;
	}
}

// There can be parser_http and more so the programm can be expansable

char	**parser_ses(char *message) 
{
	if (!message) 
		return NULL;
	
	// Getting the index for splitting to command part and to file part
	int	index_to_split = divider_finder(message);
	printf("%d",index_to_split);
}