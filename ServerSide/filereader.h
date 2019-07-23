
/*header file for fileheader class*/
#ifndef FILEREADER_H
#define FILEREADER_H

#include "commondata.h"
#include <string>
#include <iostream>



class FileReader {
private:
	string inputFilePath;
	int resolution;
	videoDataBase* videoDataBaseSD;

public:
	int *i;
	FileReader(string filePath, int resolution, videoDataBase* videoDataBaseSD,int *i);
	videoDataBase videoTemp;
	string getFilePath(int row, int col, int sec, int layer);
	int createDataBase();

};
#endif 



