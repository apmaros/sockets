#include <socknet.h>

int main(int argc, char *argv[]){
  if(argc < 2) {
    printf("Usage:\n\tlookup hostname\n");
    printf("Example:\n\tlookup www.google.com");
    exit(0);
  }

  struct addrinfo hints;
  memset(&hints, 0, sizeof(hints));
  hints.ai_flags = AI_ALL;
  struct addrinfo *peer_address;

  char target[100];
  strcpy(target, argv[1]);

  if(getaddrinfo(
        target,
        0,
        &hints,
        &peer_address)) {
    fprintf(stderr, "getaddrinfo() failed. (%d)\n", GETSOCKETERRNO());
    return 1;
  }

  printf("resolving hostname=%s\n", target);

  struct addrinfo *address = peer_address;
  do {
    char host[100];
    char serv[100];

    getnameinfo(
        address->ai_addr,
        address->ai_addrlen,
        host, sizeof(host),
        serv, sizeof(serv),
        NI_NUMERICHOST | NI_NUMERICSERV
        );

    printf("\t%s %s\n", host, serv);
  } while ((address = address->ai_next));

  exit(0);
}
