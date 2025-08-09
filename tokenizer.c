#include "server.h"


// 1 pure command(can be group of like cat x|ls) 2 operator 3 argument
int		token_definer(t_token **head, int prev_type);
void	tokens_definer(t_token **head);
void	add_token(t_token **head, char *name) ;


// 32 is our known ' ' space
// 9 is horizontal tab '	'
// 10 is newline \n
// 11 is \r moves after the newline to it first
// 12 vertical tab moves to next line same positon
// 13 deprecated

bool	is_space(char c) {
	return (c == 32 || (c > 8 && c <= 13));
}


//  ; -> separator, runs many commands in same line
// a&&b runs b if a true a||b if a not true 
//  > >> < << redirects and here doc (disallowed)
//  | pipe operator
//  & running in backround (disallowed)

bool	is_operator(char *name) 
{
	if (!name)
		return false;
	return (
    strncmp(name, "|", 1) == 0 ||        // single pipe '|'
    strncmp(name, "||", 2) == 0 ||       // Or operator
    strncmp(name, "&&", 2) == 0 ||       // AND operator '&&'
    strncmp(name, ">", 1) == 0 ||        // single '>'
    strncmp(name, ">>", 2) == 0 ||       // double '>>'
    strncmp(name, "<<", 2) == 0 ||       // here-doc '<<'
    strncmp(name, "<", 1) == 0 ||        // single '<'
    strncmp(name, ";", 1) == 0 ||        // semicolon
    strncmp(name, "&", 1) == 0            // ampersand
);
}

// This stands for tokenizeing commands by special rukl
t_token **get_token_commands(char *commands) 
{
	t_token **head = malloc(sizeof(t_token *));
	*head = NULL;
	char	*tok_start;
	char	*tok_end;
	char	*token_name;
	
	char	quote_type = 0;
	bool	in_quotes = false;
	
	tok_end = commands;
	while (*tok_end)
	{
		// Moveing towards spaces
		while (is_space(*tok_end))
			tok_end++;
		tok_start = tok_end;
		// While we havent reached to another token
		while (*(tok_end) && !is_space(*(tok_end) ))
		{
			// If we got quotes
			if (*tok_end == 39 || *tok_end == 34) {
				quote_type = *tok_end;
				tok_end++;
				// Forwarding on ignoreing spaces
				while (*tok_end && *tok_end != quote_type)
					tok_end++;
				// Getting out from quotes
				if (*tok_end == 0)
					break;
			}
			tok_end++;
		}
		// In case if we have like pwd; ls
		if (*(tok_end - 1) == ';') {
			token_name = substr(tok_start,tok_end - 1);
			add_token(head,token_name);
			add_token(head,strdup(";"));
		}
		else {
			token_name = substr(tok_start,tok_end);
			add_token(head,token_name);
		}	
		// If we got many spaces after
		// Moveing to next char not space
		while (is_space(*(tok_end)))
			tok_end++;
		tok_start = tok_end;
	}
	// After defining each token giving its type
	tokens_definer(head);
	return head;
}

// Defining one exact token returning its type
int	token_definer(t_token **token, int prev_type) {
	t_token *t = *token;
	// In case of operators

	fflush(stdout);
	if (is_operator(t->token_name)) {
		t->token_type = 2;
		return 2;
	}
	// Before was command
	if (prev_type == 1) {

		t->token_type = 3;
		return 3;
	}
	// Continious arguments
	if (prev_type == 3) {
		t->token_type = 3;
		return 3;
	}
	// Before was operator
	if (prev_type == 2) {
		t->token_type= 1;
		return 1;
	}
}


// Stands for defining token types for validation
void	tokens_definer(t_token **head)
{
	t_token *tmp = *head;
	if (!tmp)
		return ;
	// The firs always must be command
	tmp->token_type = 1;
	int	prev_type = 1;

	while (tmp)
	{
		tmp = tmp->next;
		if (!tmp)
			break;
		prev_type = token_definer(&tmp,prev_type);
	}
	// print_tokens(head);
}

void	add_token(t_token **head, char *name) 
{
	if (!head)
		return;
	t_token *new = malloc(sizeof(t_token ));
	new->token_name = name;
	new->token_type = 0;
	new->next = NULL;

	if (!*head)
	{
		*head = new;
		return ;
	}
	t_token *tmp = *head;
	// Getting to the last toke
	while (tmp->next)
		tmp = tmp->next;
	tmp->next = new;
}

void free_tokens(t_token **head) 
{
	if (!head || !*head)
		return;

	t_token *tmp = *head;
	t_token *to_free;

	while (tmp)
	{
		to_free = tmp;
		tmp = tmp->next;

		if (to_free->token_name)
			free(to_free->token_name);
		free(to_free);
	}
	free(head); // Free the pointer to the head (if dynamically allocated)
}


void	print_tokens(t_token **head) 
{
	if (!head || !*head)
		return ;
	t_token *tmp = *head;

	// Prints for debugg
	while (tmp)
	{
		printf("--------------\n");
		printf("[%s] : name \n",tmp->token_name);
		printf("%s\n",tmp->token_type  == 1 ? "command" : (tmp->token_type == 2 ? "operator" : "argument"));
		printf("--------------\n");
		fflush(stdout);
		tmp = tmp->next;	
	}	
}