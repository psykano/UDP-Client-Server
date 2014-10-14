template<class Listener>
void EnetClient<Listener>::setup(EnetClientListener<Listener>* _listener, EnetClientSettings _settings) {
	listener = _listener;
	settings = _settings;
}

template<class Listener>
bool EnetClient<Listener>::startup() {
	if (adapter.enetInit()) {
		if (adapter.enetCreateNoAddress(1, settings.channels, settings.inBandwidth, settings.outBandwidth)) {
			return true;
		} else {
			listener->errorInterface(ENET_CLIENT_ERROR_CREATE);
		}
	} else {
		listener->errorInterface(ENET_CLIENT_ERROR_INIT);
	}
	return false;
}

template<class Listener>
void EnetClient<Listener>::shutdown() {
	if (server) {
		adapter.enetDisconnectNow(server);
	}
	kill();
}

template<class Listener>
bool EnetClient<Listener>::connect(const std::string& ip, uint16_t port) {
	// Must disconnect before reconnecting
	if (!server) {
		server = adapter.enetConnectWithTimeout(ip, port, settings.channels, settings.connectTimeout);
		if (server) {
			return true;
		}
	} else {
		listener->errorInterface(ENET_CLIENT_ERROR_ALREADY_CONNECTED);
	}
	return false;
}

template<class Listener>
void EnetClient<Listener>::disconnect() {
	if (server) {
		adapter.enetDisconnectWithTimeout(server, settings.disconnectTimeout);
		server = NULL;
	} else {
		listener->errorInterface(ENET_CLIENT_ERROR_NOT_CONNECTED);
	}
}

template<class Listener>
void EnetClient<Listener>::queuePacket(const char* message, size_t messageSize, uint8_t channelId) {
	if (server) {
		adapter.enetQueuePacket(server, message, messageSize, channelId, settings.channelFlags[channelId]);
	} else {
		listener->errorInterface(ENET_CLIENT_ERROR_NOT_CONNECTED);
	}
}

template<class Listener>
void EnetClient<Listener>::poll() {
	ENetEvent event;
	while (adapter.enetPollEvent(&event) > 0) {
		switch (event.type) {
		case ENET_EVENT_TYPE_RECEIVE:
			receiveEvent(event);
			// Clean up
			enet_packet_destroy(event.packet);
			break;
		case ENET_EVENT_TYPE_DISCONNECT:
			disconnectEvent();
			break;
		}
	}
}

template<class Listener>
void EnetClient<Listener>::receiveEvent(const ENetEvent& event) {
	listener->receiveEventInterface(reinterpret_cast<char*>(event.packet->data), event.packet->dataLength, event.channelID);
}

template<class Listener>
void EnetClient<Listener>::disconnectEvent() {
	server = NULL;
	listener->disconnectEventInterface();
}