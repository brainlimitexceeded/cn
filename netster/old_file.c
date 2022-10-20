/*
 *  Here is the starting point for your netster part.1 definitions. Add the
 *  appropriate comment header as defined in the code formatting guidelines
 */
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

void chat_server(char *iface, long port, int use_udp) {
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
  if (use_udp == 0) {
    int server_socket, client_socket;
    struct sockaddr_in server_addr;
    int addrlength = sizeof(server_addr);
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(port);
    bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr));
    listen(server_socket, 3);
    int cnt = 0;
    bool run = false;
    while (1) {
      client_socket = accept(server_socket, (struct sockaddr *)&server_addr,
                          (socklen_t *)&addrlength);
      printf("connection %d from ('%s', %d)\n", cnt,
             inet_ntoa(server_addr.sin_addr), ntohs(server_addr.sin_port));
      cnt += 1;
      char buffer[256] = {0};
      while (1) {
        bzero(buffer, 256);
        int val = read(client_socket, buffer, 256);
        if (val == -1) {
          printf("error\n");
        }
        printf("got message from ('%s', %d)\n", inet_ntoa(server_addr.sin_addr),
               ntohs(server_addr.sin_port));
        if (strcmp(buffer, "hello\n") == 0) {
          send(client_socket, "world\n", sizeof("world"), 0);
        } else if (strcmp(buffer, "goodbye\n") == 0) {
          send(client_socket, "farewell\n", sizeof("farewell"), 0);
          break;
        } else if (strcmp(buffer, "exit\n") == 0) {
          send(client_socket, "ok\n", sizeof("ok"), 0);
          close(server_socket);
          run = true;
          break;
        } else if (strcmp(buffer, "\n") != 0) {
          send(client_socket, buffer, sizeof(buffer), 0);
        }
      }
      if (run) {
        break;
      }
    }
  } else {
    int socket_server, run = 0;
    char buffer[MAXBYTES];
    struct sockaddr_in saddr, caddr;
    socket_server = socket(AF_INET, SOCK_DGRAM, 0);

    memset(&saddr, 0, sizeof(saddr));
    memset(&caddr, 0, sizeof(caddr));

    saddr.sin_family = AF_INET;
    saddr.sin_addr.s_addr = INADDR_ANY;
    saddr.sin_port = htons(port);

    bind(socket_server, (const struct sockaddr *)&saddr, sizeof(saddr));

    int length;
    length = sizeof(caddr);
    while (1) {
      bzero(buffer, MAXBYTES);
      int buf_len =
          recvfrom(socket_server, (char *)buffer, MAXBYTES, MSG_WAITALL,
                   (struct sockaddr *)&caddr, (unsigned int *)&length);
      printf("got message from ('%s', %d)\n", inet_ntoa(caddr.sin_addr),
             ntohs(caddr.sin_port));
      buffer[buf_len] = '\0';
      if (strcmp(buffer, "hello\n") == 0) {
        sendto(socket_server, "world\n", sizeof("world"), 0,
               (const struct sockaddr *)&caddr, length);
      } else if (strcmp(buffer, "goodbye\n") == 0) {
        sendto(socket_server, "farewell\n", sizeof("farewell"), 0,
               (const struct sockaddr *)&caddr, length);
      } else if (strcmp(buffer, "exit\n") == 0) {
        sendto(socket_server, "ok\n", sizeof("ok"), 0,
               (const struct sockaddr *)&caddr, length);
        close(socket_server);
        break;
      } else {
        sendto(socket_server, (const char *)buffer, sizeof(buffer), 0,
               (const struct sockaddr *)&caddr, length);
      }
      if (run) {
        close(socket_server);
      }
    }
  }
}

void chat_client(char *iface, long port, int use_udp) {
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

  bool run = false;
  if (use_udp == 0) {
    int sock = 0;
    struct sockaddr_in serv_addr;
    char buffer[MAXBYTES] = {0};
    sock = socket(AF_INET, SOCK_STREAM, 0);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);
    serv_addr.sin_addr.s_addr = inet_addr(adr);

    connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
    while (1) {
      bzero(buffer, MAXBYTES);
      int n = 0;
      while ((buffer[n++] = getchar()) != '\n');
      send(sock, buffer, sizeof(buffer), 0);
      if (strcmp(buffer, "goodbye\n") == 0) {
        run = true;
      }
      if (strcmp(buffer, "exit\n") == 0) {
        run = true;
      }
      bzero(buffer, MAXBYTES);
      int val = read(sock, buffer, 256);
      if (val == -1) {
        printf("err\n");
      }
      printf("%s", buffer);
      if (run) {
        exit(0);
      }
    }
  } else {
    int socket_server;
    char buffer[MAXBYTES];
    struct sockaddr_in saddr;

    socket_server = socket(AF_INET, SOCK_DGRAM, 0);
    memset(&saddr, 0, sizeof(saddr));
    saddr.sin_family = AF_INET;
    saddr.sin_port = htons(port);
    saddr.sin_addr.s_addr = inet_addr(adr);
    int length;
    bool run = false;
    while (1) {
      bzero(buffer, MAXBYTES);
      int n = 0;
      while ((buffer[n++] = getchar()) != '\n');
      sendto(socket_server, (const char *)buffer, sizeof(buffer), 0,
             (const struct sockaddr *)&saddr, sizeof(saddr));
      if (strcmp(buffer, "goodbye\n") == 0) {
        run = true;
      }
      if (strcmp(buffer, "exit\n") == 0) {
        run = true;
      }
      bzero(buffer, MAXBYTES);
      recvfrom(socket_server, (char *)buffer, MAXBYTES, MSG_WAITALL,
               (struct sockaddr *)&saddr, (unsigned int *)&length);
      printf("%s", buffer);
      if (run) {
        close(socket_server);
        break;
      }
    }
  }
}

