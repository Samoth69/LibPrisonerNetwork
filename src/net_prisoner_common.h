/**
 * @file net_prisoner_common.h
 * @author Thomas Violent & Wolodia Zdetovetzky
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

#ifndef NET_PRISONER_COMMON_H
#define NET_PRISONER_COMMON_H

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
void _net_common_dbg(const char *format, ...);

/**
 * @brief private function
 * Should init common variable used in client and server
 */
void _net_common_init();

typedef struct {
    //1: betray
    //2: coop
    int msg_type;
    ulong delay;
    int score;
    bool has_win;
} comm;

#endif /* NET_PRISONER_COMMON_H */