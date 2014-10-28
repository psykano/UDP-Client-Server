#ifndef ENET_SERVER
#define ENET_SERVER

#include "EnetBase.h"
#include "EnetServerListener.h"
#include "EnetSettings.h"
#include <queue>

// Max IPv4 address length + termination
#define IP_MAX_ADDRESS 16

enum ENET_SERVER_ERROR_CODE {
	ENET_SERVER_ERROR_INIT,
	ENET_SERVER_ERROR_CREATE,
	ENET_SERVER_ERROR_NULL_CLIENT,
	ENET_SERVER_ERROR_CONNECT_EVENT
};

template <class Listener>
class EnetServer : public EnetBase {
public:
	EnetServer() : clients(NULL), clientIds(NULL), newClientId(0) {}
	~EnetServer();
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

	void connectEvent(const ENetEvent& event);
	void disconnectEvent(const ENetEvent& event);

	// From EnetBase
	void receiveEvent(const ENetEvent& event);

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

// Implementation
#include "EnetServer.tpp"

#endif