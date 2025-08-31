#include "initials.h"


ParserState	parse_first_line(char	*section, Request *request_struct) // the first request line parseing
{
	// Initing NULL for further handling
	request_struct->method = NULL;
	request_struct->uri = NULL;
	request_struct->version = NULL;

	// Exctracting the method
	if (strncmp(section,"GET ",4) == 0)
		request_struct->method = strdup("GET");
	else if (strncmp(section,"POST ",5) == 0)
		request_struct->method = strdup("POST");
	else {
		request_struct->err = ERR_UNSUPPORTED_METHOD; // else we got unsupported method for our server
		return PARSING_ERROR; // breaking parsing process
	}
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

	// moveing front
	section = strchr(section,' ') + 1;
	if (strncmp(section,"HTTP/1.1\r\n",10) == 0 || strncmp(section,"HTTP/1.0\r\n",10) == 0) // checking the version
		request_struct->version = strdup("HTTP/1.1");
	else {
		request_struct->err = ERR_UNSUPPORTED_VERSION; // else we got unsupported version for our server
		return PARSING_ERROR; // breaking parsing process
	}
	return PARSING_HEADERS; // first line parsing done ! continueing to headers
}

bool		has_same_key(Dictionary headers[], char *name, short count) 
{

}

bool		handle_header_pair(char *line_s, char *delim, char *end, Dictionary headers[], short *index) 
{
	// line_s -> to delimiter is the key delim + [spaces] values ->end is the value
	if (strncmp(line_s,"Host",4) == 0)
	{
		if (first_no_space(line_s + 4) != delim) // if after spaces no delim no standard header return error
			return false;
		
		headers[*index].name = strdup("Host"); // just name
		headers[*index].value = substr(delim + 1, first_no_space(delim + 1), end); // extracting the value of host
		(*index)++;
		return true;
	}
	if(strncmp(line_s,"Content-Length",14) == 0)
	{
		if (first_no_space(line_s + 14) != delim) // if after spaces no delim no standard header return error
			return false;
		headers[*index].name = strdup("Content-Length"); // just name
		headers[*index].value = substr(delim + 1, first_no_space(delim + 1), end); // extracting the value of content length
		(*index)++;
		return true;
	}
	if(strncmp(line_s,"Transfer-Encoding",17) == 0)
	{
		if (first_no_space(line_s + 17) != delim) // if after spaces no delim no standard header return error
			return false;
		headers[*index].name = strdup("Transfer-Encoding"); // just name
		headers[*index].value = substr(delim + 1, first_no_space(delim + 1), end); // extracting the value of content length
		(*index)++;
		return true;
	}
	if(strncmp(line_s,"Content-Type",12) == 0) {
		if (first_no_space(line_s + 12) != delim) // if after spaces no delim no standard header return error
			return false;
		headers[*index].name = strdup("Content-Type"); // just name
		headers[*index].value = substr(delim + 1, first_no_space(delim + 1), end); // extracting the value of content type
		(*index)++;
		return true;
	}
	if (strncmp(line_s,"User-Agent",10) == 0) {
		if (first_no_space(line_s + 10) != delim) // if after spaces no delim no standard header return error
			return false;
		headers[*index].name = strdup("User-Agent"); // just name
		headers[*index].value = substr(delim + 1, first_no_space(delim + 1), end); // extracting the value of user agent
		(*index)++;
		return true;
	}
	if(strncmp(line_s,"Connection",10) == 0) {
		if (first_no_space(line_s + 10) != delim) // if after spaces no delim no standard header return error
			return false;
		headers[*index].name = strdup("Connection"); // just name
		headers[*index].value = substr(delim + 1, first_no_space(delim + 1), end); // extracting the value of connection
		(*index)++;
		return true;
	}
}

// Function for just validating and parsing line
ParserState	parse_header_line(char *line_start, Request *request_struct, short *index) 
{
	char	*line_end = strstr(line_start, "\r\n"); //getting memory addres of line end
	char	*delimiter = memchr(line_start,':',line_end - line_start); // finding the : deliniter

	if (!delimiter) // if we have no : in our line of header its invalid
		request_struct->err = ERR_INVALID_HEADER_FORMAT;
	handle_header_pair(line_start, delimiter, line_end, request_struct->Headers, index);
}

