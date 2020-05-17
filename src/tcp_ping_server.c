#include <stdio.h>
#include <string.h>
#include <time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <errno.h>

#define ISVALIDSOCKET(s) ((s) >= 0)
#define CLOSESOCKET(s) close(s)
#define GETSOCKETERRNO() (errno)
#define PORT "8080"


int main(){
  // configuring local address
  struct addrinfo hints;
  memset(&hints, 0, sizeof(hints));
  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags = AI_PASSIVE;

  struct addrinfo *bind_address;
  getaddrinfo(0, "8080", &hints, &bind_address);

  // creating socket
  int socket_listen;
  socket_listen = socket(
      bind_address->ai_family,
      bind_address->ai_socktype,
      bind_address->ai_protocol
      );

  if (!ISVALIDSOCKET(socket_listen)) {
    fprintf(stderr, "socket() failed. (%d)\n", GETSOCKETERRNO());
    return 1;
  }

  // binding socket to local address
  if(bind(socket_listen, bind_address->ai_addr, bind_address->ai_addrlen) < 0){
    fprintf(stderr, "bind() failed. (%d)\n", GETSOCKETERRNO());
    return 1;
  }
  freeaddrinfo(bind_address);

  // listen on address
  if(listen(socket_listen, 10) < 0){
    fprintf(stderr, "listen() failed. (%d)\n", GETSOCKETERRNO());
    return 1;
  }

  fd_set master;
  FD_ZERO(&master);
  FD_SET(socket_listen, &master);
  // add socket listen to sockets
  // other, FD_CLR() - remove, FD_ISSET() - check is set
  int max_socket = socket_listen;

  printf("listening on localhost:%s\n", "8080");

  while(1) {
    fd_set reads;
    // select overwrite its data - must copy master to reads each time
    reads = master;
    // select blocks until socket in fd_set master received data
    if(select(max_socket+1, &reads, 0, 0, 0) < 0){
      fprintf(stderr, "select() failed. (%d)\n", GETSOCKETERRNO());
      return 1;
    }

    printf("received data on socket\n");
    // serve client connections
    int i;
    for(i = 1; i <= max_socket; i++){

      printf("checking socket %d\n", i);

      if(FD_ISSET(i, &reads)) {
        if(i == socket_listen){
          printf("initialized connection from client (socket=%d)\n", i);
          struct sockaddr_storage client_address;
          socklen_t client_len = sizeof(client_address);
          int socket_client = accept(
              socket_listen,
              (struct sockaddr*) &client_address,
              &client_len
              );

          // error handling
          if (!ISVALIDSOCKET(socket_client)) {
            fprintf(stderr, "accept() failed. (%d)\n",
                GETSOCKETERRNO());
            return 1;
          }

          FD_SET(socket_client, &master);
          if(socket_client > max_socket)
            max_socket = socket_client;

          char address_buffer[100];
          getnameinfo(
              (struct sockaddr*)&client_address,
              client_len,
              address_buffer,
              sizeof(address_buffer),
              0,
              0,
              NI_NUMERICHOST
              );
          printf("established connection to remote client %s\n", address_buffer);
        } else {
          printf("received data from client (socket=%d)\n", i);

          char req[1024];
          int bytes_received = recv(i, req, 1024, 0);
          if(bytes_received < 1){
            FD_CLR(i, &master);
            CLOSESOCKET(i);
            continue;
          }

          /**
           * Logic to handle request
           */
          char resp[1024] = "PONG";
          // send response
          send(i, resp, bytes_received, 0);
        }
      }
    }
  }

  printf("closing listening socket\n");
  CLOSESOCKET(socket_listen);

  printf("Finished\n");

  return 0;
}
