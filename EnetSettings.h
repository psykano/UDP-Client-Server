#ifndef ENET_SETTINGS
#define ENET_SETTINGS

#include <stdint.h>
#include "enet\enet.h"

#define SERVER_MAX_CHANNELS 4
#define CLIENT_MAX_CHANNELS 2

template <uint8_t MaxChannels>
struct EnetSettings {
	uint8_t channels;
	uint32_t channelFlags[MaxChannels];
	uint32_t inBandwidth;
	uint32_t outBandwidth;

	void default() {
		channels = 2;
		// For unreliable packets
		channelFlags[0] = 0;
		// For reliable packets
		channelFlags[1] = ENET_PACKET_FLAG_RELIABLE;
		inBandwidth = 0;
		outBandwidth = 0;
	}
};

struct EnetServerSettings : EnetSettings<SERVER_MAX_CHANNELS> {
	uint16_t connections;
};

struct EnetClientSettings : EnetSettings<CLIENT_MAX_CHANNELS> {
	uint32_t connectTimeout;
	uint32_t disconnectTimeout;

	void default() {
		EnetSettings::default();
		// 5s
		connectTimeout = 5000;
		// 3s
		disconnectTimeout = 3000;
	}
};

#endif