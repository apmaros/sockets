#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <util.h>

#define MAX_OPCODE 3
#define MAX_RCODE 5

enum opcode {
  STANDARD,
  REVERSE,
  STATUS,
  UNKNOWN
};

static const char *OPCODE_STRING[] = {
  "STANDARD", "REVERSE", "STATUS", "UNKNOWN"
};

enum rcode {
  SUCCESS,
  FORMAT_ERROR,
  SERVER_FAILURE,
  NAME_ERROR,
  NOT_IMPLEMENTED,
  REFUSED
};

static const char *RCODE_STRING[] = {
  "SUCCESS",
  "FORMAT ERROR",
  "SERVER FAILURE",
  "NAME ERROR",
  "NOT IMPLEMENTED",
  "REFUSED",
};

struct dns_header {
  unsigned char id[2]; // 0-15
  unsigned int qr; // 0=query, 1=response
  enum opcode opcode;
  bool aa; // is authoritative answer
  bool tc; // truncated messge
  bool rd; // recursion desirded
  bool ra; // recursion supported
  enum rcode rcode; // error code
  unsigned int qdcount; // question count
  unsigned int ancount; // answers count
  unsigned int nscount;
  unsigned int arcount;
};

struct dns_header make_query_header(
    char* id,
    bool rd,
    enum opcode oc){
  struct dns_header header;
  header.id[0] = id[0];
  header.id[1] = id[1];
  header.qr = 0; // query
  header.opcode = oc;
  header.aa = false; // not answer
  header.tc = false;
  header.rd = rd;
  header.ra = 0; // not answer
  header.rcode = 0; // not answer
  // only 1 question allowed for query
  header.qdcount = 1;
  // following are only for answer
  header.ancount = 0;
  header.nscount = 0;
  header.arcount = 0;

  return header;
};

unsigned char* serialize_dns_header(
    unsigned char *buffer,
    struct dns_header *header
    ) {
  // byte-0, byte-1: message ID
  buffer[0] = header->id[0];
  buffer[1] = header->id[1];

  // byte-2: flags
  if(header->qr){ SET_BIT(buffer[2], 7); }
  unsigned char opcode_val = header->opcode;
  buffer[2] |= opcode_val;
  buffer[2] = buffer[2] << 3;
  if(header->aa){ SET_BIT(buffer[2], 2); }
  if(header->tc){ SET_BIT(buffer[2], 1); }
  if(header->rd){ SET_BIT(buffer[2], 0); }

  // byte-3: rd and rcode
  if(header->rd){ SET_BIT(buffer[3], 7); }
  unsigned char rcode_val = header->rcode;
  buffer[3] |= rcode_val;

  // byte-4 - byte-11: set counts
  struct encoded_int qdcount_val = encode_int(header->qdcount);
  buffer[4] = qdcount_val.first;
  buffer[5] = qdcount_val.second;

  struct encoded_int ancount_val = encode_int(header->ancount);
  buffer[6] = ancount_val.first;
  buffer[7] = ancount_val.second;

  struct encoded_int nscount_val = encode_int(header->nscount);
  buffer[8] = nscount_val.first;
  buffer[9] = nscount_val.second;

  struct encoded_int arcount_val = encode_int(header->arcount);
  buffer[10] = arcount_val.first;
  buffer[11] = arcount_val.second;

  return buffer;
}

struct dns_header deserialize_dns_header(unsigned char *msg) {
  struct dns_header header;
  header.id[0] = msg[0];
  header.id[1] = msg[1];
  header.qr = (msg[2] & 0x80) >> 7;
  header.opcode = (msg[2] & 0x078) >> 3;
  header.aa = (msg[2] & 0x04) >> 2;
  header.tc = (msg[2] & 0x02) >> 1;
  header.rd = (msg[2] & 0x01);
  header.rcode = msg[3] & 0x07;

  struct encoded_int qdcount_enc = {msg[4], msg[5]};
  header.qdcount = decode_int(qdcount_enc);

  struct encoded_int ancount_enc = {msg[6], msg[7]};
  header.ancount = decode_int(ancount_enc);

  struct encoded_int nscount_enc = {msg[8], msg[9]};
  header.nscount = decode_int(nscount_enc);

  struct encoded_int arcount_enc = {msg[10], msg[11]};
  header.arcount = decode_int(arcount_enc);

  return header;
}

void print_dns_header(struct dns_header *header) {
  printf("id=%0X%0X\n", header->id[0], header->id[1]);
  printf("rd=%s\n", bool_str(header->rd));
  if(header->opcode > MAX_OPCODE) {
    printf("invalid value for opcode (opcode=%d)\n", header->opcode);
  } else {
    printf("opcode=%s\n", OPCODE_STRING[header->opcode]);
  }
  printf("AA=%s (authoritative answer)\n", bool_str(header->aa));
  printf(
      "tc=%s (truncated message, if true should use TCP)\n",
      bool_str(header->tc)
      );
  if(header->rcode > MAX_RCODE) {
    printf("invalid value for rcode (rcode=%d)\n", header->rcode);
  } else {
    printf("rcode=%s\n", RCODE_STRING[header->rcode]);
  }
  printf("QDCOUNT = %d\n", header->qdcount);
  printf("ANCOUNT = %d\n", header->ancount);
  printf("NSCOUNT = %d\n", header->nscount);
  printf("ARCOUNT = %d\n", header->arcount);
};
