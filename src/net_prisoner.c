#include "net_prisoner.h"

// ----------------------------------------------
//                     Common
// ----------------------------------------------

/**
 * @brief show debug message for the net lib
 * in this lib, net_dbg should be used insteadof printf directly.
 * this allow the #define NETDEBUG to easily turn on/off all message
 * from this library
 * 
 * @param format work exactly like printf
 * @param ... 
 */
void net_dbg(const char *format, ...)
{
    if (NETDEBUG)
    {
        printf("NET: ");

        //see: https://sourceware.org/git/?p=glibc.git;a=blob;f=stdio-common/printf.c;h=4c8f3a2a0c38ab27a2eed4d2ff3b804980aa8f9f;hb=3321010338384ecdc6633a8b032bb0ed6aa9b19a
        va_list arg;

        va_start(arg, format);

        // https://www.cplusplus.com/reference/cstdio/vfprintf/
        vfprintf(stdout, format, arg);
        
        va_end(arg);
    }
}

/**
 * @brief 
 * 
 * @param ptr 
 * @return void* 
 */
void * threadProcess(void * ptr) {
    char buffer_in[BUFFERSIZE];
    int len;
    while ((len = read(net_client_sockfd, buffer_in, BUFFERSIZE)) != 0) {
        if (strncmp(buffer_in, "exit", 4) == 0) {
            break;
        }

        net_dbg("receive %d chars\n", len);
        net_dbg("%.*s\n", len, buffer_in);
    }
    close(net_client_sockfd);
    net_dbg("client pthread ended, len=%d\n", len);
}

/**
 * @brief 
 * 
 * @param msg 
 */
void net_thread_process(char * msg[]) {

    pthread_t thread;
    int status = 0;

    // reading pthread creation
    pthread_create(&thread, 0, threadProcess, net_client_sockfd);
    
    //write(connection->sock,"Main APP Still running",15);
    pthread_detach(thread);
    do {
        fgets(msg, 100, stdin);
        //net_dbg("sending : %s\n", msg);
        status = write(net_client_sockfd, msg, strlen(msg));
        //memset(msg,'\0',100);
    } while (status != -1);

}

// ----------------------------------------------
//                     Client
// ----------------------------------------------

/**
 * @brief open the connexion with the server
 * @param addrServer server address IP
 * @param port server port
 */
void net_client_connexion(char * addrServer[], int port) {

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
    if (connect(net_client_sockfd, (struct sockaddr *) &serverAddr, sizeof (serverAddr)) != 0) {
        net_dbg("Fail to connect to server");
        exit(-1);
    };
}
/**
 * @brief The client want to betray the other player
 */
void net_client_betray() {

    net_dbg("%d want to betray", net_client_sockfd);
    write(net_client_sockfd, 'B', 1);

}

/**
 * @brief The client want to collaborate the other player
 */
void net_client_collab() {
    
    net_dbg("%d want to collab", net_client_sockfd);
    write(net_client_sockfd, 'C', 1);
    
}

/**
 * @brief The client want to play
 */
void net_client_acces_request() {

    net_dbg("%d want to play", net_client_sockfd);
    write(net_client_sockfd, 'A', 1);
}

/**
 * @brief The client want to quit the game
 */
void net_client_disconnect() {

    net_dbg("%d want to disconnect", net_client_sockfd);
    write(net_client_sockfd, 'D', 1);
}

// ----------------------------------------------
//                     Server
// ----------------------------------------------

void net_server_init();
void net_server_wait();
void net_server_game_start();
void net_server_round_end();
void net_server_match_end();
