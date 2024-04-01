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


//struct for input arguments
typedef struct {
    char transport_prtc;
    char* server_addr;
    uint16_t server_port;
    uint16_t timeout;
    uint8_t retransmissions;
} Arguments;

//for help output
void help_output() {
    printf("Usage: program_name -t <tcp/udp> -s <server_address> -p <server_port> [-d <timeout>] [-r <max_retransmissions>] [-h]\n");
}
//parsing arguments of program. set values in arguments structure
int parse_arguments(int argc, char *argv[], Arguments *options) {
    for (int i = 1; i < argc; i += 2) {
        if (strcmp(argv[i], "-t") == 0) {
            if (strcmp(argv[i + 1], "tcp") == 0) {
                options->transport_prtc = atoi(argv[i + 1]); 
            } else if (strcmp(argv[i + 1], "udp") == 0) {
                options->transport_prtc = atoi(argv[i + 1]); 
                exit(0); //udp was not implemented
            } else {
                fprintf(stderr, "ERR: Invalid transport protocol specified.\n");
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
            fprintf(stderr, "ERR: Invalid argument type.\n");
            //wrong type of argument
            exit(1);
        }
    }
    return 0;
}

//function that calculate ip address by name
char *get_host_by_name(char *hostname) {
    //helping strucutres
    struct addrinfo hints, *result, *p;
    //max size for IPv4
    char ip_address[INET_ADDRSTRLEN]; 
    //set memory for structure hints
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET; //IPv4
    hints.ai_socktype = SOCK_STREAM;

    if (getaddrinfo(hostname, NULL, &hints, &result) != 0) {
        fprintf(stderr, "ERR: can't get ip.\n");
        return NULL;
    }
    //go through the list of addresses
    for (p = result; p != NULL; p = p->ai_next) {
        if (p->ai_family == AF_INET) { 
            struct sockaddr_in *ipv4 = (struct sockaddr_in *)p->ai_addr;
            //convert network address to string
            inet_ntop(AF_INET, &(ipv4->sin_addr), ip_address, INET_ADDRSTRLEN);
            break; //get first IPv4 adress -> break
        }
    }

    freeaddrinfo(result);
    return strdup(ip_address);
}

//function that implements reaction to cntrl+c
int helper_value;
void sigint_handler() {
    char send_message[BUFFER_SIZE];
    sprintf(send_message, "BYE\r\n");
    //send bye to the server
    send(helper_value, send_message, strlen(send_message), 0);
    close(helper_value);
    exit(0); 
}

int main(int argc, char *argv[]) {
    //set the default values for program arguments
    Arguments options;
    options.transport_prtc = '\0'; 
    options.server_addr = NULL;
    options.server_port = DEFAULT_PORT;
    options.timeout = DEFAULT_TIMEOUT;
    options.retransmissions = DEFAULT_RETRANSMISSIONS;

    //parse command line arguments
    if (parse_arguments(argc, argv, &options) == -1) {
        help_output();
        return 1;
    }

    //check mandatory argument
    if (options.server_addr == NULL) {
        fprintf(stderr, "ERR: Server address is mandatory.\n");
        return 1;
    }
    //calculate server ip address
    options.server_addr = get_host_by_name(options.server_addr);

    int client_socket;
    struct sockaddr_in server_addr;
    char input_message[BUFFER_SIZE];//user input
    char send_message[BUFFER_SIZE];//helper array for send message
    char response_message[BUFFER_SIZE];//server response
    char error_message[BUFFER_SIZE];//helper array for error message
    char displayname[DNAME_SIZE];//array for dname


    //create socket, check if it is ok
    if ((client_socket= socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        fprintf(stderr, "ERR: Socket creation error\n");
        exit(EXIT_FAILURE);
    }

    //initialise server addr
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(options.server_port);
    if (inet_pton(AF_INET, options.server_addr, &server_addr.sin_addr) <= 0) {
        fprintf(stderr, "ERR: Invalid address / Address not supported\n");
        exit(EXIT_FAILURE);
    }

    //connect to the server
    if (connect(client_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        fprintf(stderr, "ERR: Connection failed\n");
        exit(EXIT_FAILURE);
    }
    //this value will help close the socket if user use "cntrl+c"
    helper_value=client_socket;
    //variables which provide a transition between states
    bool auth = false;
    bool open = true;
    
    //auth state
    while(!auth ){
        //read user input
        fgets(input_message, sizeof(input_message), stdin);
        //if it is cntr+c signal
        signal(SIGINT, sigint_handler);
        if(!process_help(input_message) && !process_rename(input_message, displayname) ){
            //process authentification if user don't call "local" commands
            if(process_auth(input_message, send_message, displayname)){
                //send authentification
                send(client_socket, send_message, strlen(send_message), 0);
                //receive response and divide it by '\r\n'
                int bytes_recieved = recv(client_socket, response_message, BUFFER_SIZE, 0);
                //array for singl command
                char command[BUFFER_SIZE];
                //fariable for the start byte
                int start = 0;
                //go through bytes
                for(int i=0; i<bytes_recieved; i++){
                    //find end of the command
                    if (response_message[i] == '\n') {
                        //copie command from start byte to '\n'
                        memcpy(command, response_message + start, i-start+1);
                        command[i-start+2]='\0';
                        //set the start on the beginning of the new command
                        start = i+1;
                        //process all possible types of an income message
                        if(income_err(command, error_message) || income_bye(command)){
                            auth = true; //can exit auth state
                            open = false; //can not go to open state
                            //send bye to the server, and end program
                            sprintf(send_message, "BYE\r\n");
                            send(client_socket, send_message, strlen(send_message), 0);
                            
                        }else if(income_replye(command, error_message)){
                            //success -> go to state open
                            auth = true;
                        }else if(income_msg(command)){
                            //processing state auth untill we get success reply
                            auth = false;
                        }else{
                            //for failure reply
                            //processing state auth untill we get success reply
                            auth = false;
                        }
                    }
                }
            }else{
                //if user input contains other message something but auth
                fprintf(stderr, "ERR: authentification required\n");
                auth = true; //can exit auth state
                open = false; //can not go to open state
            }
        }
    }

    //state open
    while(open){
        //read user input
        fgets(input_message, sizeof(input_message), stdin);
        //for cntrl+c processing
        signal(SIGINT, sigint_handler);
        //processing "local" commands
        if(process_help(input_message) || process_rename(input_message, displayname) ){
            
        }else if( process_join(input_message, send_message, displayname)){
            //process join command
            send(client_socket, send_message, strlen(send_message), 0);
            //split recieved message to the differen commands
            //(if there are several commands in one receive)
            int bytes_recieved = recv(client_socket, response_message, BUFFER_SIZE, 0);
            char command[BUFFER_SIZE];
            int start = 0;
            for(int i=0; i<bytes_recieved; i++){
                if (response_message[i] == '\n') {
                    memcpy(command, response_message + start, i-start+1);
                    command[i-start+2]='\0';
                    start = i+1;

                    //process all types of server response
                    if(income_err(command, error_message) || income_bye(command)){
                        open = false; //can not go to the open state again
                        //send bye to the server
                        //end program
                        sprintf(send_message, "BYE\r\n");
                        send(client_socket, send_message, strlen(send_message), 0);
                        
                    }
                    else if(income_msg(command)){
                        open = true;
                    }else{
                        income_replye(command, error_message);
                        open = true;
                    }
                }
            }
        //prosessing auth in open state lead to the duplicit auth error
        }else if(process_auth(input_message, send_message, displayname)){

            fprintf(stderr, "ERR: duplicit authentification.\n");
            open=false;//exit the open state, end the program
        }else{
            //user input is message
            process_message(input_message, send_message, displayname);
            send(client_socket, send_message, strlen(send_message), 0);
            //process received message
            int bytes_recieved = recv(client_socket, response_message, BUFFER_SIZE, 0);
            char command[BUFFER_SIZE];
            int start = 0;
            for(int i=0; i<bytes_recieved; i++){
                if (response_message[i] == '\n') {
                    memcpy(command, response_message + start, i-start+1);
                    command[i-start+2]='\0';
                    start = i+1;
                    //process all types of an income message
                    if(income_err(command, error_message) || income_bye(command)){
                        open = false; 
                        sprintf(send_message, "BYE\r\n");
                        send(client_socket, send_message, strlen(send_message), 0);
                        
                    }
                    else if(income_msg(command)){
                        open = true;;
                    }else{
                        
                        income_replye(command, error_message);
                        open = true;;
                    }
                }
            }
        }
    }
    //close socket and end program
    close(client_socket);
    return 0;
}