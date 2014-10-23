#include "ofApp.h"

// Connect to localhost by default
#define IP "127.0.0.1"
#define PORT 11999

// Reconnect every x millseconds
#define RECONNECT_TIME 5000

// Default reliability
#define RELIABLE 1

// Separate message data
const string DELIMITER = "|";
const int DELIMITER_ASCII = 124;

// Types of messages
const int INTRO_MSG = 90;
const int CONNECTED_MSG = 91;
const int DISCONNECTED_MSG = 92;
const int USER_MSG = 93;

// Character limit
const int MAX_CHARS = 100;

// Line limit
const int MAX_LINES = 5;

// Starting ASCII char
const int ASCII_START = 32;

// New line
const string NEW_LINE = "\n";

// Height of text to display
const int TEXT_HEIGHT = 15;

//--------------------------------------------------------------
void ofApp::setup(){
	// Update at 30FPS
	ofSetVerticalSync(true);
	ofSetFrameRate(30);

	// Whether we've typed
	typed = false;

	// Typing line
	messageLine = 0;

	// Typing position
	messagePos = 0;

	// Send and recieve strings
	sendMessages.push_back(string());
	receiveMessage = "";

	// Set up the client with default settings
	EnetClientSettings settings;
	settings.default();
	client.setup(this, settings);
	client.startup();

	// User defined server
	if (args.size() >= 3) {
		serverIp = args[1];
		serverPort = ofToInt(args[2]);
	} else {
		serverIp = IP;
		serverPort = PORT;
	}

	// Attempt to connect to server
	connected = client.connect(serverIp, serverPort);
	
	// To reconnect
	reconnectAttempt = 0;
	connectTime = 0;
	deltaTime = 0;

	// Send packets either reliably or unreliably
	// Unreliable packets get sent every update
	if (args.size() >= 4) {
		if (args[3] == "r") {
			reliable = true;
		} else if (args[3] == "u") {
			reliable = false;
		}
	} else {
		reliable = RELIABLE;
	}
}

//--------------------------------------------------------------
void ofApp::exit(){
	// Cleanup
	client.shutdown();
}

//--------------------------------------------------------------
void ofApp::update(){
	// Set background color
	ofBackground(20, 20, 20);

	// We're connected to the server so let's poll for incoming packets
	// and send our user message
	if (connected) {
		reconnectAttempt = 0;
		if (!reliable) {
			string message = messageToSend();
			client.queuePacket(message.c_str(), message.length() + 1, 0);
		}

		// Poll for incoming packets and send queued outgoing packets
		client.poll();
	} else {
		// If we're not connected let's try and reconnect
		deltaTime = ofGetElapsedTimeMillis() - connectTime;
		if (deltaTime > RECONNECT_TIME * reconnectAttempt) {
			if (reconnectAttempt < 3) {
				++reconnectAttempt;
			}
			connected = client.connect(serverIp, serverPort);
			connectTime = ofGetElapsedTimeMillis();
		}
	}
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
	ofDrawBitmapString("openFrameworks UDP Client Example - " + reliability, 15, 30);

	int serverPosY = 55;

	if (connected) {
		if (typed) {
			ofDrawBitmapString("Sending:", 15, 55);
			ofDrawBitmapString(messageToSend(), 85, 55);
		} else {
			ofDrawBitmapString("Status: type something to send to server", 15, 55);
		}
		serverPosY += (TEXT_HEIGHT * (MAX_LINES + 1));
		ofDrawBitmapString("Server says: " + receiveMessage, 15, serverPosY);
	} else {
		ofDrawBitmapString("Status: unable to connect to server at " + serverIp + ":" + ofToString(serverPort) +
			"\n\nReconnecting in " + ofToString((int)((RECONNECT_TIME * reconnectAttempt) - deltaTime) / 1000) + " seconds", 15, 55);
	}

	// Print each connected client's info and user message to the screen
	if (!clients.empty()) {
		int xPos = 15;
		int userMessagesPosY = serverPosY + (2 * TEXT_HEIGHT);
		int yPos;
		for (vector<int>::iterator it = clients.begin(); it != clients.end(); ++it) {

			// Give client its own color
			ofSetColor(255 - *it*30, 255 - *it * 20, 100 + *it*40);

			// Calculate where to draw the text
			yPos = userMessagesPosY + (*it * TEXT_HEIGHT * (MAX_LINES + 2));

			// Draw client's user message
			ofDrawBitmapString("client " + ofToString(*it) + " says:", xPos, yPos);
			ofDrawBitmapString(userMessages[*it], xPos, yPos + TEXT_HEIGHT);
		}
	}
}


