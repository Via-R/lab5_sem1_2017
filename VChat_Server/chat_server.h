#pragma once
#include <winsock2.h>
#include <list>
#include <iostream>
#include <string>
#pragma comment(lib, "Ws2_32.lib")
using namespace std;

class VChatServer{
public:
	VChatServer();
	~VChatServer();
	bool IsConnected() { return connEstablished; } // returns connection status
	void StartListening(); // Listen to client
private:
	bool connEstablished; // true - connected, false - not connected
	list<SOCKET> clientsList; // All sockets connected to client
	SOCKET receiver; // socket listening for client calls
};
