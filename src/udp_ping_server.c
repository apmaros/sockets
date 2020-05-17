#include <socknet.h>

int main(){
  printf("starting server...\n");
  struct addrinfo hints;
  memset(&hints, 0, sizeof(hints));
  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_DGRAM;
  hints.ai_flags = AI_PASSIVE;

  struct addrinfo *bind_address;
  getaddrinfo(0, "8080", &hints, &bind_address);

  char addr[100];
  char serv[100];
  getnameinfo(
      bind_address->ai_addr,
      bind_address->ai_addrlen,
      addr, sizeof(addr),
      serv, sizeof(serv),
      NI_NUMERICHOST | NI_NUMERICSERV
      );
  printf("host=%s:%s\n", addr, serv);

  printf("Creating socket...\n");

  // create socket
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

  // bind address
  if(bind(
        socket_listen,
        bind_address->ai_addr,
        bind_address->ai_addrlen)) {
    fprintf(stderr, "bind() failed. (%d)\n", GETSOCKETERRNO());
    return 1;
  }
  freeaddrinfo(bind_address);
  printf("listening to messages\n");

  while(1) {
    struct sockaddr_storage client_address;
    socklen_t client_len = sizeof(client_address);

    char req[1024];
    int bytes_received = recvfrom(
        socket_listen,
        req,
        1024,
        0,
        (struct sockaddr *)&client_address,
        &client_len
        );

    if (bytes_received < 1) {
      fprintf(stderr, "connection closed. (%d)\n",
          GETSOCKETERRNO());
      return 1;
    }

    char address_buffer[100];
    char service_buffer[100];
    getnameinfo(
        (struct sockaddr*)&client_address,
        client_len,
        address_buffer, sizeof(address_buffer),
        service_buffer, sizeof(service_buffer),
        NI_NUMERICHOST | NI_NUMERICSERV
        );
    printf("received message:\n\taddress=%s:%s\n\tdata=%s\n",
        address_buffer, service_buffer, req);


    int j;
    char resp[1024] = "PONG";
    sendto(
        socket_listen,
        resp,
        strlen(resp)+1,
        0,
        (struct sockaddr*)&client_address,
        client_len
        );
  }
}
