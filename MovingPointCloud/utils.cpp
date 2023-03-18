#include "utils.hpp"
#include <algorithm>
#include <cstdio>
#include <string>

#include <thread>
#include <future>
#include <random>

void loadPlyFrames(std::string basePath, int startFrame, int numFrames, std::vector<xyzrgb>& frames){
	frames.resize(numFrames);
	
	for(int i = 0; i<numFrames; i++){
		int frameCnt = startFrame + i;
		std::string cnt = std::to_string(frameCnt);
		while(cnt.length() < 4)
			cnt = "0" + cnt;
		std::string path = basePath + cnt + ".ply.bin";
		std::cout << "loading " << path << " ..." << std::endl;

		loadPly(path, frames[i]);
	}
	std::cout << "loading completed." << std::endl;
}

void savePlyFrames(std::string basePath, int startFrame, int numFrames, const std::vector<xyzrgb> &frames){
	for(int i = 0; i<numFrames; i++){
		int frameCnt = startFrame + i;
		std::string cnt = std::to_string(frameCnt);
		while(cnt.length() < 4)
			cnt = "0" + cnt;
		std::string path = basePath + cnt + ".ply.bin";
		std::cout << "saving " << path << " ..." << std::endl;

		savePly(path, frames[i]);
	}
	std::cout << "saving completed." << std::endl;
}

void loadPly_impl(const char* path, xyzrgb& v){
	int numPts;
	FILE* stream = fopen(path, "rb");

	fread(&numPts, sizeof(int), 1, stream);

	v.resize(6*numPts);
	fread(v.data(), sizeof(float) * numPts * 6, 1, stream);

	fclose(stream);
}

void savePly_impl(const char* path, const xyzrgb &v){
	int numPts = v.size()/6;
	FILE* stream = fopen(path, "wb");

	fwrite(&numPts, sizeof(int), 1, stream);
	fwrite(v.data(), sizeof(float) * numPts * 6, 1, stream);

	fclose(stream);
}

void loadPly(std::string path, xyzrgb& v){
	const char* path_ = path.c_str();
	loadPly_impl(path_, v);
}

void savePly(std::string path, const xyzrgb &v){
	const char* path_ = path.c_str();
	savePly_impl(path_, v);
}

//Scale given point cloud frame points
void scaleXYZRGBFrame(xyzrgb& frame, float scale){
	int max = std::thread::hardware_concurrency();

	std::vector<std::future<void>> work;
	work.reserve(max);
	int all = frame.size()/6;
	int num_batches = all/max;

	for(int i = 0; i<max; i++){
		work.push_back(std::async(
			[&frame, scale, i, max, num_batches, all](){
				int length = (max - 1) != i ? num_batches : (all - i*num_batches);

				for(int idx = i*num_batches; idx<i*num_batches + length; idx++){
					frame[6*idx + 0] *= scale;
					frame[6*idx + 1] *= scale;
					frame[6*idx + 2] *= scale;
				}

			}));
	}

	for (auto& item : work)
		item.get();
}

//project point cloud frame (frame) using camera intrinsic parameters (cam)
void projectFrame(xyzrgb& frame, Cam cam){
	int max = std::thread::hardware_concurrency();

	std::vector<std::future<void>> work;
	work.reserve(max);
	int all = frame.size()/6;
	int num_batches = all/max;

	for(int i = 0; i<max; i++){
		work.push_back(std::async(
			[&frame, cam, i, max, num_batches, all](){
				int length = (max - 1) != i ? num_batches : (all - i*num_batches);

				for(int idx = i*num_batches; idx<i*num_batches + length; idx++){
					float x = frame[6*idx + 0];
					float y = frame[6*idx + 1];
					float z = frame[6*idx + 2];
					frame[6*idx + 0] = cam.fx*x/z + cam.cx;
					frame[6*idx + 1] = cam.fy*y/z + cam.cy;
				}

			}));
	}

	for (auto& item : work)
		item.get();
}

//Copy source point cloud frame (source) to target empty point cloud frame (target)
void copyFrame(const xyzrgb &source, xyzrgb &target){
	target.resize(source.size());
	std::copy(source.begin(), source.end(), target.begin());
}

