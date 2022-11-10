#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
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
typedef struct  {
int sequence;
int len;
} header;
void stopandwait_server(char* iface, long port, FILE* fp) {
    struct addrinfo hints, *result;
    header rdt_server;
    char str[MAXBYTES];
    // sprintf(str, "%ld", port);
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
    int sequence_id=-1;
    bool flag=true;
    char temp[MAXBYTES-8];
    length = sizeof(caddr);
    bzero(buffer,MAXBYTES);
    while(flag) {
        n = recvfrom(socket_server,(char *)buffer, sizeof(buffer) , 0, (struct sockaddr *)&caddr, (unsigned int *)&length);
        if(n<=0) {
            break;
        }
        memcpy(&rdt_server, buffer, sizeof(rdt_server));
        if (rdt_server.len < MAXBYTES-8){
            flag = false;
        }
	    //fwrite(buffer,1,n,fp);
        // bzero(buffer,256);
        if(sequence_id!=rdt_server.sequence){
            memcpy(&temp, buffer + sizeof(rdt_server), sizeof(temp));
            fwrite(temp,1,rdt_server.len,fp);
            char str[100];
            sprintf(str, "%d", rdt_server.sequence);
            sendto(socket_server, str, sizeof(str),0, (const struct sockaddr *)&caddr,length);
            bzero(buffer,MAXBYTES);
            sequence_id = rdt_server.sequence;
        }
    }
    close(socket_server);
}

void stopandwait_client(char* iface, long port, FILE* fp) {
    struct addrinfo hints, *result;
    char str[MAXBYTES];
    sprintf(str, "%ld", port);
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;  
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_protocol = IPPROTO_UDP;
    hints.ai_flags = AI_PASSIVE;
    getaddrinfo(iface, str, &hints, &result);
    struct addrinfo *current;
    char adr[INET6_ADDRSTRLEN];
    int packet_count = 1;
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
    struct timeval t;
    bool flag = true;
    // int bufsize;
    int length, temp_size;
    char temp[MAXBYTES-8];

    socket_server = socket(AF_INET, SOCK_DGRAM, 0);
    memset(&saddr, 0, sizeof(saddr));
    saddr.sin_family = AF_INET;
    saddr.sin_port = htons(port);
    saddr.sin_addr.s_addr = inet_addr(adr);
    t.tv_sec = 0;
    t.tv_usec = 5000;
    bzero(buffer,MAXBYTES);
    setsockopt(socket_server, SOL_SOCKET, SO_RCVTIMEO, &t,sizeof(t));
    while(flag) {
        temp_size = fread(temp, 1, MAXBYTES-8, fp);
        header rdtheader;
        rdtheader.len = temp_size;
        rdtheader.sequence = packet_count;
        bzero(buffer, MAXBYTES);
        memcpy(buffer , &rdtheader , sizeof(rdtheader));
        memcpy(buffer + sizeof(rdtheader), &temp , rdtheader.len);
        while (1)
        {
            sendto(socket_server, buffer, sizeof(buffer), 0, (const struct sockaddr *)&saddr, sizeof(saddr));
            recvfrom(socket_server, (char *)buffer, MAXBYTES, MSG_WAITALL, (struct sockaddr *)&saddr, (unsigned int *)&length);
            if (atoi(buffer) == packet_count)
            {
                break;
            }
        }
        if (temp_size < MAXBYTES-8)
        {
            flag = false;
        }
        packet_count = (packet_count+1)%2;
    }
    // while( ( bufsize = fread(buffer,1,256,fp)  )>0 ) {
    //     sendto(socket_server,(char *)buffer, bufsize,0,(const struct sockaddr *)&saddr,sizeof(saddr));
    // }
    // bzero(buffer,256);
    // sendto(socket_server,(char *)buffer, strlen(buffer),0,(const struct sockaddr *)&saddr,sizeof(saddr));
    close(socket_server);
}
