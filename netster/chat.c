/*
 *  Here is the starting point for your netster part.1 definitions. Add the 
 *  appropriate comment header as defined in the code formatting guidelines
 */
#include <string.h>
#include <stdbool.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <unistd.h> // for close
/* Add function definitions */
void chat_server(char* iface, long port, int use_udp) {
	if(use_udp == 0) {
    		int server_socket;
    		struct sockaddr_in server_addr;
    		int client_socket;
    		struct sockaddr_in client_addr;
    		int bind_status, listen_status;


    		server_socket = socket(AF_INET, SOCK_STREAM, 0);   
    		if (server_socket < 0)
    		{
        		printf("Error in socket\n");
        	}
    
    		server_addr.sin_family = AF_INET;
    		server_addr.sin_port = htons(port);
    		server_addr.sin_addr.s_addr = inet_addr(iface);

    
    		bind_status = bind(server_socket, (struct sockaddr *) &server_addr, sizeof(server_addr));
    		if (bind_status < 0)
    		{
        			printf("Error binding socket%d\n",bind_status);
        	}

		listen_status = listen(server_socket, 10);
    		if (listen_status == -1)
    		{
        		printf("Error listening to socket\n");
    		}
    		bool flag = true;
    
    		int count = -1;
   	
    		socklen_t addr_len;
    		addr_len = sizeof(client_socket);
    		client_socket = accept(server_socket, (struct sockaddr *) &client_addr, &addr_len);

 		if (client_socket < 0){
        		printf("Can't accept client connection\n");
    		}
 		count = count + 1;
 		printf("connection %d from (",count);
		printf("'%d.%d.%d.%d',%d)\n",
  		(int) (client_addr.sin_addr.s_addr&0xFF),
  		(int) ((client_addr.sin_addr.s_addr&0xFF00)>>8),
  		(int) ((client_addr.sin_addr.s_addr&0xFF0000)>>16),
 		 (int) ((client_addr.sin_addr.s_addr&0xFF000000)>>24), (int)ntohs(client_addr.sin_port) );
 		char server_message[1024], client_message[1024];
		while(flag) {
			if (recv(client_socket, client_message, sizeof(client_message), 0) < 0){
			        printf("Couldn't receive\n");
			}
    			printf("got message from ('%d.%d.%d.%d',%d) %s\n", (int) (client_addr.sin_addr.s_addr&0xFF) , (int)((client_addr.sin_addr.s_addr&0xFF00)>>8) , (int) ((client_addr.sin_addr.s_addr&0xFF0000)>>16) , (int)((client_addr.sin_addr.s_addr&0xFF000000)>>24) ,ntohs(client_addr.sin_port), client_message);
			strcpy(server_message,client_message);
			int res = -1;
			char temp_message[1024];
			strcpy(temp_message,client_message);
			//int result = tolower(&temp_message);
	 		if(strstr(temp_message, "hello")) {
           			strcpy(server_message,"world\n");
	  
	  			res = send(client_socket, server_message, strlen(server_message), 0);
 		
   			 }
	 		else if( strstr(temp_message, "goodbye") ){
	 			strcpy(server_message,"farewell\n");
	   			res = send(client_socket, server_message, strlen(server_message), 0);
				close(client_socket);
    	   			addr_len = sizeof(client_socket);
           			client_socket = accept(server_socket, (struct sockaddr *) &client_addr, &addr_len);

 				if (client_socket < 0){
        				printf("Can't accept\n");
    				}
 				count = count + 1;
 				printf("connection %d from ('%s', %d)\n",count, inet_ntoa(client_addr.sin_addr),(int) ntohs(client_addr.sin_port));
				printf("%d.%d.%d.%d\n", (int) (client_addr.sin_addr.s_addr&0xFF),(int) ((client_addr.sin_addr.s_addr&0xFF00)>>8),(int) ((client_addr.sin_addr.s_addr&0xFF0000)>>16),(int) ((client_addr.sin_addr.s_addr&0xFF000000)>>24));
	 		}
	 		else if( strstr(temp_message, "exit") ){
                		strcpy(server_message,"ok\n");
	   			res = send(client_socket, server_message, strlen(server_message), 0);
				flag = false;
         		}
	 		else {
	   			res = send(client_socket, server_message, strlen(server_message), 0);
			 }
	 		if(res == -1){
	 			printf("client closed\n");
	 		}

		}
	    	close(client_socket);
    	  	close(server_socket);
	}  
	else {
	}
}

void chat_client(char* host, long port, int use_udp) {
  if(use_udp == 0) {
    int socket_c;
    struct sockaddr_in server_addr;
    char server_message[2000], client_message[2000];
    
    memset(server_message,'\0',sizeof(server_message));
    memset(client_message,'\0',sizeof(client_message));
    
    socket_c = socket(AF_INET, SOCK_STREAM, 0);
    
    if(socket_c < 0){
        printf("Error in socket\n");
      
    }
    
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = inet_addr(host);
    
    if(connect(socket_c, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0){
        printf("Error to connect with server\n");
    }

    bool flag = true;
    while(flag) {
	printf("Enter a message\n");
	int s = scanf("%s",client_message);
    	if(s) {
		strcat(client_message,"\n");
	}

    if(send(socket_c, client_message, strlen(client_message), 0) < 0){
        printf("Error to send message\n");
    }
    
    if(recv(socket_c, server_message, sizeof(server_message), 0) < 0){
        printf("Error while receiving server's msg\n");
    }
    
    printf("Server's response: %s",server_message);
    //int r = tolower(server_message)
    if( strstr(server_message, "ok") || strstr(server_message, "farewell")  ) {
    	flag = false;
    }
    }
   
    close(socket_c);
    
    
  }
  else {
  	
  }
}
