#pragma once
#include <winsock2.h>
#include <iostream>
#include <string>
#pragma comment(lib, "Ws2_32.lib")
using namespace std;

unsigned int version_major = 2;
unsigned int version_minor = 1;

class ClientPack{
public:
	ClientPack();
	~ClientPack();
	void ConnectToServer(string, int, string); // connecting to the server
	bool SendMsg(string); // false if sending failed
	bool ReceiveMsg(); // false if connection failed
	bool IsConnected() { return connEstablished; }
	void destroy();
private:
	bool connEstablished; // true - connected, false - not connected
	string serverIP;
	int serverPort;
	SOCKET connection; // socket connected to server
};
