/*This function is the communicator module which receive the client request and pass the merged tiles to the 
user.*/

/*The main functionality of this Communicator module is to listen from the port 1, to collect the requests from the client and sends
to the client using 4 sockets, including the listening sockets*/

#include "communicator.h"

Communicator::Communicator(tileBufferByte* tileBufferByte1s){
	this->tileBufferByte1s = tileBufferByte1s;
}

SOCKET* Communicator::intializeServer(){
	int res = 0;
	int tempCount1;

	//initialize the varaibles

	this->bindSucess = false;
	this->listenSucess = false;
	this->acceptSucess = false;

	this->iSendResult = 0;
	this->iResult = 0;

	for(tempCount1 =0;tempCount1< NUM_OF_TOTAL_PORTS;tempCount1++)
		this->ListenSocket[tempCount1] = INVALID_SOCKET;

	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != NO_ERROR) {
		wprintf(L"Error at WSAStartup()\n");
		res = -1;
	}

	/*Go to the next step if everything is OK*/
	assert(res == 0);

	/*listrening for the socket*/

	for (tempCount1 = 0; tempCount1 < NUM_OF_TOTAL_PORTS; tempCount1++) {
		this->ListenSocket[tempCount1] = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if (this->ListenSocket[tempCount1] == INVALID_SOCKET) {
			wprintf(L"socket function failed with error: %u\n", WSAGetLastError());
			WSACleanup();
			res = -1;
		}
	}

	/*Go to the next step if every thing is OK*/
	assert(res == 0);

	//initialize the address and the port separately
	for (tempCount1 = 0; tempCount1 < NUM_OF_TOTAL_PORTS; tempCount1++) {
		service[tempCount1].sin_family = AF_INET;
		service[tempCount1].sin_addr.s_addr = inet_addr(IP_ADDRESS);
		
		switch (tempCount1) {
		case SOCKET_0:
			service[tempCount1].sin_port = htons(DEFAULT_PORT_0);
			break;
		case SOCKET_1:
			service[tempCount1].sin_port = htons(DEFAULT_PORT_1);
			break;
		case SOCKET_2:
			service[tempCount1].sin_port = htons(DEFAULT_PORT_2);
			break;
		case SOCKET_3:
			service[tempCount1].sin_port = htons(DEFAULT_PORT_3);
			break;
		case SOCKET_4:
			service[tempCount1].sin_port = htons(DEFAULT_PORT_4);
			break;
		}
	}
	//service.sin_port = htons(DEFAULT_PORT_1);

	/*Bind the sockets*/
	for (tempCount1 = 0; tempCount1 < NUM_OF_TOTAL_PORTS; tempCount1++) {
		iResult = bind(this->ListenSocket[tempCount1], (SOCKADDR*)& service[tempCount1], sizeof(service[tempCount1]));
		if (iResult == SOCKET_ERROR) {
			wprintf(L"bind failed with error %u\n", WSAGetLastError());
			closesocket(this->ListenSocket[tempCount1]);
			WSACleanup();
			res = -1;
		}
		else {
			wprintf(L"bind returned success\n");
			bindSucess = true;
		}
	}
	
	/*Go forward if the bind suceess is true*/
	assert(bindSucess == true);
	
	/*Start listening for from the sockets*/
	for (tempCount1 = 0; tempCount1 < NUM_OF_TOTAL_PORTS; tempCount1++) {

		if (listen(this->ListenSocket[tempCount1], SOMAXCONN) == SOCKET_ERROR) {
			wprintf(L"listen failed with error: %ld\n", WSAGetLastError());
			closesocket(this->ListenSocket[tempCount1]);
			WSACleanup();
		}
		else {
			wprintf(L"Start listening to the client");
			listenSucess = true;
		}
	}
	
	/*GO forward if all the posrts are ready to listen*/
	assert(listenSucess == true);

	wprintf(L"Waiting for client to connect...\n");
	return ListenSocket;

}

SOCKET* Communicator::readFrameRequest(char (&receiveBuffer)[DEFAULT_BUFLEN_RECEIVE]){
	
	int tempCount1;

	
	wprintf(L"Waiting for client to connect...\n");

	for (tempCount1 = 0; tempCount1 < NUM_OF_TOTAL_PORTS; tempCount1++) {
		this->AcceptSocket[tempCount1] = accept(this->ListenSocket[tempCount1], NULL, NULL);
		if (this->AcceptSocket[tempCount1] == INVALID_SOCKET) {
			wprintf(L"accept failed with error: %ld\n", WSAGetLastError());
			closesocket(this->ListenSocket[tempCount1]);
			WSACleanup();
		}
		else {
			acceptSucess = true;
			wprintf(L"Client connected : %d \n", tempCount1);
		}
	}
	
	iResult = recv(this->AcceptSocket[0], receiveBuffer, recvbuflen, 0);
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
		closesocket(this->AcceptSocket[0]);
		WSACleanup();
	}

	return AcceptSocket;
}

int Communicator::getReceivedBytes() {
	return this->iResult;
}

/*int Communicator::sendData(SOCKET socket, char* recvbuf,int result) {
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
*/





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