#ifndef ENET_LAG_SERVER
#define ENET_LAG_SERVER

#include "EnetBaseInterface.h"
#include "EnetServerInterface.h"
#include "EnetServer.h"
#include "EnetServerListener.h"
#include "EnetSettings.h"
#include "EnetLagPacket.h"
#include "enet\enet.h"
#include <vector>

// Default mode is minimum ping
#define DEFAULT_MINIMUM_PING 100

template <class Listener>
class EnetLagServer : EnetBaseInterface, EnetServerInterface, EnetServerListener<EnetLagServer<Listener>> {
public:
	EnetLagServer() {}
	void setup(EnetServerListener<Listener>* _listener, EnetServerSettings _settings);
	void setMinimumPing(uint32_t _minPing);

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
	uint32_t meanPingOfClient(uint16_t clientId);
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

	uint32_t getTime();
	uint32_t calculatePing(uint16_t clientId);

	EnetServer<EnetLagServer> server;
	EnetServerListener<Listener>* listener;
	std::vector<EnetLagPacket> receivedPackets;
	std::vector<EnetLagPacket> sendPackets;

	uint32_t minPing;
};

// Implementation
#include "EnetLagServer.tpp"

#endif