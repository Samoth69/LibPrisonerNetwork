#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <pthread.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>

/**
 * Settings
 */
#define DEBUG true

/**
 * Common
 */

void net_dbg(char text[]);

/**
 * Client only
 */

void net_client_connexion();
void net_client_betray();
void net_client_collab();
void net_client_acces_request();
void net_client_disconnect();

/**
 * Server only
 */

void net_server_init();
void net_server_wait();
void net_server_game_start();
void net_server_round_end();
void net_server_match_end();