template <class Listener>
void EnetLagServer<Listener>::setup(EnetServerListener<Listener>* _listener, EnetServerSettings _settings) {
	listener = _listener;
	server.setup(this, _settings);
	setMinimumPing(DEFAULT_MINIMUM_PING);
}

template <class Listener>
void EnetLagServer<Listener>::setMinimumPing(uint32_t _minPing) {
	minPing = _minPing;
}

template <class Listener>
void EnetLagServer<Listener>::shutdown() {
	server.shutdown();
}

template <class Listener>
void EnetLagServer<Listener>::poll() {
	uint32_t time = getTime();

	// Process received packets in queue
	{
		vector<EnetLagPacket>::iterator it = receivedPackets.begin();
		while (it != receivedPackets.end()) {
			if (time >= it->processTime) {
				listener->receiveEventInterface(it->clientId, it->message, it->messageSize, it->channelId);
				it = receivedPackets.erase(it);
			} else {
				++it;
			}
		}
	}

	// Send queued packets
	{
		vector<EnetLagPacket>::iterator it = sendPackets.begin();
		while (it != sendPackets.end()) {
			if (time >= it->processTime) {
				server.queuePacket(it->clientId, it->message, it->messageSize, it->channelId);
				it = sendPackets.erase(it);
			} else {
				++it;
			}
		}
	}

	server.poll();
}

template <class Listener>
bool EnetLagServer<Listener>::startup(uint16_t port) {
	return server.startup(port);
}

template <class Listener>
void EnetLagServer<Listener>::disconnectClient(uint16_t clientId) {
	server.disconnectClient(clientId);
}

template <class Listener>
void EnetLagServer<Listener>::disconnectClientNow(uint16_t clientId) {
	server.disconnectClientNow(clientId);
}

template <class Listener>
std::string EnetLagServer<Listener>::ipOfClientAsString(uint16_t clientId) {
	return server.ipOfClientAsString(clientId);
}

template <class Listener>
uint32_t EnetLagServer<Listener>::ipOfClientAsInt(uint16_t clientId) {
	return server.ipOfClientAsInt(clientId);
}

template <class Listener>
uint16_t EnetLagServer<Listener>::portOfClient(uint16_t clientId) {
	return server.portOfClient(clientId);
}

template <class Listener>
uint32_t EnetLagServer<Listener>::meanPingOfClient(uint16_t clientId) {
	return server.meanPingOfClient(clientId);
}

template <class Listener>
void EnetLagServer<Listener>::queuePacket(uint16_t clientId, const char* message, size_t messageSize, uint8_t channelId) {
	uint32_t processTime = getTime() + calculatePing(clientId);
	EnetLagPacket packet(processTime, clientId, message, messageSize, channelId);
	sendPackets.push_back(packet);
}

template <class Listener>
void EnetLagServer<Listener>::connectEvent(uint16_t clientId) {
	listener->connectEventInterface(clientId);
}

template <class Listener>
void EnetLagServer<Listener>::disconnectEvent(uint16_t clientId) {
	listener->disconnectEventInterface(clientId);
}

template <class Listener>
void EnetLagServer<Listener>::receiveEvent(uint16_t clientId, const char* message, size_t messageSize, uint16_t channelId) {
	uint32_t processTime = getTime() + calculatePing(clientId);
	EnetLagPacket packet(processTime, clientId, message, messageSize, channelId);
	receivedPackets.push_back(packet);
}

template <class Listener>
void EnetLagServer<Listener>::error(uint8_t err, void* data) {
	listener->errorInterface(err, data);
}

template <class Listener>
uint32_t EnetLagServer<Listener>::getTime() {
	// Not expected to run for more than 49.7 days...
	return static_cast<uint32_t>(enet_time_get());
}

template <class Listener>
uint32_t EnetLagServer<Listener>::calculatePing(uint16_t clientId) {
	uint32_t clientPing = meanPingOfClient(clientId);
	if (minPing > clientPing) {
		return minPing - clientPing;
	}
	return 0;
}