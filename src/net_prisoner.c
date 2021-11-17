#include "net_prisoner.h"

/**
 * @brief 
 */
int net_client_connexion(char * addrClient[]) {
    
    int sockfd;

    struct sockaddr_in serverAddr;
    int port = 7799;

    // Create the socket. 
    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    //Configure settings of the server address
    // Address family is Internet 
    serverAddr.sin_family = AF_INET;
    //Set port number, using htons function 
    serverAddr.sin_port = htons(port);
    //Set IP address to localhost
    serverAddr.sin_addr.s_addr = inet_addr(addrClient);

    memset(serverAddr.sin_zero, '\0', sizeof serverAddr.sin_zero);

    //Connect the socket to the server using the address
    if (connect(sockfd, (struct sockaddr *) &serverAddr, sizeof (serverAddr)) != 0) {
        printf("Fail to connect to server");
        exit(-1);
    };

    return sockfd;
}

void net_client_betray();
void net_client_collab();
void net_client_acces_request();
void net_client_disconnect();

void net_dbg(char text[])
{
    printf("%s", text);
}
