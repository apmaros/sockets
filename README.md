# Sockets
This project demonstrates work with socket LINUX API and is limited on Linux, or macOS operating systems. It started by following book [Hands-On Network Programming with C: Learn socket programming in C and write secure and optimized network code](https://www.amazon.co.uk/Hands-Network-Programming-programming-optimized/dp/1789349869) and most code is taken from there (check its [github page](https://github.com/PacktPublishing/Hands-On-Network-Programming-with-C)).

# Applications
## TCP and UDP server
Creates a server listening on socket using TCP, or UDP protocol. They are following:
- `tcp_ping_server` - responds to any incomming message with pong
- `udp_ping_server`
- `tcp_broadcast` - broadcasts messages received from client to all other clients

## DNS
Resolves a IP address for a domain name
- `dns_lookup` - resolve domain name using OS API
- `dns_raw_lookup` - resolves domain name manually creating UDP message

Read [DNS Query](docs/dns\_format.md) to learn how to compose DNS message


# Usage
- `make install` - installs all applications to build folder - `./build`
- `make <application>` - installs specific application (see Applications) - `make udp_ping_server`

example on how to run a application:
```
./build/dns_lookup google.com
  resolving hostname=google.com
  2a00:1450:4009:807::200e 0
  2a00:1450:4009:807::200e 0
  216.58.204.238 0
  216.58.204.238 0
```
