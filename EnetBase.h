#ifndef ENET_BASE
#define ENET_BASE

#include "EnetAdapter.h"
#include "enet\enet.h"

class EnetBase {
public:
	void kill();
	void sendQueuedPackets();

	// Implement in derived class
	void poll();
	void receiveEvent(const ENetEvent& event);

protected:
	EnetAdapter adapter;
};

#endif