#ifndef ENET_BASE
#define ENET_BASE

#include "EnetAdapter.h"
#include "enet\enet.h"

class EnetBase {
public:
	void sendQueuedPackets();

	// Implement in derived class
	void shutdown();
	void poll();
	void receiveEvent(const ENetEvent& event);

protected:
	void kill();

	EnetAdapter adapter;
};

#endif