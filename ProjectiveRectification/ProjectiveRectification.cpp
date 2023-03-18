/*
PATH ���� : ������Ʈ �Ӽ� -> ���� �Ӽ� -> ����� -> ȯ�� ->  C:\opencv-source-4.5.5\build\x86\vc15\bin;%PATH% �Է� �ʼ�
Eigen ���� : ������Ʈ �Ӽ� -> ���� �Ӽ� -> VC++ ���丮 -> ���� ���丮 -> C:\opencv-source\eigen-3.4.0; �Է�
*/
// 443 94    -> 443 94
// 563 178   -> 563 94
// 563 628   -> 563 699
// 443 699   -> 443 699

#include<Eigen/Dense>
#include<opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include<iostream>

int main() {
	cv::Mat src_image = cv::imread("corridor.jpg");
	cv::Mat dst_image;

	cv::Point2f src[4], dst[4];
	// Set the points to remove perspective distortion
	src[0] = { 443, 94 };
	src[1] = { 563, 178 };
	src[2] = { 563, 628 };
	src[3] = { 443, 699 };

	// Set the dest points to locate points removed distortion
	dst[0] = { 443, 94 };
	dst[1] = { 563, 94 };
	dst[2] = { 563, 699 };
	dst[3] = { 443, 699 };

	// Function that get Matrix which performs Perspective Transform
	cv::Mat H = cv::getPerspectiveTransform(src, dst);

	// Does remove perspective distortion using Perspective Transform Matrix
	cv::warpPerspective(src_image, dst_image, H, src_image.size()*4);

	cv::imshow("image", dst_image);
	cv::waitKey(0);

	return 0;
}