//Transform point cloud frame (frame) points using rotation and translation (rt)
void transfromFrame(xyzrgb &frame, Rt rt){
	int max = std::thread::hardware_concurrency();

	std::vector<std::future<void>> work;
	work.reserve(max);
	int all = frame.size()/6;
	int num_batches = all/max;
	float sx = sin(rt.rx), cx = cos(rt.rx);
	float sy = sin(rt.ry), cy = cos(rt.ry);
	float sz = sin(rt.rz), cz = cos(rt.rz);
	float r00 = cy*cz,	r01 = sx*sy*cz - cx*sz,	r02 = cx*sy*cz + sx*sz;
	float r10 = cy*sz,	r11 = sx*sy*sz + cx*cz,	r12 = cx*sy*sz - sx*cz;
	float r20 = -sy,	r21 = sx*cy,			r22 = cx*cy;

	for(int i = 0; i<max; i++){
		work.push_back(std::async(
			[&frame, rt, r00, r01, r02, r10, r11, r12, r20, r21, r22, i, max, num_batches, all](){
				int length = (max - 1) != i ? num_batches : (all - i*num_batches);

				for(int idx = i*num_batches; idx<i*num_batches + length; idx++){
					float x = frame[6*idx + 0], y = frame[6*idx + 1], z = frame[6*idx + 2];
					frame[6*idx + 0] = r00*x + r01*y + r02*z + rt.tx;
					frame[6*idx + 1] = r10*x + r11*y + r12*z + rt.ty;
					frame[6*idx + 2] = r20*x + r21*y + r22*z + rt.tz;
				}

			}));
	}

	for (auto& item : work)
		item.get();
}

//Draw point cloud frame (frame) into BGR image (outColor) and depth map (outDepth)
void drawFrame(const xyzrgb &frame, cv::Mat &outColor, cv::Mat &outDepth, Cam cam){
	int max = std::thread::hardware_concurrency();
	//max = 1;

	std::vector<std::future<void>> work;
	work.reserve(max);
	int all = frame.size()/6;
	int num_batches = all/max;

	for(int i = 0; i<max; i++){
		work.push_back(std::async(
			[&frame, &outColor, &outDepth, cam, i, max, num_batches, all](){
				int length = (max - 1) != i ? num_batches : (all - i*num_batches);

				for(int idx = i*num_batches; idx<i*num_batches + length; idx++){
					int u = frame[6*idx + 0];
					int v = frame[6*idx + 1];
					float z = frame[6*idx + 2];
					if(u < 0 || u >= outColor.cols || v < 0 || v >= outColor.rows)
						continue;
					if(outDepth.at<float>(v, u) <= z || z < 0.f)
						continue;

					outDepth.at<float>(v, u) = z;
					outColor.at<cv::Vec3b>(v, u)[0] = (uchar)std::min(255.f, 255.f*frame[6*idx + 5]);
					outColor.at<cv::Vec3b>(v, u)[1] = (uchar)std::min(255.f, 255.f*frame[6*idx + 4]);
					outColor.at<cv::Vec3b>(v, u)[2] = (uchar)std::min(255.f, 255.f*frame[6*idx + 3]);
				}

			}));
	}

	for (auto& item : work)
		item.get();
}

//Resample point cloud frame (source) points ramdomly and construct new point cloud frame (target)
void randSampleFrame(const xyzrgb &source, xyzrgb &target, int mod){
	std::vector<int> isSample(source.size()/6, 0);
	int cnt = 0;
	for(int i = 0; i<source.size()/6; i++){
		if(rand()%mod != 0) continue;
		isSample[i] = 1;
		cnt ++;
	}

	target.resize(6*cnt);
	cnt = 0;
	for(int i = 0; i<source.size()/6; i++){
		if(isSample[i] == 0) continue;
		target[6*cnt + 0] = source[6*i + 0];
		target[6*cnt + 1] = source[6*i + 1];
		target[6*cnt + 2] = source[6*i + 2];
		target[6*cnt + 3] = source[6*i + 3];
		target[6*cnt + 4] = source[6*i + 4];
		target[6*cnt + 5] = source[6*i + 5];
		cnt++;
	}
}

