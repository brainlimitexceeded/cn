/*
 *  Here is the starting point for your netster part.1 definitions. Add the
 *  appropriate comment header as defined in the code formatting guidelines
 */

#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netdb.h>
#include <netinet/in.h>
#include <string.h>
#include <sys/types.h>
#include <arpa/inet.h>
#define MAXBYTES 256

void chat_server(char* iface, long port, int use_udp) 
{
  struct addrinfo hints, *result;
//  char addr[256];
  char str[256];
  sprintf(str,"%ld",port);
  memset(&hints, 0, sizeof(hints));
  hints.ai_family = AF_INET;
  if(use_udp == 0) {
        hints.ai_socktype = SOCK_STREAM;
        hints.ai_protocol = IPPROTO_TCP;
 }
  else {
        hints.ai_socktype = SOCK_DGRAM;
        hints.ai_protocol = IPPROTO_UDP;
  }
  hints.ai_flags = AI_PASSIVE;
  getaddrinfo(iface, str, &hints, &result );

if (use_udp == 0) 
{
    	int server_file_descr, socket_new;
  	struct sockaddr_in address;
  	int addrlength = sizeof(address);
	server_file_descr = socket(AF_INET, SOCK_STREAM, 0);
  	address.sin_family = AF_INET;
  	address.sin_addr.s_addr = INADDR_ANY;
  	address.sin_port = htons(port);
  	bind(server_file_descr, (struct sockaddr *)&address, sizeof(address));
  	listen(server_file_descr, 3);
    	int connection_count=0;
    	int flag = 0;
    	while(1){
      		socket_new = accept(server_file_descr, (struct sockaddr *)&address, (socklen_t*)&addrlength);
      		printf("connection %d from ('%s', %d)\n", connection_count, inet_ntoa(address.sin_addr), ntohs(address.sin_port));
      		connection_count+=1;
    		char buffer[256] = {0};
      		while(1) {
        		bzero(buffer, 256);
        		int val = read( socket_new , buffer, 256);
			if(val == -1) {printf("error\n");}
        		printf("got message from ('%s', %d)\n", inet_ntoa(address.sin_addr), ntohs(address.sin_port));
        		if(strcmp(buffer, "hello\n") == 0) {
          			send(socket_new , "world\n" , sizeof("world") , 0 );
        		}
        		else if (strcmp(buffer, "goodbye\n") == 0){
          			send(socket_new , "farewell\n" , sizeof("farewell") , 0 );
          			break;
        		}
        		else if (strcmp(buffer, "exit\n") == 0){
          			send(socket_new , "ok\n" , sizeof("ok") , 0 );
          			close(server_file_descr);
          			flag = 1;
          			break;
        		}	
        		else if(strcmp(buffer, "\n") != 0){
          			send(socket_new , buffer , sizeof(buffer) , 0 );
        		}

      		}
      		if(flag) {
        		break;
      		}
    	}
}
else {
    int socket_file_des, flag=0;
    char buffer[MAXBYTES];
    struct sockaddr_in servaddr, cliaddr;
    socket_file_des = socket(AF_INET, SOCK_DGRAM, 0);

    memset(&servaddr, 0, sizeof(servaddr));
    memset(&cliaddr, 0, sizeof(cliaddr));

    servaddr.sin_family = AF_INET; 
    servaddr.sin_addr.s_addr = INADDR_ANY;
    servaddr.sin_port = htons(port);

    bind(socket_file_des, (const struct sockaddr *)&servaddr, sizeof(servaddr));

    int length;
    length = sizeof(cliaddr);  
    while (1) {
      bzero(buffer, MAXBYTES);
      int n_length = recvfrom(socket_file_des, (char *)buffer, MAXBYTES, MSG_WAITALL, (struct sockaddr *) &cliaddr, (unsigned int *)&length);
      printf("got message from ('%s', %d)\n", inet_ntoa(cliaddr.sin_addr), ntohs(cliaddr.sin_port));
      buffer[n_length] = '\0';
      if(strcmp(buffer, "hello\n") == 0) {
        sendto(socket_file_des, "world\n", sizeof("world"),0, (const struct sockaddr *) &cliaddr,length);
      }
      else if (strcmp(buffer, "goodbye\n") == 0){
        sendto(socket_file_des, "farewell\n", sizeof("farewell"),0, (const struct sockaddr *) &cliaddr,length);
      }
      else if (strcmp(buffer, "exit\n") == 0){
        sendto(socket_file_des, "ok\n", sizeof("ok"),0, (const struct sockaddr *) &cliaddr,length);
        close(socket_file_des);
        break;
      }
      else{
        sendto(socket_file_des, (const char *)buffer, sizeof(buffer),0, (const struct sockaddr *) &cliaddr,length);
        }
      if(flag){
        close(socket_file_des);
      }
    }
}
}

