#include "net_prisoner.h"
/**
 * @file net_prisoner.c
 * @brief 
 * @author Thomas Violent & Wolodia Zdetovetzky
 * @version 1.0
 * @date 24/11/2021
 */

// ----------------------------------------------
//                     Common
// ----------------------------------------------
#pragma region Common

/**
 * @brief show debug message for the net lib
 * in this lib, net_dbg should be used insteadof printf directly.
 * this allow the #define NETDEBUG to easily turn on/off all message
 * from this library
 * 
 * @param format work exactly like printf
 * @param ... 
 */
//TODO: rendre cette fonction thread safe
void net_dbg(const char *format, ...)
{
    if (NETDEBUG)
    {
        printf("NET: ");

        // see: https://sourceware.org/git/?p=glibc.git;a=blob;f=stdio-common/printf.c;h=4c8f3a2a0c38ab27a2eed4d2ff3b804980aa8f9f;hb=3321010338384ecdc6633a8b032bb0ed6aa9b19a
        va_list arg;

        va_start(arg, format);

        // https://www.cplusplus.com/reference/cstdio/vfprintf/
        vfprintf(stdout, format, arg);

        va_end(arg);
    }
}
#pragma endregion Common

// ----------------------------------------------
//                     Client
// ----------------------------------------------
#pragma region Client

/**
 * @brief Set as global var
 */
int net_client_sockfd;

/**
 * @brief read and display received messages
 * @param ptr the net_client_sockfd
 * @return void* 
 */
void *_net_client_threadProcess(void *ptr)
{
    char buffer_in[BUFFERSIZE];
    net_client_sockfd = *((int *)ptr);
    int len;
    while ((len = read(net_client_sockfd, buffer_in, BUFFERSIZE)) != 0)
    {
        if (strncmp(buffer_in, "exit", 4) == 0)
        {
            break;
        }
        net_dbg("receive %d chars\n", len);
        net_dbg("%.*s\n", len, buffer_in);
    }
    close(net_client_sockfd);
    net_dbg("client pthread ended, len=%d\n", len);
}

/**
 * @brief Reading thread creation
 * @return char* message receive
 */
char * net_client_listening_server()
{
    char * response;
    pthread_t thread;
    int status = 0;

    // reading pthread creation
    pthread_create(&thread, 0, _net_client_threadProcess, &net_client_sockfd);

    pthread_detach(thread);
    do
    {
        fgets(response, MSGLENGHT, stdin);
        status = write(net_client_sockfd, response, strlen(response));

    } while (status != -1);

    return response;
}

/**
 * @brief open the connexion with the server
 * @param addrServer server address IP
 * @param port server port
 */
void net_client_init(char *addrServer, int port)
{
    struct sockaddr_in serverAddr;

    // Create the socket.
    net_client_sockfd = socket(AF_INET, SOCK_STREAM, 0);

    // Configure settings of the server address
    // Address family is Internet
    serverAddr.sin_family = AF_INET;

    //Set port number, using htons function
    serverAddr.sin_port = htons(port);

    //Set IP address to localhost
    serverAddr.sin_addr.s_addr = inet_addr(addrServer);

    memset(serverAddr.sin_zero, '\0', sizeof serverAddr.sin_zero);

    //Connect the socket to the server using the address
    if (connect(net_client_sockfd, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) != 0)
    {
        net_dbg("Fail to connect to server");
        exit(-1);
    };
}

/**
 * @brief The client want to betray the other player
 */
void net_client_betray()
{
    net_dbg("%d want to betray\n", net_client_sockfd);
    write(net_client_sockfd, "B", 1);
}

/**
 * @brief The client want to collaborate the other player
 */
void net_client_collab()
{
    net_dbg("%d want to collab\n", net_client_sockfd);
    write(net_client_sockfd, "C", 1);
}

/**
 * @brief The client want to quit the game
 */
void net_client_disconnect()
{
    net_dbg("%d want to disconnect", net_client_sockfd);
    write(net_client_sockfd, "D", 1);
}
#pragma endregion Client

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

int _net_server_sockfd;

/**
 * @brief Setup variables before socket start and start socket (in another thread)
 */
void net_server_init(char* ip, int port)
{
    for (int i = 0; i < MAXSIMULTANEOUSCLIENTS; i++)
    {
        _connections[i] = NULL;
    }

    _net_server_sockfd = _net_server_create_server_socket();
    pthread_create(&_net_server_main_thread, 0, _net_server_main_pthread, (void *)_net_server_sockfd);
    pthread_detach(&_net_server_main_thread);
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


        /**
         * @warning tmp
         */
        write(connection->sockfd, "B", 1);



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