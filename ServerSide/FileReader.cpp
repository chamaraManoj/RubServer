/*This source code  implements class for reading encoded video files to 
as pacckets and return those packets to the tile merger class*/

#include "filereader.h"
#include <sstream>
#include <iomanip>

/*Class for reading the encoded video files to the main memoray database*/

FileReader::FileReader(string filePath, int resolution, videoDataBase* _videoDataBase,int *i){
	this->inputFilePath = filePath;
	this->resolution = resolution;
	this->_videoDataBase = _videoDataBase;
	//this->i = i;
}

/*=====Method of File Reader class=======================
This function generate the file path to read the frames*/
string FileReader::getFilePath(int row, int col, int sec, int layer) {
	string tempString;
	
	tempString = this->inputFilePath;
	tempString.append("\\frame_").append(to_string(this->resolution)).append("_").
		append(to_string(row)).append("_").append(to_string(col)).append("\\frame_");

	stringstream frameIndex;
	frameIndex << setw(3);
	frameIndex << setfill('0') << sec;

	tempString.append(frameIndex.str()).append("_").append(to_string(layer)).append(".mp4");
	
	return tempString;
}

/*========Method of FileReader class===================='
This function creates creates the database and read 
packets from the video files. */
int FileReader::createDataBase() {
	
	int row;
	int col;
	int sec;
	int layer;
	//*i = *i + 1;
	int ret = 0, got_frame;
	AVPacket pkt;
	AVFormatContext* fmt_ctx = NULL;
	AVIOContext* input = NULL;
	uint8_t buf[DEFAULT_SUB_LAYER_LENGTH];
	int video_frame_count = 0;
	int numOfBytesread;
	int numOfBytesReadTemp;

	string tempFilePath;


	for (row = 0; row < NUM_OF_ROWS; row++) {
		for (col = 0; col < NUM_OF_COL; col++) {
			for (sec = 0; sec < NUM_OF_SEC; sec++) {
				for (layer = 0; layer < NUM_OF_LAYERS; layer++) {
					tempFilePath = this->getFilePath(row, col, sec, layer);
					input = NULL;
					/*Open the input file and allocate the format context: i.e. mp4,mpeg*/
					if (avio_open2(&input, tempFilePath.c_str(), AVIO_FLAG_READ, NULL, NULL) < 0) {
						fprintf(stderr, "Could not open source file %s\n", tempFilePath.c_str());
						exit(1);
					}

					numOfBytesread = 0;
					numOfBytesReadTemp = 0;
					/*Get the stream information: video/audio*/
					/*if (avformat_find_stream_info(fmt_ctx, NULL) < 0) {
						fprintf(stderr, "Could not find stream information\n");
						exit(1);
					}*/

					/*Printing the details about the inputformat*/
					//av_dump_format(fmt_ctx, 0, tempFilePath.c_str(), 0);

					/*initialize the packet*/
					/*av_init_packet(&pkt);
					pkt.data = NULL;
					pkt.size = 0;*/
					//av_log(NULL, AV_LOG_INFO, "readFile %d %d %02d %d  \n", row, col, sec, layer);
					cout << "read file" << " " << row << " " << col << " " << sec << " " << layer << endl;
					int frameCount=0;
					while ((numOfBytesReadTemp = avio_read(input, buf, sizeof(buf))) >= 0) {
						//frameCount++;
						//AVPacket orig_pkt = pkt;
						numOfBytesread += numOfBytesReadTemp;
					}
						
						//if (pkt.size > 1) {
							
						//printf("%d", pkt.size );
						/*stroing the file into frame*/

					
					switch (layer) {
					case 0:
						copy(begin(buf),end(buf),(*this->_videoDataBase).tiles[row][col].chunks[sec].subLayer1);
						(*this->_videoDataBase).tilesSize[row][col].chunksSize[sec].sublayer1Size = numOfBytesread;
						break;

					case 1:
						//if(frameCount == 1)
						//	break;
						//else {
						copy(begin(buf), end(buf), (*this->_videoDataBase).tiles[row][col].chunks[sec].subLayer2);
						(*this->_videoDataBase).tilesSize[row][col].chunksSize[sec].sublayer2Size = numOfBytesread;
						break;
						//}
								
					case 2:
						//if (frameCount == 1)
						//	break;
						//else {
						copy(begin(buf), end(buf), (*this->_videoDataBase).tiles[row][col].chunks[sec].subLayer3);
						(*this->_videoDataBase).tilesSize[row][col].chunksSize[sec].sublayer3Size = numOfBytesread;
						break;
						//}

					case 3:
						//if (frameCount == 1)
						//	break;
						//else {
						copy(begin(buf), end(buf), (*this->_videoDataBase).tiles[row][col].chunks[sec].subLayer4);
						(*this->_videoDataBase).tilesSize[row][col].chunksSize[sec].sublayer4Size = numOfBytesread;
						break;
						//}
									
					}
							
						//}
						//else {
						//	av_log(NULL, AV_LOG_INFO, "readFrame %d %d %02d %d %02d\n", row, col, sec, layer, frameCount);
						//}

						//av_packet_unref(&orig_pkt);
					
					memset(buf, 0, sizeof(buf));
					avio_close(input);
					//av_log(NULL, AV_LOG_INFO, "\n");

					//avformat_close_input(&fmt_ctx);
				}
				
			}
		}
	}

	if (row == NUM_OF_ROWS && col == NUM_OF_COL)
		return 0;
	else
		return -1;
}




