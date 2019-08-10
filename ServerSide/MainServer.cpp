
#include "mainserver.h"

MainServer::MainServer() {}


unsigned int __stdcall myThreadSendData(void* dataPacket)
{
	int iSendResult;
	int packetSize;

	dataPacketSendLayer&myDataPacket = *((dataPacketSendLayer*)dataPacket);
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

	int EFFECTIVE_NUM_OF_SEND_THREADS;

	/*Parameter which enables the parallel streaming*/
	/*==========important===========================*/
	mainServer->isParallelStreaming = false;

	if (mainServer->isParallelStreaming) {
		EFFECTIVE_NUM_OF_SEND_THREADS = 4;
	}
	else {
		EFFECTIVE_NUM_OF_SEND_THREADS = 1;
	}

	mainServer->i = 100;
	//mainServer->filePaths[0] = "E:\\SelfLearning\\Rubiks\\RubiksVideos\\processedVideo_SD";
	mainServer->filePaths[0] = "H:\\My_Codes\\Rubiks_implementation\\RubiksVideos\\processedVideo_SD";
	//mainServer.filePaths[1] = "H:\\My_Codes\\Rubiks_implementation\\RubiksVideos\\processedVideo_HD";
	//mainServer.filePaths[2] = "H:\\My_Codes\\Rubiks_implementation\\RubiksVideos\\processedVideo_4K";

	/*Enables the different datasets with different qualities*/
	FileReader* fileReaderSD = new FileReader(mainServer->filePaths[0], 720, &mainServer->videoDataBases[0], &mainServer->i);
	//FileReader* fileReaderHD = new FileReader(mainServer.filePaths[1], 1080, mainServer.videoDataBases[1]);
	//FileReader* fileReader4K = new FileReader(mainServer.filePaths[2], 2160, mainServer.videoDataBases[2]);

	/*Reading data to the RAM*/
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
	TileMerger* tilemerger = new TileMerger(&mainServer->tileBufferByte1s, mainServer->videoDataBases,&mainServer->tileBufferByteSend1s);

	/*After reading from tilemerger funtion, data is tranfered to the
	Communicaotr modle to be sent to the client*/
	Communicator* serverSideCommunicator = new Communicator(&mainServer->tileBufferByte1s);

	ListenSocket = serverSideCommunicator->intializeServer();

	//tilemerger->setTiles(0, 0, 0, 1, 1, 0, 1, 1, 0, 0);
	//tilemerger->mergeTiles();

	assert(ListenSocket != NULL);


	do {
		/*Accept all the socekts and chunk data from the client side*/
		AcceptSocket = serverSideCommunicator->readFrameRequest(mainServer->chunkData);
		assert(AcceptSocket != NULL);

		/*Resolve all the chunk request data for further processes in the server*/
		retRequestFrmClient = serverSideCommunicator->getReceivedBytes();
		if (retRequestFrmClient > 0) {

			/*Coordinates of the tiles*/
			x1 = tilemerger->tileXCor[mainServer->chunkData[TILE_NO_1_IND]];
			y1 = tilemerger->tileYCor[mainServer->chunkData[TILE_NO_1_IND]];
			x2 = tilemerger->tileXCor[mainServer->chunkData[TILE_NO_2_IND]];
			y2 = tilemerger->tileYCor[mainServer->chunkData[TILE_NO_2_IND]];
			x3 = tilemerger->tileXCor[mainServer->chunkData[TILE_NO_3_IND]];
			y3 = tilemerger->tileYCor[mainServer->chunkData[TILE_NO_3_IND]];
			x4 = tilemerger->tileXCor[mainServer->chunkData[TILE_NO_4_IND]];
			y4 = tilemerger->tileYCor[mainServer->chunkData[TILE_NO_4_IND]];

			/*Chunk number of the tiles*/
			chunk = (mainServer->chunkData[CHUNK_FRAME_IND_1] << 24) |
				(mainServer->chunkData[CHUNK_FRAME_IND_2] << 16) |
				(mainServer->chunkData[CHUNK_FRAME_IND_3] << 8) |
				(mainServer->chunkData[CHUNK_FRAME_IND_4]);
			quality = mainServer->chunkData[CHUNK_QUAL_IND];


			/*Merging the tiles to a byte buffer where all the data can be sent parallely later*/
			tilemerger->setTiles(x1, y1, x2, y2, x3, y3, x4, y4, quality, chunk);
			tilemerger->mergeTiles();

			/*creating array to store the sizes of each layers*/
			int totLayerSize[NUM_OF_LAYERS];
			for (tempCount1 = 0; tempCount1 < NUM_OF_LAYERS; tempCount1++)
				totLayerSize[tempCount1] = 0;


			/*Get the total number of bytes send in each layer*/
			for (tempCount1 = 0; tempCount1 < NUM_OF_TOT_TILES; tempCount1++) {
				if (tempCount1 < NUM_OF_TILES_BASE_LAYER)
					totLayerSize[0] += mainServer->tileBufferByteSend1s.layerSize[tempCount1];
				else if (tempCount1 >= NUM_OF_TILES_BASE_LAYER && tempCount1 < NUM_OF_TILES_BASE_LAYER + NUM_OF_FOV_TILES)
					totLayerSize[1] += mainServer->tileBufferByteSend1s.layerSize[tempCount1];
				else if (tempCount1 >= NUM_OF_TILES_BASE_LAYER + NUM_OF_FOV_TILES && tempCount1 < NUM_OF_TILES_BASE_LAYER + NUM_OF_FOV_TILES * 2)
					totLayerSize[2] += mainServer->tileBufferByteSend1s.layerSize[tempCount1];
				else if (tempCount1 >= NUM_OF_TILES_BASE_LAYER + NUM_OF_FOV_TILES * 2 && tempCount1 < NUM_OF_TILES_BASE_LAYER + NUM_OF_FOV_TILES * 3)
					totLayerSize[3] += mainServer->tileBufferByteSend1s.layerSize[tempCount1];
			}


			/*If the parallerl streaming is enabeled*/
			if (mainServer->isParallelStreaming) {
				
				for (tempCount1 = 0; tempCount1 < EFFECTIVE_NUM_OF_SEND_THREADS; tempCount1++) {

					/*Fill the baseLayer value*/
					if (tempCount1 == 0) {
						/*Assign the socket*/
						int cumlativeByteSum = 0;
						mainServer->packetsSendLayerParallel[tempCount1].socket = AcceptSocket[tempCount1 + 1];

						/*Include the chunk number at the begning*/
						mainServer->packetsSendLayerParallel[tempCount1].buffer[cumlativeByteSum++] = (chunk & 0xff000000) >> 24;
						mainServer->packetsSendLayerParallel[tempCount1].buffer[cumlativeByteSum++] = (chunk & 0x00ff0000) >> 16;
						mainServer->packetsSendLayerParallel[tempCount1].buffer[cumlativeByteSum++] = (chunk & 0x0000ff00) >> 8;
						mainServer->packetsSendLayerParallel[tempCount1].buffer[cumlativeByteSum++] = (chunk & 0x000000ff);

						/*Fill lenghts of each tiles first*/
						for (tempCount2 = 0; tempCount2 < NUM_OF_TILES_BASE_LAYER; tempCount2++) {
							mainServer->packetsSendLayerParallel[tempCount1].buffer[cumlativeByteSum++] = ((mainServer->tileBufferByteSend1s.layerSize[tempCount2]) & 0x0000ff00) >> 8;
							mainServer->packetsSendLayerParallel[tempCount1].buffer[cumlativeByteSum++] = ((mainServer->tileBufferByteSend1s.layerSize[tempCount2]) & 0x000000ff);
						}

						/*Fill the pixel data of the base layer*/
						int tempSize;
						for (tempCount2 = 0; tempCount2 < NUM_OF_TILES_BASE_LAYER; tempCount2++) {
							tempSize = mainServer->tileBufferByteSend1s.layerSize[tempCount2];
							copy(begin(mainServer->tileBufferByteSend1s.layerBufferArray[tempCount2].layerBuffer),
								&(mainServer->tileBufferByteSend1s.layerBufferArray[tempCount2].layerBuffer[tempSize]),
								mainServer->packetsSendLayerParallel[tempCount1].buffer + cumlativeByteSum);
							cumlativeByteSum += tempSize;
						}
						/*Fill the total length of the baselayer*/
						mainServer->packetsSendLayerParallel[tempCount1].dataPacketSize = totLayerSize[0] + 2 * NUM_OF_TILES_BASE_LAYER;
					}
					/*Fill the enhance layer sizes*/
					else {
						int cumlativeByteSum = 0;
						/*Assign the socket*/
						mainServer->packetsSendLayerParallel[tempCount1].socket = AcceptSocket[tempCount1 + 1];

						/*Include the chunk number at the begning*/
						mainServer->packetsSendLayerParallel[tempCount1].buffer[cumlativeByteSum++] = (chunk & 0xff000000) >> 24;
						mainServer->packetsSendLayerParallel[tempCount1].buffer[cumlativeByteSum++] = (chunk & 0x00ff0000) >> 16;
						mainServer->packetsSendLayerParallel[tempCount1].buffer[cumlativeByteSum++] = (chunk & 0x0000ff00) >> 8;
						mainServer->packetsSendLayerParallel[tempCount1].buffer[cumlativeByteSum++] = (chunk & 0x000000ff);

						/*fill the layer sizes*/
						for (tempCount2 = 0; tempCount2 < NUM_OF_FOV_TILES; tempCount2++) {
							mainServer->packetsSendLayerParallel[tempCount1].buffer[cumlativeByteSum++] = ((mainServer->tileBufferByteSend1s.layerSize[NUM_OF_TILES_BASE_LAYER + (tempCount1 - 1) * NUM_OF_FOV_TILES + tempCount2]) & 0x0000ff00) >> 8;
							mainServer->packetsSendLayerParallel[tempCount1].buffer[cumlativeByteSum++] = ((mainServer->tileBufferByteSend1s.layerSize[NUM_OF_TILES_BASE_LAYER + (tempCount1 - 1) * NUM_OF_FOV_TILES + tempCount2]) & 0x000000ff);
						}

						/*fill the pixel bytes in the layers*/
						int tempSize;
						for (tempCount2 = 0; tempCount2 < NUM_OF_FOV_TILES; tempCount2++) {
							tempSize = mainServer->tileBufferByteSend1s.layerSize[NUM_OF_TILES_BASE_LAYER + (tempCount1 - 1) * NUM_OF_FOV_TILES + tempCount2];
							copy(begin(mainServer->tileBufferByteSend1s.layerBufferArray[NUM_OF_TILES_BASE_LAYER + (tempCount1 - 1) * NUM_OF_LAYERS + tempCount2].layerBuffer),
								&(mainServer->tileBufferByteSend1s.layerBufferArray[NUM_OF_TILES_BASE_LAYER + (tempCount1 - 1) * NUM_OF_LAYERS + tempCount2].layerBuffer[tempSize]),
								mainServer->packetsSendLayerParallel[tempCount1].buffer + cumlativeByteSum);
							cumlativeByteSum += tempSize;
						}
						mainServer->packetsSendLayerParallel[tempCount1].dataPacketSize = totLayerSize[tempCount1] + 2 * NUM_OF_FOV_TILES;
					}


					/*fill the bytes to the buffers*/
					/*copy(begin(mainServer->tileBufferByte1s.tileBuffer[tempCount1].subLayer1), &(mainServer->tileBufferByte1s.tileBuffer[tempCount1].subLayer1[size1 - 1]),
						mainServer->packetsSend[tempCount1].buffer + BYTES_FOR_PACKET_LENGTH);

					copy(begin(mainServer->tileBufferByte1s.tileBuffer[tempCount1].subLayer2), &(mainServer->tileBufferByte1s.tileBuffer[tempCount1].subLayer2[size2 - 1]),
						mainServer->packetsSend[tempCount1].buffer + BYTES_FOR_PACKET_LENGTH + size1);

					copy(begin(mainServer->tileBufferByte1s.tileBuffer[tempCount1].subLayer3), &(mainServer->tileBufferByte1s.tileBuffer[tempCount1].subLayer3[size3 - 1]),
						mainServer->packetsSend[tempCount1].buffer + BYTES_FOR_PACKET_LENGTH + size1 + size2);

					copy(begin(mainServer->tileBufferByte1s.tileBuffer[tempCount1].subLayer4), &(mainServer->tileBufferByte1s.tileBuffer[tempCount1].subLayer4[size4 - 1]),
						mainServer->packetsSend[tempCount1].buffer + BYTES_FOR_PACKET_LENGTH + size1 + size2 + size3);*/

				}
				/**/

			}
			else {	
				/*Assign the socket*/
				int cumlativeByteSum = 0;

				/*Accepting the socket number 5551 as the data send thread*/
				mainServer->packetsSendLayer[0].socket = AcceptSocket[1];

				/*Initial size of the sending buffer in bytes*/
				mainServer->packetsSendLayer[0].dataPacketSize = 0;

				/*Include the chunk number at the begning*/
				mainServer->packetsSendLayer[0].buffer[cumlativeByteSum++] = (chunk & 0xff000000) >> 24;
				mainServer->packetsSendLayer[0].buffer[cumlativeByteSum++] = (chunk & 0x00ff0000) >> 16;
				mainServer->packetsSendLayer[0].buffer[cumlativeByteSum++] = (chunk & 0x0000ff00) >> 8;
				mainServer->packetsSendLayer[0].buffer[cumlativeByteSum++] = (chunk & 0x000000ff);
				cout << "chunk " << chunk << endl;
				mainServer->packetsSendLayer[0].dataPacketSize += 4;

				/*Filling the base layer tile lengths*/
				for (tempCount2 = 0; tempCount2 < NUM_OF_TILES_BASE_LAYER; tempCount2++) {
					mainServer->packetsSendLayer[0].buffer[cumlativeByteSum++] = ((mainServer->tileBufferByteSend1s.layerSize[tempCount2]) & 0x0000ff00) >> 8;
					mainServer->packetsSendLayer[0].buffer[cumlativeByteSum++] = ((mainServer->tileBufferByteSend1s.layerSize[tempCount2]) & 0x000000ff);
					cout << "tile " << tempCount2 + 1 << " "<<mainServer->tileBufferByteSend1s.layerSize[tempCount2] << endl;
				}
				/*Size after filling the byte lengths of the tiles in the Base layer*/
				mainServer->packetsSendLayer[0].dataPacketSize += NUM_OF_TILES_BASE_LAYER*2;

				/*Filling the enhancement layer tile sizes*/
				for (tempCount1 = 0; tempCount1 < NUM_OF_ENHC_LAYER; tempCount1++) {
					for (tempCount2 = 0; tempCount2 < NUM_OF_FOV_TILES; tempCount2++) {
						mainServer->packetsSendLayer[0].buffer[cumlativeByteSum++] = ((mainServer->tileBufferByteSend1s.layerSize[NUM_OF_TILES_BASE_LAYER + (tempCount1) * NUM_OF_FOV_TILES + tempCount2]) & 0x0000ff00) >> 8;
						mainServer->packetsSendLayer[0].buffer[cumlativeByteSum++] = ((mainServer->tileBufferByteSend1s.layerSize[NUM_OF_TILES_BASE_LAYER + (tempCount1) * NUM_OF_FOV_TILES + tempCount2]) & 0x000000ff);
						cout << "tile " << NUM_OF_TILES_BASE_LAYER+ (tempCount1)* NUM_OF_FOV_TILES + tempCount2 + 1 << " "<<mainServer->tileBufferByteSend1s.layerSize[NUM_OF_TILES_BASE_LAYER + (tempCount1)* NUM_OF_FOV_TILES + tempCount2] << endl;
					}
				}
				/*Size after filling the byte lengths of the tiles in all the enhancement layers*/
				mainServer->packetsSendLayer[0].dataPacketSize += NUM_OF_ENHC_LAYER* NUM_OF_FOV_TILES*2;


				/*Fill the real data base layer to the buffer */
				int tempSize;

				/*Accessing each tile of the buffer*/
				for (tempCount2 = 0; tempCount2 < NUM_OF_TILES_BASE_LAYER; tempCount2++) {
					tempSize = mainServer->tileBufferByteSend1s.layerSize[tempCount2];
					copy(begin(mainServer->tileBufferByteSend1s.layerBufferArray[tempCount2].layerBuffer),
						&(mainServer->tileBufferByteSend1s.layerBufferArray[tempCount2].layerBuffer[tempSize]),
						mainServer->packetsSendLayer[0].buffer + cumlativeByteSum);
					cumlativeByteSum += tempSize;
				}

				/*Size after filling the data in base layer*/
				mainServer->packetsSendLayer[0].dataPacketSize += totLayerSize[0];
				
				/*Fill the enhance layer sizes*/
		
				
				/*Selecting each layer */
				for (tempCount1 = 0; tempCount1 < NUM_OF_ENHC_LAYER; tempCount1++) {
					/*Selecting each tile in each layer*/
					for (tempCount2 = 0; tempCount2 < NUM_OF_FOV_TILES; tempCount2++) {
						tempSize = mainServer->tileBufferByteSend1s.layerSize[NUM_OF_TILES_BASE_LAYER + (tempCount1) * NUM_OF_FOV_TILES + tempCount2];
						copy(begin(mainServer->tileBufferByteSend1s.layerBufferArray[NUM_OF_TILES_BASE_LAYER + (tempCount1) * NUM_OF_LAYERS + tempCount2].layerBuffer),
							&(mainServer->tileBufferByteSend1s.layerBufferArray[NUM_OF_TILES_BASE_LAYER + (tempCount1) * NUM_OF_LAYERS + tempCount2].layerBuffer[tempSize]),
							mainServer->packetsSendLayer[0].buffer + cumlativeByteSum);
						cumlativeByteSum += tempSize;
					}
					/*Size after each layer data*/
					mainServer->packetsSendLayer[0].dataPacketSize += totLayerSize[tempCount1+1];

				}
				cout << "Total Length :" << mainServer->packetsSendLayer[0].dataPacketSize << endl;

			}
			/*Send tilebuffer seperately to the client using multiple threads*/
			HANDLE myHandle[4];

			/*Creating multiple threads*/
			for (tempCount1 = 0; tempCount1 < EFFECTIVE_NUM_OF_SEND_THREADS; tempCount1++) {
				/*If the parallel streaming is enabled packetSendLayerParrallel is used*/
				if (mainServer->isParallelStreaming) {
					myHandle[tempCount1] = (HANDLE)_beginthreadex(0, 0, &myThreadSendData, &mainServer->packetsSendLayerParallel[tempCount1], 0, 0);
				}
				/*If parallel streaming is disabled packetSendLayer is used*/
				else {
					myHandle[tempCount1] = (HANDLE)_beginthreadex(0, 0, &myThreadSendData, &mainServer->packetsSendLayer[0], 0, 0);
				}
			}

			/*Wait for objects to be completed*/
			for (tempCount1 = 0; tempCount1 < EFFECTIVE_NUM_OF_SEND_THREADS; tempCount1++) {
				WaitForSingleObject(myHandle[tempCount1], INFINITE);
			}

			/*Release the thread and resources used*/
			for (tempCount1 = 0; tempCount1 < EFFECTIVE_NUM_OF_SEND_THREADS; tempCount1++) {
				CloseHandle(myHandle[tempCount1]);
			}

			//cout << "Tile Merged" << endl;


			
			
			memset(mainServer->chunkData, 0, sizeof mainServer->chunkData);
			memset(&(mainServer->tileBufferByte1s), 0, sizeof mainServer->tileBufferByte1s);
			memset(&(mainServer->packetsSendLayer), 0, sizeof mainServer->packetsSendLayer);
		}

	} while (retRequestFrmClient > 0);


	//closesocket(serverSideCommunicator->ListenSocket);


	return 0;
}


