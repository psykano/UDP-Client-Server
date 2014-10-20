#ifndef ENET_ADAPTER
#define ENET_ADAPTER

#include <stdint.h>
#include <string>
#include "enet\enet.h"

class EnetAdapter {
public:
	bool enetInit();
	void enetDeinit();
	bool enetCreateWithAddress(const std::string& ip, uint16_t port, uint16_t connections, uint8_t channels, uint32_t inBandwidth, uint32_t outBandwidth);
	bool enetCreateWithAddressPortOnly(uint16_t port, uint16_t connections, uint8_t channels, uint32_t inBandwidth, uint32_t outBandwidth);
	bool enetCreateNoAddress(uint16_t connections, uint8_t channels, uint32_t inBandwidth, uint32_t outBandwidth);
	void enetDestroy();
	ENetPeer* enetConnectWithTimeout(const std::string& ip, uint16_t port, uint8_t channels, uint32_t timeout);
	void enetDisconnect(ENetPeer* peer);
	void enetDisconnectWithTimeout(ENetPeer* peer, uint32_t timeout);
	void enetDisconnectNow(ENetPeer* peer);
	void enetForceDisconnect(ENetPeer* peer);
	void enetDisconnectAllPeersNow();
	void enetQueuePacket(ENetPeer* peer, const char* message, size_t messageSize, uint8_t channel, uint32_t reliability);
	void enetSendQueuedPackets();
	int enetPollEvent(ENetEvent* event);

private:
	bool enetCreate(const ENetAddress* address, uint16_t connections, uint8_t channels, uint32_t inBandwidth, uint32_t outBandwidth);
	ENetPeer* enetConnect(const std::string& ip, uint16_t port, uint8_t channels);

	ENetHost* host;
};

#endif