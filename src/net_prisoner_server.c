/**
 * @file net_prisoner_server.c
 * @author Thomas Violent
 * @brief 
 * @version 0.1
 * @date 2021-11-26
 * 
 * @copyright Copyright (c) 2021
 * 
 */

#include "net_prisoner_server.h"

// ----------------------------------------------
//                     Server
// ----------------------------------------------
#pragma region Server

/**
 * @brief List of clients (connections)
 */
_net_server_connection_t *_connections[MAXSIMULTANEOUSCLIENTS];

/**
 * @brief main thread for the server
 * (this is the thread that will handle all incoming connections)
 */
pthread_t _net_server_main_thread;

/**
 * @brief server sockfd id
 */
int _net_server_sockfd;

/**
 * @brief will be switch to true if we need to stop the app
 */
bool _net_server_exit = false;

/**
 * @brief counter to keep track of client id
 */
int _net_server_client_id_counter = 0;

/**
 * @brief Use to protect lib user function from multiple executions
 * from different thread
 */
sem_t _lock_user_function;

// functions provided by the user of the lib
void (*_net_server_func_new_client)(int);
void (*_net_server_func_client_disconnect)(int);
void (*_net_server_func_cooperate)(int, ulong);
void (*_net_server_func_betray)(int, ulong);

/**
 * @brief Setup variables before socket start and start socket (in another thread)
 */
void net_server_init(char *ip, int port)
{
    _net_common_init();
    sem_init(&_lock_user_function, PTHREAD_PROCESS_SHARED, 1);

    for (int i = 0; i < MAXSIMULTANEOUSCLIENTS; i++)
    {
        _connections[i] = NULL;
    }

    _net_server_sockfd = _net_server_create_server_socket(ip, port);
    pthread_create(&_net_server_main_thread, 0, _net_server_main_pthread, (void *)_net_server_sockfd);
    pthread_detach(&_net_server_main_thread);
}

/**
 * @brief Wait for _net_server_exit to be true
 */
void net_server_wait()
{
    while (!_net_server_exit)
    {
        sleep(1);
    }
}

/**
 * @brief Tell the server that it should stop (net_server_wait will stop)
 */
void net_server_stop()
{
    _net_server_exit = true;
}

/**
 * @brief Define the function to be called when a new client is connecting to the server
 * 
 * @param f Function to be called
 * int is the client id
 */
void net_server_set_func_new_client(void (*f)(int))
{
    _net_server_func_new_client = f;
}

/**
 * @brief Define the function to be called when a client is disconnecting from the server
 * 
 * @param f Function to be called
 * int is the client id
 */
void net_server_set_func_client_disconnect(void (*f)(int))
{
    _net_server_func_client_disconnect = f;
}

/**
 * @brief Define the function to be called when a client click the cooperate button
 * 
 * @param f Function to be called
 * int is client id
 * ulong is the delay for the client to answer
 */
void net_server_set_func_cooperate(void (*f)(int, ulong))
{
    _net_server_func_cooperate = f;
}

/**
 * @brief Define the function to be called when a client click the betray button
 * 
 * @param f Function to be called
 * int is client id
 * ulong is the delay for the client 
 */
void net_server_set_func_betray(void (*f)(int, ulong))
{
    _net_server_func_betray = f;
}

/**
 * @brief Send to specified client to switch to waiting screen
 * 
 * @param client client id
 */
void net_server_send_screen_waiting(int client)
{
    _net_common_netpacket msg;
    msg.msg_type = SCREEN_WAITING;
    _net_server_send_message(&msg, client);
}

/**
 * @brief Send to specified client to switch to choice screen
 * 
 * @param client client id
 */
void net_server_send_screen_choice(int client)
{
    _net_common_netpacket msg;
    msg.msg_type = SCREEN_CHOICE;
    _net_server_send_message(&msg, client);
}

/**
 * @brief Send to specified client to switch to score screen
 * The client may show the given results
 * 
 * @param client client id
 * @param has_win should be true if this client has win
 * @param score Score for the client (this value isn't enforce, there may be
 * any value a int can handle)
 */
void net_server_send_screen_score(int client, bool has_win, int score)
{
    _net_common_netpacket msg;
    msg.msg_type = SCREEN_SCORE;
    msg.has_win = has_win;
    msg.score = score;
    _net_server_send_message(&msg, client);
}

