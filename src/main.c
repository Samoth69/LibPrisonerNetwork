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
	net_client_betray(10);
	net_server_send_screen_choice(432);
	net_server_send_screen_waiting(432);
	net_server_send_screen_score_round(432, true, 42, 3, 5);
	net_server_send_screen_score_final(432, NULL);
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
	printf("client need to display the waiting screen\n");
}

void client_choice_screen()
{
	printf("client need to display the choice screen\n");
}

void client_score_screen(bool has_win, int score)
{
	printf("client need to display the score screen, the client win : %d, score = %d\n", has_win, score);
}

void client_score_final_screen() {
	printf("Client need to display score FINAL screen\n");
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
	net_client_set_func_score_final(client_score_final_screen);

	char *addrServer = "0.0.0.0";
	net_server_init(addrServer, 7799);

	bool ret = net_client_init(addrServer, 7799, 432);

	net_server_wait();

	while (1)
	{
		//provent high cpu usage
		sleep(1);
	};

	return (EXIT_SUCCESS);
}
