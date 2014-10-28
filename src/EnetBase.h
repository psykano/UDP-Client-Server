#ifndef ENET_BASE
#define ENET_BASE

#include "EnetBaseInterface.h"
#include "EnetAdapter.h"
#include "enet\enet.h"

class EnetBase : EnetBaseInterface {
public:
	void sendQueuedPackets();

	// From EnetBaseInterface
	void shutdown();
	void poll();

	// Override
	void receiveEvent(const ENetEvent& event);

protected:
	void kill();

	EnetAdapter adapter;
};

#endif