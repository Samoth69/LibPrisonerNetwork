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

	// envoie d'un message
	net_client_betray();

	net_server_send_screen_choice(0);
	net_server_send_screen_waiting(0);
	net_server_send_screen_score(0, true, 42);
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

void client_waiting_screen() 
{
	printf("client need to display the waiting screen");
}

void client_choice_screen()
{
	printf("client need to display the choice screen");
}

void client_score_screen(bool has_win, int score) 
{
	printf("client need to display the score screen, the client win : %d, score = %d", has_win, score);
}

int main()
{
	net_server_set_func_new_client(new_client);
	net_server_set_func_client_disconnect(client_disconnecting);
	net_server_set_func_cooperate(client_cooperate);
	net_server_set_func_betray(client_betray);
	net_client_set_func_waiting_screen(client_waiting_screen);
	net_client_set_func_choice_screen(client_choice_screen);
	net_client_set_func_score_screen(client_score_screen);
	
	char *addrServer = "0.0.0.0";
	net_server_init(addrServer, 7799);
	net_client_init(addrServer, 7799);

	net_server_wait();

	while(1){};

	return (EXIT_SUCCESS);
}
