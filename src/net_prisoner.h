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

#define MSGLENGHT 100

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
int net_client_sockfd;

/**
 * @brief open the connexion with the server
 * @param addrServer server address IP
 * @param port server port
 */
void net_client_connexion(char * addrServer[], int port);

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

void net_server_init();
void net_server_wait();
void net_server_game_start();
void net_server_round_end();
void net_server_match_end();

#endif /* NET_PRISONER_H */