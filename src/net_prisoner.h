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
 * @brief Max openned connections for the server
 */
#define MAXSIMULTANEOUSCLIENTS 100

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
extern int net_client_sockfd;

/**
 * @brief Reading thread creation
 * @param msg message receive
 */
void net_thread_process(char * msg);

/**
 * @brief open the connexion with the server
 * @param addrServer server address IP
 * @param port server port
 */
void net_client_connexion(char * addrServer, int port);

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
 * 
 */
typedef struct {
    int sockfd;
    struct sockaddr address;
    int addr_len;
    int index;
} connection_t;

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
 */
void net_server_send_screen_round_end(int client, bool has_win);

//private
void _net_server_connection_add(connection_t *connection);
void _net_server_connection_del(connection_t *connection);
int _net_server_create_server_socket();
void *_net_server_main_pthread(int sockfd);
void *_net_server_thread_process(void *ptr);


#endif /* NET_PRISONER_H */