/**
 * @brief Ajoute une connexion à la liste des connexions ouvertes
 * 
 * @param connection 
 */
void _net_server_connection_add(_net_server_connection_t *connection)
{
    bool found = false;

    for (int i = 0; i < MAXSIMULTANEOUSCLIENTS; i++)
    {
        if (_connections[i] == NULL)
        {
            _connections[i] = connection;
            found = true;
            break;
        }
    }
    if (!found)
    {
        perror("Too much simultaneous connections");
        exit(-5);
    }
    _net_server_call_new_client(_net_server_client_id_counter);
    _net_server_client_id_counter++;
}

/**
 * @brief Enlève une connexion
 * 
 * @param connection 
 */
void _net_server_connection_del(_net_server_connection_t *connection)
{
    bool found = false;

    for (int i = 0; i < MAXSIMULTANEOUSCLIENTS; i++)
    {
        if (_connections[i] == connection)
        {
            _connections[i] = NULL;
            found = true;
            break;
        }
    }
    if (!found)
    {
        perror("Connection not in pool ");
        exit(-5);
    }
    _net_server_call_client_disconnect(connection->client_id);
}

/**
 * @brief Create a server socket object
 * 
 * @return int 
 */
int _net_server_create_server_socket(char *ip_address, int network_port)
{
    int sockfd = -1;
    struct sockaddr_in address;

    /* create socket */
    sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sockfd <= 0)
    {
        fprintf(stderr, "%s: error: cannot create socket\n");
        return -3;
    }

    /* bind socket to port */
    address.sin_family = AF_INET;
    //bind to all ip :
    //address.sin_addr.s_addr = INADDR_ANY;
    //ou 0.0.0.0
    //Sinon  127.0.0.1
    address.sin_addr.s_addr = inet_addr(ip_address);
    address.sin_port = htons(network_port);

    /* prevent the 60 secs timeout */
    int reuse = 1;
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, (const char *)&reuse, sizeof(reuse));

    /* bind */
    if (bind(sockfd, (struct sockaddr *)&address, sizeof(struct sockaddr_in)) < 0)
    {
        fprintf(stderr, "error: cannot bind socket to port %d\n", network_port);
        return -4;
    }

    return sockfd;
}

/**
 * @brief Create the main server pthread (this thread will listen to incoming connections)
 * 
 * @param sockfd socket id (provided by OS)
 */
void *_net_server_main_pthread(int sockfd)
{
    int index = 1;
    _net_server_connection_t *connection;
    pthread_t thread;

    /* listen on port , stack size 50 for incoming connections*/
    if (listen(sockfd, 50) < 0)
    {
        fprintf(stderr, "%s: error: cannot listen on port\n");
        return -5;
    }

    _net_common_dbg("ready and listening\n");

    //Wait for connection
    while (true)
    {
        /* accept incoming connections */
        connection = (_net_server_connection_t *)malloc(sizeof(_net_server_connection_t));
        connection->sockfd = accept(sockfd, &connection->address, &connection->addr_len);
        connection->index = index++;
        if (connection->sockfd <= 0)
        {
            free(connection);
        }
        else
        {
            /* start a new thread but do not wait for it */
            pthread_create(&thread, 0, _net_server_thread_process, (void *)connection);
            pthread_detach(thread);
        }
    }
}

/**
 * Thread allowing server to handle multiple client connections
 * @param ptr _net_server_connection_t 
 * @return 
 */
