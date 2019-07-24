/*This function takes the tile coordinates, quality levels 
and chunk number as the input and sends the data to the
server to be sent to the client. Data read from database 
is stored in a seperateBuffer which is defined as tile1Sec 
array with 6 elements with additional 2 tile1Sec elements*/

#include "tilemerger.h"

TileMerger::TileMerger(tileBuffer* tileBuffer1s, videoDataBase videoDataBases[QUALITY_LEVEL]) {
	this->tileBuffer1s = tileBuffer1s;
	this->videoDataBases[0] = &videoDataBases[0];
	this->videoDataBases[1] = &videoDataBases[1];
	this->videoDataBases[2] = &videoDataBases[2];
}
 
/*In this context x = col and y = row*/
void TileMerger::setTiles(int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4, int qualityL, int chunk) {
	this->xCor[0] = x1; this->yCor[0] = y1;
	this->xCor[1] = x2; this->yCor[1] = y2;
	this->xCor[2] = x3; this->yCor[2] = y3;
	this->xCor[3] = x4; this->yCor[3] = y4;
	this->qualityL = qualityL;
	this->chunk = chunk;
}

void TileMerger::mergeTiles() {
	int tileNum;

	for (tileNum = 0; tileNum < NUM_OF_TILES; tileNum++) {
		switch (this->qualityL) {
		case QUALITY_SD:
			(*this->tileBuffer1s).tileBuffer[tileNum] = (*this->videoDataBases[0]).tiles[xCor[tileNum]][yCor[tileNum]].chunks[chunk];
			break;
		case QUALITY_HD:
			(*this->tileBuffer1s).tileBuffer[tileNum] = (*this->videoDataBases[1]).tiles[xCor[tileNum]][yCor[tileNum]].chunks[chunk];
			break;

		case QUALITY_4K:
			(*this->tileBuffer1s).tileBuffer[tileNum] = (*this->videoDataBases[1]).tiles[xCor[tileNum]][yCor[tileNum]].chunks[chunk];
			break;
		}
	}
}
