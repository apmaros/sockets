#include <dnsnet.h>
#include <dns_header.h>
#include <string.h>
#include <stdio.h>

const unsigned char *print_name(
    const unsigned char *msg,
    const unsigned char *p,
    const unsigned char *end
    ){
  // return error if name is empty
  if(p + 2 > end){
    fprintf(stderr, "End of message.\n");
    exit(1);
  }

  // check if p points to a pointer
  // call recursivelly if it does
  if((*p & 0xC0)  == 0xC0) {
    const int k = ((*p & 0x3F) << 8) + p[1];
    p += 2;
    printf(" (pointer %d) ", k);
    print_name(msg, msg+k, end);
    return p;
  }

  // store the length of the current label
  const int len = *p++;
  // make sure that len did not reach the end of the message
  if(p + len + 1 > end){
    fprintf(stderr, "End of message.\n");
    exit(1);
  }

  printf("%.*s", len, p);

  // move the pointer
  p += len;
  // if it is not 0, continue printing name
  if(*p) {
    // print '.' to parate labels
    printf(".");
    return print_name(msg, p, end);
  }

  return p+1;
}

void print_formatted_message(
    const char *message,
    int msg_length
    ){

  // check that the message is of lengt of valid DNS message
  // (HEADER=12bits) - the message must be longer
  if(msg_length < 12) {
    fprintf(stderr, "Message is too short to be valid.\n");
    exit(1);
  }

  const unsigned char *msg = (const unsigned char *)message;

  printf("Formatted message:\n");

  unsigned char encoded_header[12];
  memcpy(encoded_header, msg, 12);
  struct dns_header h = deserialize_dns_header(encoded_header);
  print_dns_header(&h);

  const unsigned char *p = msg + 12;
  // ensure reading only from the message
  const unsigned char *end = msg + msg_length;

  if(h.qdcount) {
    int i;
    // no DNS server accept multiple questions
    // but DNS RFC defines format for multiple q
    for (i = 0; i < h.qdcount; i++){
      if(p >= end){
        fprintf(stderr, "End of message.\n");
        exit(1);
      }

      printf("Query %2d\n", i + 1);
      printf("\tname: ");
      p = print_name(msg, p, end);
      printf("\n");

      if(p + 4 > end){
        fprintf(stderr, "End of message\n");
        exit(1);
      }

      const int type = (p[0] << 8) + p[1];
      printf("\ttpye: %d\n", type);
      p += 2;

      const int qclass = (p[0] << 8) + p[1];
      printf("\tclass: %d\n", qclass);
      p += 2;
    }
  }

  if(h.ancount || h.nscount || h.arcount){
    int i;
    for (i = 0; i < h.ancount + h.nscount + h.arcount; ++i) {
      if (p >= end) {
        fprintf(stderr, "End of message.\n"); exit(1);}

      printf("Answer %2d\n", i + 1);
      printf("  name: ");

      p = print_name(msg, p, end); printf("\n");

      if (p + 10 > end) {
        fprintf(stderr, "End of message.\n"); exit(1);}

      const int type = (p[0] << 8) + p[1];
      printf("  type: %d\n", type);
      p += 2;

      const int qclass = (p[0] << 8) + p[1];
      printf(" class: %d\n", qclass);
      p += 2;

      const unsigned int ttl = (p[0] << 24) + (p[1] << 16) +
        (p[2] << 8) + p[3];
      printf("   ttl: %u\n", ttl);
      p += 4;

      const int rdlen = (p[0] << 8) + p[1];
      printf(" rdlen: %d\n", rdlen);
      p += 2;

      if (p + rdlen > end) {
        fprintf(stderr, "End of message.\n"); exit(1);}

      if (rdlen == 4 && type == 1) {
        /* A Record */
        printf("Address ");
        printf("%d.%d.%d.%d\n", p[0], p[1], p[2], p[3]);

      } else if (rdlen == 16 && type == 28) {
        /* AAAA Record */
        printf("Address ");
        int j;
        for (j = 0; j < rdlen; j+=2) {
          printf("%02x%02x", p[j], p[j+1]);
          if (j + 2 < rdlen) printf(":");
        }
        printf("\n");

      } else if (type == 15 && rdlen > 3) {
        /* MX Record */
        const int preference = (p[0] << 8) + p[1];
        printf("  pref: %d\n", preference);
        printf("MX: ");
        print_name(msg, p+2, end); printf("\n");

      } else if (type == 16) {
        /* TXT Record */
        printf("TXT: '%.*s'\n", rdlen-1, p+1);

      } else if (type == 5) {
        /* CNAME Record */
        printf("CNAME: ");
        print_name(msg, p, end); printf("\n");
      }

      p += rdlen;
    }
  }

  if (p != end) {
    printf("There is some unread data left over.\n");
  }

  printf("\n");
};

