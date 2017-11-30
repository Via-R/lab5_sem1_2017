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

void setFontColor(size_t c) {
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(hConsole, c);
}


unsigned long __stdcall MessageRecThread(void* pParam);

ClientPack::ClientPack():connEstablished(false){
	cout << "Client starting...\n\n";
}

void ClientPack::ConnectToServer(string address, int port, string locID) {

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
	SendMsg(locID);
	connEstablished = true;
}

ClientPack::~ClientPack() {
	if (connEstablished)
		closesocket(connection);
}

bool ClientPack::SendMsg(string text) {
	if (send(connection, text.c_str(), text.size() + 1, 0) == -1)
		return false;
	return true;
}

bool ClientPack::ReceiveMsg(){
	char message[4096];
	if (recv(connection, message, 4096, 0) == -1)
		return false;
	cout << "\b\b";

	string msg(message);
	size_t pos1 = msg.find("$");
	size_t pos2 = msg.find(": ");
	size_t fontNum = stoi(msg.substr(pos2 + 2, pos1-pos2));
	string name = msg.substr(0, pos2);
	string text = msg.substr(pos1 + 1);
	
	setFontColor(fontNum);
	cout << name;
	setFontColor(7);
	cout << ": " << text << endl;
	cout << "> ";
	return true;
}


unsigned long __stdcall MessageRecThread(void* pParam){
	while (1) {
		if (!ClientEntity.ReceiveMsg())
			break;
	}
	return 0;
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
	cout << "Please enter your id (using natural numbers): ";
	string locID;
	cin >> locID;
	cin.ignore();
	ClientEntity.ConnectToServer(serverAddress.c_str(), 10007, locID);
	if (!ClientEntity.IsConnected()){
		printError("Unable to connect to the IP specified in server.ini", true);
		return;
	}

	DWORD threadId;
	CreateThread(NULL, NULL, MessageRecThread, NULL, NULL, &threadId);
	cout << "> ";
	while (gets_s(buf)){
		if (strlen(buf) == 0) {
			cout << "\nClient stop was initiated manually\n";
			break;
		}
		if (!ClientEntity.SendMsg(buf)){
			printError("Cannot connect to the server. Check whether server is running", false);
			break;
		}
		cout << "> ";
	}

	cout << "Client stopped\n\n";
	system("pause");
}

