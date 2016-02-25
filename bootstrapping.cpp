#include "bootstrapping.hpp"


void help() {
  std::cout << "Bootstrapping code for a learning based detector that takes uses ";
  std::cout << "a detection and ground truth file to generate additional hard ";
  std::cout << "negatives and positives based on false positives and false negatives.\n"<< std::endl;

  std::cout << "Usage: ./bootstrapping <detectionFile.csv> <groundTruthFile> <imagesDirectory>\n" << std::endl;


}
int getdir (std::string dir, std::vector<std::string> &files)
{
    DIR *dp;
    struct dirent *dirp;
    if((dp  = opendir(dir.c_str())) == NULL) {
        std::cout << "Error(" << errno << ") opening " << dir << std::endl;
        return errno;
    }

    while ((dirp = readdir(dp)) != NULL) {
        files.push_back(std::string(dirp->d_name));
    }
    closedir(dp);
    return 0;
}


float calcPascalOverlap(cv::Rect bbOne, cv::Rect  bbTwo){
	/* bbOne and bbTwo syntax is: [leftTopX, leftTopY, width, height] */
	float pascalVar;

	int bbOneRight = (bbOne.x + bbOne.width);
	int bbOneLeft = bbOne.x ;
	int bbTwoRight = (bbTwo.x + bbTwo.width);
	int bbTwoLeft = bbTwo.x;

	int bbOneTop = bbOne.y;
	int bbOneBot = (bbOne.y + bbOne.height);

	int bbTwoTop = bbTwo.y;
	int bbTwoBot = (bbTwo.y + bbTwo.height);

	if ( bbOneRight < bbTwoLeft || bbOneLeft > bbTwoRight || bbOneBot < bbTwoTop ||  bbOneTop > bbTwoBot ) 
	{
		pascalVar = 0; // bbOne and bbTwo does not overlap.
	} 
	else{ 
		int SA = bbOne.width * bbOne.height;
		int SB = bbTwo.width * bbTwo.height;

		int XA1 = bbOne.x;
		int XA2 = bbOne.x + bbOne.width;
		int YA1 = bbOne.y;
		int YA2 = bbOne.y + bbOne.height;

		int XB1 = bbTwo.x;
		int XB2 = bbTwo.x + bbTwo.width;
		int YB1 = bbTwo.y;
		int YB2 = bbTwo.y + bbTwo.height;

		float SI = std::max(0, std::min(XA2, XB2) - std::max(XA1, XB1) ) * std::max(0, std::min(YA2, YB2) - std::max(YA1, YB1) );

		//std::cout << "SI: " << SI << " SA: " << SA << " SB: " << SB << std::endl;
		float SU = SA + SB - SI;

		pascalVar = SI/SU;
	}
	return pascalVar;
}

int numberOfFrames(std::vector<std::string> &files) {
  int numberOfImages = 0;
  for (size_t i = 0; i < files.size(); i++) {
    if ( files[i] != "." && files[i] != ".." && files[i] != ".DS_Store" && files[i] != ".git" && files[i] != ".gitignore"  ){
        // std::cout << files[i] << std::endl;
        numberOfImages = numberOfImages + 1;
      }
    }
  return numberOfImages;
}

std::vector<int> findMatchesInStruct(int frameOfInterest, std::vector<int> structFrameOverview){
	std::vector<int> returnFrames;
	for (int i = 0; i < structFrameOverview.size(); ++i)
	{
		if ( structFrameOverview[i] == frameOfInterest){
			returnFrames.push_back(i);
		}
	}
	return returnFrames;
}