//Downsample point cloud frame (source) points and construct new point cloud frame (target)
void downSampleFrame(const xyzrgb &source, xyzrgb &target, int level){
	int cell = 1024/level;
	std::vector<std::vector<std::vector<std::vector<int>>>> vox(cell);
	for(int i = 0; i<vox.size(); i++){
		vox[i].resize(cell);
		for(int j = 0; j<vox[i].size(); j++){
			vox[i][j].resize(cell);
		}
	}

	int cnt = 0;
	std::vector<int> vx, vy, vz;
	for(int i = 0; i<source.size()/6; i++){
		int x = source[6*i + 0]/level;
		int y = source[6*i + 1]/level;
		int z = source[6*i + 2]/level;
		vox[x][y][z].push_back(i);

		if(vox[x][y][z].size() == 1){
			cnt++;
			vx.push_back(x);
			vy.push_back(y);
			vz.push_back(z);
		}
	}
	for(int i = 0; i < vx.size(); i++){
		int x = vx[i], y = vy[i], z = vz[i];
		if(vox[x][y][z].empty()) continue;

		float r = 0.f;
		float g = 0.f;
		float b = 0.f;

		float w = vox[x][y][z].size();
		for(int k = 0; k<vox[x][y][z].size(); k++){
			int idx = vox[x][y][z][k];
			r += source[6*idx + 3];
			g += source[6*idx + 4];
			b += source[6*idx + 5];
		}

		r /= w;
		g /= w;
		b /= w;

		target.push_back(level*x);
		target.push_back(level*y);
		target.push_back(level*z);
		target.push_back(r);
		target.push_back(g);
		target.push_back(b);

		vox[x][y][z].clear();
	}
}

void makePyr(cv::Mat &prev, cv::Mat &prevDepth, cv::Mat &cur, cv::Mat &curDepth){
	int max = std::thread::hardware_concurrency();

	int dx[4] = {0, 1, 0, 1};
	int dy[4] = {0, 0, 1, 1};

	std::vector<std::future<void>> work;
	work.reserve(max);
	int all = cur.rows*cur.cols;
	int num_batches = all/max;

	for(int i = 0; i<max; i++){
		work.push_back(std::async(
			[&cur, &curDepth, &prev, &prevDepth, dx, dy, i, max, num_batches, all](){
				int length = (max - 1) != i ? num_batches : (all - i*num_batches);

				for(int idx = i*num_batches; idx<i*num_batches + length; idx++){
					int x = idx%cur.cols, y = idx/cur.cols;
					for(int j = 0; j<4; j++){
						int xPrev = 2*x+dx[j];
						int yPrev = 2*y+dy[j];
						float z = prevDepth.at<float>(yPrev, xPrev);
						if(z >= curDepth.at<float>(idx)) continue;

						curDepth.at<float>(idx) = z;
						cur.at<cv::Vec3b>(idx) = prev.at<cv::Vec3b>(yPrev, xPrev);
					}
				}

			}));
	}

	for (auto& item : work)
		item.get();
}