//--------------------------------------------------------------
void ofApp::keyPressed(int key){
	// Only record keystrokes when connected
	if (connected) {

		// Ignore special keys and delimiter
		if (key == OF_KEY_MODIFIER ||
			key == OF_KEY_F1 ||
			key == OF_KEY_F2 ||
			key == OF_KEY_F3 ||
			key == OF_KEY_F4 ||
			key == OF_KEY_F5 ||
			key == OF_KEY_F6 ||
			key == OF_KEY_F7 ||
			key == OF_KEY_F8 ||
			key == OF_KEY_F9 ||
			key == OF_KEY_F10 ||
			key == OF_KEY_F11 ||
			key == OF_KEY_F12 ||
			key == OF_KEY_LEFT ||
			key == OF_KEY_UP ||
			key == OF_KEY_RIGHT ||
			key == OF_KEY_DOWN ||
			key == OF_KEY_PAGE_UP ||
			key == OF_KEY_PAGE_DOWN ||
			key == OF_KEY_HOME ||
			key == OF_KEY_END ||
			key == OF_KEY_INSERT ||
			key == OF_KEY_CONTROL ||
			key == OF_KEY_ALT ||
			key == OF_KEY_SHIFT ||
			key == OF_KEY_SUPER ||
			key == OF_KEY_LEFT_SHIFT ||
			key == OF_KEY_RIGHT_SHIFT ||
			key == OF_KEY_LEFT_CONTROL ||
			key == OF_KEY_RIGHT_CONTROL ||
			key == OF_KEY_LEFT_ALT ||
			key == OF_KEY_RIGHT_ALT ||
			key == OF_KEY_LEFT_SUPER ||
			key == OF_KEY_RIGHT_SUPER ||
			key == DELIMITER_ASCII) {
			return;
		}

		if (key == OF_KEY_RETURN) {
			// Start a new line and have other lines scroll up
			if (messageLine == MAX_LINES-1) {
				sendMessageSwapLines();
			} else {
				++messageLine;
				sendMessages.push_back(string());
			}
			messagePos = 0;
		} else if (key == OF_KEY_BACKSPACE || key == OF_KEY_DEL) {
			// Delete last keystroke on current line
			if (messagePos != 0) {
				--messagePos;
				sendMessages.at(messageLine) = sendMessages.at(messageLine).substr(0, messagePos);
			} else {
				sendMessages.at(messageLine) = "";
			}
		} else if (messagePos < MAX_CHARS && (key >= ASCII_START || key == OF_KEY_TAB)) {
			// Add new keystroke to current line
			sendMessages.at(messageLine).append(1, (char)key);
			++messagePos;
		} else {
			// Ignore keystroke
			return;
		}

		typed = true;

		if (reliable) {
			// Queue message to be sent next update
			string message = messageToSend();
			client.queuePacket(message.c_str(), message.length() + 1, 1);
			client.sendQueuedPackets();
		}
	}
}

//--------------------------------------------------------------
void ofApp::disconnectEvent(){
	connected = false;
	clients.clear();
	userMessages.clear();
}

//--------------------------------------------------------------
void ofApp::receiveEvent(const char* message, size_t messageSize, uint16_t channelId){
	// Split the message into parts
	vector<string> receivedMessages = ofSplitString(message, DELIMITER);
	
	// Parse message by type
	int messageType = ofToInt(receivedMessages[0]);

	// Intro message
	if (messageType == INTRO_MSG) {
		receiveMessage = receivedMessages[1];
		int clientId;
		for (vector<string>::iterator it = receivedMessages.begin() + 2; it != receivedMessages.end(); ++it) {
			clientId = ofToInt(*it);
			clients.push_back(clientId);
			while (clientId >= userMessages.size()) {
				userMessages.push_back(string());
			}
		}
	} else if (messageType == CONNECTED_MSG) {
		// Connection message
		// Store connected client
		int clientId = ofToInt(receivedMessages[1]);
		clients.push_back(clientId);
		while (clientId >= userMessages.size()) {
			userMessages.push_back(string());
		}
	} else if (messageType == DISCONNECTED_MSG) {
		// Disconnection message
		// Remove disconnected client from storage
		int clientId = ofToInt(receivedMessages[1]);
		for (vector<int>::iterator it = clients.begin(); it != clients.end(); ++it) {
			if (*it == clientId) {
				clients.erase(it);
				break;
			}
		}
	} else if (messageType == USER_MSG) {
		// User messages
		// Store user messages
		int clientId;
		for (vector<string>::iterator it = receivedMessages.begin() + 1; it != receivedMessages.end(); ++it) {
			clientId = ofToInt(*it);
			++it;
			if (clientId < userMessages.size()) {
				userMessages[clientId] = *it;
			}
		}
	}
}

//--------------------------------------------------------------
void ofApp::error(uint8_t err){
}

//--------------------------------------------------------------
string ofApp::messageToSend(){
	vector<string>::iterator it = sendMessages.begin();
	string messageToSend(*it);
	for (++it; it != sendMessages.end(); ++it) {
		messageToSend += NEW_LINE + *it;
	}
	return messageToSend;
}

void ofApp::sendMessageSwapLines(){
	if (sendMessages.size() > 1) {
		for (vector<string>::iterator it = sendMessages.begin() + 1; it != sendMessages.end(); ++it) {
			*(it - 1) = string(*it);
		}
	}
	sendMessages.back() = "";
}