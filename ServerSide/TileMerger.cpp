/*This function takes the tile coordinates, quality levels 
and chunk number as the input and sends the data to the
server to be sent to the client. Data read from database 
is stored in a seperateBuffer which is defined as tile1Sec 
array with 6 elements with additional 2 tile1Sec elements*/

#include "tilemerger.h"

TileMerger::TileMerger(tileBufferByte* tileBufferByte1s, videoDataBase videoDataBases[QUALITY_LEVEL], tileBufferByteSend* tileBufferByteSend1s) {
	this->tileBufferByte1s = tileBufferByte1s;
	this->tileBufferByteSend1s = tileBufferByteSend1s;
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
	int layerNum;
	static int counterTiles = 0;
	for (layerNum = 0; layerNum < NUM_OF_LAYERS; layerNum++) {
		switch (this->qualityL) {
		case QUALITY_SD:
			/*copy(begin((*this->videoDataBases[0]).tiles[xCor[layerNum]][yCor[layerNum]].chunks[chunk].subLayer1),
				end((*this->videoDataBases[0]).tiles[xCor[layerNum]][yCor[layerNum]].chunks[chunk].subLayer1),
				(*this->tileBufferByte1s).tileBuffer[layerNum].subLayer1);

			copy(begin((*this->videoDataBases[0]).tiles[xCor[layerNum]][yCor[layerNum]].chunks[chunk].subLayer2),
				end((*this->videoDataBases[0]).tiles[xCor[layerNum]][yCor[layerNum]].chunks[chunk].subLayer2),
				(*this->tileBufferByte1s).tileBuffer[layerNum].subLayer2);

			copy(begin((*this->videoDataBases[0]).tiles[xCor[layerNum]][yCor[layerNum]].chunks[chunk].subLayer3),
				end((*this->videoDataBases[0]).tiles[xCor[layerNum]][yCor[layerNum]].chunks[chunk].subLayer3),
				(*this->tileBufferByte1s).tileBuffer[layerNum].subLayer3);

			copy(begin((*this->videoDataBases[0]).tiles[xCor[layerNum]][yCor[layerNum]].chunks[chunk].subLayer4),
				end((*this->videoDataBases[0]).tiles[xCor[layerNum]][yCor[layerNum]].chunks[chunk].subLayer4),
				(*this->tileBufferByte1s).tileBuffer[layerNum].subLayer4);*/

			/*==============================================================================*/
			/*Reading the tile buffer values read*/
			/*(*this->tileBufferByte1s).tileBuffer[layerNum] = 
				(*this->videoDataBases[0]).tiles[yCor[layerNum]][xCor[layerNum]].chunks[chunk];*/
			/*==============================================================================*/

			if (layerNum == 0) {
				int layerSize = 0;
				for (int raw = 0; raw < NUM_OF_ROWS; raw++) {
					for (int col = 0; col < NUM_OF_COL; col++) {
						copy(begin((*this->videoDataBases[0]).tiles[raw][col].chunks[chunk].subLayer1),
							end((*this->videoDataBases[0]).tiles[raw][col].chunks[chunk].subLayer1),
							(*this->tileBufferByteSend1s).layerBufferArray[raw*NUM_OF_COL + col].layerBuffer);

						(*this->tileBufferByteSend1s).layerSize[raw*NUM_OF_COL + col] = 
							(*this->videoDataBases[0]).tilesSize[raw][col].chunksSize->sublayer1Size;

						counterTiles++;
					}
				}				
			}
			else if(layerNum==1) {
				int layerSize = 0;
				for (int tempCounter = 0; tempCounter < NUM_OF_FOV_TILES;tempCounter++) {
					copy(begin((*this->videoDataBases[0]).tiles[yCor[tempCounter]][xCor[tempCounter]].chunks[chunk].subLayer2),
						end((*this->videoDataBases[0]).tiles[yCor[tempCounter]][xCor[tempCounter]].chunks[chunk].subLayer2),
						(*this->tileBufferByteSend1s).layerBufferArray[counterTiles].layerBuffer);

					(*this->tileBufferByteSend1s).layerSize[counterTiles] =
						(*this->videoDataBases[0]).tilesSize[yCor[tempCounter]][xCor[tempCounter]].chunksSize->sublayer2Size;
					counterTiles++;
				}
			}

			else if (layerNum == 2) {
				int layerSize = 0;
				for (int tempCounter = 0; tempCounter < NUM_OF_FOV_TILES; tempCounter++) {
					copy(begin((*this->videoDataBases[0]).tiles[yCor[tempCounter]][xCor[tempCounter]].chunks[chunk].subLayer3),
						end((*this->videoDataBases[0]).tiles[yCor[tempCounter]][xCor[tempCounter]].chunks[chunk].subLayer3),
						(*this->tileBufferByteSend1s).layerBufferArray[counterTiles].layerBuffer);

					(*this->tileBufferByteSend1s).layerSize[counterTiles] =
						(*this->videoDataBases[0]).tilesSize[yCor[tempCounter]][xCor[tempCounter]].chunksSize->sublayer3Size;
					counterTiles++;
				}
			}

			else if (layerNum == 3) {
				int layerSize = 0;
				for (int tempCounter = 0; tempCounter < NUM_OF_FOV_TILES; tempCounter++) {
					copy(begin((*this->videoDataBases[0]).tiles[yCor[tempCounter]][xCor[tempCounter]].chunks[chunk].subLayer4),
						end((*this->videoDataBases[0]).tiles[yCor[tempCounter]][xCor[tempCounter]].chunks[chunk].subLayer4),
						(*this->tileBufferByteSend1s).layerBufferArray[counterTiles].layerBuffer);

					(*this->tileBufferByteSend1s).layerSize[counterTiles] =
						(*this->videoDataBases[0]).tilesSize[yCor[tempCounter]][xCor[tempCounter]].chunksSize->sublayer4Size;
					counterTiles++;
				}
			}



			/*Reading the sizes of each layer*/
			/*(*this->tileBufferByte1s).tileBufferSize[layerNum].sublayer1Size =
				(*this->videoDataBases[0]).tilesSize[yCor[layerNum]][xCor[layerNum]].chunksSize->sublayer1Size;

			(*this->tileBufferByte1s).tileBufferSize[layerNum].sublayer2Size =
				(*this->videoDataBases[0]).tilesSize[yCor[layerNum]][xCor[layerNum]].chunksSize->sublayer2Size;

			(*this->tileBufferByte1s).tileBufferSize[layerNum].sublayer3Size =
				(*this->videoDataBases[0]).tilesSize[yCor[layerNum]][xCor[layerNum]].chunksSize->sublayer3Size;

			(*this->tileBufferByte1s).tileBufferSize[layerNum].sublayer4Size =
				(*this->videoDataBases[0]).tilesSize[yCor[layerNum]][xCor[layerNum]].chunksSize->sublayer4Size;*/

			
			//(*this->tileBufferByte1s).tileBuffer[layerNum] = (*this->videoDataBases[0]).tiles[xCor[layerNum]][yCor[layerNum]].chunks[chunk];
			break;
		case QUALITY_HD:
			(*this->tileBufferByte1s).tileBuffer[layerNum] = (*this->videoDataBases[1]).tiles[xCor[layerNum]][yCor[layerNum]].chunks[chunk];
			break;

		case QUALITY_4K:
			(*this->tileBufferByte1s).tileBuffer[layerNum] = (*this->videoDataBases[1]).tiles[xCor[layerNum]][yCor[layerNum]].chunks[chunk];
			break;
		}
	}
}