void *_net_server_thread_process(void *ptr)
{
    char buffer_in[BUFFERSIZE];
    memset(buffer_in, '\0', BUFFERSIZE);
    int len;
    bool quit = false;

    _net_server_connection_t *connection;
    _net_common_netpacket packet;

    if (!ptr)
        pthread_exit(0);
    connection = (_net_server_connection_t *)ptr;
    _net_common_dbg("New incoming connection \n");

    _net_server_connection_add(connection);

    //Welcome the new client
    // printf("Welcome #%i\n", connection->index);
    // sprintf(buffer_out, "Welcome #%i\n", connection->index);
    // write(connection->sockfd, buffer_out, strlen(buffer_out));

    while ((len = read(connection->sockfd, buffer_in, BUFFERSIZE)) > 0)
    {
        if (quit)
            break;

        if (len != sizeof(packet))
        {
            _net_common_dbg("WARN: Invalid packet received, ignoring\n");
            continue;
        }

        _net_common_dbg("Received from client #%d length %d\n", connection->client_id, len);
        memcpy(&packet, &buffer_in, len);

        switch (packet.msg_type)
        {
        case ACTION_BETRAY:
            _net_common_dbg("received ACTION_BETRAY from client %d\n", connection->client_id);
            _net_server_call_betray(connection->client_id, packet.delay);
            break;
        case ACTION_COLLAB:
            _net_common_dbg("received ACTION_COLLAB from client %d\n", connection->client_id);
            _net_server_call_cooperate(connection->client_id, packet.delay);
            break;
        case ACTION_QUIT:
            _net_common_dbg("received ACTION_QUIT from client %d\n", connection->client_id);
            _net_server_func_client_disconnect(connection->client_id);
            quit = true;
            break;
        case SCREEN_WAITING:
            _net_common_dbg("ERROR: received SCREEN_WAITING from client %d\n", connection->client_id);
            break;
        case SCREEN_CHOICE:
            _net_common_dbg("ERROR: received SCREEN_CHOICE from client %d\n", connection->client_id);
            break;
        case SCREEN_SCORE:
            _net_common_dbg("ERROR: received SCREEN_SCORE from client %d\n", connection->client_id);
            break;

        default:
            _net_common_dbg("Unknown message type, do you have the latest version of the lib ?\n");
            break;
        }

        //clear input buffer
        memset(buffer_in, '\0', BUFFERSIZE);
    }
    _net_common_dbg("Connection to client %i (id %d) ended \n", connection->index, connection->client_id);
    close(connection->sockfd);
    _net_server_connection_del(connection);
    free(connection);
    pthread_exit(0);
}

/**
 * @brief Send the netpacket to appropriate client id, if the client is not found, a warning will
 * be shown on _net_common_dbg()
 * 
 * @param msg netpacket
 * @param client_id client id, if not valid/found, will send a warning
 */
void _net_server_send_message(_net_common_netpacket *msg, int client_id)
{
    //used to warn user if the client_id isn't found
    //will switch to true if a message has been sent
    //should stay to false otherwise
    bool found = false;
    for (int i = 0; i < MAXSIMULTANEOUSCLIENTS; i++)
    {
        if (_connections[i] != NULL && _connections[i]->client_id == client_id)
        {
            _net_common_dbg("sending msg_type%d to client #%d\n", msg->msg_type, client_id);
            write(_connections[i]->sockfd, msg, sizeof(msg));
            found = true;
        }
    }
    if (!found)
        _net_common_dbg("WARNING: client id %d not found, ignoring\n", client_id);
}

/**
 * @brief Trigger call new client on lib user side
 * 
 * @param client new client id
 */
void _net_server_call_new_client(int client)
{
    if (THREAD_SAFETY)
        sem_wait(&_lock_user_function);
    (*_net_server_func_new_client)(client);
    sem_post(&_lock_user_function);
}

/**
 * @brief Trigger call client disconnect on lib user side
 * 
 * @param client client id of the disconnecting client
 */
void _net_server_call_client_disconnect(int client)
{
    if (THREAD_SAFETY)
        sem_wait(&_lock_user_function);
    (*_net_server_func_client_disconnect)(client);
    sem_post(&_lock_user_function);
}

/**
 * @brief trigger call on cooperate on lib user side
 * 
 * @param client client id
 * @param delay time spend answering the question
 */
void _net_server_call_cooperate(int client, ulong delay)
{
    if (THREAD_SAFETY)
        sem_wait(&_lock_user_function);
    (*_net_server_func_cooperate)(client, delay);
    sem_post(&_lock_user_function);
}

/**
 * @brief trigger call on betray on lib user side
 * 
 * @param client client id
 * @param delay time spend answering the question
 */
void _net_server_call_betray(int client, ulong delay)
{
    if (THREAD_SAFETY)
        sem_wait(&_lock_user_function);
    (*_net_server_func_betray)(client, delay);
    sem_post(&_lock_user_function);
}

#pragma endregion Server