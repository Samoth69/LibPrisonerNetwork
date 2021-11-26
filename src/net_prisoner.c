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
 * @brief Semaphore used for locking the _net_common_dbg function
 * This should prevent multiple thread from writing to STDOUT at the same time
 */
sem_t _lock_log_dbg;

/**
 * @brief show debug message for the net lib
 * in this lib, _net_common_dbg should be used insteadof printf directly.
 * this allow the #define NETDEBUG to easily turn on/off all message
 * from this library
 * 
 * @param format work exactly like printf
 * @param ... 
 */
void _net_common_dbg(const char *format, ...)
{
    if (NETDEBUG)
    {
        // grab the semaphore
        sem_wait(&_lock_log_dbg);

        printf("NET: ");

        // see: https://sourceware.org/git/?p=glibc.git;a=blob;f=stdio-common/printf.c;h=4c8f3a2a0c38ab27a2eed4d2ff3b804980aa8f9f;hb=3321010338384ecdc6633a8b032bb0ed6aa9b19a
        va_list arg;

        va_start(arg, format);

        // https://www.cplusplus.com/reference/cstdio/vfprintf/
        vfprintf(stdout, format, arg);

        va_end(arg);

        // release the semaphore
        sem_post(&_lock_log_dbg);
    }
}

/**
 * @brief initialize common variable for client and server, should be called
 * in client and server init function.
 */
void _net_common_init()
{
    // initialize the semaphore before use
    sem_init(&_lock_log_dbg, PTHREAD_PROCESS_SHARED, 1);
}

/**
 * @brief Set as global var
 */
int net_client_sockfd;


void *_net_client_func_waiting_screen;
void (*_net_client_func_round_end)(bool, int);


void *net_client_set_func_waiting_screen(void (*f)) 
{
    _net_client_func_waiting_screen = f;
}


void _net_client_event(char * buffer_in, int length) {
    if (strcmp(buffer_in, "W")) {
        *_net_client_func_waiting_screen;
    }
}

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
        _net_common_dbg("client %d receive %d chars\n", net_client_sockfd, len);
        _net_common_dbg("%.*s\n", len, buffer_in);
        _net_client_event(buffer_in, len);
    }
    close(net_client_sockfd);
    _net_common_dbg("client pthread ended, len=%d\n", len);
}

#pragma endregion Common

// ----------------------------------------------
//                     Client
// ----------------------------------------------
#pragma region Client

// Globals vars
int net_client_sockfd;

/**
 * @brief open the connexion with the server
 * @param addrServer server address IP
 * @param port server port
 */
void net_client_init(char *addrServer, int port)
{
    struct sockaddr_in serverAddr;
    pthread_t thread;

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
    if (connect(net_client_sockfd, (struct sockaddr *) &serverAddr, sizeof (serverAddr)) != 0) {
        _net_common_dbg("Fail to connect to server");
        exit(-1);
    };

    // reading pthread creation
    pthread_create(&thread, 0, _net_client_threadProcess, &net_client_sockfd);
    pthread_detach(thread);
}

/**
 * @brief The client want to betray the other player
 */
void net_client_betray() 
{
    _net_common_dbg("%d want to betray", net_client_sockfd);
    write(net_client_sockfd, "B", 2);
}

/**
 * @brief The client want to collaborate the other player
 */
void net_client_collab() 
{  
    _net_common_dbg("%d want to collab", net_client_sockfd);
    write(net_client_sockfd, "C", 2);
}

/**
 * @brief The client want to play
 */
void net_client_acces_request() {
    _net_common_dbg("%d want to play", net_client_sockfd);
    write(net_client_sockfd, "P", 2);
}

/**
 * @brief The client want to quit the game
 */
void net_client_disconnect() 
{
    _net_common_dbg("%d want to disconnect", net_client_sockfd);
    write(net_client_sockfd, "D", 2);
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