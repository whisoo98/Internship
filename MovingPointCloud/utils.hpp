#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include "opencv2/opencv.hpp"

#define PI 3.14159265358979

//Point Cloud Frame
typedef std::vector<float> xyzrgb;

//Rotation and Translation values
struct Rt{
	float rx, ry, rz, tx, ty, tz;
};

//Camera intrinsic parameters
struct Cam{
	int w, h;				//Image plane size
	float fx, fy, cx, cy;	//focal lengths and principle points
};

void makePyr(cv::Mat &prev, cv::Mat &prevDepth, cv::Mat &cur, cv::Mat &curDepth);

//Point Cloud I/O
void loadPly(std::string path, xyzrgb& v);
void savePly(std::string path, const xyzrgb& v);
void loadPlyFrames(std::string basePath, int startFrame, int numFrames, std::vector<xyzrgb>& frames);
void savePlyFrames(std::string basePath, int startFrame, int numFrames, const std::vector<xyzrgb>& frames);

//Point Cloud Frame Processing Functions
void scaleXYZRGBFrame(xyzrgb& frame, float scale);
void transfromFrame(xyzrgb &frame, Rt rt);
void projectFrame(xyzrgb &frame, Cam cam);
void copyFrame(const xyzrgb &source, xyzrgb &target);
void drawFrame(const xyzrgb &frame, cv::Mat &outColor, cv::Mat &outDepth, Cam cam);
void randSampleFrame(const xyzrgb &source, xyzrgb &target, int mod);
void downSampleFrame(const xyzrgb &source, xyzrgb &target, int level);

//Image Processing Functions
void interpolateFrame(const cv::Mat &input, const cv::Mat &inputDepth, cv::Mat &output, cv::Mat &outputDepth, int level);
void interpolateFrame_bilateral(const cv::Mat &input, const cv::Mat &inputDepth, cv::Mat &output, cv::Mat &outputDepth, const cv::Mat &mask, int radius, float sig, float sigZ);
void genMask(const cv::Mat &inputDepth, cv::Mat &outputMask, float near, float far);
void fillMask(const cv::Mat &inputMask, cv::Mat &outputMask, int iter, int radius);

void setArgs(std::string& inputPath, std::string& outputPath, int* startFrame, int* sumFrames, int* width, int* height, float* f, float* cx, float* cy);