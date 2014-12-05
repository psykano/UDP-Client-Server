#ifndef ENET_SERVER_INTERFACE
#define ENET_SERVER_INTERFACE

struct EnetServerInterface {
	bool startup(uint16_t port);
	void disconnectClient(uint16_t clientId);
	void disconnectClientNow(uint16_t clientId);
	std::string ipOfClientAsString(uint16_t clientId);
	uint32_t ipOfClientAsInt(uint16_t clientId);
	uint16_t portOfClient(uint16_t clientId);
	uint32_t meanPingOfClient(uint16_t clientId);
	void queuePacket(uint16_t clientId, const char* message, size_t messageSize, uint8_t channelId);
};

#endif