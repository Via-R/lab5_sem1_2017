#pragma once
#include <winsock2.h>
#include <list>
#include <iostream>
#include <string>
#include <map>
#pragma comment(lib, "Ws2_32.lib")
using namespace std;

unsigned int version_major = 2;
unsigned int version_minor = 1;

class VChatServer{
public:
	VChatServer();
	~VChatServer();
	bool IsConnected() { return connEstablished; } // returns connection status
	bool StartListening(); // Listen to client
	bool SendMessageToAll(string, string); // Send message to all clients, true if successful.
	bool RecClient(SOCKET); // receive message for a particular socket, false if connection ends
	void sendTermination(); // Send message about server shutdown
	void destroy();
private:
	bool connEstablished; // true - connected, false - not connected
	list<pair<SOCKET, string>> clientsList; // All sockets connected to client
	SOCKET receiver; // socket listening for client calls
	map<string, size_t> cliColors;
};
