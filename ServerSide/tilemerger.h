#ifndef TILEMERGER_H
#define TILEMERGER_H



/*header file for TileMerger class*/
//#include "filereader.h"
#include "commondata.h"


class TileMerger {
private:
	int xCor[4];
	int yCor[4];
	int qualityL;
	int chunk;
	videoDataBase* videoDataBases[QUALITY_LEVEL];
	tileBufferByte* tileBufferByte1s;
	tileBufferByteSend* tileBufferByteSend1s;

public:
	int tileXCor[20] = {0,1,2,3,4,0,1,2,3,4,0,1,2,3,4,0,1,2,3,4};
	int tileYCor[20] = {0,0,0,0,0,1,1,1,1,1,2,2,2,2,2,3,3,3,3,3};

	TileMerger(tileBufferByte* tileBufferByte1s, videoDataBase videoDataBases[QUALITY_LEVEL], tileBufferByteSend* tileBufferByteSend1s);
	void setTiles(int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4, int qualityL, int chunk);
	void mergeTiles();

};

#endif // !TILEMERGER_H
