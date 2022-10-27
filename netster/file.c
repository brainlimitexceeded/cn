#include "file.h"
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
 *  Here is the starting point for your netster part.2 definitions. Add the 
 *  appropriate comment header as defined in the code formatting guidelines
 */

/* Add function definitions */
void file_server(char* iface, long port, int use_udp, FILE* fp) {
struct addrinfo hints, *result;
  //  char addr[256];
  char str[256];
  sprintf(str, "%ld", port);
  memset(&hints, 0, sizeof(hints));
  hints.ai_family = AF_INET;
  if (use_udp == 0) {
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
  } else {
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_protocol = IPPROTO_UDP;
  }
  hints.ai_flags = AI_PASSIVE;
  getaddrinfo(iface, str, &hints, &result); 
if(use_udp == 0) {
    int server_socket, client_socket;
    struct sockaddr_in server_addr;
    int addrlength = sizeof(server_addr);
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(port);
    bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr));
    listen(server_socket, 3);
    client_socket = accept(server_socket, (struct sockaddr *)&server_addr,
                          (socklen_t *)&addrlength);
    if(client_socket == -1) {
    	printf("Error connecting to client");
    }
    int n;
    char buffer[256];
    while(1) {
    	n = recv(server_socket,buffer,256,0);
	if(n<=0) {
		break;
		return;
	}
	fprintf(fp,"%s", buffer);
	bzero(buffer,256);
    }
    close(server_socket);
}
else {

}
}
void file_client(char* iface, long port, int use_udp, FILE* fp) {
  struct addrinfo hints, *result;
  // char addr[256];
  char str[256];
  sprintf(str, "%ld", port);
  memset(&hints, 0, sizeof(hints));
  hints.ai_family = AF_INET;
  if (use_udp == 0) {
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
  } else {
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_protocol = IPPROTO_UDP;
  }
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
if(use_udp == 0) {
int sock = 0;
    struct sockaddr_in serv_addr;
    char buffer[MAXBYTES] = {0};
    sock = socket(AF_INET, SOCK_STREAM, 0);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);
    serv_addr.sin_addr.s_addr = inet_addr(adr);

    connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
    int n;
    while(fgets(buffer,256,fp)!= NULL) {
    	send(sock,buffer, sizeOf(buffer),0);
	bzero(buffer,256);
    }
    close(sock);
}
else {

}
}
