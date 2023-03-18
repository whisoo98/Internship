/*
 * Copyright (c) 2019 Flight Dynamics and Control Lab
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 */
#pragma warning(disable : 4996)
#include <opencv2/opencv.hpp>
#include <opencv2/aruco.hpp>
#include <iostream>
#include <cstdlib>
#include "aruco_samples_utility.hpp"

using namespace cv;

namespace {
	const char* about = "Detect ArUco marker images";
	const char* markerkeys =
		"{d        | 10    | dictionary: DICT_4X4_50=0, DICT_4X4_100=1, "
		"DICT_4X4_250=2, DICT_4X4_1000=3, DICT_5X5_50=4, DICT_5X5_100=5, "
		"DICT_5X5_250=6, DICT_5X5_1000=7, DICT_6X6_50=8, DICT_6X6_100=9, "
		"DICT_6X6_250=10, DICT_6X6_1000=11, DICT_7X7_50=12, DICT_7X7_100=13, "
		"DICT_7X7_250=14, DICT_7X7_1000=15, DICT_ARUCO_ORIGINAL = 16}"
		"{h        | false | Print help }"
		"{v        | <none>| Custom video source, otherwise '0' }"
		"{@outfile | <none>| Output image }"
		"{cd       |       | Input file with custom dictionary }"
		"{id       | 23    | Marker id in the dictionary }"
		"{ms       | 200   | Marker size in pixels }"
		"{bb       | 1     | Number of bits in marker borders }"
		"{si       | true  | show generated image }";
	const char* calibrationkeys =
		"{w        | 5     | Number of squares in X direction }"
		"{h        | 7     | Number of squares in Y direction }"
		"{l        | 100   | Marker side length (in meters) }"
		"{s        | 10    | Separation between two consecutive markers in the grid (in meters) }"
		"{d        | 10    | dictionary: DICT_4X4_50=0, DICT_4X4_100=1, DICT_4X4_250=2,"
		"DICT_4X4_1000=3, DICT_5X5_50=4, DICT_5X5_100=5, DICT_5X5_250=6, DICT_5X5_1000=7, "
		"DICT_6X6_50=8, DICT_6X6_100=9, DICT_6X6_250=10, DICT_6X6_1000=11, DICT_7X7_50=12,"
		"DICT_7X7_100=13, DICT_7X7_250=14, DICT_7X7_1000=15, DICT_ARUCO_ORIGINAL = 16}"
		"{cd       |       | Input file with custom dictionary }"
		"{@outfile |<none> | Output file with calibrated camera parameters }"
		"{v        |       | Input from video file, if ommited, input comes from camera }"
		"{ci       | 0     | Camera id if input doesnt come from video (-v) }"
		"{dp       |       | File of marker detector parameters }"
		"{rs       | false | Apply refind strategy }"
		"{zt       | false | Assume zero tangential distortion }"
		"{a        |       | Fix aspect ratio (fx/fy) to this value }"
		"{pc       | false | Fix the principal point at the center }";
}


int main(int argc, char **argv)
{
	/* marker creatation */

	CommandLineParser parser(argc, argv, markerkeys);
	parser.about(about);

	/*if (argc < 4) {
		parser.printMessage();
		return 0;
	}*/

	int markerId = parser.get<int>("id");
	int borderBits = parser.get<int>("bb");
	int markerSize = parser.get<int>("ms");
	bool showImage = parser.get<bool>("si");

	if (!parser.check()) {
		parser.printErrors();
		return 0;
	}

	Ptr<aruco::Dictionary> dictionary;
	if (parser.has("d")) {
		int dictionaryId = parser.get<int>("d");
		dictionary = aruco::getPredefinedDictionary(aruco::PREDEFINED_DICTIONARY_NAME(dictionaryId));
	}
	else if (parser.has("cd")) {
		FileStorage fs(parser.get<std::string>("cd"), FileStorage::READ);
		bool readOk = aruco::Dictionary::readDictionary(fs.root(), dictionary);
		if (!readOk) {
			std::cerr << "Invalid dictionary file" << std::endl;
			return 0;
		}
	}
	else {
		std::cerr << "Dictionary not specified" << std::endl;
		return 0;
	}

	Mat markerImg;
	aruco::drawMarker(dictionary, markerId, markerSize, markerImg, borderBits);

	if (showImage) {
		//imshow("marker", markerImg);
		waitKey(0);
	}


	/* marker detection */

	if (parser.get<bool>("h")) {
		parser.printMessage();
		return 0;
	}

	int dictionaryId = parser.get<int>("d");
	int wait_time = 10;
	cv::String videoInput = "0";
	cv::VideoCapture in_video;
	if (parser.has("v")) {
		videoInput = parser.get<cv::String>("v");
		if (videoInput.empty()) {
			parser.printMessage();
			return 1;
		}
		char* end = nullptr;
		int source = static_cast<int>(std::strtol(videoInput.c_str(), &end, \
			10));
		if (!end || end == videoInput.c_str()) {
			in_video.open(videoInput); // url
		}
		else {
			in_video.open(source); // id
		}
	}
	else {
		in_video.open(0);
	}

	if (!parser.check()) {
		parser.printErrors();
		return 1;
	}
	if (!in_video.isOpened()) {
		std::cerr << "failed to open video input: " << videoInput << std::endl;
		return 1;
	}
	Mat cameraMatrix, distCoeffs;
	readCameraParameters("calibration.yml", cameraMatrix, distCoeffs);

	while (in_video.grab()) {
		
		cv::Mat image, image_copy;
		in_video.retrieve(image);
		image.copyTo(image_copy);

		std::vector<int> ids;
		std::vector<std::vector<cv::Point2f>> corners;
		cv::aruco::detectMarkers(image, dictionary, corners, ids);

		// If at least one marker detected
		if (ids.size() > 0) {
			cv::aruco::drawDetectedMarkers(image_copy, corners, ids);
			std::vector< cv::Vec3d > rvecs, tvecs;
			cv::aruco::estimatePoseSingleMarkers(corners, 0.05, cameraMatrix, distCoeffs, rvecs, tvecs);

			for (int i = 0; i < ids.size(); i++) {
				cv::aruco::drawAxis(image_copy, cameraMatrix, distCoeffs, rvecs[i], tvecs[i], 0.1);
				std::cout << rvecs[i] << std::endl;
				std::cout << tvecs[i] << std::endl;
			}

		}
		cv::imshow("Detected markers", image_copy);
		char key = (char)cv::waitKey(wait_time);
		if (key == 27)
			break;
	}

	in_video.release();

	return 0;
}