#ifndef TILEMERGER_H
#define TILEMERGER_H



/*header file for TileMerger class*/
//#include "filereader.h"
#include "commondata.h"
#define NUM_OF_TILES	4

class TileMerger {
private:
	int xCor[4];
	int yCor[4];
	int qualityL;
	int chunk;
	videoDataBase* videoDataBases[QUALITY_LEVEL];
	tileBuffer* tileBuffer1s;

public:
	TileMerger(tileBuffer* tileBuffer1s, videoDataBase videoDataBases[QUALITY_LEVEL]);
	void setTiles(int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4, int qualityL, int chunk);
	void mergeTiles();

};

#endif // !TILEMERGER_H
