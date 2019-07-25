
#include "mainserver.h"

MainServer::MainServer() {}

int main() {

	int retRequestFrmClient=0;
	int chunk;
	int x1, x2, x3, x4;
	int y1, y2, y3, y4;
	

	MainServer* mainServer = new MainServer();

	mainServer->i = 100;
	mainServer->filePaths[0] = "H:\\My_Codes\\Rubiks_implementation\\RubiksVideos\\processedVideo_SD";
	//mainServer.filePaths[1] = "H:\\My_Codes\\Rubiks_implementation\\RubiksVideos\\processedVideo_HD";
	//mainServer.filePaths[2] = "H:\\My_Codes\\Rubiks_implementation\\RubiksVideos\\processedVideo_4K";

	FileReader* fileReaderSD = new FileReader(mainServer->filePaths[0], 720, &mainServer->videoDataBases[0],&mainServer->i);
	//FileReader* fileReaderHD = new FileReader(mainServer.filePaths[1], 1080, mainServer.videoDataBases[1]);
	//FileReader* fileReader$K = new FileReader(mainServer.filePaths[2], 2160, mainServer.videoDataBases[2]);


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
	This code section should have the HTTP server communication module.
	Server module returns the current tile positions, chunk number and
	quality to be streamed from the server. These values are directly 
	passed to the tileMerger object which in turn returns the selected 
	tiles from SD, HD or 4K database.
	***/
	TileMerger* tilemerger = new TileMerger(&mainServer->tileBufferByte1s,mainServer->videoDataBases);
	
	/*After reading from tilemerger funtion, data is tranfered to the
	Communicaotr modle to be sent to the client*/
	Communicator* serverSideCommunicator = new Communicator(&mainServer->tileBufferByte1s);
	
	bool listenSuccessed = serverSideCommunicator->intializeServer();

	//tilemerger->setTiles(0, 0, 0, 1, 1, 0, 1, 1, 0, 0);
	//tilemerger->mergeTiles();

	assert(listenSuccessed == true);


	do {
		retRequestFrmClient = serverSideCommunicator->readFrameRequest(mainServer->chunkData);	
		cout << retRequestFrmClient << endl;
		
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

		tilemerger->setTiles(x1,y1,x2,y2,x3,y3,x4,y4, mainServer->chunkData[CHUNK_QUAL_IND],chunk);
		tilemerger->mergeTiles();

		cout << "Tile Merged" << endl;
		




		memset(mainServer->chunkData, 0, sizeof mainServer->chunkData);
		
	} while (retRequestFrmClient > 0);
	
	
	closesocket(serverSideCommunicator->ListenSocket);
	

	return 0;
}