void print_raw_message(const char *message, int msg_length){

  const unsigned char *msg = (const unsigned char *)message;

  printf("---------------START OF THE MESSAGE--------------------\n");
  int i;
  for (i = 0; i < msg_length; ++i) {
    unsigned char r = msg[i];
    printf("\t%02d:   %02X  %03d  '%c'\n", i, r, r, r);
  }
  printf("----------------END OF THE MESSAGE--------------------\n");
};

int main(int argc, char *argv[]){
    if (argc < 3) {
        printf("Usage:\n\tdns_query hostname type\n");
        printf("Example:\n\tdns_query example.com aaaa\n");
        exit(0);
    }

    if (strlen(argv[1]) > 255) {
        fprintf(stderr, "Hostname too long.");
        exit(1);
    }

    unsigned char type;
    if (strcmp(argv[2], "a") == 0) {
        type = 1;
    } else if (strcmp(argv[2], "mx") == 0) {
        type = 15;
    } else if (strcmp(argv[2], "txt") == 0) {
        type = 16;
    } else if (strcmp(argv[2], "aaaa") == 0) {
        type = 28;
    } else if (strcmp(argv[2], "any") == 0) {
        type = 255;
    } else {
        fprintf(
            stderr,
            "Unknown type '%s'. Use a, aaaa, txt, mx, or any.",
            argv[2]
            );
        exit(1);
    }

    struct addrinfo hints;
    memset(&hints, 0, sizeof(hints));
    hints.ai_socktype = SOCK_DGRAM;
    struct addrinfo *peer_address;

    if(getaddrinfo(DNS_SERVER_HOST, DNS_SERVER_PORT, &hints, &peer_address)){
      fprintf(stderr, "getaddrinfo() failed. (%d)\n", GETSOCKETERRNO());
      return 1;
    }

    int socket_peer;
    socket_peer = socket(
        peer_address->ai_family,
        peer_address->ai_socktype,
        peer_address->ai_protocol
        );
    if (!ISVALIDSOCKET(socket_peer)) {
        fprintf(stderr, "socket() failed. (%d)\n", GETSOCKETERRNO());
        return 1;
    }

    char query[1024];

    // make and serialize header
    struct dns_header header;
    // id is unique message identifier
    // useful when sending multiple messages async
    char id[2] = {0xFF, 0xAA};
    header = make_query_header(id, false, STANDARD);
    unsigned char encoded_header[12];
    serialize_dns_header(encoded_header, &header);

    // copy header bytes to query
    memcpy(query ,encoded_header, 15);

    // set p to the end of the query header
    char *p = query + 12;
    // pointer used to loop trought the hostname
    char *h = argv[1];

    // loop until h != 0
    // because *h is equal to 0 when we finished reading the hostname
    while(*h) {
      // store the position of the label beginning
      char *len = p;
      // set the position for the upcomming field
      p++;
      if (h != argv[1]) ++h;

      // copy characters from *h to *p
      // until we find a dot `.`, or end of the hostname
      while(*h && *h != '.') *p++ = *h++;
      // set len to be equal to the label lenght
      *len = p - len - 1;
    }

    // add terminating `0` byte to indicate end of the name section
    *p++ = 0;

    *p++ = 0x00; *p++ = type; /* QTYPE */
    *p++ = 0x00; *p++ = 0x01; /* QCLASS */

    // calculate the query size
    const int query_size = p - query;
    int bytes_sent = sendto(
        socket_peer,
        query,
        query_size,
        0,
        peer_address->ai_addr,
        peer_address->ai_addrlen
        );

    printf("sent %d bytes.\n", bytes_sent);
    print_raw_message(query, query_size);
    print_formatted_message(query, query_size);

    char read[1024];
    int bytes_received = recvfrom(
        socket_peer,
        read,
        1024,
        0, 0, 0
        );

    printf("Received %d bytes.\n", bytes_received);
    print_formatted_message(read, bytes_received);
    printf("\n");
}

