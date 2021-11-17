#include <stdio.h>		
#include "net_prisoner.h"

#include "net_prisoner.h"

int main()
{	
	char * addrServer = "127.0.0.1";  
	net_client_connexion(7799, &addrServer);

	return (EXIT_SUCCESS);
}