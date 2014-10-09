#ifndef ENET_SERVER
#define ENET_SERVER

#include "EnetBase.h"
#include "EnetServerListener.h"
#include "EnetSettings.h"
#include <queue>

//tmp
#include <iostream>

template <class Listener>
class EnetServer : public EnetBase {
public:
	EnetServer() : clients(NULL), clientIds(NULL), newClientId(0) {}
	~EnetServer();
	void setup(EnetServerListener<Listener>* _listener, EnetServerSettings _settings);
	bool startup(uint16_t port);
	void disconnect(uint16_t clientId);
	void queuePacket(uint16_t clientId, const char* message, size_t messageSize, uint8_t channelId);

	// From EnetBase
	void poll();
	void receiveEvent(const ENetEvent& event);

	void connectEvent(const ENetEvent& event);
	void disconnectEvent(const ENetEvent& event);

private:
	// To avoid problems with destruction
	EnetServer(const EnetServer& other) {}
	EnetServer& operator= (const EnetServer& other) {return *this;}

	uint16_t nextFreeClientId();

	EnetServerListener<Listener>* listener;
	EnetServerSettings settings;
	ENetPeer** clients;
	uint16_t* clientIds;
	uint16_t newClientId;
	// Abbreviation for disconnected client indices
	std::queue<uint16_t> dClientIds;
};

// TODO implementation in separate file
//#include "EnetServer.tpp"

template <class Listener>
EnetServer<Listener>::~EnetServer() {
	// Deallocate memory for clients
	if (clients) {
		delete [] clients;
	}
	if (clientIds) {
		delete clientIds;
	}
}

template <class Listener>
void EnetServer<Listener>::setup(EnetServerListener<Listener>* _listener, EnetServerSettings _settings) {
	listener = _listener;
	settings = _settings;
	// Allocate memory for clients
	clients = new ENetPeer*[_settings.connections];
	clientIds = new uint16_t[_settings.connections];
	for (uint16_t i = 0; i < _settings.connections; ++i) {
		clients[i] = NULL;
		clientIds[i] = i;
	}
}

template <class Listener>
bool EnetServer<Listener>::startup(uint16_t port) {
	if (adapter.enetInit()) {
		if (adapter.enetCreateWithAddressPortOnly(port, settings.connections, settings.channels, settings.inBandwidth, settings.outBandwidth)) {
			return true;
		}
	}
	return false;
}

template <class Listener>
void EnetServer<Listener>::disconnect(uint16_t clientId) {
	ENetPeer* client = clients[clientId];
	if (client) {
		adapter.enetDisconnect(client);
	} else {
		// TODO log
	}
}

template <class Listener>
void EnetServer<Listener>::queuePacket(uint16_t clientId, const char* message, size_t messageSize, uint8_t channelId) {
	ENetPeer* client = clients[clientId];
	if (client) {
		adapter.enetQueuePacket(client, message, messageSize, channelId, settings.channelFlags[channelId]);
	} else {
		// TODO log
	}
}

template <class Listener>
void EnetServer<Listener>::poll() {
	ENetEvent event;
	while (adapter.enetPollEvent(&event) > 0) {
		switch (event.type) {
		case ENET_EVENT_TYPE_RECEIVE:
			receiveEvent(event);
			// Clean up
			enet_packet_destroy(event.packet);
			break;
		case ENET_EVENT_TYPE_CONNECT:
			connectEvent(event);
			break;
		case ENET_EVENT_TYPE_DISCONNECT:
			disconnectEvent(event);
			break;
		}
	}
}

template <class Listener>
void EnetServer<Listener>::connectEvent(const ENetEvent& event) {
	uint16_t i = nextFreeClientId();
	// Error
	if (i < 0) {
		// TODO log
		return;
	}
	// Mark and index client
	{
		event.peer->data = &clientIds[i];
		clients[i] = event.peer;
	}
	std::cout << "connect event" << " num: " << i << std::endl;
	listener->connectEventInterface(i);
}

template <class Listener>
void EnetServer<Listener>::disconnectEvent(const ENetEvent& event) {
	uint16_t i = *static_cast<uint16_t *>(event.peer->data);
	// Unmark client, remove from index, and store index
	{
		event.peer->data = NULL;
		clients[i] = NULL;
		dClientIds.push(i);
	}
	std::cout << "disconnect event" << " num: " << i << std::endl;
	listener->disconnectEventInterface(i);
}

template <class Listener>
void EnetServer<Listener>::receiveEvent(const ENetEvent& event) {
	uint16_t i = *static_cast<uint16_t *>(event.peer->data);
	std::cout << "receive packet event" << " num: " << i << " datalength: " << event.packet->dataLength << " data: " << event.packet->data << std::endl;
	listener->receiveEventInterface(i, reinterpret_cast<char*>(event.packet->data), event.packet->dataLength, event.channelID);
}

// O(1)
// Returns next available client index and removes its availability
// Returns -1 if none are available
template <class Listener>
uint16_t EnetServer<Listener>::nextFreeClientId() {
	if (!dClientIds.empty()) {
		uint16_t n = dClientIds.front();
		dClientIds.pop();
		return n;
	}
	if (newClientId == settings.connections) {
		return -1;
	}
	return newClientId++;
}

#endif