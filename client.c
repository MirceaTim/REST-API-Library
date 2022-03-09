#include <stdio.h>
#include <stdlib.h>     
#include <unistd.h>     
#include <string.h>    
#include <sys/socket.h> 
#include <netinet/in.h> 
#include <netdb.h>      
#include <arpa/inet.h>
#include "helpers.h"
#include "requests.h"
#include "parson.h"

int main(int argc, char *argv[])
{
    char *message;
    char *response;
    int sockfd;
    char cookie[BUFLEN];
    char token[BUFLEN];
    //char *token;
    char **cookies = malloc(sizeof(char *));
    char data[] = "application/json";
    cookies[0] = malloc(1);

    while(1) {
        sockfd = open_connection("34.118.48.238", 8080, AF_INET, SOCK_STREAM, 0);
        char *buffer = (char*)malloc(15 * sizeof(char));
        char **input = malloc(5 * sizeof(char *));
        for(int i = 0; i <= 4; ++i) {
            input[i] = malloc(BUFLEN);
        } 
        scanf("%s", buffer);
        if(!strcmp(buffer, "register")) {
            write_login(input);
            printf("\n");
            message = compute_post_request("34.118.48.238", "/api/v1/tema/auth/register", data, input, 2, NULL, 0, NULL);
            printf("SENT TO SERVER:\n");
            puts(message);
            send_to_server(sockfd, message);
            response = receive_from_server(sockfd);
            printf("RECEIVED FROM SERVER:\n");
            check_for_errors(response);
        }
        else if(!strcmp(buffer, "login")) {
            write_login(input);
            printf("\n");
            message = compute_post_request("34.118.48.238", "/api/v1/tema/auth/login", data, input, 2, NULL, 0, NULL);
            printf("SENT TO SERVER:\n");
            puts(message);
            send_to_server(sockfd, message);
            response = receive_from_server(sockfd);
            printf("RECEIVED FROM SERVER:\n");
            strcpy(cookie, extract_cookie(response));
            cookies[0] = malloc(strlen(cookie) + 1);
            strcpy(cookies[0], cookie);
        }   
        else if (!strcmp(buffer, "enter_library")) {
            printf("\n");
            message = compute_get_request("34.118.48.238", "/api/v1/tema/library/access", NULL, cookies, 1);
            printf("SENT TO SERVER:\n");
            puts(message);
            send_to_server(sockfd, message);
            response = receive_from_server(sockfd);
            printf("RECEIVED FROM SERVER:\n");
            strcpy(token, extract_token(response));
        }
        else if (!strcmp(buffer, "get_books")) {
            printf("\n");
            message = compute_get_request("34.118.48.238", "/api/v1/tema/library/books", token, cookies, 1);
            printf("SENT TO SERVER:\n");
            puts(message);
            send_to_server(sockfd, message);
            response = receive_from_server(sockfd);
            printf("RECEIVED FROM SERVER:\n");
            char* s;
            s = strtok(response, "\n");
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
                if(s[0] == '[') {
                    JSON_Value *schema = json_parse_string(s);
                    JSON_Array *lista;
                    JSON_Object *element;
                    lista = json_value_get_array(schema);
                    for(int i = 0; i < json_array_get_count(lista); ++i) {
                        element = json_array_get_object(lista, i);
                        printf("ID: %d\nTitle: %s\n\n", 
                            (int)json_object_get_number(element, "id"),
                            json_object_get_string(element, "title"));
                    }
                }
                s = strtok(NULL, "\n");
            }
        }
        else if (!strcmp(buffer, "get_book")) {
            int id2;
            printf("id=");
            scanf("%d", &id2);
            printf("\n");
            char id[100];
            sprintf(id, "%d", id2);
            char locatie[200];
            strcpy(locatie, "/api/v1/tema/library/books/");
            strcat(locatie, id);
            message = compute_get_request("34.118.48.238", locatie, token, cookies, 1);
            printf("SENT TO SERVER:\n");
            puts(message);
            send_to_server(sockfd, message);
            response = receive_from_server(sockfd);
            printf("RECEIVED FROM SERVER:\n");

            char* s1;
            s1 = strtok(response, "\n");
            while(s1 != NULL) {
                if(s1[0] == 'H' && s1[1] == 'T' && s1[2] == 'T' && s1[3] == 'P') {
                    printf("%s\n", s1);
                }
                else if(s1[0] == '{') {
                    JSON_Value *schema = json_parse_string(s1);
                    JSON_Object *element;
                    element = json_value_get_object(schema);
                    if(json_object_get_string(element, "error") != NULL) {
                        printf("Error: %s\n\n", json_object_get_string(element, "error"));
                    }
                }
                else if(s1[0] == '[') {
                    JSON_Value *schema1 = json_parse_string(s1);
                    JSON_Array *lista1;
                    JSON_Object *element1;
                    lista1 = json_value_get_array(schema1);
                    for(int j = 0; j < json_array_get_count(lista1); ++j) {
                        element1 = json_array_get_object(lista1, j);
                        printf("ID: %d\n", id2);
                        printf("Title: %s\nAuthor: %s\nGenre: %s\nPage count: %d\nPublisher: %s\n\n", 
                            json_object_get_string(element1, "title"),
                            json_object_get_string(element1, "author"),
                            json_object_get_string(element1, "genre"),
                            (int)json_object_get_number(element1, "page_count"),
                            json_object_get_string(element1, "publisher"));
                    }
                }
                s1 = strtok(NULL, "\n");
            }
        }
        else if(!strcmp(buffer, "add_book")) {
            write_book(input);
            printf("\n");
            message = compute_post_request("34.118.48.238", "/api/v1/tema/library/books", data, input, 5, NULL, 0, token);
            printf("SENT TO SERVER:\n");
            puts(message);
            send_to_server(sockfd, message);
            response = receive_from_server(sockfd);
            printf("RECEIVED FROM SERVER:\n");
            check_for_errors(response);
        }
        else if (!strcmp(buffer, "delete_book")) {
            int id2;
            printf("id=");
            scanf("%d", &id2);
            printf("\n");
            char id3[100];
            sprintf(id3, "%d", id2);
            char locatie[200];
            strcpy(locatie, "/api/v1/tema/library/books/");
            strcat(locatie, id3);
            message = compute_delete_request("34.118.48.238", locatie, token, cookies, 1);
            printf("SENT TO SERVER:\n");
            puts(message);
            send_to_server(sockfd, message);
            response = receive_from_server(sockfd);
            printf("RECEIVED FROM SERVER:\n");
            check_for_errors(response);
        }
        if(!strcmp(buffer, "logout")) {
            printf("\n");
            message = compute_get_request("34.118.48.238", "/api/v1/tema/auth/logout", NULL, cookies, 1);
            printf("SENT TO SERVER:\n");
            puts(message);
            send_to_server(sockfd, message);
            response = receive_from_server(sockfd);
            printf("RECEIVED FROM SERVER:\n");
            check_for_errors(response);
            //free(cookies[0]);
            memset(cookies[0], 0, BUFLEN);
            memset(token, 0, BUFLEN);
            //token[0] = '\n';
        }
        else if(!strcmp(buffer, "exit")) {
            close_connection(sockfd);
            break;
        }
        
        for(int i = 0; i <= 4; ++i) {
            free(input[i]);
        } 
        free(input);
        free(buffer);
        close_connection(sockfd);
    }
    return 0;
}
