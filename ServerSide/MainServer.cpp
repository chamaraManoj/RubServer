
#include "mainserver.h"

MainServer::MainServer() {}

int main() {
	MainServer mainServer;

	mainServer.i = 100;
	mainServer.filePaths[0] = "H:\\My_Codes\\Rubiks_implementation\\RubiksVideos\\processedVideo_SD";
	//mainServer.filePaths[1] = "H:\\My_Codes\\Rubiks_implementation\\RubiksVideos\\processedVideo_HD";
	//mainServer.filePaths[2] = "H:\\My_Codes\\Rubiks_implementation\\RubiksVideos\\processedVideo_4K";

	FileReader* fileReaderSD = new FileReader(mainServer.filePaths[0], 720, &mainServer.videoDataBases[0],&mainServer.i);
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
	TileMerger* tilemerger = new TileMerger(&mainServer.tileBuffer1s,mainServer.videoDataBases);
	
	tilemerger->setTiles(0, 0, 0, 1, 1, 0, 1, 1, 0, 0);
	tilemerger->mergeTiles();

	return 0;
}