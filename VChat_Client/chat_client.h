#pragma once
#include <winsock2.h>
#include <iostream>
#include <string>
#pragma comment(lib, "Ws2_32.lib")
using namespace std;

class ClientPack{
public:
	ClientPack();
	~ClientPack();
	void ConnectToServer(string, int); //connecting to the server
	bool IsConnected() { return connEstablished; }
private:
	bool connEstablished; //true - connected, false - not connected
	string serverIP;
	int serverPort;
	SOCKET connection; //socket connected to server
};
