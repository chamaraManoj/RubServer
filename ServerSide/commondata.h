
#ifndef COMMONDATA_H
#define COMMONDATA_H

#define WIN32_LEAN_AND_MEAN

#include <vector>
#include <iostream>
#include <string>
#include <string.h>
#include <sstream>
#include <iomanip>
#include <assert.h>
//#include <Windows.h>
#include <process.h>
#include <stdio.h>

#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>

/*Each video is 60 seconds long and each frame is divided into 
4 x 5 tiles. Each tile stream is divided to 4 layers*/
#define NUM_OF_ROWS					4
#define NUM_OF_COL					5
#define NUM_OF_SEC					60
#define NUM_OF_LAYERS 				4


#define QUALITY_SD					0
#define QUALITY_HD					1
#define QUALITY_4K					2

#define QUALITY_LEVEL				3	
#define DEFAULT_BUFLEN_RECEIVE		10

/*Inidices of the received data packet from the client*/
#define CHUNK_FRAME_IND_1			0
#define CHUNK_FRAME_IND_2			1
#define CHUNK_FRAME_IND_3			2
#define CHUNK_FRAME_IND_4			3
#define TILE_NO_1_IND				4
#define TILE_NO_2_IND				5
#define TILE_NO_3_IND				6
#define TILE_NO_4_IND				7
#define CHUNK_QUAL_IND				8	




#define LAYER_1							0
#define LAYER_2							1
#define LAYER_3							2
#define LAYER_4							3

/*Communication model uses 5 sockets
SOCKET 0 listen to the request from the server
SOCKET 1,2,3,4 send data to the server*/
#define SOCKET_0						0
#define SOCKET_1						1
#define SOCKET_2						2
#define SOCKET_3						3
#define SOCKET_4						4



/*Each sub layer of a frame is 1024*10 long and
each packet related to the 1s video chunk of tile is 
1024*10*4 + 4*2 long*/
#define NUM_OF_SEND_THREADS				4
#define NUM_OF_TOTAL_PORTS				5 //including the listening port
#define NUM_OF_TILES_BASE_LAYER			NUM_OF_COL*NUM_OF_ROWS
#define NUM_OF_ENHC_LAYER				3
#define NUM_OF_TOT_TILES				NUM_OF_TILES_BASE_LAYER + NUM_OF_ENHC_LAYER * 4

#define NUM_OF_FOV_TILES				4
#define NUM_OF_LAYERS					4

#define DEFAULT_SUB_LAYER_LENGTH		1024*8

#define DEFAULT_PACKET_BUFFER_B_LAYER	DEFAULT_SUB_LAYER_LENGTH*NUM_OF_TILES_BASE_LAYER + 2*NUM_OF_TILES_BASE_LAYER + 4  //2*20 => 2 Bytes * 20 for layer length data , Last 2 is for adding
																														  // the chunk number
#define DEFAULT_PACKET_BUFFER_E_LAYER	DEFAULT_SUB_LAYER_LENGTH*NUM_OF_FOV_TILES + 2*NUM_OF_FOV_TILES

#define TOT_BUFFER_LENGTH				DEFAULT_PACKET_BUFFER_B_LAYER + DEFAULT_PACKET_BUFFER_E_LAYER*3





using namespace std;

extern "C" {
#include <libavutil/imgutils.h>
#include <libavutil/samplefmt.h>
#include <libavutil/timestamp.h>
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavfilter/buffersink.h>
#include <libavfilter/buffersrc.h>
#include <libavutil/opt.h>
#include <libavutil/pixdesc.h>
#include <libavutil/rational.h>
}

/*Structure of the database of video*/

/*typedef struct tile1Sec {
	std::vector<AVPacket> subLayer1;
	std::vector<AVPacket> subLayer2;
	std::vector<AVPacket> subLayer3;
	std::vector<AVPacket> subLayer4;
}tile1Sec;*/

/*typedef struct chunk60Sec {
	tile1Sec chunks[60];
}chunk60Sec;*/

/*typedef struct tileBuffer {
	tile1Sec tileBuffer[4];
}tileBuffer;*/

typedef struct tile1SecByte {
	uint8_t subLayer1[DEFAULT_SUB_LAYER_LENGTH];
	uint8_t subLayer2[DEFAULT_SUB_LAYER_LENGTH];
	uint8_t subLayer3[DEFAULT_SUB_LAYER_LENGTH];
	uint8_t subLayer4[DEFAULT_SUB_LAYER_LENGTH];
}tile1SecByte;

typedef struct tile1SecSize {
	uint16_t sublayer1Size;
	uint16_t sublayer2Size;
	uint16_t sublayer3Size;
	uint16_t sublayer4Size;
}tile1SecSize;

typedef struct chunk60SecByte {
	tile1SecByte chunks[60];
}chunk60SecByte;

typedef struct chunk60SecSize {
	tile1SecSize chunksSize[60];
}chunk60SecSize;

typedef struct videoDataBase {
	chunk60SecByte tiles[4][5];
	chunk60SecSize tilesSize[4][5];
}videoDataBase;

typedef struct tileBufferByte {
	tile1SecByte tileBuffer[4];
	tile1SecSize tileBufferSize[4];
}tileBufferByte;
/*typedef struct videoDataBaseSize {
	chunk60SecSize tilesSize[4][5];
}videoDataBaseSize;
*/

/*===================================================*/
typedef struct layer {
	uint8_t layerBuffer[DEFAULT_SUB_LAYER_LENGTH];
}layer;

typedef struct tileBufferByteSend {
	layer layerBufferArray[NUM_OF_TOT_TILES];
	uint16_t layerSize[NUM_OF_TOT_TILES];
}tileBufferByteSend;

typedef struct dataPacketSendBLayer {
	int dataPacketSize;
	//uint16_t layerSizes[NUM_OF_LAYERS];
	uint8_t buffer[TOT_BUFFER_LENGTH];
	SOCKET socket;
}dataPacketSendLayer;

/*typedef struct dataPacketSendELayer {
	int dataPacketSize;
	uint16_t layerSizes[NUM_OF_LAYERS];
	uint8_t buffer[DEFAULT_PACKET_BUFFER_E_LAYER];
	SOCKET socket;
}dataPacketSendELayer;*/
/*==================================================*/
#endif // !COMMONDATA_HL