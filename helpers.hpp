#ifndef _HELPERS_
#define _HELPERS_

#define BUFLEN 4096
#define LINELEN 1000

#define REGISTER_ROUTE "/api/v1/tema/auth/register"
#define LOGIN_ROUTE "/api/v1/tema/auth/login"
#define LIBRARY_ACCESS_ROUTE "/api/v1/tema/library/access"
#define LOGOUT_ROUTE "/api/v1/tema/auth/logout"
#define LIBRARY_BOOKS_ROUTE "/api/v1/tema/library/books"

#define USERNAME "username"
#define PASSWORD "password"
#define ID "id"
#define TITLE "title"
#define AUTHOR "author"
#define PUBLISHER "publisher"
#define GENRE "genre"
#define PAGE_COUNT "page_count"

#include <string>

// shows the current error
void error(const char *msg);

// adds a line to a string message
void compute_message(std::string &message, std::string line);

// opens a connection with server host_ip on port portno, returns a socket
int open_connection(const char *host_ip, int portno, int ip_type, int socket_type, int flag);

// closes a server connection on socket sockfd
void close_connection(int sockfd);

// send a message to a server
void send_to_server(int sockfd, std::string message);

// receives and returns the message from a server
std::string receive_from_server(int sockfd);

// extracts and returns a JSON from a server response
char *basic_extract_json_response(char *str);

#endif
