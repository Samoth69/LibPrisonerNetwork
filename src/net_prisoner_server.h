/**
 * @file net_prisoner_server.h
 * @author Thomas Violent
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

#ifndef NET_PRISONER_SERVER_H
#define NET_PRISONER_SERVER_H

// ----------------------------------------------
//                     Server
// ----------------------------------------------

/**
 * @brief Structure for keeping track of active connections
 */
typedef struct
{
    /**
     * @brief socket id
     */
    int sockfd;
    
    /**
     * @brief socket address
     */
    struct sockaddr address;
    int addr_len;
    int index;

    /**
     * @brief Client id. must be unique
     */
    int client_id;
} _net_server_connection_t;

/**
 * @brief Initialize the server socket and start it in another thread (non-bloking function)
 * 
 * in details:
 * 1 - Initialize variables
 * 2 - Create the server socket (=bind) with the given ip and port
 * 3 - Create a thread that will be listening to the socket
 * 
 * ! This function isn't blocking, once the thread is started,
 * ! this function will exit with the thread running in the background
 * 
 * @param ip address ip which the server will listen (put '0.0.0.0' to listen everywhere)
 * @param port network port to listen to
 */
void net_server_init(char* ip, int port);

/**
 * @brief Block current thread until net_server_stop() is called
 */
void net_server_wait();

/**
 * @brief Stop the main socket thread and all client thread
 * This will basically shutdown the whole network lib
 */
void net_server_stop();

/**
 * @brief Set the function that the server will trigger when a new client (player)
 * connect to this server
 * 
 * @param f pointer to function (int is the client id)
 */
void net_server_set_func_new_client(void (*f)(int));

/**
 * @brief Set the function that the server will trigger when a client disconnect
 * 
 * @param f pointer to function (int is the client id)
 */
void net_server_set_func_client_disconnect(void (*f)(int));

/**
 * @brief Set the function that the server will trigger when a client has click the cooperate button
 * 
 * @param f pointer to function (int is the client id; ulong is the time spent answering)
 */
void net_server_set_func_cooperate(void (*f)(int, ulong));

/**
 * @brief Set the function that the server will trigger when a client has click the betray button
 * 
 * @param f pointer to function (int is the client id; ulong is the time spent answering)
 */
void net_server_set_func_betray(void (*f)(int, ulong));

/**
 * @brief Tell to specified client that it need to show 'waiting' view
 * 
 * @param client client id
 */
void net_server_send_screen_waiting(int client);

/**
 * @brief Tell to specified client that it need to show 'choice' view
 *  
 * @param client client id
 */
void net_server_send_screen_choice(int client);

/**
 * @brief send to specified client to switch to 'round end' view
 * This view should also display on client side if the player has winned or not 
 * 
 * @param client client id
 * @param has_win true if the client has winned, false otherwise
 * @param score 
 */
void net_server_send_screen_score_round(int client, bool has_win, int score, int round_current, int round_total);

/**
 * @brief Send the final results to the client
 * 
 * @param client client id
 * @param result dual dimension array
 * first dimension is the round list
 *  second dimension should contain result for each round,
 *  with client A and B answers (this arg can be null)
 */
void net_server_send_screen_score_final(int client, int **result);

//private
void _net_server_connection_add(_net_server_connection_t *connection);
void _net_server_connection_del(_net_server_connection_t *connection);
int _net_server_create_server_socket(char *ip_address, int network_port);
void *_net_server_main_pthread(int sockfd);
void *_net_server_thread_process(void *ptr);

void _net_server_send_message(_net_common_netpacket *msg, int client_id);
void _net_server_call_new_client(int client);
void _net_server_call_client_disconnect(int client);
void _net_server_call_cooperate(int client, ulong delay);
void _net_server_call_betray(int client, ulong delay);

#endif /* NET_PRISONER_SERVER_H */