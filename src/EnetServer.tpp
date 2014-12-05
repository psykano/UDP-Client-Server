template <class Listener>
EnetServer<Listener>::~EnetServer() {
	// Deallocate memory for clients
	if (clients) {
		delete [] clients;
	}
	if (clientIds) {
		delete clientIds;
	}
}

template <class Listener>
void EnetServer<Listener>::setup(EnetServerListener<Listener>* _listener, EnetServerSettings _settings) {
	listener = _listener;
	settings = _settings;
	// Allocate memory for clients
	clients = new ENetPeer*[_settings.connections];
	clientIds = new uint16_t[_settings.connections];
	for (uint16_t i = 0; i < _settings.connections; ++i) {
		clients[i] = NULL;
		clientIds[i] = i;
	}
}

template<class Listener>
void EnetServer<Listener>::shutdown() {
	adapter.enetDisconnectAllPeersNow();
	kill();
}

template <class Listener>
void EnetServer<Listener>::poll() {
	ENetEvent event;
	while (adapter.enetPollEvent(&event) > 0) {
		switch (event.type) {
		case ENET_EVENT_TYPE_RECEIVE:
			receiveEvent(event);
			// Clean up
			enet_packet_destroy(event.packet);
			break;
		case ENET_EVENT_TYPE_CONNECT:
			connectEvent(event);
			break;
		case ENET_EVENT_TYPE_DISCONNECT:
			disconnectEvent(event);
			break;
		}
	}
}

template <class Listener>
bool EnetServer<Listener>::startup(uint16_t port) {
	if (adapter.enetInit()) {
		if (adapter.enetCreateWithAddressPortOnly(port, settings.connections, settings.channels, settings.inBandwidth, settings.outBandwidth)) {
			return true;
		} else {
			listener->errorInterface(ENET_SERVER_ERROR_CREATE);
		}
	} else {
		listener->errorInterface(ENET_SERVER_ERROR_INIT);
	}
	return false;
}

template <class Listener>
void EnetServer<Listener>::disconnectClient(uint16_t clientId) {
	ENetPeer* client = clients[clientId];
	if (client) {
		adapter.enetDisconnect(client);
	} else {
		listener->errorInterface(ENET_SERVER_ERROR_NULL_CLIENT, &clientId);
	}
}

template <class Listener>
void EnetServer<Listener>::disconnectClientNow(uint16_t clientId) {
	ENetPeer* client = clients[clientId];
	if (client) {
		adapter.enetDisconnectNow(client);
		clients[clientId] = NULL;
		dClientIds.push(clientId);
	} else {
		listener->errorInterface(ENET_SERVER_ERROR_NULL_CLIENT, &clientId);
	}
}

template <class Listener>
std::string EnetServer<Listener>::ipOfClientAsString(uint16_t clientId) {
	ENetPeer* client = clients[clientId];
	if (client) {
		char hostName[IP_MAX_ADDRESS];
		enet_address_get_host_ip(&client->address, hostName, IP_MAX_ADDRESS);
		return std::string(hostName);
	} else {
		listener->errorInterface(ENET_SERVER_ERROR_NULL_CLIENT, &clientId);
	}
	return std::string();
}

template <class Listener>
uint32_t EnetServer<Listener>::ipOfClientAsInt(uint16_t clientId) {
	ENetPeer* client = clients[clientId];
	if (client) {
		return client->address.host;
	} else {
		listener->errorInterface(ENET_SERVER_ERROR_NULL_CLIENT, &clientId);
	}
	return 0;
}

template <class Listener>
uint16_t EnetServer<Listener>::portOfClient(uint16_t clientId) {
	ENetPeer* client = clients[clientId];
	if (client) {
		return client->address.port;
	} else {
		listener->errorInterface(ENET_SERVER_ERROR_NULL_CLIENT, &clientId);
	}
	return 0;
}

template <class Listener>
uint32_t EnetServer<Listener>::meanPingOfClient(uint16_t clientId) {
	ENetPeer* client = clients[clientId];
	if (client) {
		return client->roundTripTime;
	} else {
		listener->errorInterface(ENET_SERVER_ERROR_NULL_CLIENT, &clientId);
	}
	return 0;
}

template <class Listener>
void EnetServer<Listener>::queuePacket(uint16_t clientId, const char* message, size_t messageSize, uint8_t channelId) {
	ENetPeer* client = clients[clientId];
	if (client) {
		adapter.enetQueuePacket(client, message, messageSize, channelId, settings.channelFlags[channelId]);
	} else {
		listener->errorInterface(ENET_SERVER_ERROR_NULL_CLIENT, &clientId);
	}
}

template <class Listener>
void EnetServer<Listener>::connectEvent(const ENetEvent& event) {
	uint16_t i = nextFreeClientId();
	// Error
	if (i < 0) {
		listener->errorInterface(ENET_SERVER_ERROR_CONNECT_EVENT);
		return;
	}
	// Mark and index client
	{
		event.peer->data = &clientIds[i];
		clients[i] = event.peer;
	}
	listener->connectEventInterface(i);
}

template <class Listener>
void EnetServer<Listener>::disconnectEvent(const ENetEvent& event) {
	uint16_t i = *static_cast<uint16_t *>(event.peer->data);
	// Unmark client, remove from index, and store index
	{
		event.peer->data = NULL;
		clients[i] = NULL;
		dClientIds.push(i);
	}
	listener->disconnectEventInterface(i);
}

template <class Listener>
void EnetServer<Listener>::receiveEvent(const ENetEvent& event) {
	uint16_t i = *static_cast<uint16_t *>(event.peer->data);
	listener->receiveEventInterface(i, reinterpret_cast<char*>(event.packet->data), event.packet->dataLength, event.channelID);
}

// O(1)
// Returns next available client index and removes its availability
// Returns -1 if none are available
template <class Listener>
uint16_t EnetServer<Listener>::nextFreeClientId() {
	if (!dClientIds.empty()) {
		uint16_t n = dClientIds.front();
		dClientIds.pop();
		return n;
	}
	if (newClientId == settings.connections) {
		return -1;
	}
	return newClientId++;
}