/**
 * @file net_prisoner_client.c
 * @author Wolodia Zdetovetzky
 * @brief 
 * @version 0.1
 * @date 2021-11-26
 * 
 * @copyright Copyright (c) 2021
 * 
 */

#include "net_prisoner_client.h"

/**
 * @brief Set as global var
 */
int net_client_sockfd;


void (*_net_client_func_waiting_screen)();
void (*_net_client_func_round_end)(bool, int);


void *net_client_set_func_waiting_screen(void (*f)()) 
{
    _net_client_func_waiting_screen = f;
}


void _net_client_event(char * buffer_in, int length) {
    if (strcmp(buffer_in, "W")) {
        *_net_client_func_waiting_screen;
    }
}

/**
 * @brief read and display received messages
 * @param ptr the net_client_sockfd
 * @return void* 
 */
void *_net_client_threadProcess(void *ptr)
{
    char buffer_in[BUFFERSIZE];
    net_client_sockfd = *((int *)ptr);
    int len;
    while ((len = read(net_client_sockfd, buffer_in, BUFFERSIZE)) != 0)
    {
        if (strncmp(buffer_in, "exit", 4) == 0)
        {
            break;
        }
        _net_common_dbg("client %d receive %d chars\n", net_client_sockfd, len);
        _net_common_dbg("%.*s\n", len, buffer_in);
        _net_client_event(buffer_in, len);
    }
    close(net_client_sockfd);
    _net_common_dbg("client pthread ended, len=%d\n", len);
}

#pragma endregion Common

// ----------------------------------------------
//                     Client
// ----------------------------------------------
#pragma region Client

// Globals vars
int net_client_sockfd;

/**
 * @brief open the connexion with the server
 * @param addrServer server address IP
 * @param port server port
 */
void net_client_init(char *addrServer, int port)
{
    struct sockaddr_in serverAddr;
    pthread_t thread;

    // Create the socket.
    net_client_sockfd = socket(AF_INET, SOCK_STREAM, 0);

    // Configure settings of the server address
    // Address family is Internet
    serverAddr.sin_family = AF_INET;

    //Set port number, using htons function
    serverAddr.sin_port = htons(port);

    //Set IP address to localhost
    serverAddr.sin_addr.s_addr = inet_addr(addrServer);

    memset(serverAddr.sin_zero, '\0', sizeof serverAddr.sin_zero);

    //Connect the socket to the server using the address
    if (connect(net_client_sockfd, (struct sockaddr *) &serverAddr, sizeof (serverAddr)) != 0) {
        _net_common_dbg("Fail to connect to server");
        exit(-1);
    };

    // reading pthread creation
    pthread_create(&thread, 0, _net_client_threadProcess, &net_client_sockfd);
    pthread_detach(thread);
}

/**
 * @brief The client want to betray the other player
 */
void net_client_betray() 
{
    _net_common_dbg("%d want to betray", net_client_sockfd);
    write(net_client_sockfd, "B", 2);
}

/**
 * @brief The client want to collaborate the other player
 */
void net_client_collab() 
{  
    _net_common_dbg("%d want to collab", net_client_sockfd);
    write(net_client_sockfd, "C", 2);
}

/**
 * @brief The client want to play
 */
void net_client_acces_request() {
    _net_common_dbg("%d want to play", net_client_sockfd);
    write(net_client_sockfd, "P", 2);
}

/**
 * @brief The client want to quit the game
 */
void net_client_disconnect() 
{
    _net_common_dbg("%d want to disconnect", net_client_sockfd);
    write(net_client_sockfd, "D", 2);
}
#pragma endregion Client