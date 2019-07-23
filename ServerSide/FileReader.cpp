/*This source code  implements class for reading encoded video files to 
as pacckets and return those packets to the tile merger class*/

#include "filereader.h"
#include <sstream>
#include <iomanip>

/*Class for reading the encoded video files to the main memoray database*/

FileReader::FileReader(string filePath, int resolution, videoDataBase* videoDataBaseSD,int *i){
	this->inputFilePath = filePath;
	this->resolution = resolution;
	this->videoDataBaseSD = videoDataBaseSD;
	this->i = i;
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
	*i = *i + 1;
	int ret = 0, got_frame;
	AVPacket pkt;
	AVFormatContext* fmt_ctx = NULL;
	int video_frame_count = 0;

	string tempFilePath;


	for (row = 0; row < NUM_OF_ROWS; row++) {
		for (col = 0; col < NUM_OF_COL; col++) {
			for (sec = 0; sec < NUM_OF_SEC; sec++) {
				for (layer = 0; layer < NUM_OF_LAYERS; layer++) {
					tempFilePath = this->getFilePath(row, col, sec, layer);

					/*Open the input file and allocate the format context: i.e. mp4,mpeg*/
					if (avformat_open_input(&fmt_ctx, tempFilePath.c_str(), NULL, NULL) < 0) { 
						fprintf(stderr, "Could not open source file %s\n", tempFilePath.c_str());
						exit(1);
					}

					/*Get the stream information: video/audio*/
					if (avformat_find_stream_info(fmt_ctx, NULL) < 0) {
						fprintf(stderr, "Could not find stream information\n");
						exit(1);
					}

					/*Printing the details about the inputformat*/
					//av_dump_format(fmt_ctx, 0, tempFilePath.c_str(), 0);

					/*initialize the packet*/
					av_init_packet(&pkt);
					pkt.data = NULL;
					pkt.size = 0;

					int frameCount=0;
					while (av_read_frame(fmt_ctx, &pkt) >= 0) {
						frameCount++;
						AVPacket orig_pkt = pkt;

						av_log(NULL, AV_LOG_INFO, "readFrame %d %d %02d %d %01d \n",row,col,sec,layer, frameCount);
						if (pkt.size > 1) {
							
							//printf("%d", pkt.size );
							/*stroing the file into frame*/
							switch (layer) {
							case 0:
								(*this->videoDataBaseSD).tiles[row][col].chunks[sec].subLayer1.push_back(orig_pkt);
								break;

							case 1:
								if(frameCount == 1)
									break;
								else {
									(*this->videoDataBaseSD).tiles[row][col].chunks[sec].subLayer2.push_back(orig_pkt);
									break;
								}
								
							case 2:
								if (frameCount == 1)
									break;
								else {
									(*this->videoDataBaseSD).tiles[row][col].chunks[sec].subLayer3.push_back(orig_pkt);
									break;
								}

							case 3:
								if (frameCount == 1)
									break;
								else {
									(*this->videoDataBaseSD).tiles[row][col].chunks[sec].subLayer4.push_back(orig_pkt);
									break;
								}
									
							}
							
						}
						else {
							av_log(NULL, AV_LOG_INFO, "readFrame %d %d %02d %d %02d\n", row, col, sec, layer, frameCount);
						}

						av_packet_unref(&orig_pkt);
					}
					av_log(NULL, AV_LOG_INFO, "\n");
					avformat_close_input(&fmt_ctx);
				}
				
			}
		}
	}

	if (row == NUM_OF_ROWS && col == NUM_OF_COL)
		return 0;
	else
		return -1;
}




