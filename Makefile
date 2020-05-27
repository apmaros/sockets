#
# build based on tutorial https://www.cs.swarthmore.edu/~newhall/unixhelp/howto_makefiles.html
#
CC=gcc
# compiler flags:
#  -g    adds debugging information to the executable file
#  -Wall turns on most, but not all, compiler warnings
CFLAGS  = -g -Wall
INCLUDES = -I./src/include

SRC_DIR = ./src
BUILD_DIR = ./build

TCP_PING_SERVER_TARGET = tcp_ping_server
TCP_SERVER_TARGET = tcp_broadcast
UDP_SERVER_TARGET = udp_ping_server
DNS_LOOKUP_TARGET = dns_lookup
DNS_RAW_LOOKUP_TARGET = dns_raw_lookup

install: make_build_dir $(TCP_PING_SERVER_TARGET) $(TCP_SERVER_TARGET) $(UDP_SERVER_TARGET) $(DNS_LOOKUP_TARGET) $(DNS_RAW_LOOKUP_TARGET)

$(TCP_PING_SERVER_TARGET):
	$(CC) $(INCLUDES) $(SRC_DIR)/$(TCP_PING_SERVER_TARGET).c -o $(BUILD_DIR)/$(TCP_PING_SERVER_TARGET)

$(TCP_SERVER_TARGET):
	$(CC) $(INCLUDES) $(SRC_DIR)/$(TCP_SERVER_TARGET).c -o $(BUILD_DIR)/$(TCP_SERVER_TARGET)

$(UDP_SERVER_TARGET):
	$(CC) $(INCLUDES) $(SRC_DIR)/$(UDP_SERVER_TARGET).c -o $(BUILD_DIR)/$(UDP_SERVER_TARGET)

$(DNS_LOOKUP_TARGET):
	$(CC) $(INCLUDES) $(SRC_DIR)/$(DNS_LOOKUP_TARGET).c -o $(BUILD_DIR)/$(DNS_LOOKUP_TARGET)

$(DNS_RAW_LOOKUP_TARGET):
	$(CC) $(INCLUDES) $(SRC_DIR)/$(DNS_RAW_LOOKUP_TARGET).c -o $(BUILD_DIR)/$(DNS_RAW_LOOKUP_TARGET)

make_build_dir:
	mkdir -p ./build

clean:
	rm -rf ./$(BUILD_DIR)