//Interpolate BGR image (input) and depth map (inputDepth) into output BGR image (output) and depth map (outputDepth)
void interpolateFrame(const cv::Mat &input, const cv::Mat &inputDepth, cv::Mat &output, cv::Mat &outputDepth, int level){
	int max = std::thread::hardware_concurrency();

	std::vector<cv::Mat> imgPyr(1);
	std::vector<cv::Mat> depthPyr(1);

	input.copyTo(imgPyr[0]);
	inputDepth.copyTo(depthPyr[0]);
	for(int i = 1; i<level; i++){
		cv::Size size = imgPyr[i - 1].size()/2;
		cv::Mat cur = cv::Mat::zeros(size, CV_8UC3);
		cv::Mat curDepth = cv::Mat::ones(size, CV_32FC1)*100.f;
		
		makePyr(imgPyr[i-1], depthPyr[i-1], cur, curDepth);
		imgPyr.emplace_back(cur);
		depthPyr.emplace_back(curDepth);
	}

	std::vector<std::future<void>> work;
	work.reserve(max);
	int all = imgPyr[0].rows*imgPyr[0].cols;
	int num_batches = all/max;

	for(int i = 0; i<max; i++){
		work.push_back(std::async(
			[&imgPyr, &depthPyr, level, i, max, num_batches, all](){
				int length = (max - 1) != i ? num_batches : (all - i*num_batches);

				for(int idx = i*num_batches; idx<i*num_batches + length; idx++){
					int x = idx%imgPyr[0].cols;
					int y = idx/imgPyr[0].cols;
					int xFrom = x;
					int yFrom = y;
					int from;
					int idxFrom;
					float z = 100.f;
					for(int k = 1; k < level; k++){
						from = k;
						xFrom /= 2;
						yFrom /= 2;
						idxFrom = yFrom*imgPyr[k].cols + xFrom;
						if(depthPyr[k].at<float>(idxFrom) < z)
							z = depthPyr[k].at<float>(idxFrom);

						if(abs(z - depthPyr[0].at<float>(idx)) < 0.01f) continue;
						imgPyr[0].at<cv::Vec3b>(idx) = imgPyr[from].at<cv::Vec3b>(idxFrom);
						depthPyr[0].at<float>(idx) = depthPyr[from].at<float>(idxFrom);
					}
				}

			}));
	}

	for (auto& item : work)
		item.get();

	depthPyr[0].copyTo(outputDepth);
	imgPyr[0].copyTo(output);
}

//Generate mask from rendered depth
void genMask(const cv::Mat &inputDepth, cv::Mat &outputMask, float near, float far){
	int max = std::thread::hardware_concurrency();

	std::vector<std::future<void>> work;
	work.reserve(max);
	int all = inputDepth.rows*inputDepth.cols;
	int num_batches = all/max;

	outputMask = cv::Mat::zeros(inputDepth.size(), CV_8U);
	for(int i = 0; i<max; i++){
		work.push_back(std::async(
			[&inputDepth, &outputMask, near, far, i, max, num_batches, all](){
				int length = (max - 1) != i ? num_batches : (all - i*num_batches);

				for(int idx = i*num_batches; idx<i*num_batches + length; idx++){
					if(inputDepth.at<float>(idx) < far)
						outputMask.at<uchar>(idx) = 255;
				}

			}));
	}

	for (auto& item : work)
		item.get();

}

//Fill holes in mask using filtering
void fillMask(const cv::Mat &inputMask, cv::Mat &outputMask, int iter, int radius){
	int max = std::thread::hardware_concurrency();

	std::vector<std::future<void>> work;
	work.reserve(max);
	int all = inputMask.rows*inputMask.cols;
	int num_batches = all/max;

	cv::Mat tempMask;

	inputMask.copyTo(tempMask);
	inputMask.copyTo(outputMask);
	for(int k = 0; k<iter; k++){
		outputMask.copyTo(tempMask);
		outputMask *= 0;

		for(int i = 0; i<max; i++){
			work.push_back(std::async(
				[&tempMask, &outputMask, radius, i, max, num_batches, all](){
					int length = (max - 1) != i ? num_batches : (all - i*num_batches);

					for(int idx = i*num_batches; idx<i*num_batches + length; idx++){
						int x = idx%outputMask.cols;
						int y = idx/outputMask.cols;

						bool isFilled = false;
						for(int h = -radius; h <= radius; h++){
							if(isFilled) break;
							for(int w = -radius; w <= radius; w++){
								if(isFilled) break;
								int x_new = x + w;
								int y_new = y + h;
								if(x_new < 0 || x_new >= tempMask.cols) continue;
								if(y_new < 0 || y_new >= tempMask.rows) continue;
								int idx_new = y_new*tempMask.cols + x_new;
								if(tempMask.at<uchar>(idx_new) != 0) isFilled = true;
							}
						}
						if(isFilled)
							outputMask.at<uchar>(idx) = 255;
					}

				}));
		}

		for (auto& item : work)
			item.get();

		work.clear();
	}
}

