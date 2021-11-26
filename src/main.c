/**
 * @file main.c
 * @brief 
 * @author Thomas Violent & Wolodia Zdetovetzky
 * @version 1.0
 * @date 24/11/2021
 */

#include <stdio.h>		
#include <stdlib.h>
#include "net_prisoner.h"

void *trahir(int client)
{

}

void *cooperer(int client)
{

}

int main()
{
	net_dbg("yolo");
	net_server_init("0.0.0.0", 7799);
	net_server_set_trahir(*trahir);


	// création de la connexion
	char *addrServer = "127.0.0.1\0";
	net_client_connexion(addrServer, 7799);

	// envoie d'un message
	net_client_betray();
	
	// écoute d'une réponse
	char * returned = net_client_listening_server();
	printf("received: %s\n", returned);

	return (EXIT_SUCCESS);
}
