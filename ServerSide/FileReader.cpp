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
					AVPacket pkt;

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
					open_input_file(tempFilePath.c_str());
					

					

			
					ret = av_read_frame(ifmt_ctx, &pkt);

					while (ret >= 0) {
						ret=av_read_frame(ifmt_ctx, &pkt);
						uint8_t* ref = pkt.data;
						
						cout << "decompression time " << pkt.dts*60 << endl;
						cout << "presentation time  " << pkt.pts*60<<endl;
						cout << "size               " << pkt.size<<endl;
						cout << "flags              " << pkt.flags<<endl;
						
						if (ref == NULL) {
							cout << "NULL" << endl;
						}
						/*for (int i = 0; i < pkt.size+10; i++) {
							printf("%d\n", ref[i]);
						}*/
						cout << endl;

					}
					cout << endl;
					avformat_close_input(&ifmt_ctx);
					


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


int  FileReader::open_input_file(const char* filename)
{
	int ret;
	unsigned int i;

	ifmt_ctx = NULL;
	if ((ret = avformat_open_input(&ifmt_ctx, filename, NULL, NULL)) < 0) {
		av_log(NULL, AV_LOG_ERROR, "Cannot open input file\n");
		return ret;
	}

	if ((ret = avformat_find_stream_info(ifmt_ctx, NULL)) < 0) {
		av_log(NULL, AV_LOG_ERROR, "Cannot find stream information\n");
		return ret;
	}

	//stream_ctx = (StreamContext*)av_mallocz_array(ifmt_ctx->nb_streams, sizeof(*stream_ctx));
	//if (!stream_ctx)
		//return AVERROR(ENOMEM);

	/*for (i = 0; i < ifmt_ctx->nb_streams; i++) {
		AVStream* stream = ifmt_ctx->streams[i];
		AVCodec* dec = avcodec_find_decoder(stream->codecpar->codec_id);
		AVCodecContext* codec_ctx;
		if (!dec) {
			av_log(NULL, AV_LOG_ERROR, "Failed to find decoder for stream #%u\n", i);
			return AVERROR_DECODER_NOT_FOUND;
		}
		codec_ctx = avcodec_alloc_context3(dec);
		if (!codec_ctx) {
			av_log(NULL, AV_LOG_ERROR, "Failed to allocate the decoder context for stream #%u\n", i);
			return AVERROR(ENOMEM);
		}
		ret = avcodec_parameters_to_context(codec_ctx, stream->codecpar);
		if (ret < 0) {
			av_log(NULL, AV_LOG_ERROR, "Failed to copy decoder parameters to input decoder context "
				"for stream #%u\n", i);
			return ret;
		}
		/* Reencode video & audio and remux subtitles etc. */
		/*if (codec_ctx->codec_type == AVMEDIA_TYPE_VIDEO
			|| codec_ctx->codec_type == AVMEDIA_TYPE_AUDIO) {
			if (codec_ctx->codec_type == AVMEDIA_TYPE_VIDEO)
				codec_ctx->framerate = av_guess_frame_rate(ifmt_ctx, stream, NULL);
			/* Open decoder */
		/*	ret = avcodec_open2(codec_ctx, dec, NULL);
			if (ret < 0) {
				av_log(NULL, AV_LOG_ERROR, "Failed to open decoder for stream #%u\n", i);
				return ret;
			}
		}*/
		//stream_ctx[i].dec_ctx = codec_ctx;
	//}

	//av_dump_format(ifmt_ctx, 0, filename, 0);
	
	return 0;
}
