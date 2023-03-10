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
#include <sys/time.h>
#define MAXBYTES 1024
/*
 *  Here is the starting point for your netster part.4 definitions. Add the 
 *  appropriate comment header as defined in the code formatting guidelines
 */

/* Add function definitions */
//Referred to psuedo code online 
typedef struct  {
int sequence;
int len;
} header;
void gbn_server(char* iface, long port, FILE* fp) {
    struct addrinfo hints, *result;
    header rdt_server;
    char str[MAXBYTES];
    sprintf(str, "%ld", port);
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_protocol = IPPROTO_UDP;
    hints.ai_flags = AI_PASSIVE;
    getaddrinfo(iface, str, &hints, &result); 
    int socket_server;
    char buffer[MAXBYTES];
    int op = 1;
    struct sockaddr_in saddr, caddr;
    socket_server = socket(AF_INET, SOCK_DGRAM, 0);
    setsockopt(socket_server, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &op, sizeof(op));
    memset(&saddr, 0, sizeof(saddr));
    memset(&caddr, 0, sizeof(caddr));
    //printf("%d\n",socket_server);
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
	//printf("goes here\n");
        n = recvfrom(socket_server,(char *)buffer, MAXBYTES , 0, (struct sockaddr *)&caddr, (unsigned int *)&length);
        //printf("%d\n",n);
	    if(n<=0) {
            continue;
        }
        bzero(&rdt_server.sequence,sizeof(rdt_server.sequence));
       	memcpy(&rdt_server, buffer, sizeof(rdt_server));
	    char s[100];
        if(sequence_id+1==rdt_server.sequence){
            if(rdt_server.len < MAXBYTES-8){
                flag = false;
            }
            memcpy(&temp, buffer + sizeof(rdt_server), sizeof(temp));
            fwrite(temp,1,rdt_server.len,fp);
            sprintf(s, "%d", rdt_server.sequence);
            sendto(socket_server, s, sizeof(s),0, (const struct sockaddr *)&caddr,length);
            //bzero(buffer,MAXBYTES);
            if(!flag) {
                int times = 3;
                while(times>=0 && !sendto(socket_server, s, sizeof(s),0, (const struct sockaddr *)&caddr,length) ) {
                    --times;
                }
	        }
            sequence_id+=1;
        }
        else {
            sprintf(s,"%d",sequence_id);
            sendto(socket_server, s, sizeof(s),0, (const struct sockaddr *)&caddr,length);
        }

    }
    close(socket_server);
}

void gbn_client(char* iface, long port, FILE* fp) {
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
    struct sockaddr_in saddr,caddr;
    struct timeval t,s,e;
    bool flag = true;
    int length, temp_size;
    char temp[MAXBYTES-8];
    int start = 0;
    int n = 0;
    long end = 5000;
    int window = 10;
    char message[MAXBYTES*2][MAXBYTES];
    int seq = -1;
    socket_server = socket(AF_INET, SOCK_DGRAM, 0);
    memset(&saddr, 0, sizeof(saddr));
    memset(&caddr, 0, sizeof(caddr));
    length = sizeof(caddr);
    saddr.sin_family = AF_INET;
    saddr.sin_port = htons(port);
    saddr.sin_addr.s_addr = inet_addr(adr);
    t.tv_sec = 0;
    t.tv_usec = 5000;
    bzero(buffer,MAXBYTES);
    header h;
    setsockopt(socket_server, SOL_SOCKET, SO_RCVTIMEO, &t,sizeof(t));
    
    while(flag) {
	//printf("%d\n",window);
        if(window+start>n) {
            if(seq<0) {
                bzero(&temp, MAXBYTES-8);
                temp_size = fread(&temp,1,MAXBYTES-8,fp);
                h.len = temp_size;
                h.sequence = n;
                bzero(buffer,MAXBYTES);
                memcpy(buffer, &h, sizeof(h));
                memcpy(buffer + sizeof(h), &temp, temp_size);
                bzero(&message[n], MAXBYTES);
                memcpy(message[n], &buffer, sizeof(buffer));

                sendto(socket_server, &buffer, sizeof(buffer), 0, (const struct sockaddr *)&saddr, sizeof(saddr));
                if(start == n) {
                    gettimeofday(&s,NULL);
                }
                if(temp_size<MAXBYTES-8) {
                    seq = n;
                }
                n+=1;
            }
        }
        int r = recvfrom(socket_server, (char *)buffer, MAXBYTES, MSG_WAITALL, (struct sockaddr *)&caddr, (unsigned int *)&length);
        if(r>0 && atoi(buffer) != -1 ) {
            start=atoi(buffer)+1;
            if(seq == start-1) {
                flag = false;
            }
        }
        gettimeofday(&e, NULL);
        if( (e.tv_usec-s.tv_usec)+(e.tv_sec-s.tv_sec)*1000000 <= end) {
            ++window;
            continue;
        }
        if(window>1) {
            window/=2;
        }
        gettimeofday(&s,NULL);
        int value = start;
        while(value<n) {
            sendto(socket_server, &message[value], sizeof(message[value]), 0, (const struct sockaddr *)&saddr, sizeof(saddr));
            ++value;
	    }
    }
    close(socket_server);
}


