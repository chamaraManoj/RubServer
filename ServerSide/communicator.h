#ifndef COMMUNICATOR_H
#define COMMUNICATOR_H




#include "commondata.h"


// Need to link with Ws2_32.lib
#pragma comment (lib, "Ws2_32.lib")
#pragma warning(disable:4996) 
// #pragma comment (lib, "Mswsock.lib")

#define DEFAULT_PORT 5555
//#define DEFAULT_PORT 5552
#define IP_ADDRESS "10.130.1.229"

class Communicator {
private:
	/*as the function output parameters*/
	int iSendResult;
	int iResult;

	//flags for getting the sucess of each state
	bool bindSucess;
	bool listenSucess;
	bool acceptSucess;

	char recvbuf[DEFAULT_BUFLEN_RECEIVE];
	int recvbuflen = DEFAULT_BUFLEN_RECEIVE;

	WSADATA wsaData;

	// The socket address to be passed to bind
	sockaddr_in service;

	/*Buffer to store the data to be sent. ideally this is a 
	~7 frames x 4 layers x 4 tiles which has extra 2 capacity to store
	extra 2 tiles data*/
	tileBufferByte* tileBufferByte1s;


public:
	Communicator(tileBufferByte* tileBufferByte1s);
	bool intializeServer();
	int readFrameRequest(char (&receiveBuffer)[DEFAULT_BUFLEN_RECEIVE]);
	int sendData();

	// the listening socket to be created
	SOCKET ListenSocket;
	SOCKET AcceptSocket;
};


#endif // !COMMUNICATOR_H

