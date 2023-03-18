#if 0
#include <opencv2/opencv.hpp>
#include "utils.hpp"

int main(int argc, char* argv[]){
	std::string inputPath = argv[1];
	std::string outputPath = argv[2];
	int startFrame = std::atoi(argv[3]);
	int numFrames = std::atoi(argv[4]);
	
	std::vector<xyzrgb> frames;
	loadPlyFrames(outputPath, startFrame, numFrames, frames);

	std::vector<xyzrgb> framesSampled(frames.size());
	for(int i = 0; i<frames.size(); i++){
		downSampleFrame(frames[i], framesSampled[i], 4);
		std::cout << "frame " << i << " is processed." << std::endl;
	}
	savePlyFrames(outputPath, startFrame, numFrames, framesSampled);

	return 0;
}
#endif