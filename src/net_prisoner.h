#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <pthread.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>

#ifndef NET_PRISONER_H
#define NET_PRISONER_H

// ----------------------------------------------
//                     Settings
// ----------------------------------------------

#define DEBUG true
#define BUFFERSIZE 2048

// ----------------------------------------------
//                     Common
// ----------------------------------------------

void net_dbg(char text[]);


// ----------------------------------------------
//                     Client
// ----------------------------------------------

/*** @brief socket file id */
int net_client_sockfd;

/**
 * @brief open the connexion with the server
 * @param port server port
 * @param addrServer server address IP
 */
void net_client_connexion(int port, char * addrServer[]);
void net_client_betray();
void net_client_collab();
void net_client_acces_request();
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