#include <stdio.h>		
#include "net_prisoner.h"

int main()
{	

	// création de la connexion
	char addrServer = "127.0.0.1\0";  
	net_client_connexion(&addrServer, 7799);

	// envoie d'un message
	char msg[MSGLENGHT];
	strcpy(msg, "socket test\n");
	printf("sending : %s\n", msg);
	write(net_client_sockfd, msg, strlen(msg));

	// écoute d'une réponse
    memset(msg,'\0', MSGLENGHT);
	net_thread_process(&msg);
	printf("received : %s\n", msg);

	return (EXIT_SUCCESS);
}