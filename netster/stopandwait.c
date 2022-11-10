#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <stdbool.h>
#define MAXBYTES 256
/*
 *  Here is the starting point for your netster part.3 definitions. Add the 
 *  appropriate comment header as defined in the code formatting guidelines
 */
/* Add function definitions */
void stopandwait_server(char* iface, long port, FILE* fp) {
    struct addrinfo hints, *result;
    char str[256];
    sprintf(str, "%ld", port);
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_protocol = IPPROTO_UDP;
    hints.ai_flags = AI_PASSIVE;
    getaddrinfo(iface, str, &hints, &result); 
    int socket_server;
    char buffer[MAXBYTES];
    struct sockaddr_in saddr, caddr;
    socket_server = socket(AF_INET, SOCK_DGRAM, 0);

    memset(&saddr, 0, sizeof(saddr));
    memset(&caddr, 0, sizeof(caddr));

    saddr.sin_family = AF_INET;
    saddr.sin_addr.s_addr = INADDR_ANY;
    saddr.sin_port = htons(port);

    bind(socket_server, (const struct sockaddr *)&saddr, sizeof(saddr));
    int n;
    int length;
    length = sizeof(caddr);
    while(1) {
        n = recvfrom(socket_server,(char *)buffer, sizeof(buffer) , 0, (struct sockaddr *)&caddr, (unsigned int *)&length);
        if(n<=0) {
                break;
        }
	    fwrite(buffer,1,n,fp);
        bzero(buffer,256);
    }
    close(socket_server);
}

void stopandwait_client(char* iface, long port, FILE* fp) {
    struct addrinfo hints, *result;
    char str[256];
    sprintf(str, "%ld", port);
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;  
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_protocol = IPPROTO_UDP;
    hints.ai_flags = AI_PASSIVE;
    getaddrinfo(iface, str, &hints, &result);
    struct addrinfo *current;
    char adr[INET6_ADDRSTRLEN];
    for (current = result; current != NULL; current = current->ai_next) {
        void *raw_addr;
        if (current->ai_family == AF_INET) {
            struct sockaddr_in *tmp = (struct sockaddr_in *)current->ai_addr;
            raw_addr = &(tmp->sin_addr);
            if (inet_ntop(current->ai_family, raw_addr, adr, INET6_ADDRSTRLEN) == NULL) {
                exit(0);
            }
            break;
        }
    }
    int socket_server;
    char buffer[MAXBYTES];
    struct sockaddr_in saddr;

    socket_server = socket(AF_INET, SOCK_DGRAM, 0);
    memset(&saddr, 0, sizeof(saddr));
    saddr.sin_family = AF_INET;
    saddr.sin_port = htons(port);
    saddr.sin_addr.s_addr = inet_addr(adr);
    int bufsize;
    bzero(buffer,256);
    while( ( bufsize = fread(buffer,1,256,fp)  )>0 ) {
        sendto(socket_server,(char *)buffer, bufsize,0,(const struct sockaddr *)&saddr,sizeof(saddr));
    }
    bzero(buffer,256);
    sendto(socket_server,(char *)buffer, strlen(buffer),0,(const struct sockaddr *)&saddr,sizeof(saddr));
    close(socket_server);
}
