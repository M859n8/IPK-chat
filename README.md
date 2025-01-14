# IPK24-CHAT
This project's task was to implement a client application, which is able to communicate with a remote server using the IPK24-CHAT protocol. I implemented only the TCP variant.

## Content
1. [Program arguments](#1)
2. [Client behaviour](#2)
3. [Client output](#3)
4. [Implementation](#4)
5. [Testing](#5)
6. [Resourses](#6)


## <a id="1"></a> Program arguments
Program support this types of arguments:
- `-t` : transport protocol (tcp/udp).
- `-s` : server IP or hostname.
- `-p` : server port (default value 4567).
- `-d` : UDP confirmation timeout (default value 250).
- `-r` : maximum number of UDP retransmissions (default value 3).
- `-h` : prints program help output and exits.
Transport protocol and server IP are mandatory arguments.

## <a id="2"></a> Client behaviour
Client writes commands in `stdin` and receives a response from server to the `stdin` or `stderr`.
- `/auth {Username} {Secret} {DisplayName}` Sends AUTH message with the provided data.
- `/join {ChannelID}` Sends JOIN message with channel name.
- `/rename {DisplayName}` Locally changes the display name of the user.
- `/help` Prints out supported local commands with their parameters and a description.
Order of the command parameters must stay stay the same as shown.

## <a id="3"></a> Client output
- Incoming message `{DisplayName}: {MessageContent}\n`.
- Incoming error `ERR FROM {DisplayName}: {MessageContent}\n`.
- Internal client aplication error `ERR: {MessageContent}\n`.
- Incoming reply `Success\Failure: {MessageContent}\n`.

## <a id="4"></a> Implementation
The program is written in the C programming language. The source code is located in the files main.c, tcp.c, and tcp.h. By using the `make` command in the root folder, a binary file named `ipk24chat-client` will be created, which can be executed. The main implementation of the protocol behavior (parsing arguments, creating a socket, connecting to the server, communicating with the server, transitioning between different program states) is implemented in the `main.c` file. Auxiliary functions (converting user-entered commands into the format required by the server and vice versa) are implemented in the `tcp.c` and `tcp.h` files.
The function `sigint_handler()` was implemented to recognize the `Ctrl+C` signal and perform the proper termination of all processes and exit the program. The function `get_host_by_name(char *hostname)` was written to handle the hostname argument and convert it into an IP address.

## <a id="5"></a> Testing
The testing was conducted manually using the command `nc -4 -c -l -v 127.0.0.1 4567`. I utilized the provided examples of system and user behavior.

Basic test. User.
![Basic test. User](img/image1.png)
Basic test. Server.
![Basic test. Server](img/image.png)

Tested authentification errors: no auth, multiple auth. Program correctly identify this errors and ends behavior. User.
![Tested authentification errors: no auth, multiple auth. Program correctly identify this errors and ends behavior. User](img/image-1.png)

Tested rename command and message command. Program change displayname and correctly hamdle income and outcome messages. User.
![Tested rename command and message command. Program change displayname and correctly hamdle income and outcome messages. User](img/image-2.png)
Rename test. Server.
![Rename test. Server](img/image-3.png)

Tested error command. Program process income error message, send bye to server and correctly ends behavior. User.
![Tested error command. Program process income error message, send bye to server and correctly ends behavior. User](img/image-4.png)
Server.
![ Server](img/image-5.png)

The program can receive multiple receives between sends, but it is impossible to test this on a local server.

## <a id="6"></a> Sources
1. [Task](https://git.fit.vutbr.cz/NESFIT/IPK-Projects-2024/src/branch/master/Project%201#program-execution)
2. [Lectures](https://moodle.vut.cz/pluginfile.php/823898/mod_folder/content/0/IPK2023-24L-04-PROGRAMOVANI.pdf)
3. [Guide to Network Programming](https://beej.us/guide/bgnet/html/)