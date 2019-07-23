/*This function is the communicator module which receive 
the client request and pass the merged tiles to the 
user.*/

#include "communicator.h"

Communicator::Communicator(){}

bool Communicator::intializeServer(){
	int res = 0;

	//initialize the varaibles

	bindSucess = false;
	ListenSocket = false;
	acceptSucess = false;

	iSendResult = 0;
	iResult = 0;

	ListenSocket = INVALID_SOCKET;

	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != NO_ERROR) {
		wprintf(L"Error at WSAStartup()\n");
		res = -1;
	}

	//listrening for the socket
	assert(res == 0);

	ListenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (ListenSocket == INVALID_SOCKET) {
		wprintf(L"socket function failed with error: %u\n", WSAGetLastError());
		WSACleanup();
		res = -1;
	}

	assert(res == 0);
	//initialize the address and the port
	service.sin_family = AF_INET;
	service.sin_addr.s_addr = inet_addr(IP_ADDRESS);
	service.sin_port = htons(DEFAULT_PORT);

	iResult = bind(ListenSocket, (SOCKADDR*)& service, sizeof(service));
	if (iResult == SOCKET_ERROR) {
		wprintf(L"bind failed with error %u\n", WSAGetLastError());
		closesocket(ListenSocket);
		WSACleanup();
	}
	else {
		wprintf(L"bind returned success\n");
		res = -1;
		bindSucess = true;
	}
	assert(res == 0);
	return bindSucess;
}

void Communicator::readFrameRequest() {
	
	assert(bindSucess == true);
	if (listen(ListenSocket, SOMAXCONN) == SOCKET_ERROR) {
		wprintf(L"listen failed with error: %ld\n", WSAGetLastError());
		closesocket(ListenSocket);
		WSACleanup();
	}
	else {
		wprintf(L"Start listening to the client");
		listenSucess = true;
	}
	

	assert(listenSucess == true);

	SOCKET AcceptSocket;
	wprintf(L"Waiting for client to connect...\n");

	do {
		wprintf(L"Waiting for client to connect...\n");
		AcceptSocket = accept(ListenSocket, NULL, NULL);
		if (AcceptSocket == INVALID_SOCKET) {
			wprintf(L"accept failed with error: %ld\n", WSAGetLastError());
			closesocket(ListenSocket);
			WSACleanup();
		}
		/*else {
			acceptSucess = true;
			wprintf(L"Client connected.\n");
			closesocket(ListenSocket);
		}*/
		iResult = recv(AcceptSocket, recvbuf, recvbuflen, 0);
		if (iResult > 0) {
			printf("Bytes received: %d\n", iResult);
			for (int i = 0; i < iResult; i++) {
				printf("%d\n", recvbuf[i]);
				recvbuf[i] *= 2;
			}
			// Echo the buffer back to the sender
			iSendResult = send(AcceptSocket, recvbuf, iResult, 0);
			if (iSendResult == SOCKET_ERROR) {
				printf("send failed with error: %d\n", WSAGetLastError());
				closesocket(AcceptSocket);
				WSACleanup();
				return 1;
			}
			printf("Bytes sent: %d\n", iSendResult);
			memset(recvbuf, 0, sizeof recvbuf);
		}
		else if (iResult == 0)
			printf("Connection closing...\n");
		else {
			printf("recv failed with error: %d\n", WSAGetLastError());
			closesocket(AcceptSocket);
			WSACleanup();
			
			
		}
	} while (iResult > -1);



}