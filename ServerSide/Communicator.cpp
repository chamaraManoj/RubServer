/*This function is the communicator module which receive 
the client request and pass the merged tiles to the 
user.*/

#include "communicator.h"

Communicator::Communicator(tileBufferByte* tileBufferByte1s){
	this->tileBufferByte1s = tileBufferByte1s;
}

bool Communicator::intializeServer(){
	int res = 0;

	//initialize the varaibles

	this->bindSucess = false;
	this->ListenSocket = false;
	this->acceptSucess = false;

	this->iSendResult = 0;
	this->iResult = 0;

	this->ListenSocket = INVALID_SOCKET;

	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != NO_ERROR) {
		wprintf(L"Error at WSAStartup()\n");
		res = -1;
	}

	//listrening for the socket
	assert(res == 0);

	this->ListenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (this->ListenSocket == INVALID_SOCKET) {
		wprintf(L"socket function failed with error: %u\n", WSAGetLastError());
		WSACleanup();
		res = -1;
	}

	assert(res == 0);
	//initialize the address and the port
	service.sin_family = AF_INET;
	service.sin_addr.s_addr = inet_addr(IP_ADDRESS);
	service.sin_port = htons(DEFAULT_PORT);

	iResult = bind(this->ListenSocket, (SOCKADDR*)& service, sizeof(service));
	if (iResult == SOCKET_ERROR) {
		wprintf(L"bind failed with error %u\n", WSAGetLastError());
		closesocket(this->ListenSocket);
		WSACleanup();
		res = -1;
	}
	else {
		wprintf(L"bind returned success\n");
		bindSucess = true;
	}

	assert(bindSucess == true);
	if (listen(this->ListenSocket, SOMAXCONN) == SOCKET_ERROR) {
		wprintf(L"listen failed with error: %ld\n", WSAGetLastError());
		closesocket(this->ListenSocket);
		WSACleanup();
	}
	else {
		wprintf(L"Start listening to the client");
		listenSucess = true;
	}


	assert(listenSucess == true);

	wprintf(L"Waiting for client to connect...\n");
	
	return listenSucess;
}

int  Communicator::readFrameRequest(char (&receiveBuffer)[DEFAULT_BUFLEN_RECEIVE]){
	
		wprintf(L"Waiting for client to connect...\n");
		this->AcceptSocket = accept(this->ListenSocket, NULL, NULL);
		if (this->AcceptSocket == INVALID_SOCKET) {
			wprintf(L"accept failed with error: %ld\n", WSAGetLastError());
			closesocket(this->ListenSocket);
			WSACleanup();
		}
		
		else {
			acceptSucess = true;
			wprintf(L"Client connected.\n");
		}

		iResult = recv(this->AcceptSocket, receiveBuffer, recvbuflen, 0);
		if (iResult > 0) {
			printf("Bytes received: %d\n", iResult);
			for (int i = 0; i < iResult; i++) {
				printf("%d\n", receiveBuffer[i]);
				recvbuf[i] *= 2;
			}
			//sendData();
			//memset(recvbuf, 0, sizeof recvbuf);
		}
		else if (iResult == 0)
			printf("Connection closing...\n");
		else {
			printf("recv failed with error: %d\n", WSAGetLastError());
			closesocket(this->AcceptSocket);
			WSACleanup();
		}

	return iResult;
}

int Communicator::sendData() {
	iSendResult = send(this->AcceptSocket, this->recvbuf, this->iResult, 0);
	if (this->iSendResult == SOCKET_ERROR) {
		printf("send failed with error: %d\n", WSAGetLastError());
		closesocket(this->AcceptSocket);
		WSACleanup();
		//return 1;
	}
	else {
		printf("Bytes sent: %d\n", iSendResult);
	}

	return 0;
}
/*	assert(bindSucess == true);
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

	//SOCKET AcceptSocket;
	wprintf(L"Waiting for client to connect...\n");

	do {
		wprintf(L"Waiting for client to connect...\n");
		AcceptSocket = accept(ListenSocket, NULL, NULL);
		if (AcceptSocket == INVALID_SOCKET) {
			wprintf(L"accept failed with error: %ld\n", WSAGetLastError());
			closesocket(ListenSocket);
			WSACleanup();
		}

		else {
			acceptSucess = true;
			wprintf(L"Client connected.\n");
		}

		iResult = recv(AcceptSocket, recvbuf, recvbuflen, 0);
		if (iResult > 0) {
			printf("Bytes received: %d\n", iResult);
			for (int i = 0; i < iResult; i++) {
				printf("%d\n", recvbuf[i]);
				recvbuf[i] *= 2;
			}
			//sendData();
			iSendResult = send(this->AcceptSocket, this->recvbuf, this->iResult, 0);
			if (this->iSendResult == SOCKET_ERROR) {
				printf("send failed with error: %d\n", WSAGetLastError());
				closesocket(this->AcceptSocket);
				WSACleanup();
				//return 1;
			}
			else {
				printf("Bytes sent: %d\n", iSendResult);
			}
			
			memset(recvbuf, 0, sizeof recvbuf);
		}
		else if (iResult == 0)
			printf("Connection closing...\n");
		else {
			printf("recv failed with error: %d\n", WSAGetLastError());
			closesocket(AcceptSocket);
			WSACleanup();
		}
	} while (iResult > 0);
	closesocket(ListenSocket);
*/