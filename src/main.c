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
	printf("new_client: %d (id: %d)", counter, client_id);
}

int main()
{
	net_server_init("0.0.0.0", 7799);
	net_server_set_func_new_client(new_client);

	// création de la connexion
	char *addrServer = "0.0.0.0";
	net_client_init(addrServer, 7799);

	// envoie d'un message
	net_client_betray();

	while(1){};

	net_server_wait();

	while (1)
	{
		sleep(1);
	}
	return (EXIT_SUCCESS);
}
