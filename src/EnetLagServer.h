#ifndef ENET_LAG_SERVER
#define ENET_LAG_SERVER

#include "EnetBaseInterface.h"
#include "EnetServerInterface.h"
#include "EnetServer.h"
#include "EnetServerListener.h"
#include "EnetSettings.h"

template <class Listener>
class EnetLagServer : EnetBaseInterface, EnetServerInterface, EnetServerListener<EnetLagServer<Listener>> {
public:
	EnetLagServer() {}
	void setup(EnetServerListener<Listener>* _listener, EnetServerSettings _settings);

	// From EnetBaseInterface
	void shutdown();
	void poll();

	// From EnetServerInterface
	bool startup(uint16_t port);
	void disconnectClient(uint16_t clientId);
	void disconnectClientNow(uint16_t clientId);
	std::string ipOfClientAsString(uint16_t clientId);
	uint32_t ipOfClientAsInt(uint16_t clientId);
	uint16_t portOfClient(uint16_t clientId);
	void queuePacket(uint16_t clientId, const char* message, size_t messageSize, uint8_t channelId);

	// From EnetServerListener
	void connectEvent(uint16_t clientId);
	void disconnectEvent(uint16_t clientId);
	void receiveEvent(uint16_t clientId, const char* message, size_t messageSize, uint16_t channelId);
	void error(uint8_t err, void* data);

private:
	// To avoid problems with destruction
	EnetLagServer(const EnetLagServer& other) {}
	EnetLagServer& operator= (const EnetLagServer& other) {return *this;}

	EnetServer<EnetLagServer> server;
	EnetServerListener<Listener>* listener;
};

template <class Listener>
void EnetLagServer<Listener>::shutdown() {
	server.shutdown();
}

template <class Listener>
void EnetLagServer<Listener>::poll() {
	// todo
	//
	// When we decide it's been long enough, process queued packet:
	// listener->receiveEventInterface(packet.clientId, packet.message, packet.messageSize, packet.channelId);
	//
	// when we decide it's been long enough, send queued packet:
	// server.queuePacket(packet.clientId, packet.message, packet.messageSize, packet.channelId);

	server.poll();
}

template <class Listener>
void EnetLagServer<Listener>::setup(EnetServerListener<Listener>* _listener, EnetServerSettings _settings) {
	listener = _listener;
	server.setup(this, _settings);
}

template <class Listener>
bool EnetLagServer<Listener>::startup(uint16_t port) {
	return server.startup(port);
}

template <class Listener>
void EnetLagServer<Listener>::disconnectClient(uint16_t clientId) {
	server.disconnectClient(clientId);
}

template <class Listener>
void EnetLagServer<Listener>::disconnectClientNow(uint16_t clientId) {
	server.disconnectClientNow(clientId);
}

template <class Listener>
std::string EnetLagServer<Listener>::ipOfClientAsString(uint16_t clientId) {
	return server.ipOfClientAsString(clientId);
}

template <class Listener>
uint32_t EnetLagServer<Listener>::ipOfClientAsInt(uint16_t clientId) {
	return server.ipOfClientAsInt(clientId);
}

template <class Listener>
uint16_t EnetLagServer<Listener>::portOfClient(uint16_t clientId) {
	return server.portOfClient(clientId);
}

template <class Listener>
void EnetLagServer<Listener>::queuePacket(uint16_t clientId, const char* message, size_t messageSize, uint8_t channelId) {
	// todo
}

template <class Listener>
void EnetLagServer<Listener>::connectEvent(uint16_t clientId) {
	listener->connectEventInterface(clientId);
}

template <class Listener>
void EnetLagServer<Listener>::disconnectEvent(uint16_t clientId) {
	listener->disconnectEventInterface(clientId);
}

template <class Listener>
void EnetLagServer<Listener>::receiveEvent(uint16_t clientId, const char* message, size_t messageSize, uint16_t channelId) {
	// todo
}

template <class Listener>
void EnetLagServer<Listener>::error(uint8_t err, void* data) {
	listener->errorInterface(err, data);
}

// Implementation
#include "EnetLagServer.tpp"

#endif