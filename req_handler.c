#include "initials.h"

bool	got_full_message(char	*section) // checking the case that we got all request at once
{

};


ParserState	parse_first_line(char	*section, char *end, Request *request_struct) // the first request line parseing
{

}

ParserState	free_parser(char	*section, Request request_struct, ParserState state) 
{
	if (section != NULL) // freeing remained memory of section char *
		free(section);
	if (state == PARSING_ERROR)
		perror(NULL); // at this time just perror will be good error formatter in future
	return 1;
}

ParserState	parse_section_by_state(char	*section, char *end, Request *request_struct, ParserState state) 
{
	if (state == PARSING_REQUEST_LINE) // the first line parseing
		return parse_first_line(section,end, request_struct);
	// if (state == PARSING_HEADERS)
}

char	*section_end_ptr(char *section,ParserState state) 
{
	if (state == PARSING_REQUEST_LINE) // the first line before headers ends with \r\n
		return strstr(section,"\r\n");
	return strstr(section,"\r\n");
}

char	*change_section_by_state(char *section, ParserState state) //extracting remained part FE removing headers part if done 
{
	
} 

ParserState	http_parser(int	client_socket) 
{
	Request		request_struct = {0}; // struct for filing info about request
	char		buff[2048] = {0}; // the buffer which reads from socket
	char		*http_section = NULL; // the section for parseing
	ParserState state = PARSING_REQUEST_LINE; // the state of parser
	short		bytes_read = 0; // the bytes that was readen from socket
	while (1)
	{
		bytes_read = recv(client_socket,buff,2,0); // reading
		if (bytes_read == -1) // error while reading
			return free_parser(http_section,request_struct,PARSING_ERROR); // cleaning memory with logging error message
		if (bytes_read == 0) // connection closed by client
			return  free_parser(http_section,request_struct,PARSING_CLOSED); // with logging that connection closed
		http_section = str_realloc(http_section,buff); // adding 
		
		char	*section_end = NULL; // the end of section to find
		while (1) // the inner loop for handleing gotted info
		{
			section_end = section_end_ptr(http_section,state); //getting the end of section depends on state
			if (section_end == NULL) //if section unfinished breaking the loop waiting for recv to get full
				break;
			state = parse_section_by_state(section,section_end,&request_struct,state); // starting the parsing of section
			if (state == PARSING_ERROR) // if we had issues from validation
				return  free_parser(http_section,request_struct,PARSING_CLOSED); 
			http_section = change_section_by_state(http_section,state); // refreshing section remained for parsing
		}
		memset(buff,0,bytes_read); //cleaning buffer up each time
	}	
}

// The main request handler func for each thread for first parsing then executing and response sending
void	request_handler(void *conn_fd) 
{
	
	http_parser(*(int *)conn_fd); // Parseing
	// Executing
	// Response formatter sending the easy peasy part
}