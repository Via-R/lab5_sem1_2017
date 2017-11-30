#include "chat_server.h"
#include <algorithm>
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

void setFontColor(size_t c) {
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(hConsole, c);
}

size_t getFontNumber(string text);
unsigned long __stdcall ServerRecThread(void* pParam);

void VChatServer::StartListening(){
	SOCKET currCliSocket;
	sockaddr_in incoming;
	int incominglen = sizeof(incoming);

	currCliSocket = accept(receiver, (struct sockaddr*)&incoming, &incominglen);

	char temp[4096];
	int locStatus;
	locStatus = recv(currCliSocket, temp, 4096, 0);
	if (locStatus == -1) {
		printError("Cannot get user's id", false);
		return;
	}
	string resID(temp);

	cout << "\b\b";
	string message = "#" + resID + " connected";
	cout << "> ";
	setFontColor(12);
	cout << message << endl;
	setFontColor(7);
	cout << "> ";
	message = "12$" + message;
	SendMessageToAll(message, "System");
	if (currCliSocket != INVALID_SOCKET) {
		clientsList.push_back(make_pair(currCliSocket, resID));
		cliColors.insert(make_pair(resID, getFontNumber(resID)));
	}

	DWORD threadId;
	CreateThread(NULL, NULL, ServerRecThread, (void *)currCliSocket, NULL, &threadId);
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



bool VChatServer::SendMessageToAll(string text, string id = ""){
	int locStatus = 0;
	if(!id.size())
		text = "Server: 9$" + text;
	else if (id == "System") {
		text = "System: " + text;
	}
	else
		text = "#" + id + ": " + text;
	if(clientsList.size() == 0)
		return true;

	
	for (auto it = clientsList.begin(); it != clientsList.end(); it++) {
		if (id != "" && id == it->second)
			continue;
		locStatus = send(it->first, text.c_str(), text.size() + 1, 0);
		if (locStatus == -1)
			clientsList.remove(*it);
	}
	if(locStatus == -1)
		return false;
	return true;
}

bool VChatServer::RecClient(SOCKET cliSocket){
	char temp[4096];
	int locStatus;
	auto it = find_if(clientsList.begin(), clientsList.end(), [=](const std::pair<SOCKET, string>& element) { return element.first == cliSocket; });

	locStatus = recv(cliSocket, temp, 4096, 0);
	if (locStatus == -1) {
		clientsList.erase(it);
		return false;
	}
	else {
		string message(temp);
		cout << "\b\b";
		setFontColor(cliColors[it->second]);
		cout << "#" << it->second;
		setFontColor(7);
		cout << ": " << message << "\n";
		message = to_string(cliColors[it->second]) + "$" + message;
		//cout << message << endl;
		SendMessageToAll(message, it->second);
		cout << "> ";
	}
	return true;
}

unsigned long __stdcall ServerRecThread(void* pParam) {
	SOCKET cliSocket = (SOCKET)pParam;
	while (1){
		if (!ServerEntity.RecClient(cliSocket))
			break;
	}
	return 0;
}

unsigned long __stdcall ServerListenThread(void* pParam) {
	while (1)
		ServerEntity.StartListening();
	return 0;
}

size_t getFontNumber(string text) {
	hash<string> hash_fn;
	size_t str_hash = hash_fn(text);
	size_t temp = 0;
	do {
		temp = 0;
		while (str_hash > 0) {
			temp += str_hash % 10;
			str_hash /= 10;
		}
		str_hash = temp;
	} while (temp > 10);
	temp += 5;
	if (temp == 9 || temp == 12)
		--temp;
	return temp;
}

void main(){
	if (!ServerEntity.IsConnected()) {
		printError("Failed to initialise server socket", true);
		return;
	}
	
	char buf[4096];
	cout << ">--------------------------------------<\n";
	cout << "VChat 1.1\n";
	cout << "\"A meaningful silence is always better than meaningless words\" - Anonymous\n\n";
	cout << "Running locally on port 10007\n";
    cout << "Send an empty message to stop the server\n";
	cout << ">--------------------------------------<\n\n";

	DWORD threadId;
	CreateThread(NULL, NULL, ServerListenThread, NULL, NULL, &threadId);
	
	cout << "> ";
	while(gets_s(buf)){
		if (strlen(buf) == 0) {
			cout << "\nServer stop was initiated manually\n";
			break;
		}
		if(!ServerEntity.SendMessageToAll(buf)){
			printError("Cannot connect to receivers", false);
			break;
		}
		cout << "> ";
	}

	cout << "Server stopped\n\n";
	system("pause");
}