// Headers important 1Host : for all types , 2Content-Lenght or 2Transfer encoding, 3Content type cant be two times for POST 
// NOT IMPORTANT LIKE 4USER-AGENT(NULL) can be two times 5Connection(default will not close) can be two times
ParserState	parse_headers(char *section, Request *request_struct) 
{
	short	index = 0; //index of array of headers

	fprintf(stderr,"PARSING headers %s\n",section);
	while (1)
	{
		section = strstr(section,"\r\n");
		if (strncmp(section,"\r\n\r\n",4) == 0) //weve got to the end of headers section
			break;
		parse_header_line(section + 2,request_struct,&index);
		section += 2;
	}
	request_struct->headers_count = index; // filling the count of headers
	return PARSING_BODY; // every thing ok going to body parsing
}

void	free_headers(int count, Dictionary headers[]) // function for freeing headers
{
	// count is size of headers array that was filled
	for (int i = 0; i < count; i++) {
		if (headers[i].name != NULL) {
			free(headers[i].name);
			headers[i].name = NULL;
		}
		if (headers[i].value != NULL) {
			free(headers[i].value);
			headers[i].value = NULL;
		}
	}
}


void	free_parser(char	*section, Request *request_struct, ParserState state) // is used in error case SO IF WE FREED PARSER IN PARSING STATE THAT MEANS WE GOT ERROR
{
	if (section != NULL) // freeing remained memory of section char *
		free(section);
	short i = 0;
	while (i <= state) // freeing all headers
	{
		if (i == PARSING_REQUEST_LINE) //if we are on request line
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
		if (i == PARSING_HEADERS)
			free_headers(request_struct->headers_count, request_struct->Headers);
		i++;
	}
}


ParserState	parse_section_by_state(char	*section, char *end, Request *request_struct, ParserState state) 
{
	if (state == PARSING_REQUEST_LINE) // the first line parseing
		return parse_first_line(section,request_struct); // in first line we know that we dont need end just go that \r\n
	if (state == PARSING_HEADERS) // the headers parsing
		return parse_headers(section, request_struct); 
}

char	*section_end_ptr(char *section,ParserState state) 
{
	if (state == PARSING_REQUEST_LINE) // the first line before headers ends with \r\n
		return strstr(section,"\r\n");
	return strstr(section,"\r\n"); // else all ends with \r\n\r\n
}

char	*change_section_by_state(char *section, ParserState state) //extracting remained part FE removing headers part if done 
{
	char *res = NULL;
	if (state == PARSING_REQUEST_LINE) // the first line before headers ends with \r\n
		res = substr(section, strstr(section,"\r\n"), section + strlen(section));	
	if (state == PARSING_HEADERS) // the headers end is \r\n\r\n
		res = substr(section, strstr(section,"\r\n\r\n"), section + strlen(section));
	free(section); // freeing the previous section memory address
	return res;
}

void	print_headers(short size, Dictionary headers[]) 
{
	for (short i = 0; i < size; i++)
	{
		if (headers[i].name != NULL && headers[i].value != NULL)
			fprintf(stderr,"Header %d: %s: %s\n", i, headers[i].name, headers[i].value);
	}
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
			if (state == PARSING_BODY)
				break; // just breaking for this moment
			fprintf(stderr,"-----------------\n");
		}
		if (state == PARSING_BODY)
			break;
		if (state == PARSING_ERROR)
			break;
		memset(buff,0,bytes_read); //cleaning buffer up each time
	}
	fprintf(stderr,"%s : method %s : uri %s : version \n",request_struct.method,request_struct.uri,request_struct.version);
	fprintf(stderr,"Headers count: %d\n", request_struct.headers_count);
	print_headers(request_struct.headers_count, request_struct.Headers);
}

// The main request handler func for each thread for first parsing then executing and response sending
void	request_handler(void *conn_fd) 
{
	
	http_parser(*(int *)conn_fd); // Parseing
	// Executing
	// Response formatter sending the easy peasy part
}