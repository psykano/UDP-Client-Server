#pragma once

#include "ofMain.h"
#include "Networking\EnetServer.h"

class ofApp : public ofBaseApp, public EnetServerListener<ofApp> {

	public:

		ofApp(vector<string> _args) : args(_args) {}

		void setup();
		void exit();
		void update();
		void draw();

		void connectEvent(uint16_t clientId);
		void disconnectEvent(uint16_t clientId);
		void receiveEvent(uint16_t clientId, const char* message, size_t messageSize, uint16_t channelId);
		void error(uint8_t err, void* data);

		vector<string> args;

	private:

		EnetServer<ofApp> server;

		int port;

		vector<int> clients;

		vector<string> userMessages;

		bool reliable;

};

