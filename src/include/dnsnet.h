#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ISVALIDSOCKET(s) ((s) >= 0)
#define CLOSESOCKET(s) close(s)
#define SOCKET int
#define GETSOCKETERRNO() (errno)
// google DNS
#define DNS_SERVER_HOST "8.8.8.8"
#define DNS_SERVER_PORT "53"

const unsigned char *print_name(
    // pointer to the message beginning
    const unsigned char *msg,
    // pointer to the name
    const unsigned char *p,
    // pointer to the end of the message
    const unsigned char *end
    );


void print_dns_message(
    // pointer to the start of the message
    const char *message,
    int msg_length
    );
