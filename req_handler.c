#include "initials.h"


ParserState	parse_first_line(char	*section, Request *request_struct) // the first request line parseing
{
	// Initing NULL for further handling
	request_struct->method = NULL;
	request_struct->uri = NULL;
	request_struct->version = NULL;

	fprintf(stderr,"PARSEING THE FIRST LINE\n");
	// Exctracting the method
	fprintf(stderr,"%s\n",section);
	if (strncmp(section,"GET ",4) == 0)
		request_struct->method = strdup("GET");
	else if (strncmp(section,"POST ",5) == 0)
		request_struct->method = strdup("POST");
	else {
		request_struct->err = ERR_UNSUPPORTED_METHOD; // else we got unsupported method for our server
		return PARSING_ERROR; // breaking parsing process
	}
	fprintf(stderr,"OKAY FOR METHOD %s\n",request_struct->method);
	section = strchr(section,' ') + 1; // moveing to first char after space
	// Extracting the uri for request
	if (strncmp(section,"/exec ",6) == 0 || strncmp(section,"/status ",8) == 0) 
	{
		if (strcmp(request_struct->method,"POST") != 0) {
			request_struct->err = ERR_UNSUPPORTED_METHOD; // only POST allowed for /exec and /status
			return PARSING_ERROR;
		}
		char *end_of_uri = strchr(section,' '); // finding the end of uri
		request_struct->uri = substr(section,section,end_of_uri); // extracting uri
	}
	else if (strncmp(section,"/commands ",10) == 0 || strncmp(section,"/operators ",11) == 0) 
	{
		if (strcmp(request_struct->method,"GET") != 0) {
			request_struct->err = ERR_UNSUPPORTED_METHOD; // only GET allowed for /commands and /operators
			return PARSING_ERROR;
		}
		char *end_of_uri = strchr(section,' '); // finding the end of uri
		request_struct->uri = substr(section,section,end_of_uri); // extracting uri
	}
	else {
		request_struct->err = ERR_MALFORMED_REQUEST_LINE; // wrong uri
		return PARSING_ERROR;
	}
	fprintf(stderr,"OKAY FOR PATH %s\n",request_struct->uri);

	// moveing front
	section = strchr(section,' ') + 1;
	if (strncmp(section,"HTTP/1.1\r\n",10) == 0 || strncmp(section,"HTTP/1.0\r\n",10) == 0) // checking the version
		request_struct->version = strdup("HTTP/1.1");
	else {
		request_struct->err = ERR_UNSUPPORTED_VERSION; // else we got unsupported version for our server
		return PARSING_ERROR; // breaking parsing process
	}
	fprintf(stderr,"OKAY FOR VERSION %s\n",request_struct->version);
	return PARSING_HEADERS; // first line parsing done ! continueing to headers
}

void	free_parser(char	*section, Request *request_struct, ParserState state) // is used in error case SO IF WE FREED PARSER IN PARSING STATE THAT MEANS WE GOT ERROR
{
	if (section != NULL) // freeing remained memory of section char *
		free(section);
	if (state < PARSING_HEADERS) //if we are on request line
	{
		if (request_struct->method != NULL) {
			free(request_struct->method);
			request_struct->method = NULL;
		}
		if (request_struct->uri != NULL) {
			free(request_struct->uri);
			request_struct->uri = NULL;
		}
		if (request_struct->version != NULL) {
			free(request_struct->version);
			request_struct->version = NULL;
		}
	}
}

ParserState	parse_section_by_state(char	*section, char *end, Request *request_struct, ParserState state) 
{
	if (state == PARSING_REQUEST_LINE) // the first line parseing
		return parse_first_line(section,request_struct); // in first line we know that we dont need end just go that \r\n
	if (state == PARSING_HEADERS)
		return 
}

char	*section_end_ptr(char *section,ParserState state) 
{
	if (state == PARSING_REQUEST_LINE) // the first line before headers ends with \r\n
		return strstr(section,"\r\n");
	return strstr(section,"\r\n");
}

char	*change_section_by_state(char *section, ParserState state) //extracting remained part FE removing headers part if done 
{
	char *res = NULL;
	if (state == PARSING_REQUEST_LINE) {
		res = substr(section, strstr(section,"\r\n"), section + strlen(section));	
		fprintf(stderr,"CHANGED SECTION TO %s\n",res);
	}
	free(section); // freeing the previous section memory address
	return res;
}

Request	http_parser(int	client_socket) 
{
	Request		request_struct = {0}; // struct for filing info about request
	char		buff[2048] = {0}; // the buffer which reads from socket
	char		*http_section = NULL; // the section for parseing
	ParserState state = PARSING_REQUEST_LINE; // the state of parser
	short		bytes_read = 0; // the bytes that was readen from socket
	while (1)
	{
		bytes_read = recv(client_socket,buff,2048,0); // reading
		if (bytes_read == -1) { // error while reading
			free_parser(http_section,&request_struct,state); // cleaning memory with logging error message
			break;
		}
		if (bytes_read == 0) { // connection closed by client
			free_parser(http_section,&request_struct,state); // with logging that connection was closed
			break;
		}
		http_section = str_realloc(http_section,buff); // adding 
		char		*section_end = NULL; // the end of section to find
		ParserState	prev_state = state; // the previous state for freeing in error case
		while (1) // the inner loop for handling gotted info
		{
			section_end = section_end_ptr(http_section,state); //getting the end of section depends on state
			if (section_end == NULL) //if section unfinished breaking the loop waiting for recv to get full
				break;
			fprintf(stderr,"-----------------\n");
			state = parse_section_by_state(http_section,section_end,&request_struct,state); // starting the parsing of section
			if (state == PARSING_ERROR) { // if we had issues from validation
				free_parser(http_section,&request_struct,prev_state);
				break;
			}
			http_section = change_section_by_state(http_section,prev_state); // refreshing section remained for parsing
			fprintf(stderr,"remained %s\n",http_section);
			if (state == PARSING_HEADERS)
				break; // just breaking for this moment
			fprintf(stderr,"-----------------\n");
		}
		if (state == PARSING_HEADERS)
			break;
		if (state == PARSING_ERROR)
			break;
		memset(buff,0,bytes_read); //cleaning buffer up each time
	}
	fprintf(stderr,"%s : method %s : uri %s : version \n",request_struct.method,request_struct.uri,request_struct.version);
}

// The main request handler func for each thread for first parsing then executing and response sending
void	request_handler(void *conn_fd) 
{
	
	http_parser(*(int *)conn_fd); // Parseing
	// Executing
	// Response formatter sending the easy peasy part
}