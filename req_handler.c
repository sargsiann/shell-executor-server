#include "initials.h"



ParseResult parsing_req_line(Request *all_req) 
{
	
}


// short	parsing_headers() Headers to take attention Connection , Content-type, Content-Lenght
// short	parsing_body()

ParserState process_request_state(Request *all_req, ParserState state) 
{
	ParseResult result; // Result of action on state

	switch (state)
	{
		// Case of first request line handling
		case PARSING_REQUEST_LINE: 
			result = parsing_req_line(all_req);
			if (result == PARSE_OK)
				return PARSING_HEADERS;
			return PARSING_ERROR;
			break;
		default:
			break;
	}
}


void	http_reader() 
{
	Request		http_request; // Main struct of request
	char		buff[2048]; // Buffer where read to
	char		*message = NULL; // complete message
	ParserState	state = PARSING_INITIAL; // state of parseing

	while (1)
	{

	}
}

// The main request handler func for each thread for first parsing then executing and response sending
void	request_handler(void *conn_fd) 
{
	// Parseing
	// Executing
	// Response formatter sending the easy peasy part
}