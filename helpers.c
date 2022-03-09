#include <stdlib.h>     /* exit, atoi, malloc, free */
#include <stdio.h>
#include <unistd.h>     /* read, write, close */
#include <string.h>     /* memcpy, memset */
#include <sys/socket.h> /* socket, connect */
#include <netinet/in.h> /* struct sockaddr_in, struct sockaddr */
#include <netdb.h>      /* struct hostent, gethostbyname */
#include <arpa/inet.h>
#include "helpers.h"
#include "buffer.h"
#include "parson.h"

#define HEADER_TERMINATOR "\r\n\r\n"
#define HEADER_TERMINATOR_SIZE (sizeof(HEADER_TERMINATOR) - 1)
#define CONTENT_LENGTH "Content-Length: "
#define CONTENT_LENGTH_SIZE (sizeof(CONTENT_LENGTH) - 1)

void error(const char *msg)
{
    perror(msg);
    exit(0);
}

void compute_message(char *message, const char *line)
{
    strcat(message, line);
    strcat(message, "\r\n");
}

int open_connection(char *host_ip, int portno, int ip_type, int socket_type, int flag)
{
    struct sockaddr_in serv_addr;
    int sockfd = socket(ip_type, socket_type, flag);
    if (sockfd < 0)
        error("ERROR opening socket");

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = ip_type;
    serv_addr.sin_port = htons(portno);
    inet_aton(host_ip, &serv_addr.sin_addr);

    /* connect the socket */
    if (connect(sockfd, (struct sockaddr*) &serv_addr, sizeof(serv_addr)) < 0)
        error("ERROR connecting");

    return sockfd;
}

void close_connection(int sockfd)
{
    close(sockfd);
}

void send_to_server(int sockfd, char *message)
{
    int bytes, sent = 0;
    int total = strlen(message);

    do
    {
        bytes = write(sockfd, message + sent, total - sent);
        if (bytes < 0) {
            error("ERROR writing message to socket");
        }

        if (bytes == 0) {
            break;
        }

        sent += bytes;
    } while (sent < total);
}

char *receive_from_server(int sockfd)
{
    char response[BUFLEN];
    buffer buffer = buffer_init();
    int header_end = 0;
    int content_length = 0;

    do {
        int bytes = read(sockfd, response, BUFLEN);

        if (bytes < 0){
            error("ERROR reading response from socket");
        }

        if (bytes == 0) {
            break;
        }

        buffer_add(&buffer, response, (size_t) bytes);
        
        header_end = buffer_find(&buffer, HEADER_TERMINATOR, HEADER_TERMINATOR_SIZE);

        if (header_end >= 0) {
            header_end += HEADER_TERMINATOR_SIZE;
            
            int content_length_start = buffer_find_insensitive(&buffer, CONTENT_LENGTH, CONTENT_LENGTH_SIZE);
            
            if (content_length_start < 0) {
                continue;           
            }

            content_length_start += CONTENT_LENGTH_SIZE;
            content_length = strtol(buffer.data + content_length_start, NULL, 10);
            break;
        }
    } while (1);
    size_t total = content_length + (size_t) header_end;
    
    while (buffer.size < total) {
        int bytes = read(sockfd, response, BUFLEN);

        if (bytes < 0) {
            error("ERROR reading response from socket");
        }

        if (bytes == 0) {
            break;
        }

        buffer_add(&buffer, response, (size_t) bytes);
    }
    buffer_add(&buffer, "", 1);
    return buffer.data;
}

char *basic_extract_json_response(char *str)
{
    return strstr(str, "{\"");
}

void write_login(char **input) {
    char input_str[BUFLEN];
    printf("username=");
    scanf("%s", input_str);
    strcpy(input[0], "\"username\": ");
    strcat(input[0], "\"");
    strcat(input[0], input_str);
    strcat(input[0], "\"");
    printf("password=");
    scanf("%s", input_str);
    strcpy(input[1], "\"password\": ");
    strcat(input[1], "\"");
    strcat(input[1], input_str); 
    strcat(input[1], "\"");
}

void write_book(char **input) {
    char input_str[BUFLEN];
    printf("title=");
    scanf("%s", input_str);
    strcpy(input[0], "\"title\": ");
    strcat(input[0], "\"");
    strcat(input[0], input_str);
    strcat(input[0], "\"");
    printf("author=");
    scanf("%s", input_str);
    strcpy(input[1], "\"author\": ");
    strcat(input[1], "\"");
    strcat(input[1], input_str); 
    strcat(input[1], "\"");
    printf("genre=");
    scanf("%s", input_str);
    strcpy(input[2], "\"genre\": ");
    strcat(input[2], "\"");
    strcat(input[2], input_str);
    strcat(input[2], "\"");
    printf("publisher=");
    scanf("%s", input_str);
    strcpy(input[4], "\"publisher\": ");
    strcat(input[4], "\"");
    strcat(input[4], input_str);
    strcat(input[4], "\"");
    printf("page_count=");
    scanf("%s", input_str);
    strcpy(input[3], "\"page_count\": ");
    strcat(input[3], input_str); 
}

char* extract_cookie(char *response){
    char* s = NULL;
    char* q = NULL;
    s = strtok(response, "\r\n");
    while(s != NULL) {
        if(s[0] == 'H' && s[1] == 'T' && s[2] == 'T' && s[3] == 'P') {
            printf("%s\n", s);
        }
        else if(s[0] == '{') {
            JSON_Value *schema = json_parse_string(s);
            JSON_Object *element;
            element = json_value_get_object(schema);
            if(json_object_get_string(element, "error") != NULL) {
                printf("Error: %s\n\n", json_object_get_string(element, "error"));
            }
        }
        if(s[0] == 'S' && s[1] == 'e' && s[2] == 't') {
            q = strtok(s, " ");
            while(q != NULL) {
                if(q[0] == 'c') {
                    printf("Cookie: %s\n", q);
                    return q;
                }
                q = strtok (NULL, " ;");
            }
        }
        s = strtok(NULL,"\r\n");
    }
    return "";
}

const char* extract_token(char *response) {
    char* s = NULL;
    s = strtok(response, "\r\n");
    while(s != NULL) {
        if(s[0] == 'H' && s[1] == 'T' && s[2] == 'T' && s[3] == 'P') {
            printf("%s\n", s);
        }
        else if(s[0] == '{') {
            JSON_Value *schema = json_parse_string(s);
            JSON_Object *element;
            element = json_value_get_object(schema);
            if(json_object_get_string(element, "token") != NULL) {
                printf("Token: %s\n\n", json_object_get_string(element, "token"));
                return json_object_get_string(element, "token");
            }
            if(json_object_get_string(element, "error") != NULL) {
                printf("Error: %s\n\n", json_object_get_string(element, "error"));
            }
        }
        s = strtok(NULL,"\r\n");
    }
    return "";
}
void check_for_errors(char* response) {
    char* s = NULL;
    s = strtok(response, "\r\n");
    while(s != NULL) {
        if(s[0] == 'H' && s[1] == 'T' && s[2] == 'T' && s[3] == 'P') {
            printf("%s\n", s);
        }
        else if(s[0] == '{') {
            JSON_Value *schema = json_parse_string(s);
            JSON_Object *element;
            element = json_value_get_object(schema);
            if(json_object_get_string(element, "error") != NULL) {
                printf("Error: %s\n\n", json_object_get_string(element, "error"));
            }
        }
        s = strtok(NULL,"\r\n");
    }
}