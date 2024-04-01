#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <sys/socket.h>
#include "tcp.h"
#include <errno.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>
#include <sys/types.h>
#include <signal.h>

#define DEFAULT_PORT 4567
#define DEFAULT_TIMEOUT 250
#define DEFAULT_RETRANSMISSIONS 3

#define BUFFER_SIZE 2048
#define DNAME_SIZE 20



typedef struct {
    char transport_prtc;
    char* server_addr;
    uint16_t server_port;
    uint16_t timeout;
    uint8_t retransmissions;
} Arguments;

void help_output() {
    printf("Usage: program_name -t <tcp/udp> -s <server_address> -p <server_port> [-d <timeout>] [-r <max_retransmissions>] [-h]\n");
}

int parse_arguments(int argc, char *argv[], Arguments *options) {
    
    for (int i = 1; i < argc; i += 2) {
        if (strcmp(argv[i], "-t") == 0) {
            if (strcmp(argv[i + 1], "tcp") == 0) {
                options->transport_prtc = atoi(argv[i + 1]); 
            } else if (strcmp(argv[i + 1], "udp") == 0) {
                options->transport_prtc = atoi(argv[i + 1]); 
            } else {
                perror("ERR: Invalid transport protocol specified.\n");
                return -1;
            }
        } else if (strcmp(argv[i], "-s") == 0) {
            options->server_addr = argv[i + 1];
        } else if (strcmp(argv[i], "-p") == 0) {
            options->server_port = atoi(argv[i + 1]);
        } else if (strcmp(argv[i], "-d") == 0) {
            options->timeout = atoi(argv[i + 1]);
        } else if (strcmp(argv[i], "-r") == 0) {
            options->retransmissions = atoi(argv[i + 1]);
        } else if (strcmp(argv[i], "-h") == 0) {
            help_output();
            exit(0);
        } else {
            exit(1);
        }
    }
    return 0;
}

char *get_host_by_name(char *hostname) {

    struct addrinfo hints, *result, *p;
    char ip_address[INET_ADDRSTRLEN]; 

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET; 
    hints.ai_socktype = SOCK_STREAM;

    if (getaddrinfo(hostname, NULL, &hints, &result) != 0) {
        fprintf(stderr, "ERR: can't get ip.\n");
        return NULL;
    }

    for (p = result; p != NULL; p = p->ai_next) {
        if (p->ai_family == AF_INET) { 
            struct sockaddr_in *ipv4 = (struct sockaddr_in *)p->ai_addr;
            inet_ntop(AF_INET, &(ipv4->sin_addr), ip_address, INET_ADDRSTRLEN);
            break; // Отримали першу IPv4 адресу
        }
    }

    freeaddrinfo(result);
    return strdup(ip_address);
}

int helper_value;
void sigint_handler() {
    char send_message[BUFFER_SIZE];
    sprintf(send_message, "BYE\r\n");
    send(helper_value, send_message, strlen(send_message), 0);
    close(helper_value);
    exit(0); 
}