//Interpolate BGR image (input) and depth map (inputDepth) into output BGR image (output) and depth map (outputDepth)
void interpolateFrame_bilateral(const cv::Mat &input, const cv::Mat &inputDepth, cv::Mat &output, cv::Mat &outputDepth, const cv::Mat &mask, int radius, float sig, float sigZ){
	int max = std::thread::hardware_concurrency();

	output = cv::Mat::zeros(input.size(), CV_8UC3);
	outputDepth = cv::Mat::zeros(inputDepth.size(), CV_32FC1)*100.f;

	std::vector<std::future<void>> work;
	work.reserve(max);
	int all = input.rows*input.cols;
	int num_batches = all/max;

	for(int i = 0; i<max; i++){
		work.push_back(std::async(
			[&input, &output, &inputDepth, &outputDepth, &mask, radius, sig, sigZ, i, max, num_batches, all](){
				int length = (max - 1) != i ? num_batches : (all - i*num_batches);

				for(int idx = i*num_batches; idx<i*num_batches + length; idx++){
					if(mask.at<uchar>(idx) == 0) continue;

					int x = idx%output.cols;
					int y = idx/output.cols;

					std::vector<float> weights;
					std::vector<cv::Vec3b> colors;
					std::vector<float> depths;
					int reservedLength = 4*radius*radius + 4*radius + 1; 
					weights.reserve(4*radius*radius + 4*radius + 1);
					colors.reserve(4*radius*radius + 4*radius + 1);
					depths.reserve(4*radius*radius + 4*radius + 1);
					float peakDepth = 100.f;

					for(int h = -radius; h <= radius; h++){
						for(int w = -radius; w <= radius; w++){
							int x_new = x + w;
							int y_new = y + h;
							if(x_new < 0 || x_new >= input.cols) continue;
							if(y_new < 0 || y_new >= input.rows) continue;
							int idx_new = y_new*input.cols + x_new;

							if(inputDepth.at<float>(idx_new) >= 99.f) continue;

							peakDepth = std::min(peakDepth, inputDepth.at<float>(idx_new));
							colors.push_back(input.at<cv::Vec3b>(idx_new));
							depths.push_back(inputDepth.at<float>(idx_new));
							float distX = std::abs(w), distY = std::abs(h);
							float distSqr = distX*distX + distY*distY;
							float weight = std::exp(-0.5*(distSqr/(sig*sig)))/(sig*sqrt(2*PI));
							weights.push_back(weight);
						}
					}

					float B = 0.f;
					float G = 0.f;
					float R = 0.f;
					float weight = 0.f;
					float depth = 0.f;

					for(int i = 0; i<weights.size(); i++){
						float zDist = std::abs(peakDepth - depths[i]);
						float w2 = std::exp(-0.5*(zDist*zDist/(sigZ*sigZ)))/(sigZ*sqrt(2*PI));
						B += w2*weights[i]*colors[i][0];
						G += w2*weights[i]*colors[i][1];
						R += w2*weights[i]*colors[i][2];
						depth += w2*weights[i]*depths[i];
						weight += w2*weights[i];
					}

					if(weight < 0.01f) continue;
					B /= weight;
					G /= weight;
					R /= weight;
					depth /= weight;

					output.at<cv::Vec3b>(idx)[0] = B;
					output.at<cv::Vec3b>(idx)[1] = G;
					output.at<cv::Vec3b>(idx)[2] = R;
					outputDepth.at<float>(idx) = depth;
				}

			}));
	}

	for (auto& item : work)
		item.get();
}

void setArgs(std::string& inputPath, std::string& outputPath, int* startFrame, int* numFrame, int* width, int* height, float* f, float* cx, float* cy) {
	std::string kinds[4] = { "longdress", "loot", "redandblack" };
	srand(time(0));
	int kind = rand() % 3;
	int startFrames[4] = { 1051, 1000, 1450,0536 };
	int numFrames[4] = { 300, 300,300,300 };

	inputPath = "E:\\github\\Internship\\pcbinary\\plyDownSampled2\\" + kinds[kind] + "\\" + kinds[kind] + "_";
	outputPath = "..\\result\\" + kinds[kind] + "_";

	*startFrame = startFrames[kind];
	*numFrame = numFrames[kind];
	*width = 960;
	*height = 960;
	*f = 960.0;
	*cx = 480.0;
	*cy = 480.0;
}