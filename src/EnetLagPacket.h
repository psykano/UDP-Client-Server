#ifndef ENET_LAG_PACKET
#define ENET_LAG_PACKET

#include <stdint.h>

struct EnetLagPacket {
	uint32_t processTime;
	uint16_t clientId;
	char* message;
	size_t messageSize;
	uint16_t channelId;

	EnetLagPacket(uint32_t _processTime, uint16_t _clientId, const char* _message, size_t _messageSize, uint16_t _channelId) : 
		processTime(_processTime), clientId(_clientId), messageSize(_messageSize), channelId(_channelId) {
		message = (char*)malloc(sizeof(char) * _messageSize);
		memcpy(message, _message, _messageSize);
	}

	EnetLagPacket(const EnetLagPacket& that) :
		processTime(that.processTime), clientId(that.clientId), messageSize(that.messageSize), channelId(that.channelId) {
		message = (char*)malloc(sizeof(char) * that.messageSize);
		memcpy(message, that.message, that.messageSize);
	}

	~EnetLagPacket() {
		free(message);
	}

	EnetLagPacket& operator=(const EnetLagPacket& that) {
		EnetLagPacket tmp(that);
		processTime = tmp.processTime;
		clientId = tmp.clientId;
		messageSize = tmp.messageSize;
		channelId = tmp.channelId;

		free(message);
		message = (char*)malloc(sizeof(char) * tmp.messageSize);
		memcpy(message, tmp.message, tmp.messageSize);

		return *this;
	}
};

#endif