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

int main(int argc, char* argv[]) {
  if (argc < 3) {
    help();
  }
  detectionFilePath = argv[1];
  groundTruthFilePath = argv[2];
  imagesPreBootstrappingDir = argv[3];

  bootstrappingDirPos = "outputImages/positives";
  bootstrappingDirNeg = "outputImages/negatives";

  std::cout << "Starting bootstraping" << std::endl;
  std::cout << "Detection file: " << detectionFilePath << std::endl;
  std::cout << "Ground truth file: " << groundTruthFilePath << std::endl;
  std::cout << "Input images dir: " << imagesPreBootstrappingDir << std::endl;

  dir = std::string(imagesPreBootstrappingDir);

  getdir(dir, files);
  totalNumberOfFrames = numberOfFrames(files);
  for (size_t i = 0; i < totalNumberOfFrames; i++) {
    std::cout << "File: " << files[i] << std::endl;
    
  }
  return 0;
}
