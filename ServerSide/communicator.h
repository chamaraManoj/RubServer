#ifndef COMMUNICATOR_H
#define COMMUNICATOR_H

/*Packet structure defined for the transmission of one tile

[[Layer 1 size][Layer 2 size][layer 3 size][layer 4 size][Layer 1 Buffer ][Layer 2 Buffer ][Layer 3 Buffer ][Layer 4 Buffer ]]

[[ 2 uint8_t  ][ 2 uint8_t  ][ 2 uint8_t  ][ 2 uint8_t  ][1024*10 uint8_t][1024*10 uint8_t][1024*10 uint8_t][1024*10 uint8_t]]

each tile contains above data to be sent to the client over Wifi*/


#include "commondata.h"


// Need to link with Ws2_32.lib
#pragma comment (lib, "Ws2_32.lib")
#pragma warning(disable:4996) 
// #pragma comment (lib, "Mswsock.lib")

#define DEFAULT_PORT_0 5550
#define DEFAULT_PORT_1 5551
#define DEFAULT_PORT_2 5552
#define DEFAULT_PORT_3 5553
#define DEFAULT_PORT_4 5554

//#define IP_ADDRESS "10.130.1.229"
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

	char recvbuf[DEFAULT_BUFLEN_RECEIVE];
	int recvbuflen = DEFAULT_BUFLEN_RECEIVE;

	WSADATA wsaData;

	// The socket address to be passed to bind
	sockaddr_in service[NUM_OF_TOTAL_PORTS];

	/*Buffer to store the data to be sent. ideally this is a 
	~7 frames x 4 layers x 4 tiles which has extra 2 capacity to store
	extra 2 tiles data*/
	tileBufferByte* tileBufferByte1s;


public:
	Communicator(tileBufferByte* tileBufferByte1s);
	SOCKET* intializeServer();
	SOCKET*  readFrameRequest(char (&receiveBuffer)[DEFAULT_BUFLEN_RECEIVE]);
	int sendData(SOCKET socket, char* recvbuf, int resul);
	int getReceivedBytes();

	// the listening socket to be created
	SOCKET ListenSocket[NUM_OF_TOTAL_PORTS];
	SOCKET AcceptSocket[NUM_OF_TOTAL_PORTS];
};


#endif // !COMMUNICATOR_H

