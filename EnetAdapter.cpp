#include "EnetAdapter.h"

bool EnetAdapter::enetInit() {
	if (enet_initialize()) {
		return false;
	}
	return true;
}

void EnetAdapter::enetDeinit() {
	enet_deinitialize();
}

bool EnetAdapter::enetCreateWithAddress(const std::string& ip, uint16_t port, uint16_t connections, uint8_t channels, uint32_t inBandwidth, uint32_t outBandwidth) {
	ENetAddress address;
	enet_address_set_host(&address, ip.c_str());
	address.port = port;
	return enetCreate(&address, connections, channels, inBandwidth, outBandwidth);
}

bool EnetAdapter::enetCreateWithAddressPortOnly(uint16_t port, uint16_t connections, uint8_t channels, uint32_t inBandwidth, uint32_t outBandwidth) {
	ENetAddress address;
	address.host = ENET_HOST_ANY;
	address.port = port;
	return enetCreate(&address, connections, channels, inBandwidth, outBandwidth);
}

bool EnetAdapter::enetCreateNoAddress(uint16_t connections, uint8_t channels, uint32_t inBandwidth, uint32_t outBandwidth) {
	return enetCreate(NULL, connections, channels, inBandwidth, outBandwidth);
}

void EnetAdapter::enetDestroy() {
	enet_host_destroy(host);
}

ENetPeer* EnetAdapter::enetConnectWithTimeout(const std::string& ip, uint16_t port, uint8_t channels, uint32_t timeout) {
	if (ENetPeer* peer = enetConnect(ip, port, channels)) {
		ENetEvent event;
		if (enet_host_service(host, &event, timeout) > 0 && event.type == ENET_EVENT_TYPE_CONNECT) {
			return peer;
		} else {
			// Timed out
			enetForceDisconnect(peer);
		}
	}
	return NULL;
}

void EnetAdapter::enetDisconnect(ENetPeer* peer) {
	// No user data
	enet_peer_disconnect(peer, 0);
}

void EnetAdapter::enetDisconnectWithTimeout(ENetPeer* peer, uint32_t timeout) {
	enetDisconnect(peer);
	ENetEvent event;
	while (enet_host_service(host, &event, timeout) > 0) {
		switch (event.type) {
		case ENET_EVENT_TYPE_RECEIVE:
			enet_packet_destroy(event.packet);
			break;
		case ENET_EVENT_TYPE_DISCONNECT:
			return;
		}
	}
	// Timed out
	enetForceDisconnect(peer);
}

void EnetAdapter::enetForceDisconnect(ENetPeer* peer) {
	enet_peer_reset(peer);
}

void EnetAdapter::enetQueuePacket(ENetPeer* peer, const char* message, size_t messageSize, uint8_t channel, uint32_t reliability) {
	ENetPacket* packet = enet_packet_create(message, messageSize, reliability);
	enet_peer_send(peer, channel, packet);
}

void EnetAdapter::enetSendQueuedPackets() {
	enet_host_flush(host);
}

int EnetAdapter::enetPollEvent(ENetEvent* event) {
	return enet_host_service(host, event, 0);
}

bool EnetAdapter::enetCreate(const ENetAddress* address, uint16_t connections, uint8_t channels, uint32_t inBandwidth, uint32_t outBandwidth) {
	host = enet_host_create(address, connections, channels, inBandwidth, outBandwidth);
	if (!host) {
		return false;
	}
	return true;
}

ENetPeer* EnetAdapter::enetConnect(const std::string& ip, uint16_t port, uint8_t channels) {
	ENetAddress address;
	enet_address_set_host(&address, ip.c_str());
	address.port = port;
	// No user data
	return enet_host_connect(host, &address, channels, 0);
}