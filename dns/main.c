#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <string.h>
#include <arpa/inet.h>

/*
  Use the `getaddrinfo` and `inet_ntop` functions to convert a string host and
  integer port into a string dotted ip address and port.
 */
int main(int argc, char* argv[]) {
  if (argc != 3) {
    printf("Invalid arguments - %s <host> <port>", argv[0]);
    return -1;
  }
  char* host = argv[1];
  long port = atoi(argv[2]);
  struct addrinfo hints, *result;
  char addr[100];
  char str[256];
  sprintf(str,"%ld",port);
  memset(&hints, 0, sizeof(hints));
  hints.ai_family = PF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_protocol = IPPROTO_TCP;
  hints.ai_flags = AI_PASSIVE;
  int err = getaddrinfo(host, str, &hints, &result );
  if(err!= 0) {
  	perror("getaddrinfo");
	return -1;
  }
  //res = result;
  while(result) {
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
printf("IPv%d %s\n", result->ai_family == AF_INET6?6:4, addr);
result = result->ai_next;
}
  return 0;
}
