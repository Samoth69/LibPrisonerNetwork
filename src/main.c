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
	char msg[MSGLENGHT];
	strcpy(msg, "socket test\n");
	printf("sending : %s\n", msg);
	write(net_client_sockfd, msg, strlen(msg));

	// écoute d'une réponse
	memset(msg, '\0', MSGLENGHT);
	net_thread_process(&msg);
	printf("received : %s\n", msg);

	return (EXIT_SUCCESS);
}