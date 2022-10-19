#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <string.h>
#include <stdbool.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <unistd.h>
#include<stdlib.h>
#include<netinet/in.h>
#include <fcntl.h>
#include<pthread.h>
/* Add function definitions */
bool serve;
struct sockaddr_in clientAddr;
int mainSocket, serverSocket;
//struct sockaddr_in clientAddr;
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

void * socketThread(void *arg)
{
  struct sockaddr_in client_addr = clientAddr;
  int newSocket = *((int *)arg);
  bool flag = true;
  char client_message[256];
  char buffer[256];
  while(flag) {
  	recv(newSocket , client_message , 256 , 0);
	printf("got message from ('%d.%d.%d.%d',%d), %s", (int) (client_addr.sin_addr.s_addr&0xFF) , (int)((client_addr.sin_addr.s_addr&0xFF00)>>8) , (int) ((client_addr.sin_addr.s_addr&0xFF0000)>>16) , (int)((client_addr.sin_addr.s_addr&0xFF000000)>>24) ,ntohs(client_addr.sin_port), client_message); 
  	pthread_mutex_lock(&lock);
  	char *message = malloc(sizeof(client_message)+20);
  	if(strcmp(client_message, "hello\n") == 0) {
		strcpy(message,"world\n");
 	 }
	else if(strcmp(client_message, "goodbye\n") == 0) {
		flag = false;
		strcpy(message,"farewell\n");
 	 }
  	else if( strcmp(client_message, "exit\n") == 0 ){
	  	flag = false;
	  	serve = false;
	  	strcpy(message,"ok\n");
  	}
  	else {
    		strcpy(message,client_message);
	//	strcat(message,"\n");
 	 }
  	strcpy(buffer,message);
  	free(message);
  	pthread_mutex_unlock(&lock);
  	sleep(1);
  	send(newSocket,buffer,strlen(buffer),0);
  	memset(client_message,'\0',sizeof(client_message));
        memset(buffer,'\0',sizeof(buffer));
  }
  close(newSocket);
  if(serve == false) {
	close(serverSocket);
	exit(0);
  }
  pthread_exit(NULL);
}
void chat_server(char* iface, long port, int use_udp) {
  int  newSocket;
  int serverSocket;
  struct sockaddr_in serverAddr, serverStorage;
   char serverMessage[256], clientMessage[256];
   struct sockaddr_in client_addr;
   int len = sizeof(client_addr);
  socklen_t addr_size;
 

//here
struct addrinfo hints, *result;
  char addr[256];
  char str[256];
  sprintf(str,"%ld",port);
  memset(&hints, 0, sizeof(hints));
  hints.ai_family = PF_UNSPEC;
  if(use_udp == 0) {
        hints.ai_socktype = SOCK_STREAM;
        hints.ai_protocol = IPPROTO_TCP;
 }
  else {
        hints.ai_socktype = SOCK_DGRAM;
        hints.ai_protocol = IPPROTO_UDP;
  }
  hints.ai_flags = AI_PASSIVE;
  int err = getaddrinfo(iface, str, &hints, &result );
  if(err!= 0) {
        perror("getaddrinfo");
  }
 //while(result) {
  inet_ntop(result->ai_family, result->ai_addr->sa_data,addr,256);
  void* raw_addr;
if (result->ai_family == AF_INET) { // Address is IPv4
  struct sockaddr_in* tmp = (struct sockaddr_in*)result->ai_addr; // Cast addr into AF_INET container
  raw_addr = &(tmp->sin_addr); // Extract the address from the container
}
else { // Address is IPv6
  struct sockaddr_in6* tmp = (struct sockaddr_in6*)result->ai_addr; // Cast addr into AF_INET6 container
  raw_addr = &(tmp->sin6_addr); // Extract the address from the container
}
inet_ntop(result->ai_family, raw_addr, addr, 256);
//printf("IPv%d %s\n", result->ai_family == AF_INET6?6:4, addr);
//result = result->ai_next;
//}
//here

  if(use_udp == 0)
	serverSocket = socket(AF_INET, SOCK_STREAM, 0);
  else
	serverSocket = socket(AF_INET,SOCK_DGRAM,0);

  serverAddr.sin_family = result->ai_family;
 
  serverAddr.sin_port = htons(port);

  serverAddr.sin_addr.s_addr = inet_addr(addr);

  memset(serverAddr.sin_zero, '\0', sizeof serverAddr.sin_zero);
 
  bind(serverSocket, (struct sockaddr *) &serverAddr, sizeof(serverAddr));
 
  if(use_udp == 0) {
 	listen(serverSocket,50);
   }
   pthread_t tid[60];
   int i = 0;
   serve = true;
   mainSocket = serverSocket;
   while(serve)
    {
	if(use_udp == 0) {
        	addr_size = sizeof serverStorage;
        	newSocket = accept(serverSocket, (struct sockaddr *) &serverStorage, &addr_size);
		printf("connection %d from (",i);
		printf("'%d.%d.%d.%d',%d)\n",
  		(int) (serverStorage.sin_addr.s_addr&0xFF),
  		(int) ((serverStorage.sin_addr.s_addr&0xFF00)>>8),
  		(int) ((serverStorage.sin_addr.s_addr&0xFF0000)>>16),
  		(int) ((serverStorage.sin_addr.s_addr&0xFF000000)>>24), (int)ntohs(serverStorage.sin_port) );
       		clientAddr = serverStorage;
        	if( pthread_create(&tid[i++], NULL, socketThread, &newSocket) != 0 )
           		printf("Failed to create thread\n");
		if(i >= 50)
        	{
			i = 0;
          		while(i < 50 )
          		{
            			pthread_join(tid[i++],NULL);
          		}
          		i = 0;
        	}
	}
	else {
		recvfrom(serverSocket, clientMessage, sizeof(clientMessage), 0, (struct sockaddr *)&client_addr,(unsigned int *)&len ); 
		printf("got message from ('%d.%d.%d.%d',%d), %s", (int) (client_addr.sin_addr.s_addr&0xFF) , (int)((client_addr.sin_addr.s_addr&0xFF00)>>8) , (int) ((client_addr.sin_addr.s_addr&0xFF0000)>>16) , (int)((client_addr.sin_addr.s_addr&0xFF000000)>>24) ,ntohs(client_addr.sin_port), clientMessage);
		if(strcmp(clientMessage, "hello\n") == 0) {
           		strcpy(serverMessage,"world\n");
   		}
	 	else if( strcmp(clientMessage, "goodbye\n") == 0 ){
	 		strcpy(serverMessage,"farewell\n");
		}
	 	else if( strcmp(clientMessage, "exit\n") == 0 ){
			strcpy(serverMessage,"ok\n");
  			serve = false;
         	}
		else {
			strcpy(serverMessage,clientMessage);
		 }
		sendto(serverSocket, serverMessage, strlen(serverMessage), 0 , (struct sockaddr*)&client_addr, sizeof(client_addr) );	 		
		memset(clientMessage,'\0',sizeof(clientMessage));
		memset(serverMessage,'\0',sizeof(serverMessage));
	}
    }
   close(serverSocket);
}
void chat_client(char* host, long port, int use_udp) {
    //printf("%ld",port); 
    int socket_c;
    struct sockaddr_in server_addr;
    char serverMessage[256], clientMessage[256];

//here
struct addrinfo hints, *result;
  char addr[100];
  char str[256];
  sprintf(str,"%ld",port);
  memset(&hints, 0, sizeof(hints));
  hints.ai_family = PF_UNSPEC;
  if(use_udp == 0) {
  	hints.ai_socktype = SOCK_STREAM;
        hints.ai_protocol = IPPROTO_TCP;
 }
  else {
	hints.ai_socktype = SOCK_DGRAM;
	hints.ai_protocol = IPPROTO_UDP;
  }
  hints.ai_flags = AI_PASSIVE;
  int err = getaddrinfo(host, str, &hints, &result );
  if(err!= 0) {
  	perror("getaddrinfo");
  }
 //while(result) {
  inet_ntop(result->ai_family, result->ai_addr->sa_data,addr,100);
  void* raw_addr;
if (result->ai_family == AF_INET) { // Address is IPv4
  struct sockaddr_in* tmp = (struct sockaddr_in*)result->ai_addr; // Cast addr into AF_INET container
  raw_addr = &(tmp->sin_addr); // Extract the address from the container
}
else { // Address is IPv6
  struct sockaddr_in6* tmp = (struct sockaddr_in6*)result->ai_addr; // Cast addr into AF_INET6 container
  raw_addr = &(tmp->sin6_addr); // Extract the address from the container
}
inet_ntop(result->ai_family, raw_addr, addr, 100);
//printf("IPv%d %s\n", result->ai_family == AF_INET6?6:4, addr);
//result = result->ai_next;
//}
//here

    //memset(serverMessage,'\0',sizeof(serverMessage));
    //memset(clientMessage,'\0',sizeof(clientMessage));
    if(use_udp == 0) {
    	socket_c = socket(result->ai_family, SOCK_STREAM, 0);
    }
    else {
    	socket_c = socket(result->ai_family, SOCK_DGRAM,0);
    }
    server_addr.sin_family = result->ai_family;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = inet_addr(addr);
    if(use_udp == 0) {
	//connect(socket_c,result->ai_addr, result->ai_addrlen);
    	connect(socket_c, (struct sockaddr*)&server_addr, sizeof(server_addr));
    //printf("%d\n",serve);
    }
    bool flag = true;
    while(flag) {
	memset(serverMessage,'\0',sizeof(serverMessage));
    	memset(clientMessage,'\0',sizeof(clientMessage));
	char *s = fgets(clientMessage, 256, stdin);
	//int s = scanf("%s[^\n]",clientMessage);
    	if(s != NULL) {
	//	strcat(clientMessage,"\n");
	}
    	if(use_udp == 0) {
    		send(socket_c, clientMessage, strlen(clientMessage), 0);
     		recv(socket_c, serverMessage, sizeof(serverMessage), 0);
    	}
    	else {
    		sendto(socket_c, clientMessage, strlen(clientMessage),0, (struct sockaddr*)&server_addr, sizeof(server_addr));
		int len = sizeof(server_addr);
	 	recvfrom(socket_c, serverMessage, sizeof(serverMessage), 0, (struct sockaddr*)&server_addr, (unsigned int *)&len);
    	}
    	printf("%s",serverMessage);
    	if( strcmp(serverMessage, "ok\n") == 0 || strcmp(serverMessage, "farewell\n") == 0  ) {
    		flag = false;
    	}
    }
    close(socket_c);
 }
