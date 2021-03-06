#ifndef ENET_SERVER_LISTENER
#define ENET_SERVER_LISTENER

// CRTP listener
template <class Derived>
struct EnetServerListener {
	void connectEventInterface(uint16_t clientId) {
		static_cast<Derived*>(this)->connectEvent(clientId);
	}

	void disconnectEventInterface(uint16_t clientId) {
		static_cast<Derived*>(this)->disconnectEvent(clientId);
	}

	void receiveEventInterface(uint16_t clientId, const char* message, size_t messageSize, uint16_t channelId) {
		static_cast<Derived*>(this)->receiveEvent(clientId, message, messageSize, channelId);
	}

	void errorInterface(uint8_t err, void* data = NULL) {
		static_cast<Derived*>(this)->error(err, data);
	}

	void connectEvent(uint16_t clientId);
	void disconnectEvent(uint16_t clientId);
	void receiveEvent(uint16_t clientId, const char* message, size_t messageSize, uint16_t channelId);
	void error(uint8_t err, void* data);
};

#endif