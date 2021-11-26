/**
 * @file net_prisoner.h
 * @brief 
 * @author Thomas Violent & Wolodia Zdetovetzky
 * @version 1.0
 * @date 24/11/2021
 * @attention do not use methods with a name begginning with "_"
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <stdbool.h>

#include <pthread.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>

#ifndef NET_PRISONER_H
#define NET_PRISONER_H

// ----------------------------------------------
//                     Settings
// ----------------------------------------------

/**
 * @brief Enable logging for the net lib
 * Set this to true to allow the lib to log messages to STDOUT
 * It may be output usefull informations for debuging
 */
#define NETDEBUG true

/**
 * @brief  max size of the buffer 
 */
#define BUFFERSIZE 2048

/**
 * @brief max size of messages
 */
#define MSGLENGHT 100

/**
 * @brief Max openned connections for the server
 */
#define MAXSIMULTANEOUSCLIENTS 100

// ----------------------------------------------
//                     Common
// ----------------------------------------------

/**
 * @brief Internal.
 * Allow the lib to log message (only if NETDEBUG == true)
 * 
 * @param format même fonctionnement que printf
 * @param ... 
 */
void net_dbg(const char *format, ...);

// ----------------------------------------------
//                     Client
// ----------------------------------------------

/**
 * @brief socket file id 
*/
extern int net_client_sockfd;

/**
 * @brief read and display received messages
 * @param ptr 
 * @return void* 
 */
void *_net_client_threadProcess(void *ptr);

/**
 * @brief Reading thread creation
 * @return char* message receive
 */
char * net_client_listening_server();

/**
 * @brief open the connexion with the server
 * @param addrServer server address IP
 * @param port server port
 */
void net_client_init(char *addrServer, int port);

/**
 * @brief The client want to betray the other player
 */
void net_client_betray();

/**
 * @brief The client want to collaborate the other player
 */
void net_client_collab();

/**
 * @brief The client want to play
 */
void net_client_acces_request();

/**
 * @brief The client want to quit the game
 */
void net_client_disconnect();

// ----------------------------------------------
//                     Server
// ----------------------------------------------

/**
 * @brief Structure for keeping track of active connections
 */
typedef struct
{
    int sockfd;
    struct sockaddr address;
    int addr_len;
    int index;
} connection_t;

void net_server_init();
void net_server_wait();
void net_server_game_start();
void net_server_round_end();
void net_server_match_end();

//private
void _net_server_connection_add(connection_t *connection);
void _net_server_connection_del(connection_t *connection);
int _net_server_create_server_socket();
void *_net_server_main_pthread(int sockfd);
void *_net_server_thread_process(void *ptr);

#endif /* NET_PRISONER_H */