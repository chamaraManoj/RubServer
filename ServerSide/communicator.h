#ifndef COMMUNICATOR_H
#define COMMUNICATOR_H


#define WIN32_LEAN_AND_MEAN

#include "commondata.h"
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>

// Need to link with Ws2_32.lib
#pragma comment (lib, "Ws2_32.lib")
#pragma warning(disable:4996) 
// #pragma comment (lib, "Mswsock.lib")

#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT 5555
#define IP_ADDRESS "10.1.1.35"

class Communicator {
private:
	/*as the function output parameters*/
	int iSendResult;
	int iResult;

	//flags for getting the sucess of each state
	bool bindSucess;
	bool listenSucess;
	bool acceptSucess;

	char recvbuf[DEFAULT_BUFLEN];
	int recvbuflen = DEFAULT_BUFLEN;

	WSADATA wsaData;

	// the listening socket to be created
	SOCKET ListenSocket;

	// The socket address to be passed to bind
	sockaddr_in service;



public:
	Communicator();
	bool intializeServer();
	void readFrameRequest();


};


#endif // !COMMUNICATOR_H

