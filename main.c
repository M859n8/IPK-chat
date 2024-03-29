#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <sys/socket.h>

#include <unistd.h>
#include <arpa/inet.h>

#define DEFAULT_PORT 4567
#define DEFAULT_TIMEOUT 250
#define DEFAULT_RETRANSMISSIONS 3




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
                printf("Invalid transport protocol specified.\n");
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
        printf("Server address is mandatory.\n");
        return 1;
    }

    // int client_socket = socket(AF_INET, SOCK_DGRAM, 0);
    // if (client_socket <= 0)
    // {
    //     perror("ERROR: socket");
    //     exit(EXIT_FAILURE);
    // }
    // struct sockaddr_in server_address;
    // server_address.sin_family = AF_INET;
    // server_address.sin_port = htons(options.server_port);
    // server_address.sin_addr.s_addr = inet_addr(options.server_addr);

    // char input_message[1400]; //user write message
    // char response[1400];

    // bool bye_message = false;
    // while (!bye_message) {
    //     fgets(input_message, sizeof(input_message), stdin);
    //     sendto(client_socket, input_message, strlen(input_message), 0, (struct sockaddr *)&server_address, sizeof(server_address));
        
    //     recvfrom(client_socket, response, sizeof(response), 0, NULL, NULL);
        
    //     printf("Server response: %s\n", response);
    //     if (strcmp(response, "BYE") == 0) {
    //         bye_message = true;
    //         break; 
    //     }
    // }
    // close(client_socket);
    
    return 0;
}
