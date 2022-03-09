#include <stdlib.h>     
#include <stdio.h>
#include <unistd.h>     
#include <string.h>     
#include <sys/socket.h> 
#include <netinet/in.h> 
#include <netdb.h>      
#include <arpa/inet.h>
#include "helpers.h"
#include "requests.h"

char *compute_get_request(char *host, char *url, char *token,
                            char **cookies, int cookies_count)
{
    char *message = calloc(BUFLEN, sizeof(char));
    char *line = calloc(LINELEN, sizeof(char));
    sprintf(line, "GET %s HTTP/1.1", url);
    compute_message(message, line);

    sprintf(line, "Host: %s", host);
    compute_message(message, line);

    if (cookies != NULL) {
        if(strlen(cookies[0]) != 0) {
            sprintf(line, "Cookie:");
            for (int i = 0; i < cookies_count; i++) {
                strcat(line, " ");
                strcat(line, cookies[i]);
            }
            compute_message(message, line);
        }
    }

    if(token != NULL) { 
        if(strlen(token) != 0){
            sprintf(line, "Authorization: Bearer %s", token);
            compute_message(message, line);
        }
    }
    compute_message(message, "\n");
    return message;
}


char *compute_delete_request(char *host, char *url, char *token,
                            char **cookies, int cookies_count)
{
    char *message = calloc(BUFLEN, sizeof(char));
    char *line = calloc(LINELEN, sizeof(char));
    sprintf(line, "DELETE %s HTTP/1.1", url);
    compute_message(message, line);

    sprintf(line, "Host: %s", host);
    compute_message(message, line);

    if (cookies != NULL) {
        if(strlen(cookies[0]) != 0) {
            sprintf(line, "Cookie:");
            for (int i = 0; i < cookies_count; i++) {
                strcat(line, " ");
                strcat(line, cookies[i]);
            }
            compute_message(message, line);
        }
    }

    if(token != NULL) {
        if(strlen(token) != 0) {
            sprintf(line, "Authorization: Bearer %s", token);
            compute_message(message, line);
        }
    }
    compute_message(message, "\n");
    return message;
}




char *compute_post_request(char *host, char *url, char* content_type, char **body_data,
                            int body_data_fields_count, char **cookies, int cookies_count, char* token)
{
    char *message = calloc(BUFLEN, sizeof(char));
    char *line = calloc(LINELEN, sizeof(char));
    char *body_data_buffer = calloc(LINELEN, sizeof(char));

    sprintf(line, "POST %s HTTP/1.1", url);
    compute_message(message, line);
    
    sprintf(line, "Host: %s", host);
    compute_message(message, line);

    if(token != NULL) {
        if(strlen(token) != 0) {
            sprintf(line, "Authorization: Bearer %s", token);
            compute_message(message, line);
        }
    }

    strcat(body_data_buffer, "{\n   ");
    for (int i = 0; i < body_data_fields_count; i++) {
        if (i < body_data_fields_count - 1) {
            strcat(body_data_buffer, body_data[i]);  
            strcat(body_data_buffer, ",\n   ");
        }
        else {
            strcat(body_data_buffer, body_data[i]);
            strcat(body_data_buffer, "\n}");
        }
    }

    sprintf(line, "Content-Type: %s", content_type);
    compute_message(message, line);
    sprintf(line, "Content-Length: %ld", strlen(body_data_buffer));
    compute_message(message, line);

    if (cookies != NULL) {
        if(strlen(cookies[0]) != 0) {
            sprintf(line, "Cookie:");
            for (int i = 0; i < cookies_count; i++) {
                sprintf(line, " %s;", cookies[i]);
                compute_message(message, line);
            }
        }
    }
    compute_message(message, "");

    memset(line, 0, LINELEN);
    compute_message(message, body_data_buffer);

    free(line);
    return message;
}