/*uint16_t size1 = mainServer->tileBufferByte1s.tileBufferSize[tempCount1].sublayer1Size;
uint16_t size2 = mainServer->tileBufferByte1s.tileBufferSize[tempCount1].sublayer2Size;
uint16_t size3 = mainServer->tileBufferByte1s.tileBufferSize[tempCount1].sublayer3Size;
uint16_t size4 = mainServer->tileBufferByte1s.tileBufferSize[tempCount1].sublayer4Size;*/

/*Filling the Layer Sizes*/

/*mainServer->packetsSend[tempCount1].buffer[0] = (size1 & 0x0000ff00) >> 8;
mainServer->packetsSend[tempCount1].buffer[1] = (size1 & 0x000000ff);
mainServer->packetsSend[tempCount1].buffer[2] = (size2 & 0x0000ff00) >> 8;
mainServer->packetsSend[tempCount1].buffer[3] = (size2 & 0x000000ff);
mainServer->packetsSend[tempCount1].buffer[4] = (size3 & 0x0000ff00) >> 8;
mainServer->packetsSend[tempCount1].buffer[5] = (size3 & 0x000000ff);
mainServer->packetsSend[tempCount1].buffer[6] = (size4 & 0x0000ff00) >> 8;
mainServer->packetsSend[tempCount1].buffer[7] = (size4 & 0x000000ff);*/

/*mainServer->packetsSend[tempCount1].buffer[tempCount1*NUM_OF_LAYERS+0] = (size1 & 0xff000000) >> 24;
mainServer->packetsSend[tempCount1].buffer[tempCount1*NUM_OF_LAYERS+1] = (size1 & 0x00ff0000) >> 16;
mainServer->packetsSend[tempCount1].buffer[tempCount1*NUM_OF_LAYERS+2] = (size1 & 0x0000ff00) >> 8;
mainServer->packetsSend[tempCount1].buffer[tempCount1*NUM_OF_LAYERS+3] = (size1 & 0x000000ff);*/