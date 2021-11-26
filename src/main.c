/**
 * @file main.c
 * @brief 
 * @author Thomas Violent & Wolodia Zdetovetzky
 * @version 1.0
 * @date 24/11/2021
 */

#include <stdio.h>		
#include <stdlib.h>
#include "net_prisoner_client.h"
#include "net_prisoner_server.h"

int counter;

void new_client(int client_id)
{
	counter++;
	printf("new_client: %d (id: %d)\n", counter, client_id);
}

void client_disconnecting(int client_id)
{
	counter--;
	printf("client disconnecting: %d\n", client_id);
}

void client_cooperate(int client_id, ulong tps)
{
	printf("client #%d coop (%ld ms)\n", client_id, tps);
}

void client_betray(int client_id, ulong tps)
{
	printf("client #%d betray (%ld ms)\n", client_id, tps);
}

int main()
{
	net_server_init("0.0.0.0", 7799);
	net_server_set_func_new_client(new_client);
	net_server_set_func_client_disconnect(client_disconnecting);
	net_server_set_func_cooperate(client_cooperate);
	net_server_set_func_betray(client_betray);

	// création de la connexion
	char *addrServer = "0.0.0.0";
	net_client_init(addrServer, 7799);

	// envoie d'un message
	net_client_betray();

	while(1){};

	net_server_wait();

	return (EXIT_SUCCESS);
}
