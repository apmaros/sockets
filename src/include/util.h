#define SET_BIT(byte, bit) ((byte) |= (1UL << (bit)))


/**
 * formats boolean value to string
 */
const char* bool_str(int b) {
  return b ? "true" : "false";
}

/**
 * convenience structure to represent
 * integer with 2 bytes
 */
struct encoded_int {
  unsigned char first;
  unsigned char second;
};

struct encoded_int encode_int(int num){
  struct encoded_int encoded;
  encoded.first = ((num & 0x0000FF00) >> 8);
  encoded.second = (num & 0x000000FF);
  return encoded;
}

int decode_int(struct encoded_int encoded){
  int val;
  val = ((int) encoded.first << 8);
  val += ((int) encoded.second);

  return val;
}