int main(int argc, char* argv[]) {
  if (argc < 3) {
    help();
  }
  detectionFilePath = argv[1];
  groundTruthFilePath = argv[2];
  imagesPreBootstrappingDir = argv[3];

  bootstrappingDirPos = "outputFiles/pos";
  bootstrappingDirNeg = "outputFiles/neg";

  std::cout << "Starting bootstraping" << std::endl;
  std::cout << "Detection file: " << detectionFilePath<< std::endl;
  std::cout << "Ground truth file: " << groundTruthFilePath << std::endl;
  std::cout << "Input images dir: " << imagesPreBootstrappingDir << std::endl << std::endl;

  std::cout << "Reading in detections" << std::endl;
  clock_t start = clock();
  std::ifstream file(detectionFilePath);
  std::string str; 

  std::string frameNaming;

  while (std::getline(file, str))
  {
	std::istringstream ss(str);
	std::string token;
	std::vector<std::string> tokens;

	while(std::getline(ss, token, ';')) {
		tokens.push_back(token);
	}
	detectionStruct currentDetection;

	int num;
	
  	//std::sscanf( char(token[0]), "%*[^_]_%d", &num );

	std::string delimiter = "--";
	std::string isolatedFrameNumber = tokens[0].substr(tokens[0].find(delimiter) + delimiter.length());
	frameNaming = tokens[0].substr(0,tokens[0].find(delimiter));
	//std::cout << frameNaming << std::endl;
	isolatedFrameNumber = isolatedFrameNumber.substr(0,isolatedFrameNumber.find(".png"));
	//std::cout << isolatedFrameNumber << std::endl;

	currentDetection.frameNumber = stoi(isolatedFrameNumber);
	currentDetection.topLeftX = stoi(tokens[1]);
	currentDetection.topLeftY = stoi(tokens[2]);
	currentDetection.botRightX = stoi(tokens[3]);
	currentDetection.botRightY = stoi(tokens[4]);
	currentDetection.score = stoi(tokens[5]);

	workingDetectionStruct.push_back(currentDetection);
  }
  clock_t end = clock();
  double execTime = (double) (end-start) / CLOCKS_PER_SEC * 1000.0;
  std::cout << "Finished in " << execTime << " ms" << std::endl << std::endl;

  std::cout << "Reading in annotations" << std::endl;
  start = clock();
  std::ifstream fileGT(groundTruthFilePath);
  std::getline(fileGT, str); // Skipping first line
  while (std::getline(fileGT, str))
  {
	std::istringstream ss(str);
	std::string token;
	std::vector<std::string> tokensGt;

	while(std::getline(ss, token, ';')) {
		tokensGt.push_back(token);
	}
	annotationStruct currentAnnotation;

	currentAnnotation.frameNumber = stoi(tokensGt[7]);
	currentAnnotation.topLeftX = stoi(tokensGt[2]);
	currentAnnotation.topLeftY = stoi(tokensGt[3]);
	currentAnnotation.botRightX = stoi(tokensGt[4]);
	currentAnnotation.botRightY = stoi(tokensGt[5]);

	workingAnnotationStruct.push_back(currentAnnotation);
  }
  end = clock();
  execTime = (double) (end-start) / CLOCKS_PER_SEC * 1000.0;
  std::cout << "Finished in " << execTime << " ms" << std::endl << std::endl;

  std::vector<int> frameOverviewDT,frameOverviewGT;

  for (int i = 0; i < workingAnnotationStruct.size(); ++i)
  {
  	frameOverviewGT.push_back(workingAnnotationStruct[i].frameNumber);
  }

  for (int i = 0; i < workingDetectionStruct.size(); ++i)
  {
  	frameOverviewDT.push_back(workingDetectionStruct[i].frameNumber);
  }
  //dir = std::string(imagesPreBootstrappingDir);
  getdir(imagesPreBootstrappingDir, files);
  totalNumberOfFrames = numberOfFrames(files);
  int frameDifference = files.size()-totalNumberOfFrames;
  std::cout << "Starting generating results from " << totalNumberOfFrames <<  " frames." << std::endl;
  start = clock();
  for (int i = 0; i < totalNumberOfFrames; ++i)
  {
  	//std::cout << "For frame " << i << ": " << std::endl;
  	std::vector<int> foundFramesInGt = findMatchesInStruct(i,frameOverviewGT);
  	std::vector<int> foundFramesInDt = findMatchesInStruct(i,frameOverviewDT);

  	std::vector<int> workingFoundFramesInGt(foundFramesInGt.size(),-1);
  	std::vector<int> workingFoundFramesInDt(foundFramesInDt.size(),-1);



  	for (int j = 0; j < foundFramesInDt.size(); ++j)
  	{
  		cv::Rect boxOne, boxTwo;
  		boxOne.x = workingDetectionStruct[foundFramesInDt[j]].topLeftX;
  		boxOne.y = workingDetectionStruct[foundFramesInDt[j]].topLeftY;
  		boxOne.width = workingDetectionStruct[foundFramesInDt[j]].botRightX - workingDetectionStruct[foundFramesInDt[j]].topLeftX;
  		boxOne.height = workingDetectionStruct[foundFramesInDt[j]].botRightY - workingDetectionStruct[foundFramesInDt[j]].topLeftY;
  		//std::cout << "x: " << boxOne.x << " y: " << boxOne.y << " width: " << boxOne.width << " height: " << boxOne.height << std::endl; 

  		for (int k = 0; k < foundFramesInGt.size(); ++k)
  		{
			boxTwo.x = workingAnnotationStruct[foundFramesInGt[k]].topLeftX;
		  	boxTwo.y = workingAnnotationStruct[foundFramesInGt[k]].topLeftY;
		  	boxTwo.width = workingAnnotationStruct[foundFramesInGt[k]].botRightX - workingAnnotationStruct[foundFramesInGt[k]].topLeftX ;
		  	boxTwo.height = workingAnnotationStruct[foundFramesInGt[k]].botRightY - workingAnnotationStruct[foundFramesInGt[k]].topLeftY;
						
			float pascal = calcPascalOverlap(boxOne,boxTwo);

			if (pascal >= 0.5)
			{
				//std::cout << pascal << std::endl;
				workingFoundFramesInGt[k] = foundFramesInGt[k];
			}
			if( pascal > 0 ) { // we want to make sure we do not get generate new negatives based on false positives that are partly around a tp.
				//std::cout << pascal << std::endl;
				workingFoundFramesInDt[j] = foundFramesInDt[j];
			}
			
  		}

  	}
  	cv::Mat frame;
  	
  	std::ostringstream oss;
	oss << imagesPreBootstrappingDir << "/" << files[i+frameDifference];
	std::string framePath = oss.str();
 
  	frame = cv::imread(framePath);
  	//std::cout << "Frame path: " << framePath << std::endl;
  	std::cout << "Generating Positives" << std::endl;
  	for (int a = 0; a < workingFoundFramesInGt.size(); ++a)
  	{
  		if (workingFoundFramesInGt[a] == -1)
  		{
  			int workingTopLeftX = workingAnnotationStruct[foundFramesInGt[a]].topLeftX;
  			int workingTopLeftY = workingAnnotationStruct[foundFramesInGt[a]].topLeftY;
  			int workingWidth = workingAnnotationStruct[foundFramesInGt[a]].botRightX - workingAnnotationStruct[foundFramesInGt[a]].topLeftX;
  			int workingHeight = workingAnnotationStruct[foundFramesInGt[a]].botRightY - workingAnnotationStruct[foundFramesInGt[a]].topLeftY;

  			if ( workingTopLeftX >= 0 && workingTopLeftY >= 0 && (workingTopLeftX+workingWidth) < frame.cols && (workingTopLeftY+workingHeight) < frame.rows )
  			{
	  			cv::Mat croppedFrame = frame(cv::Rect(workingTopLeftX, workingTopLeftY, workingWidth,workingHeight));
	  			cv::resize(croppedFrame,croppedFrame,cv::Size(25,25));
	  			std::ostringstream outPosStream;
	  			int v1 = rand() % 1000000000;  
				outPosStream << bootstrappingDirPos << "/" << frameNaming << "--" << v1 << ".png";
				std::string outPosPath = outPosStream.str();
	  			cv::imwrite( outPosPath, croppedFrame );
  			}
  	

  		}
  	}

  	std::cout << "Generating Negatives" << std::endl;
  	for (int a = 0; a < workingFoundFramesInDt.size(); ++a)
  	{
  		if (workingFoundFramesInDt[a] == -1)
  		{
  			int workingTopLeftX = workingDetectionStruct[foundFramesInDt[a]].topLeftX;
  			int workingTopLeftY = workingDetectionStruct[foundFramesInDt[a]].topLeftY;
  			int workingWidth = workingDetectionStruct[foundFramesInDt[a]].botRightX - workingDetectionStruct[foundFramesInDt[a]].topLeftX;
  			int workingHeight = workingDetectionStruct[foundFramesInDt[a]].botRightY - workingDetectionStruct[foundFramesInDt[a]].topLeftY;

  			//std::cout << "Something " << std::endl;
  			if ( workingTopLeftX >= 0 && workingTopLeftY >= 0 && (workingTopLeftX+workingWidth) < frame.cols && (workingTopLeftY+workingHeight) < frame.rows )
  			{
				cv::Mat croppedFrame = frame(cv::Rect(workingDetectionStruct[foundFramesInDt[a]].topLeftX, workingDetectionStruct[foundFramesInDt[a]].topLeftY, workingDetectionStruct[foundFramesInDt[a]].botRightX - workingDetectionStruct[foundFramesInDt[a]].topLeftX , workingDetectionStruct[foundFramesInDt[a]].botRightY - workingDetectionStruct[foundFramesInDt[a]].topLeftY));
	  			cv::resize(croppedFrame,croppedFrame,cv::Size(25,25));
	  			std::ostringstream outNegStream;
	  			int v1 = rand() % 1000000000;  
				outNegStream << bootstrappingDirNeg << "/" << frameNaming << "--" << v1 << ".png";

				std::string outNegPath = outNegStream.str();
	  			cv::imwrite( outNegPath, croppedFrame );
				
  			}
  			


  		}
  	}
  	
  }
  end = clock();
  execTime = (double) (end-start) / CLOCKS_PER_SEC * 1000.0;
  std::cout << "Finished in " << execTime << " ms" << std::endl << std::endl;




  

  return 0;
}
