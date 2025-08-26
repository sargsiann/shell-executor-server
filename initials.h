#ifndef IN_H

 #define PORT 3490
 #define BUFF_SIZE 256
 #include <string.h>
 #include <arpa/inet.h>
 #include <sys/types.h>
 #include <netdb.h>
 #include <stdio.h>
 #include <unistd.h>
 #include <sys/socket.h>
 #include <fcntl.h>
 #include <stdlib.h>
 #include <stdbool.h>
 #include <pthread.h>

typedef struct sockaddr SA;

// For main thread server
typedef struct t_server {
	int	listen_fd;
}	t_server;

typedef enum  {
    PARSING_REQUEST_LINE,
    PARSING_HEADERS,
    PARSING_MULTIPART_HEADERS,
    PARSING_MULTIPART_BODY,
    PARSING_DONE,
    PARSING_ERROR
} ParserState;

typedef enum {
    PARSE_OK, // Ok
    // This for request Line
	ERR_MALFORMED_REQUEST_LINE, // wrong format of req line
    ERR_UNSUPPORTED_METHOD, // not the POST method
    ERR_UNSUPPORTED_VERSION,  // not the HTTP 1.1

    ERR_HEADERS_TOO_LARGE,
    ERR_INVALID_HEADER_FORMAT,
    ERR_INVALID_BOUNDARY,
    ERR_BODY_TOO_LARGE,
    ERR_MULTIPART_TOO_LARGE,
    ERR_UNKNOWN
} ParseResult ; 

typedef	struct File {
	char	*name;
	char	*content;
	size_t	len;
}	File;

typedef struct Body {
	char	*command;
	File	*files;
	int		files_len;
}	Body;

typedef struct Header {
	char	*name;
	char	*value;
}	Header;

typedef struct Headers {
	int		size;
	Header	*headers;
}	Headers;

typedef struct Request {
	char	*method; // POST
	char	*uri; // should be /exec
	char	*version; // Should be http 1.1
	Body	*body; // Body 
	Header	*Headers; // Headers
}	Request;



void	exit_error(const char *msg,t_server *to_free) ;
bool	is_ipv6_mapped(struct in6_addr sin6_addr) ;
void	log_client_ip_info(struct sockaddr_storage client_info) ;

#endif