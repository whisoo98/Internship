#include <opencv2/imgcodecs.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include <string>
#include "utils.hpp"

int main(int argc, char* argv[]) {
	/*std::string inputPath = argv[1];
	std::string outputPath = argv[2];
	int startFrame = std::atoi(argv[3]);
	int numFrames = std::atoi(argv[4]);
	int width = std::atoi(argv[5]);
	int height = std::atoi(argv[6]);
	float f = std::atof(argv[7]);
	float cx = std::atof(argv[8]);
	float cy = std::atof(argv[9]);*/

	std::string inputPath;
	std::string outputPath;
	int startFrame;
	int numFrames;
	int width;
	int height;
	float f;
	float cx;
	float cy;

	setArgs(inputPath, outputPath, &startFrame, &numFrames, &width, &height, &f, &cx, &cy);
	std::vector<xyzrgb> frames;
	loadPlyFrames(inputPath, startFrame, numFrames, frames);

	for (auto& frame : frames)
		scaleXYZRGBFrame(frame, 1.f / 1024.f);

	Cam cam = { width, height, f, f, cx, cy };
	Rt rt = { PI, 0.f, 0.f, -0.25f, 0.5f, 1.6f };
	xyzrgb temp;
	cv::Mat res = cv::Mat::zeros(cam.h, cam.w, CV_8UC3);
	cv::Mat resDepth = cv::Mat::ones(cam.h, cam.w, CV_32FC1)*(100.f);
	cv::Mat show;
	float moved = 0.01;
	float rotated = 0.1;
	for (int i = 0; i < frames.size(); i++) {
		res *= 0;
		resDepth *= 0;
		resDepth += 100.f;

		//Copy point cloud frames into temp
		copyFrame(frames[i], temp);
		//Transform 3d points in temp point cloud using rt (rotation&translation)
		transfromFrame(temp, rt);
		//Project 3d points in temp point cloud into 2d image plane
		projectFrame(temp, cam);
		//Draw BGR image and depth map from projected points (temp point cloud)
		drawFrame(temp, res, resDepth, cam);

		cv::Mat inter, interDepth;

		bool usePyramid = true;
		if (usePyramid) {
			//Interpolate image to fill uncovered holes (Pyramid Method)
			interpolateFrame(res, resDepth, inter, interDepth, 3);
		}
		else {
			cv::Mat mask;
			genMask(resDepth, mask, 0.f, 100.f);
			fillMask(mask, mask, 2, 1);

			//Interpolate image to fill uncovered holes (Bilateral Method)
			interpolateFrame_bilateral(res, resDepth, inter, interDepth, mask, 3, 1.0f, 0.005f);
		}

		//Show raw image (left) and interpolated image (right)
		cv::hconcat(res, inter, show);
		cv::imshow("Result, raw (left) and interpolated (right) images.", show);

		//Get input key code
		int key = cv::waitKeyEx(1);
		int closed = 0;
		//If Esc is pressed, break the loop
		//if(key == 27) break;
		if (key == 27) {
			closed = 1;
		}
		else if (key == 0x250000) // Left Arrow ←
			rt.tx -= moved;
		else if (key == 0x270000) // Right Arrow →  
			rt.tx += moved;
		else if (key == 0x260000) // Up Arrow ↑
			rt.ty -= moved;
		else if (key == 0x280000) // Down Arrow ↓
			rt.ty += moved;
		else if (key == '-') // Closed to Display
			rt.tz += moved; 
		else if (key == '+') // Far to Display
			rt.tz -= moved;
		else if (key == '<') 
			rt.rz -= rotated;
		else if (key == '>')
			rt.rz += rotated;
		else if (key == 'l')
			rt.ry += rotated;
		else if (key == 'r')
			rt.ry -= rotated;
		else if (key == 'u')
			rt.rx -= rotated;
		else if (key == 'b')
			rt.rx += rotated;
		
		if (closed) break;
		if (i == frames.size() - 1) i = 0;
	}

	return 0;
}
