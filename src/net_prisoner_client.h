/**
 * @file net_prisoner_client.h
 * @author Wolodia Zdetovetzky
 * @brief 
 * @version 0.1
 * @date 2021-11-26
 * 
 * @copyright Copyright (c) 2021
 * 
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
#include <semaphore.h>
#include "net_prisoner_common.h"

#ifndef NET_PRISONER_CLIENT_H
#define NET_PRISONER_CLIENT_H

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

#endif /* NET_PRISONER_CLIENT_H */