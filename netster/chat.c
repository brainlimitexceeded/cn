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
	
    		int server_socket;
    		struct sockaddr_in server_addr;
    		int client_socket;
    		struct sockaddr_in client_addr;
    		int bind_status, listen_status;
		int len = sizeof(client_addr);
		    char server_message[1024], client_message[1024];

    memset(server_message,'\0',sizeof(server_message));
    memset(client_message,'\0',sizeof(client_message));
		//memset(&client_addr,0,sizeof(client_addr));
	//	memset(server_message,'\0',sizeof(server_message));
    	//	memset(client_message,'\0',sizeof(client_message));
		if(use_udp == 0) {
    			server_socket = socket(AF_INET, SOCK_STREAM, 0);   
		}
		else {
			server_socket = socket(AF_INET,SOCK_DGRAM, 0);
		}
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
		if(use_udp == 0) {
			listen_status = listen(server_socket, 10);
    			if (listen_status == -1)
    			{
        			printf("Error listening to socket\n");
    			}
		}
    		bool flag = true;
    
    		int count = -1;
   	
    		socklen_t addr_len;
    		addr_len = sizeof(client_socket);
		if(use_udp == 0) {
    		client_socket = accept(server_socket, (struct sockaddr *) &client_addr, &addr_len);

 		if (client_socket < 0){
        		printf("Can't accept client connection\n");
    		}
		}
		else {
			client_socket = server_socket;
		}
 		count = count + 1;
		if(use_udp == 0) {
 			printf("connection %d from (",count);
			printf("'%d.%d.%d.%d',%d)\n",
  			(int) (client_addr.sin_addr.s_addr&0xFF),
  			(int) ((client_addr.sin_addr.s_addr&0xFF00)>>8),
  			(int) ((client_addr.sin_addr.s_addr&0xFF0000)>>16),
 		 	(int) ((client_addr.sin_addr.s_addr&0xFF000000)>>24), (int)ntohs(client_addr.sin_port) );
		}
		while(flag) {
			//char server_message[1024], client_message[1024];
		//	    memset(client_message,'\0',sizeof(client_message));
    		//		memset(server_message,'\0',sizeof(server_message));
			if(use_udp == 0) {
			if (recv(client_socket, client_message, sizeof(client_message), 0) < 0){
			        printf("Couldn't receive\n");
			}
			}
			else {
				if( recvfrom(server_socket, client_message, sizeof(client_message), 0, (struct sockaddr *)&client_addr,(unsigned int *)&len ) < 0 ) {
					printf("Error in receiving message\n");
				}
			}
			                                printf("got message from ('%d.%d.%d.%d',%d) %s\n", (int) (client_addr.sin_addr.s_addr&0xFF) , (int)((client_addr.sin_addr.s_addr&0xFF00)>>8) , (int) ((client_addr.sin_addr.s_addr&0xFF0000)>>16) , (int)((client_addr.sin_addr.s_addr&0xFF000000)>>24) ,ntohs(client_addr.sin_port), client_message);
			int res = -1;
			//char temp_message[1024];
			//strcpy(temp_message,client_message);
			//int result = tolower(&temp_message);
	 		if(strstr(client_message, "hello")) {
           			strcpy(server_message,"world\n");
	  			if(use_udp == 0)
	  			res = send(client_socket, server_message, strlen(server_message), 0);
 				else 
					res = sendto(server_socket, server_message, strlen(server_message), 0 , (struct sockaddr*)&client_addr, sizeof(client_addr) );
   			 }
	 		else if( strstr(client_message, "goodbye") ){
	 			strcpy(server_message,"farewell\n");
				if(use_udp == 0)
	   			res = send(client_socket, server_message, strlen(server_message), 0);
				else
    	   				res = sendto(server_socket, server_message, strlen(server_message), 0 , (struct sockaddr*)&client_addr, sizeof(client_addr) );
				count = count+1;
				if(use_udp == 0){
					close(client_socket);
					addr_len = sizeof(client_socket);
           				client_socket = accept(server_socket, (struct sockaddr *) &client_addr, &addr_len);

 				if (client_socket < 0){
        				printf("Can't accept\n");
    				}
				
 				printf("connection %d from ('",count);
				printf("%d.%d.%d.%d',%d)\n", (int) (client_addr.sin_addr.s_addr&0xFF),(int) ((client_addr.sin_addr.s_addr&0xFF00)>>8),(int) ((client_addr.sin_addr.s_addr&0xFF0000)>>16),(int) ((client_addr.sin_addr.s_addr&0xFF000000)>>24), (int)ntohs(client_addr.sin_port) );
	 			}
			}
	 		else if( strstr(client_message, "exit") ){
                		strcpy(server_message,"ok\n");
				if(use_udp == 0)
	   			res = send(client_socket, server_message, strlen(server_message), 0);
				else
			                                        res = sendto(server_socket, server_message, strlen(server_message), 0 , (struct sockaddr*)&client_addr, sizeof(client_addr) );
				flag = false;
         		}
	 		else {
				strcpy(server_message,client_message);
				if(use_udp == 0)
	   			res = send(client_socket, server_message, strlen(server_message), 0);
				else
                                        res = sendto(server_socket, server_message, strlen(server_message), 0 , (struct sockaddr*)&client_addr, sizeof(client_addr) );
			 }
	 		if(res == -1){
	 			printf("client closed\n");
	 		}
			                            memset(client_message,'\0',sizeof(client_message));
						    memset(server_message,'\0',sizeof(server_message));
					//	memset(temp_message,'\0',sizeof(temp_message));
		}
		if(use_udp == 0) 
	    		close(client_socket);
    	  	close(server_socket);
}

void chat_client(char* host, long port, int use_udp) {
  
    int socket_c;
    struct sockaddr_in server_addr;
    char server_message[1024], client_message[1024];
    
    memset(server_message,'\0',sizeof(server_message));
    memset(client_message,'\0',sizeof(client_message));
    if(use_udp == 0) {
    	socket_c = socket(AF_INET, SOCK_STREAM, 0);
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
    }
    bool flag = true;
    while(flag) {
	    memset(server_message,'\0',sizeof(server_message));
    		memset(client_message,'\0',sizeof(client_message));

	//char server_message[2000], client_message[2000];
	printf("Enter a message\n");
	int s = scanf("%s",client_message);
    	if(s) {
		strcat(client_message,"\n");
	}
    if(use_udp == 0) {
    if(send(socket_c, client_message, strlen(client_message), 0) < 0){
        printf("Error to send message\n");
    }
     if(recv(socket_c, server_message, sizeof(server_message), 0) < 0){
        printf("Error while receiving server's msg\n");
    }
    }
    else {
    	if( sendto(socket_c, client_message, strlen(client_message),0, (struct sockaddr*)&server_addr, sizeof(server_addr)) <0 ) {
		printf("Error sending message\n");
	}
	int len = sizeof(server_addr);
	 if(recvfrom(socket_c, server_message, sizeof(server_message), 0, (struct sockaddr*)&server_addr, (unsigned int *)&len) < 0){
        	printf("Error while receiving server's msg\n");
    	}
    }
    
    printf("Server's response: %s\n",server_message);
    //int r = tolower(server_message)
    if( strstr(server_message, "ok") || strstr(server_message, "farewell")  ) {
    	flag = false;
    }
    }
   
    close(socket_c);
    
    
}
