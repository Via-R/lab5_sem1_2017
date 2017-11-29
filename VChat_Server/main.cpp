#include "chat_server.h"

VChatServer ServerEntity;

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

void VChatServer::StartListening(){
	SOCKET currCliSocket;
	sockaddr_in incoming;
	int incominglen = sizeof(incoming);

	currCliSocket = accept(receiver, (struct sockaddr*)&incoming, &incominglen);

	if (currCliSocket != INVALID_SOCKET)
		clientsList.push_back(currCliSocket);
	cout << ">> Client connected!\n";
}

VChatServer::VChatServer(){
    cout << "Starting up server...\n\n";
	connEstablished = false;

    WSADATA wsaData;

    sockaddr_in local;

	int wsaret = WSAStartup(0x101, &wsaData);

	if (wsaret != 0)
        return;

	local.sin_family = AF_INET; 
	local.sin_addr.s_addr = INADDR_ANY; 
	local.sin_port = htons((u_short)10007);

	receiver = socket(AF_INET, SOCK_STREAM, 0);


	if (receiver == INVALID_SOCKET) {
		printError("Couldn't establish receiving socket", false);
		return;
	}

	if (bind(receiver, (sockaddr*)&local, sizeof(local)) != 0) {
		printError("Couldn't bind socket", false);
		return;
	}

	if (listen(receiver, 10) != 0) {
		printError("Couldn't start listening on socket", false);
		return;
	}

	connEstablished = true;
}

VChatServer::~VChatServer() {
    closesocket(receiver);
    WSACleanup();
}

unsigned long __stdcall ServerListenThread(void* pParam) {
	while (1)
		ServerEntity.StartListening();
	return 0;
}

void main(){
	if (!ServerEntity.IsConnected()) {
		printError("Failed to initialise server socket", true);
		return;
	}
	
	char buf[4096];
	cout << ">--------------------------------------<\n";
	cout << "VChat 1.0\n";
	cout << "\"A meaningful silence is always better than meaningless words\" - Anonymous\n\n";
	cout << "Running locally on port 10007\n";
    cout << "Send an empty message to stop the server\n";
	cout << ">--------------------------------------<\n\n";

	
	DWORD threadId;
	CreateThread(NULL, NULL, ServerListenThread, NULL, NULL, &threadId);

	while(gets_s(buf)){
		if (strlen(buf) == 0) {
			cout << "Server stop was initiated manually\n";
			break;
		}
	}
	cout << "Server stopped\n\n";
	system("pause");
}