int main(int argc, char *argv[]) {

    Arguments options;
    options.transport_prtc = '\0'; 
    options.server_addr = NULL;
    options.server_port = DEFAULT_PORT;
    options.timeout = DEFAULT_TIMEOUT;
    options.retransmissions = DEFAULT_RETRANSMISSIONS;

    // Parse command line arguments
    if (parse_arguments(argc, argv, &options) == -1) {
        help_output();
        return 1;
    }

    // Validate mandatory argument
    if (options.server_addr == NULL) {
        perror("ERR: Server address is mandatory.\n");
        return 1;
    }
    options.server_addr = get_host_by_name(options.server_addr);


    int client_socket;
    struct sockaddr_in server_addr;
    char input_message[BUFFER_SIZE]; //user write message
    char send_message[BUFFER_SIZE];
    char response_message[BUFFER_SIZE];
    //char output_message[BUFFER_SIZE];
    char error_message[BUFFER_SIZE];
    char displayname[DNAME_SIZE];

   
     // Створення сокету
    if ((client_socket= socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("ERR: Socket creation error\n");
        exit(EXIT_FAILURE);
    }

    // Ініціалізація адреси сервера
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(options.server_port);
    if (inet_pton(AF_INET, options.server_addr, &server_addr.sin_addr) <= 0) {
        perror("ERR: Invalid address / Address not supported\n");
        exit(EXIT_FAILURE);
    }

  


    // Підключення до сервера
    if (connect(client_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("ERR: Connection failed\n");
        exit(EXIT_FAILURE);
    }

    helper_value=client_socket;
    // bool bye_message = false;
    // while (!bye_message) {
        //fgets(input_message, sizeof(input_message), stdin);

        bool auth = false;
        bool open = true;
        
        //auth state
        while(!auth ){
            fgets(input_message, sizeof(input_message), stdin);
            signal(SIGINT, sigint_handler);
            if(!process_help(input_message) && !process_rename(input_message, displayname) ){
                if(process_auth(input_message, send_message, displayname)){
                    send(client_socket, send_message, strlen(send_message), 0);
                    recv(client_socket, response_message, BUFFER_SIZE, 0);
                    if(income_err(response_message, error_message) || income_bye(response_message)){
                        auth = true; //can exit auth state
                        open = false; //can not go to open state
                        sprintf(send_message, "BYE\r\n");
                        send(client_socket, send_message, strlen(send_message), 0);
                        
                    }else if(income_replye(response_message, error_message)){
                        auth = true;
                    }else if(income_msg(response_message)){
                        auth = false;
                    }else{
                        auth = false;
                        //fgets(input_message, sizeof(input_message), stdin);
                    }
                }else{
                    fprintf(stderr, "ERR: authentification required\n");
                }
            }
            // else{
                // fgets(input_message, sizeof(input_message), stdin);
            // }
        }

        //open state
        while(open){
            fgets(input_message, sizeof(input_message), stdin);
            signal(SIGINT, sigint_handler);
            if(process_help(input_message) || process_rename(input_message, displayname) ){
                //fgets(input_message, sizeof(input_message), stdin);
                
            }else if( process_join(input_message, send_message, displayname)){
                send(client_socket, send_message, strlen(send_message), 0);
                recv(client_socket, response_message, BUFFER_SIZE, 0);

                if(income_err(response_message, error_message) || income_bye(response_message)){
                    open = false; //can not go to open state
                    sprintf(send_message, "BYE\r\n");
                    send(client_socket, send_message, strlen(send_message), 0);
                    
                }
                else if(income_msg(response_message)){
                    open = true;
                    // printf("Response from server: %s", output_message);
                }else{
                    income_replye(response_message, error_message);
                    open = true;
                }
            }else if(process_auth(input_message, send_message, displayname)){

                fprintf(stderr, "ERR: duplicit authentification.\n");
                open=false;
            }else{
                process_message(input_message, send_message, displayname);
                send(client_socket, send_message, strlen(send_message), 0);
                recv(client_socket, response_message, BUFFER_SIZE, 0);

                if(income_err(response_message, error_message) || income_bye(response_message)){
                    open = false; 
                    sprintf(send_message, "BYE\r\n");
                    send(client_socket, send_message, strlen(send_message), 0);
                    
                }
                else if(income_msg(response_message)){
                    open = true;;
                    // printf("Response from server: %s", output_message);
                }else{
                    income_replye(response_message, error_message);
                    open = true;;
                }
            }

        }

        // income_msg(response_message, output_message);
        //income_err(response_message, error_message);
        // printf("strlen %d\n", strlen(output_message) );

        // if(strlen(output_message) != 2){
        //     printf("Response from server: %s", output_message);
        // }
        
    // }
    close(client_socket);
    return 0;
}