#pragma once
#include <winsock2.h>
#include <list>
#include <iostream>
#include <string>
#include <map>
#pragma comment(lib, "Ws2_32.lib")
using namespace std;

class VChatServer{
public:
	VChatServer();
	~VChatServer();
	bool IsConnected() { return connEstablished; } // returns connection status
	void StartListening(); // Listen to client
	bool SendMessageToAll(string, string); // Send message to all clients, true if successful.
	bool RecClient(SOCKET); // receive message for a particular socket, false if connection ends
private:
	bool connEstablished; // true - connected, false - not connected
	list<pair<SOCKET, string>> clientsList; // All sockets connected to client
	SOCKET receiver; // socket listening for client calls
	map<string, size_t> cliColors;
};
