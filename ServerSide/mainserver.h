#pragma once
/*This class act as the main server which controls
	-- Maintaining connection with the client
	-- Retreive the chunk number, tile numbers and quality
	-- Read the tiles from the secondary storage to the database
	-- Given the tile numbers, merge them together
	-- Send the merged tiles to the client*/
#ifndef MAINSERVER_H
#define MAINSERVER_H

#include "commondata.h"
#include "filereader.h"
#include "tilemerger.h"
#include "communicator.h"

class MainServer {
private:
	
public:
	MainServer();
	int i;
	tileBufferByte tileBufferByte1s;
	tileBufferByteSend tileBufferByteSend1s;
	videoDataBase videoDataBases[3];
	string filePaths[QUALITY_LEVEL];
	char chunkData[DEFAULT_BUFLEN_RECEIVE];

	dataPacketSendLayer packetsSendLayer[4]; //NUM_OF_TILES is the same value
	//dataPacketSendELayer packetsSendELayer[3];


};

#endif // !MAINSERVER_H
