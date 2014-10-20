#include "ofApp.h"

// Default port
#define PORT 11999

// Maximum possible connected clients
#define CLIENTS 5

// Default reliability
#define RELIABLE 1

// Separate message data
const string DELIMITER = "|";

// Types of messages
const int INTRO_MSG = 90;
const int CONNECTED_MSG = 91;
const int DISCONNECTED_MSG = 92;
const int USER_MSG = 93;

// Line limit
const int MAX_LINES = 5;

// Height of text to display
const int TEXT_HEIGHT = 15;

//--------------------------------------------------------------
void ofApp::setup(){
	// Update at 30FPS
	ofSetVerticalSync(true);
	ofSetFrameRate(30);

	// Set up the server with default settings
	EnetServerSettings settings;
	settings.default();
	settings.connections = CLIENTS;
	server.setup(this, settings);

	// Use user defined port
	if (args.size() >= 2) {
		port = ofToInt(args[1]);
	} else {
		port = PORT;
	}

	// Start server and attempt to bind to port
	server.startup(port);

	// Send packets either reliably or unreliably
	// Unreliable packets get sent every update
	if (args.size() >= 3) {
		if (args[2] == "r") {
			reliable = true;
		} else if (args[2] == "u") {
			reliable = false;
		}
	} else {
		reliable = RELIABLE;
	}
}

//--------------------------------------------------------------
void ofApp::exit(){
	// Cleanup
	server.shutdown();
}

//--------------------------------------------------------------
void ofApp::update(){
	// Set background color
	ofBackground(20, 20, 20);

	if (!reliable) {
		// Send each client the user messages (including their own)
		if (!clients.empty()) {
			string message = ofToString(USER_MSG) + DELIMITER;
			{
				vector<int>::iterator it = clients.begin();
				message += ofToString(*it) + DELIMITER + userMessages[*it];
				for (++it; it != clients.end(); ++it) {
					message += DELIMITER + ofToString(*it) + DELIMITER + userMessages[*it];
				}
			}
			for (vector<int>::iterator it = clients.begin(); it != clients.end(); ++it) {
				server.queuePacket(*it, message.c_str(), message.length() + 1, 0);
			}
		}
	}

	// Poll for incoming packets and send queued outgoing packets
	server.poll();
}

//--------------------------------------------------------------
void ofApp::draw(){
	ofSetHexColor(0xDDDDDD);

	string reliability;
	if (reliable) {
		reliability = "Reliable delivery";
	} else {
		reliability = "Unreliable delivery";
	}
	ofDrawBitmapString("openFrameworks UDP Server Example - " + reliability + "\n\nconnected on port: "+ofToString(port), 10, 20);

	ofSetHexColor(0x000000);
	ofRect(10, 60, ofGetWidth()-24, ofGetHeight() - 65 - 15);

	ofSetHexColor(0xDDDDDD);

	// Print each connected client's info and user message to the screen
	if (!clients.empty()) {
		int xPos = 15;
		int userMessagesPosY = 80;
		int yPos;
		for (vector<int>::iterator it = clients.begin(); it != clients.end(); ++it) {

			// Give client its own color
			ofSetColor(255 - *it*30, 255 - *it * 20, 100 + *it*40);

			// Calculate where to draw the text
			yPos = userMessagesPosY + (*it * TEXT_HEIGHT * (MAX_LINES + 2));

			// Get client's info: IP and port
			string clientPort = ofToString(server.portOfClient(*it));
			string clientIp = server.ipOfClientAsString(*it);
			string info = "client "+ofToString(*it)+" -connected from "+clientIp+" on port: "+clientPort;

			// Draw client's user message below info
			ofDrawBitmapString(info, xPos, yPos);
			ofDrawBitmapString(userMessages[*it], 25, yPos + TEXT_HEIGHT);
		}
	}
}

//--------------------------------------------------------------
void ofApp::connectEvent(uint16_t clientId){
	// Send client a reliable message letting them know which ID they connected as and the connected client IDs
	{
		string clientIds;
		for (vector<int>::iterator it = clients.begin(); it != clients.end(); ++it) {
			clientIds += DELIMITER + ofToString(*it);
		}
		string message = ofToString(INTRO_MSG) + DELIMITER + "hello client - you are connected as ID - " + ofToString(clientId) + clientIds;
		server.queuePacket(clientId, message.c_str(), message.length() + 1, 1);
	}

	// Store connected client
	clients.push_back(clientId);
	if (clientId >= userMessages.size()) {
		userMessages.push_back(string());
	}
	userMessages[clientId] = "";

	// Send all clients a reliable message about the connected client
	{
		string message = ofToString(CONNECTED_MSG) + DELIMITER + ofToString(clientId);
		for (vector<int>::iterator it = clients.begin(); it != clients.end(); ++it) {
			server.queuePacket(*it, message.c_str(), message.length() + 1, 1);
		}
	}

	// Send client all the user messages
	if (reliable) {
		string message = ofToString(USER_MSG) + DELIMITER;
		vector<int>::iterator it = clients.begin();
		message += ofToString(*it) + DELIMITER + userMessages[*it];
		for (++it; it != clients.end(); ++it) {
			message += DELIMITER + ofToString(*it) + DELIMITER + userMessages[*it];
		}
		server.queuePacket(clientId, message.c_str(), message.length() + 1, 0);
	}

	// Don't wait for next update to send
	server.sendQueuedPackets();
}

//--------------------------------------------------------------
void ofApp::disconnectEvent(uint16_t clientId){
	// Remove disconnected client from storage
	for (vector<int>::iterator it = clients.begin(); it != clients.end(); ++it) {
		if (*it == clientId) {
			clients.erase(it);
			break;
		}
	}

	// Send all clients a reliable message about the disconnected client
	{
		string message = ofToString(DISCONNECTED_MSG) + DELIMITER + ofToString(clientId);
		for (vector<int>::iterator it = clients.begin(); it != clients.end(); ++it) {
			server.queuePacket(*it, message.c_str(), message.length() + 1, 1);
		}
	}

	// Don't wait for next update to send
	server.sendQueuedPackets();
}

//--------------------------------------------------------------
void ofApp::receiveEvent(uint16_t clientId, const char* message, size_t messageSize, uint16_t channelId){
	// Store client's user message
	userMessages[clientId] = message;

	if (reliable) {
		// Send each client the user message (including the original sender)
		string sendMessage = ofToString(USER_MSG) + DELIMITER + ofToString(clientId) + DELIMITER + message;
		for (vector<int>::iterator it = clients.begin(); it != clients.end(); ++it) {
			server.queuePacket(*it, sendMessage.c_str(), sendMessage.length() + 1, 1);
		}
	}
}

void ofApp::error(uint8_t err, void* data) {
}