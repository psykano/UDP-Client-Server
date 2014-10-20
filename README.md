#UDP-Client-Server

A simple UDP client/server built on ENet.

## Chat example

Using [openFrameworks](http://www.openframeworks.cc), create a new project and overwrite the existing `main.cpp`, `ofApp.h`, `ofApp.cpp` with those of either the server or the client. Create the `Network` directory and put this repository's `src` in there.

### Command-line arguments

#### Server
The server takes two arguments: the port number and whether to send packets reliably `r` or unreliably `u`, e.g.,
```
server.exe 11999 r
```
This has the server bind to port `11999` and send packets reliably.

#### Client
The client takes three arguments: the ip of the server, the port of the server, and whether to send packets reliably `r` or unreliably `u`, e.g.,
```
client.exe 127.0.0.1 11999 u
```
This has the client connect to the server at `127.0.0.1:11999` and send packets unreliably.

#### Note
If unreliable packet delivery is chosen, packets are sent every update. Otherwise, packets are sent for new data only.
