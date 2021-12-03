# Net Prisoner Network Lib

This lib allow network communication between client and server. it has been designed for the [Prisoner's dilemma](https://en.wikipedia.org/wiki/Prisoner%27s_dilemma). it allows clients and server to send event when an action happened, for example when a client click the betray button, the server is notified that client has click the button.

This lib has 3 files (.c/.h):
- net_prisoner_client: client part of the lib. client should import .h file only
- net_prisoner_common: common code for client and server. this file shouldn't be included directly
- net_prisoner_server: server part of the lib. server should import .h file only.

## Settings

the [net_prisoner_common.h](src/net_prisoner_common.h) file contains settings for the lib. they are explained in the file itself

## Technicals notes

- The lib only use TCP as protocol
- The lib use pthread internally to make function less blocking
- By default, server function are protected against concurrency with a semaphore, basically, none of the user provided function will be executed in parallel.

## Building

```
mkdir lib
cd lib
git clone https://github.com/Samoth69/LibPrisonerNetwork .
make libs
```
**Compiled files should be in output folder**

## Samples

### Server

```c
/**
 * @file main.c
 * @brief 
 * @author Thomas Violent & Wolodia Zdetovetzky
 * @version 1.0
 * @date 03/12/2021
 */

#include <stdio.h>		
#include <stdlib.h>
#include "net_prisoner_server.h"

// triggered when a client is successfully connected to server
void new_client(int client_id)
{
	printf("new_client id %d\n", client_id);
	net_server_send_screen_waiting(432);

    //additionnal server logic here when a new client arrived
}

//triggered when a server is disconnecting from the server
void client_disconnecting(int client_id)
{
	printf("client disconnecting: %d\n", client_id);
}

//triggered when a client has clicked the cooperate button
void client_cooperate(int client_id, ulong tps)
{
	printf("client #%d coop (%ld ms)\n", client_id, tps);
}

//triggered when a client has clicked the betray button
void client_betray(int client_id, ulong tps)
{
	printf("client #%d betray (%ld ms)\n", client_id, tps);
}

int main()
{
	net_server_set_func_new_client(new_client);
	net_server_set_func_client_disconnect(client_disconnecting);
	net_server_set_func_cooperate(client_cooperate);
	net_server_set_func_betray(client_betray);
	
	char *addrServer = "0.0.0.0";
	net_server_init(addrServer, 7799);

	net_server_wait();

	while(1){};

    //this line isn't reached, it's here to make the compiler happy :)
	return (EXIT_SUCCESS);
}
```

### Client

```c
/**
 * @file main.c
 * @brief 
 * @author Thomas Violent & Wolodia Zdetovetzky
 * @version 1.0
 * @date 03/12/2021
 */

#include <stdio.h>		
#include <stdlib.h>
#include "net_prisoner_client.h"

//Triggered when the server ask to display the waiting screen
void client_waiting_screen() 
{
	printf("client need to display the waiting screen\n");
}

//Triggered when the server ask to display the choice screen
void client_choice_screen()
{
	printf("client need to display the choice screen\n");
}

//Triggered when the server ask to display the score screen
//the server will provide the score
void client_score_screen(bool has_win, int score) 
{
	printf("client need to display the score screen, the client win : %d, score = %d\n", has_win, score);
}

int main()
{
	net_client_set_func_waiting_screen(client_waiting_screen);
	net_client_set_func_choice_screen(client_choice_screen);
	net_client_set_func_score_screen(client_score_screen);
	
	char *addrServer = "0.0.0.0";
	net_client_init(addrServer, 7799, 432);

	while(1){};

    //this line isn't reached, it's here to make the compiler happy :)
	return (EXIT_SUCCESS);
}
```