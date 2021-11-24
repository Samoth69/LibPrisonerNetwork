#include <stdio.h>		
#include "net_prisoner.h"

#include "net_prisoner.h"

int main()
{	
	net_dbg("yolo");
	
	char * addrServer = "127.0.0.1";  
	net_client_connexion(&addrServer, 7799);

	return (EXIT_SUCCESS);
}