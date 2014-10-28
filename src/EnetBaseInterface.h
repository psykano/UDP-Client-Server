#ifndef ENET_BASE_INTERFACE
#define ENET_BASE_INTERFACE

struct EnetBaseInterface {
	void shutdown();
	void poll();
};

#endif