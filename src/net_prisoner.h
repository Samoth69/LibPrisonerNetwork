#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <stdbool.h>

#include <pthread.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>

/***********
 * Settings
 ***********/

/**
 * @brief Enable logging for the net lib
 * Set this to true to allow the lib to log messages to STDOUT
 * It may be output usefull informations for debuging
 */
#define NETDEBUG true

/*********
 * Common
 *********/


/**
 * @brief Internal.
 * Allow the lib to log message (only if NETDEBUG == true)
 * 
 * @param format même fonctionnement que printf
 * @param ... 
 */
void net_dbg(const char *format, ...);

/**************
 * Client only
 **************/

/**
 * @brief open the connexion with the server
 * @param port server port
 * @param addrServer server address IP
 * @return int sockfd : socket id
 */
int net_client_connexion(int port, char * addrServer[]);
void net_client_betray();
void net_client_collab();
void net_client_acces_request();
void net_client_disconnect();

/**************
 * Server only
 **************/

void net_server_init();
void net_server_wait();
void net_server_game_start();
void net_server_round_end();
void net_server_match_end();