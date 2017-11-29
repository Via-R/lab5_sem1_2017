#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include "chat_client.h"

ClientPack ClientEntity;

void printError(string text, bool critical) {
	cout << "\n>-------- Error --------<\n\n";
	cout << boolalpha << "Critical: " << critical << endl;
	cout << text << endl;
	cout << "\n>-----------------------<\n\n";
	if (critical) {
		cout << "Server stopped\n\n";
		system("pause");
	}
}

ClientPack::ClientPack():connEstablished(false){
	cout << "Client starting...\n\n";
}

void ClientPack::ConnectToServer(string address, int port) {

	serverIP = address;
	serverPort = port;

	struct hostent *hostInfo;
	unsigned int addr;
	struct sockaddr_in server;

	WSADATA wsaData;

	if (WSAStartup(0x101, &wsaData) != 0) {
		printError("Winsock initiating failed", false);
		return;
	}

	connection = socket(AF_INET, SOCK_STREAM, 0);

	if (connection == INVALID_SOCKET) {
		printError("Invalid socket", false);
		return;
	}

	addr = inet_addr(serverIP.c_str());

	hostInfo = gethostbyaddr((char*)&addr, sizeof(addr), AF_INET);

	if (hostInfo == NULL) {
		printError("Failed to retrieve information about requested server", false);
		closesocket(connection);
		return;
	}

	server.sin_addr.s_addr = *((unsigned long*)hostInfo->h_addr);
	server.sin_family = AF_INET;
	server.sin_port = htons(serverPort);

	if (connect(connection, (struct sockaddr*)&server, sizeof(server))) {
		printError("Cannot establish a connection to the server", false);
		closesocket(connection);
		return;
	}
	connEstablished = true;
	cout << ">> Connected!\n";
}

ClientPack::~ClientPack() {
	if (connEstablished)
		closesocket(connection);
}

void main() {
	FILE *file;
	fopen_s(&file, "server.ini", "r");
	if (file == NULL) {
		printError("Unable to open server.ini", true);
		return;
	}
	
	char buf[4096];
	string serverAddress;
	while ((fgets(buf, 4096, file)) != NULL) {
		if (buf[0] == '#')
			continue;
		serverAddress = buf;
	}
	fclose(file);

	if (serverAddress.size() == 0) {
		printError("No IP specified in server.ini", true);
		return;
	}
	cout << ">--------------------------------------<\n";
	cout << "VChat 1.0\n";
	cout << "\"A meaningful silence is always better than meaningless words\" - Anonymous\n\n";
	cout << "Welcome, stranger\n";
	cout << "Send an empty message to stop the server\n";
	cout << ">--------------------------------------<\n\n";

	ClientEntity.ConnectToServer(serverAddress.c_str(), 10007);
	if (!ClientEntity.IsConnected()){
		printError("Unable to connect to the IP specified in server.ini", true);
		return;
	}

	while (gets_s(buf)){
		if (strlen(buf) == 0) {
			cout << "Client stop was initiated manually\n";
			break;
		}
	}

	cout << "Client stopped\n\n";
	system("pause");
}