void chat_client(char* iface, long port, int use_udp) {
	struct addrinfo hints, *result;
  //char addr[256];
  char str[256];
  sprintf(str,"%ld",port);
  memset(&hints, 0, sizeof(hints));
  hints.ai_family = AF_INET;
  if(use_udp == 0) {
        hints.ai_socktype = SOCK_STREAM;
        hints.ai_protocol = IPPROTO_TCP;
 }
  else {
        hints.ai_socktype = SOCK_DGRAM;
        hints.ai_protocol = IPPROTO_UDP;
  }
  hints.ai_flags = AI_PASSIVE;
  getaddrinfo(iface, str, &hints, &result );
  struct addrinfo *ptr;
  char buf[INET6_ADDRSTRLEN];
  for ( ptr = result; ptr != NULL; ptr = ptr->ai_next) 
  {
    void* raw_addr;
    if (ptr->ai_family == AF_INET) 
    { 
      struct sockaddr_in* tmp = (struct sockaddr_in*)ptr->ai_addr;
      raw_addr = &(tmp->sin_addr); 
      if (inet_ntop(ptr->ai_family, raw_addr, buf, INET6_ADDRSTRLEN) == NULL) 
      {
        exit(0);
      }
      break;
    }
  }

  int flag=0;
  if(use_udp == 0) {
    int sock = 0;
    struct sockaddr_in serv_addr;
    char buffer[MAXBYTES] = {0};
    sock = socket(AF_INET, SOCK_STREAM, 0);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);
    serv_addr.sin_addr.s_addr = inet_addr(buf);

    connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
    while (1) {
      bzero(buffer, MAXBYTES);
      int n = 0;
      while((buffer[n++] = getchar()) != '\n');
      send(sock , buffer , sizeof(buffer) , 0);
      if (strcmp(buffer,"goodbye\n")==0){
		flag=1; 
	}
      if(strcmp(buffer,"exit\n")==0){
		flag=1;
	}
      bzero(buffer, MAXBYTES);
     int val = read( sock , buffer, 256);
     	if(val == -1) { printf("err\n"); }
      printf("%s",buffer );
      if(flag==1)
          { exit(0);}
    }
  }
  else {
      int socket_file_des;
      char buffer[MAXBYTES];
      struct sockaddr_in servaddr;

      socket_file_des = socket(AF_INET, SOCK_DGRAM, 0);
      memset(&servaddr, 0, sizeof(servaddr));
      servaddr.sin_family = AF_INET;
      servaddr.sin_port = htons(port);
      servaddr.sin_addr.s_addr = inet_addr(buf);
      int length,flag=0;
      while(1){
        bzero(buffer, MAXBYTES);
        int n = 0;
        while((buffer[n++] = getchar()) != '\n');
        sendto(socket_file_des, (const char *)buffer, sizeof(buffer), 0, (const struct sockaddr *) &servaddr, sizeof(servaddr));
              if (strcmp(buffer,"goodbye\n")==0){
		flag = 1;
	      }
	if(strcmp(buffer,"exit\n")==0)
          { 
		  flag=1;
          }
        bzero(buffer, MAXBYTES);
        recvfrom(socket_file_des, (char *)buffer, MAXBYTES,  MSG_WAITALL, (struct sockaddr *) &servaddr,(unsigned int *) &length);
        printf("%s", buffer);
        if (flag==1)
          { close(socket_file_des);
            break; }
      }

  }
}

