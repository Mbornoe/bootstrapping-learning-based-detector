#include "opencv2/opencv.hpp"
#include <stdlib.h>
#include <stdio.h>
#include <dirent.h>
#include <string>
#include <vector>
#include <errno.h>
#include <string>
#include <iostream>

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
