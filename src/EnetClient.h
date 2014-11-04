#ifndef ENET_CLIENT
#define ENET_CLIENT

#include "EnetBase.h"
#include "EnetClientListener.h"
#include "EnetSettings.h"

enum ENET_CLIENT_ERROR_CODE {
	ENET_CLIENT_ERROR_INIT,
	ENET_CLIENT_ERROR_CREATE,
	ENET_CLIENT_ERROR_NULL_SERVER,
	ENET_CLIENT_ERROR_ALREADY_CONNECTED,
	ENET_CLIENT_ERROR_NOT_CONNECTED
};

template<class Listener>
class EnetClient : public EnetBase {
public:
	EnetClient() : server(NULL) {}
	void setup(EnetClientListener<Listener>* _listener, EnetClientSettings _settings);

	// From EnetBaseInterface
	void shutdown();
	void poll();

	bool startup();
	bool connect(const std::string& ip, uint16_t port);
	void disconnect();
	void queuePacket(const char* message, size_t messageSize, uint8_t channelId);
	void disconnectEvent();
	uint32_t meanServerPing();

	// From EnetBase
	void receiveEvent(const ENetEvent& event);

private:
	EnetClientListener<Listener>* listener;
	EnetClientSettings settings;
	ENetPeer* server;
};

// Implementation
#include "EnetClient.tpp"

#endif