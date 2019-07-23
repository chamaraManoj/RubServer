
#ifndef COMMONDATA_H
#define COMMONDATA_H


#include <vector>
#include <iostream>
#include <string>
#include <string.h>
#include <sstream>
#include <iomanip>
#include <assert.h>



#define NUM_OF_ROWS		4
#define NUM_OF_COL		5
#define NUM_OF_SEC		60
#define NUM_OF_LAYERS 	4

#define QUALITY_SD		0
#define QUALITY_HD		1
#define QUALITY_4K		2

#define QUALITY_LEVEL   3

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

typedef struct tile1Sec {
	std::vector<AVPacket> subLayer1;
	std::vector<AVPacket> subLayer2;
	std::vector<AVPacket> subLayer3;
	std::vector<AVPacket> subLayer4;
}tile1Sec;

typedef struct chunk60Sec {
	tile1Sec chunks[60];
}chunk60Sec;

typedef struct video {
	chunk60Sec tiles[4][5];
}videoDataBase;

typedef struct tileBuffer {
	tile1Sec tileBuffer[6];
}tileBuffer;
#endif // !COMMONDATA_H