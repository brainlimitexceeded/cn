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
char client_message[1024];
char buffer[1024];
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
  while(flag) {
  	recv(newSocket , client_message , 1024 , 0);
	printf("got message from ('%d.%d.%d.%d',%d), %s\n", (int) (client_addr.sin_addr.s_addr&0xFF) , (int)((client_addr.sin_addr.s_addr&0xFF00)>>8) , (int) ((client_addr.sin_addr.s_addr&0xFF0000)>>16) , (int)((client_addr.sin_addr.s_addr&0xFF000000)>>24) ,ntohs(client_addr.sin_port), client_message); 
  	pthread_mutex_lock(&lock);
  	char *message = malloc(sizeof(client_message)+20);
  	if(strstr(client_message, "hello")) {
		strcpy(message,"world\n");
 	 }
  	else if(strstr(client_message, "goodbye")) {
		flag = false;
		strcpy(message,"farewell\n");
 	 }
  	else if( strstr(client_message, "exit") ){
	  	flag = false;
	  	serve = false;
	  	strcpy(message,"ok\n");
  	}
  	else {
    		strcpy(message,client_message);
    		strcat(message,"\n");
 	 }
  	strcpy(buffer,message);
  	free(message);
  	pthread_mutex_unlock(&lock);
  	sleep(1);
  	send(newSocket,buffer,13,0);
  	memset(client_message,'\0',sizeof(client_message));
  //memset(server_message,'\0',sizeof(server_message));
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
   char serverMessage[1024], clientMessage[1024];
   struct sockaddr_in client_addr;
   int len = sizeof(client_addr);
  socklen_t addr_size;
 
  if(use_udp == 0)
	serverSocket = socket(AF_INET, SOCK_STREAM, 0);
  else
	serverSocket = socket(AF_INET,SOCK_DGRAM,0);

  serverAddr.sin_family = AF_INET;
 
  serverAddr.sin_port = htons(port);

  serverAddr.sin_addr.s_addr = inet_addr(iface);

  memset(serverAddr.sin_zero, '\0', sizeof serverAddr.sin_zero);
 
  bind(serverSocket, (struct sockaddr *) &serverAddr, sizeof(serverAddr));
 
  if(use_udp == 0) {
  	if(listen(serverSocket,50)==0)
    		printf("Listening\n");
  	else
    		printf("Error\n");
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
		printf("got message from ('%d.%d.%d.%d',%d), %s\n", (int) (client_addr.sin_addr.s_addr&0xFF) , (int)((client_addr.sin_addr.s_addr&0xFF00)>>8) , (int) ((client_addr.sin_addr.s_addr&0xFF0000)>>16) , (int)((client_addr.sin_addr.s_addr&0xFF000000)>>24) ,ntohs(client_addr.sin_port), clientMessage);
		if(strstr(clientMessage, "hello")) {
           		strcpy(serverMessage,"world\n");
   		}
	 	else if( strstr(clientMessage, "goodbye") ){
	 		strcpy(serverMessage,"farewell\n");
		}
	 	else if( strstr(clientMessage, "exit") ){
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
  
    int socket_c;
    struct sockaddr_in server_addr;
    char serverMessage[1024], clientMessage[1024];
    //memset(serverMessage,'\0',sizeof(serverMessage));
    //memset(clientMessage,'\0',sizeof(clientMessage));
    if(use_udp == 0) {
    	socket_c = socket(PF_INET, SOCK_STREAM, 0);
    }
    else {
    	socket_c = socket(AF_INET, SOCK_DGRAM,0);
    }
    if(socket_c < 0){
        printf("Error in socket\n");
      
    }
    
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = inet_addr(host);
    if(use_udp == 0) {
    if(connect(socket_c, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0){
        printf("Error to connect with server\n");
    }
    //printf("%d\n",serve);
    }
    bool flag = true;
    while(flag) {
	memset(serverMessage,'\0',sizeof(serverMessage));
    	memset(clientMessage,'\0',sizeof(clientMessage));
	int s = scanf("%s",clientMessage);
    	if(s) {
		strcat(clientMessage,"\n");
	}
    	if(use_udp == 0) {
    		if(send(socket_c, clientMessage, strlen(clientMessage), 0) < 0){
        		printf("Error to send message\n");
    		}
     		if(recv(socket_c, serverMessage, sizeof(serverMessage), 0) < 0){
        		printf("Error while receiving server's msg\n");
    		}
    	}
    	else {
    		if( sendto(socket_c, clientMessage, strlen(clientMessage),0, (struct sockaddr*)&server_addr, sizeof(server_addr)) <0 ) {
			printf("Error sending message\n");
		}
		int len = sizeof(server_addr);
	 	if(recvfrom(socket_c, serverMessage, sizeof(serverMessage), 0, (struct sockaddr*)&server_addr, (unsigned int *)&len) < 0){
        		printf("Error while receiving server's msg\n");
    		}
    	}
    	printf("%s",serverMessage);
    	if( strstr(serverMessage, "ok") || strstr(serverMessage, "farewell")  ) {
    		flag = false;
    	}
    }
    close(socket_c);
 }
