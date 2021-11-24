#include <stdlib.h>
#include "net_prisoner.h"

int main()
{
	net_dbg("yolo");
	net_server_init();

	while(true)
	{
		sleep(1);
	}

	return (EXIT_SUCCESS);
}