#include "opencv2/opencv.hpp"
#include <stdlib.h>
#include <stdio.h>
#include <dirent.h>
#include <string>
#include <vector>
#include <errno.h>
#include <string>
#include <iostream>
#include <sstream>
#include <fstream>
#include <string>


// Structs

struct annotationStruct{
	int frameNumber;
	int topLeftX;
	int topLeftY;
	int botRightX;
	int botRightY;
};

struct detectionStruct{
	int frameNumber;
	int topLeftX;
	int topLeftY;
	int botRightX;
	int botRightY;
	float score;
};

// Input argv variables
std::string imagesPreBootstrappingDir;
std::string detectionFilePath;
std::string groundTruthFilePath;
std::string bootstrappingDirPos, bootstrappingDirNeg;

// Initial the loop
std::vector<std::string> files = std::vector<std::string>();
std::string dir;

// Keeping the loop alive
int totalNumberOfFrames;
std::vector<detectionStruct> workingDetectionStruct;
std::vector<annotationStruct> workingAnnotationStruct;

