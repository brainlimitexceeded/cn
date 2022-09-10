#include <stdio.h>
#include <string.h>

int connect_smtp(const char* host, int port);
void send_smtp(int sock, const char* msg, char* resp, size_t len);



/*
  Use the provided 'connect_smtp' and 'send_smtp' functions
  to connect to the "lunar.open.sice.indian.edu" smtp relay
  and send the commands to write emails as described in the
  assignment wiki.
 */
int main(int argc, char* argv[]) {
  if (argc != 3) {
    printf("Invalid arguments - %s <email-to> <email-filepath>", argv[0]);
    return -1;
  }
  char* rcpt = argv[1];
  char* filepath = argv[2];
  FILE *fp;
  if ( (fp = fopen(filepath,"r")) == NULL ) {
    printf("ERROR");
  }
  
  char filedata[4096];
  char email[4096];
   while (fgets(filedata, 4096, fp) != NULL)
    {
        strcat(email,filedata);
    }
  fclose(fp); 
  int socket = connect_smtp("lunar.open.sice.indiana.edu", 25);
  char response[4096];


  send_smtp(socket, "HELO iu.edu\n", response, 4096);
  printf("%s\n", response);
  char send_cmd[100] = "MAIL FROM:<";
  strcat(send_cmd,rcpt);
  strcat(send_cmd,">\n");
  send_smtp(socket, send_cmd,response,4096);
  printf("%s\n", response);
  char rc_cmd[100] = "RCPT TO:<";
  strcat(rc_cmd,rcpt);
  strcat(rc_cmd,">\n");
  send_smtp(socket, rc_cmd,response,4096);
  printf("%s\n", response);
  send_smtp(socket,"DATA\n",response,4096);
  printf("%s\n",response);
  char data[5000] = "<";
  strcat(data,email);
  strcat(data,">\r\n.\r\n");
  send_smtp(socket, data,response,4096);
  printf("%s\n", response);
  send_smtp(socket,"QUIT\n",response,4096);
  return 0;
}


