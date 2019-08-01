
#include "mainserver.h"

MainServer::MainServer() {}


unsigned int __stdcall myThreadSendData(void* dataPacket)
{
	int iSendResult;
	int packetSize;

	dataPacketSend &myDataPacket = *((dataPacketSend*)dataPacket);
	packetSize = myDataPacket.dataPacketSize;


	iSendResult = send(myDataPacket.socket, (const char*)myDataPacket.buffer, packetSize, 0);

	//cout << myDataPacket.layerSizes[0] << " "<<  myDataPacket.layerSizes[1] << " " << myDataPacket.layerSizes[2] << " " << myDataPacket.layerSizes[3] << endl;
	cout << "\n" << iSendResult << endl;

	return 0;

}

/*int Communicator::sendData(SOCKET socket, char* recvbuf, int result) {
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
	*/
int main() {

	int retRequestFrmClient = 0;
	int chunk;
	int quality;
	int x1, x2, x3, x4;
	int y1, y2, y3, y4;

	int tempCount1;
	int tempCount2;

	SOCKET* AcceptSocket;
	SOCKET* ListenSocket;
	MainServer* mainServer = new MainServer();



	mainServer->i = 100;
	mainServer->filePaths[0] = "E:\\SelfLearning\\Rubiks\\RubiksVideos\\processedVideo_SD";
	//mainServer->filePaths[0] = "H:\\My_Codes\\Rubiks_implementation\\RubiksVideos\\processedVideo_SD";
	//mainServer.filePaths[1] = "H:\\My_Codes\\Rubiks_implementation\\RubiksVideos\\processedVideo_HD";
	//mainServer.filePaths[2] = "H:\\My_Codes\\Rubiks_implementation\\RubiksVideos\\processedVideo_4K";

	FileReader* fileReaderSD = new FileReader(mainServer->filePaths[0], 720, &mainServer->videoDataBases[0], &mainServer->i);
	//FileReader* fileReaderHD = new FileReader(mainServer.filePaths[1], 1080, mainServer.videoDataBases[1]);
	//FileReader* fileReader4K = new FileReader(mainServer.filePaths[2], 2160, mainServer.videoDataBases[2]);


	if (fileReaderSD->createDataBase() == 0)
		av_log(NULL, AV_LOG_INFO, "Read fileReader1 sucessful");
	else
		av_log(NULL, AV_LOG_INFO, "Read fileReader1 unsucessful");

	/*if (fileReaderHD->createDataBase(fileReaderHD->videoTemp) == 0)
		av_log(NULL, AV_LOG_INFO, "Read fileReader2 sucessful");
	else
		av_log(NULL, AV_LOG_INFO, "Read fileReader2 unsucessful");

	/*if (fileReader3.createDataBase(fileReader3.videoNum1) == 0)
		av_log(NULL, AV_LOG_INFO, "Read fileReader3 sucessful");
	else
		av_log(NULL, AV_LOG_INFO, "Read fileReader3 unsucessful");*/

		/***
		This code section should have the HTTP server communication module. Server module returns the current tile positions, chunk number and
		quality to be streamed from the server. These values are directly passed to the tileMerger object which in turn returns the selected
		tiles from SD, HD or 4K database.
		***/
	TileMerger* tilemerger = new TileMerger(&mainServer->tileBufferByte1s, mainServer->videoDataBases);

	/*After reading from tilemerger funtion, data is tranfered to the
	Communicaotr modle to be sent to the client*/
	Communicator* serverSideCommunicator = new Communicator(&mainServer->tileBufferByte1s);

	ListenSocket = serverSideCommunicator->intializeServer();

	//tilemerger->setTiles(0, 0, 0, 1, 1, 0, 1, 1, 0, 0);
	//tilemerger->mergeTiles();

	assert(ListenSocket != NULL);


	do {
		AcceptSocket = serverSideCommunicator->readFrameRequest(mainServer->chunkData);
		assert(AcceptSocket != NULL);

		retRequestFrmClient = serverSideCommunicator->getReceivedBytes();
		if (retRequestFrmClient > 0) {

			x1 = tilemerger->tileXCor[mainServer->chunkData[TILE_NO_1_IND]];
			y1 = tilemerger->tileYCor[mainServer->chunkData[TILE_NO_1_IND]];
			x2 = tilemerger->tileXCor[mainServer->chunkData[TILE_NO_2_IND]];
			y2 = tilemerger->tileYCor[mainServer->chunkData[TILE_NO_2_IND]];
			x3 = tilemerger->tileXCor[mainServer->chunkData[TILE_NO_3_IND]];
			y3 = tilemerger->tileYCor[mainServer->chunkData[TILE_NO_3_IND]];
			x4 = tilemerger->tileXCor[mainServer->chunkData[TILE_NO_4_IND]];
			y4 = tilemerger->tileYCor[mainServer->chunkData[TILE_NO_4_IND]];

			chunk = (mainServer->chunkData[CHUNK_FRAME_IND_1] << 24) |
				(mainServer->chunkData[CHUNK_FRAME_IND_2] << 16) |
				(mainServer->chunkData[CHUNK_FRAME_IND_3] << 8) |
				(mainServer->chunkData[CHUNK_FRAME_IND_4]);
			quality = mainServer->chunkData[CHUNK_QUAL_IND];


			/*Merging the tiles to a byte buffer where all the data can be sent parallely later*/
			tilemerger->setTiles(x1, y1, x2, y2, x3, y3, x4, y4, quality, chunk);
			tilemerger->mergeTiles();

			/*creating array of dataPacketSend to be sent to the client*/
			for (tempCount1 = 0; tempCount1 < NUM_OF_SEND_THREADS; tempCount1++) {
				mainServer->packetsSend[tempCount1].socket = AcceptSocket[tempCount1 + 1];

				uint16_t size1 = mainServer->tileBufferByte1s.tileBufferSize[tempCount1].sublayer1Size;
				uint16_t size2 = mainServer->tileBufferByte1s.tileBufferSize[tempCount1].sublayer2Size;
				uint16_t size3 = mainServer->tileBufferByte1s.tileBufferSize[tempCount1].sublayer3Size;
				uint16_t size4 = mainServer->tileBufferByte1s.tileBufferSize[tempCount1].sublayer4Size;

				mainServer->packetsSend[tempCount1].buffer[0] = (size1 & 0x0000ff00) >> 8;
				mainServer->packetsSend[tempCount1].buffer[1] = (size1 & 0x000000ff);
				mainServer->packetsSend[tempCount1].buffer[2] = (size2 & 0x0000ff00) >> 8;
				mainServer->packetsSend[tempCount1].buffer[3] = (size2 & 0x000000ff);
				mainServer->packetsSend[tempCount1].buffer[4] = (size3 & 0x0000ff00) >> 8;
				mainServer->packetsSend[tempCount1].buffer[5] = (size3 & 0x000000ff);
				mainServer->packetsSend[tempCount1].buffer[6] = (size4 & 0x0000ff00) >> 8;
				mainServer->packetsSend[tempCount1].buffer[7] = (size4 & 0x000000ff);

				/*fill the bytes to the buffers*/
				copy(begin(mainServer->tileBufferByte1s.tileBuffer[tempCount1].subLayer1), &(mainServer->tileBufferByte1s.tileBuffer[tempCount1].subLayer1[size1 - 1]),
					mainServer->packetsSend[tempCount1].buffer + BYTES_FOR_PACKET_LENGTH);

				copy(begin(mainServer->tileBufferByte1s.tileBuffer[tempCount1].subLayer2), &(mainServer->tileBufferByte1s.tileBuffer[tempCount1].subLayer2[size2 - 1]),
					mainServer->packetsSend[tempCount1].buffer + BYTES_FOR_PACKET_LENGTH + size1);

				copy(begin(mainServer->tileBufferByte1s.tileBuffer[tempCount1].subLayer3), &(mainServer->tileBufferByte1s.tileBuffer[tempCount1].subLayer3[size3 - 1]),
					mainServer->packetsSend[tempCount1].buffer + BYTES_FOR_PACKET_LENGTH + size1 + size2);

				copy(begin(mainServer->tileBufferByte1s.tileBuffer[tempCount1].subLayer4), &(mainServer->tileBufferByte1s.tileBuffer[tempCount1].subLayer4[size4 - 1]),
					mainServer->packetsSend[tempCount1].buffer + BYTES_FOR_PACKET_LENGTH + size1 + size2 + size3);

				mainServer->packetsSend[tempCount1].layerSizes[0] = size1;
				mainServer->packetsSend[tempCount1].layerSizes[1] = size2;
				mainServer->packetsSend[tempCount1].layerSizes[2] = size3;
				mainServer->packetsSend[tempCount1].layerSizes[3] = size4;

				mainServer->packetsSend[tempCount1].dataPacketSize = size1 + size2 + size3 + size4 + BYTES_FOR_PACKET_LENGTH;
			
			}


			/*Send tilebuffer seperately to the client using multiple threads*/
			HANDLE myHandle[4];

			/*Creating multiple threads*/
			for (tempCount1 = 0; tempCount1 < NUM_OF_SEND_THREADS; tempCount1++) {
				myHandle[tempCount1] = (HANDLE)_beginthreadex(0, 0, &myThreadSendData, &mainServer->packetsSend[tempCount1], 0, 0);
			}

			/*Wait for objects to be completed*/
			for (tempCount1 = 0; tempCount1 < NUM_OF_SEND_THREADS; tempCount1++) {
				WaitForSingleObject(myHandle[tempCount1], INFINITE);
			}

			/*Release the thread and resources used*/
			for (tempCount1 = 0; tempCount1 < NUM_OF_SEND_THREADS; tempCount1++) {
				CloseHandle(myHandle[tempCount1]);
			}

			//cout << "Tile Merged" << endl;

			
		}
		memset(mainServer->chunkData, 0, sizeof mainServer->chunkData);
		memset(&(mainServer->tileBufferByte1s), 0, sizeof mainServer->tileBufferByte1s);
		memset(&(mainServer->packetsSend), 0, sizeof mainServer->packetsSend);

	} while (retRequestFrmClient > 0);


	//closesocket(serverSideCommunicator->ListenSocket);


	return 0;
}