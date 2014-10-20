#ifndef ENET_CLIENT_LISTENER
#define ENET_CLIENT_LISTENER

#include "enet\enet.h"

// CRTP listener
template <class Derived>
struct EnetClientListener {
	void disconnectEventInterface() {
		static_cast<Derived*>(this)->disconnectEvent();
	}

	void receiveEventInterface(const char* message, size_t messageSize, uint16_t channelId) {
		static_cast<Derived*>(this)->receiveEvent(message, messageSize, channelId);
	}

	void errorInterface(uint8_t err) {
		static_cast<Derived*>(this)->error(err);
	}

	void disconnectEvent();
	void receiveEvent(const char* message, size_t messageSize, uint16_t channelId);
	void error(uint8_t err);
};

#endif