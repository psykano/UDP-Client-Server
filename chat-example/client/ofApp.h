#pragma once

#include "ofMain.h"
#include "Networking\EnetClient.h"

class ofApp : public ofBaseApp, public EnetClientListener<ofApp> {

	public:

		ofApp(vector<string> _args) : args(_args) {}

		void setup();
		void exit();
		void update();
		void draw();
		void keyPressed(int key);

		void disconnectEvent();
		void receiveEvent(const char* message, size_t messageSize, uint16_t channelId);
		void error(uint8_t err);

		vector<string> args;

	private:

		string messageToSend();
		void sendMessageSwapLines();

		EnetClient<ofApp> client;

		string serverIp;
		int serverPort;

		vector<string> sendMessages;
		int messageLine;
		int messagePos;

		string receiveMessage;

		vector<int> clients;

		vector<string> userMessages;

		float reconnectAttempt;
		int connectTime;
		int deltaTime;

		bool connected;
		bool typed;
		bool messageSent;

		bool reliable;

};

