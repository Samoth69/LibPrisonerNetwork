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
connection_t *_connections[MAXSIMULTANEOUSCLIENTS];

/**
 * @brief main thread for the server
 * (this is the thread that will handle all incoming connections)
 */
pthread_t _net_server_main_thread;

/**
 * @brief server sockfd id
 */
int _net_server_sockfd;

void (*net_server_func_new_client)(int);

/**
 * @brief Setup variables before socket start and start socket (in another thread)
 */
void net_server_init(char* ip, int port)
{
    _net_common_init();
    for (int i = 0; i < MAXSIMULTANEOUSCLIENTS; i++)
    {
        _connections[i] = NULL;
    }

    _net_server_sockfd = _net_server_create_server_socket();
    pthread_create(&_net_server_main_thread, 0, _net_server_main_pthread, (void *)_net_server_sockfd);
    pthread_detach(&_net_server_main_thread);
}

void net_server_wait()
{
    //todo
}

void net_server_stop()
{
    //todo
}

void net_server_set_func_new_client(void (*f)(int))
{
    net_server_func_new_client = f;
}



/**
 * @brief Ajoute une connexion à la liste des connexions ouvertes
 * 
 * @param connection 
 */
void _net_server_connection_add(connection_t *connection)
{
    for (int i = 0; i < MAXSIMULTANEOUSCLIENTS; i++)
    {
        if (_connections[i] == NULL)
        {
            _connections[i] = connection;
            return;
        }
    }
    perror("Too much simultaneous connections");
    exit(-5);
}

/**
 * @brief Enlève une connexion
 * 
 * @param connection 
 */
void _net_server_connection_del(connection_t *connection)
{
    for (int i = 0; i < MAXSIMULTANEOUSCLIENTS; i++)
    {
        if (_connections[i] == connection)
        {
            _connections[i] = NULL;
            return;
        }
    }
    perror("Connection not in pool ");
    exit(-5);
}

/**
 * @brief Create a server socket object
 * 
 * @return int 
 */
int _net_server_create_server_socket()
{
    int sockfd = -1;
    struct sockaddr_in address;
    int port = 7799;

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
    address.sin_addr.s_addr = inet_addr("0.0.0.0");
    address.sin_port = htons(port);

    /* prevent the 60 secs timeout */
    int reuse = 1;
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, (const char *)&reuse, sizeof(reuse));

    /* bind */
    if (bind(sockfd, (struct sockaddr *)&address, sizeof(struct sockaddr_in)) < 0)
    {
        fprintf(stderr, "error: cannot bind socket to port %d\n", port);
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
    connection_t *connection;
    pthread_t thread;

    /* listen on port , stack size 50 for incoming connections*/
    if (listen(sockfd, 50) < 0)
    {
        fprintf(stderr, "%s: error: cannot listen on port\n");
        return -5;
    }

    printf("ready and listening\n");

    //Wait for connection
    while (true)
    {
        /* accept incoming connections */
        connection = (connection_t *)malloc(sizeof(connection_t));
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
 * @param ptr connection_t 
 * @return 
 */
void *_net_server_thread_process(void *ptr)
{
    char buffer_in[BUFFERSIZE];
    char buffer_out[BUFFERSIZE];

    int len;
    connection_t *connection;

    if (!ptr)
        pthread_exit(0);
    connection = (connection_t *)ptr;
    printf("New incoming connection \n");

    _net_server_connection_add(connection);

    (*net_server_func_new_client)(42);

    //Welcome the new client
    printf("Welcome #%i\n", connection->index);
    sprintf(buffer_out, "Welcome #%i\n", connection->index);
    write(connection->sockfd, buffer_out, strlen(buffer_out));

    while ((len = read(connection->sockfd, buffer_in, BUFFERSIZE)) > 0)
    {

        if (strncmp(buffer_in, "bye", 3) == 0)
        {
            break;
        }
#if NETDEBUG
        printf("DEBUG-----------------------------------------------------------\n");
        printf("len : %i\n", len);
        printf("Buffer : %.*s\n", len, buffer_in);
        printf("----------------------------------------------------------------\n");
#endif
        strcpy(buffer_out, "\nServer Echo : ");
        strncat(buffer_out, buffer_in, len);

        if (buffer_in[0] == '@')
        {
            for (int i = 0; i < MAXSIMULTANEOUSCLIENTS; i++)
            {
                if (_connections[i] != NULL)
                {
                    write(_connections[i]->sockfd, buffer_out, strlen(buffer_out));
                }
            }
        }
        else if (buffer_in[0] == '#')
        {
            int client = 0;
            int read = sscanf(buffer_in, "%*[^0123456789]%d ", &client);
            for (int i = 0; i < MAXSIMULTANEOUSCLIENTS; i++)
            {
                if (client == _connections[i]->index)
                {
                    write(_connections[i]->sockfd, buffer_out, strlen(buffer_out));
                    break;
                } //no client found ? : we dont care !!
            }
        }
        else
        {
            write(connection->sockfd, buffer_out, strlen(buffer_out));
        }

        //clear input buffer
        memset(buffer_in, '\0', BUFFERSIZE);
    }
    printf("Connection to client %i ended \n", connection->index);
    close(connection->sockfd);
    _net_server_connection_del(connection);
    free(connection);
    pthread_exit(0);
}

#pragma endregion Server