/*
PATH ���� : ������Ʈ �Ӽ� -> ���� �Ӽ� -> ����� ȯ�� ->  C:\opencv-source-4.5.5\build\x86\vc15\bin;%PATH% �Է� �ʼ�
Eigen ���� : ������Ʈ �Ӽ� -> ���� �Ӽ� -> VC++ ���丮 -> ���� ���丮 -> C:\opencv-source\eigen-3.4.0; �Է�
*/
#define _CRT_SECURE_NO_DEPRECATE
#include<Eigen/Dense>
#include<opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include<iostream>
#include<algorithm>
int main() {
	cv::Mat img2 = cv::imread("img2.jpg");
	cv::Mat img3 = cv::imread("img3.jpg");

	double homograpy_array[] = // Given Homography Matrix
	{
		1.962,-0.172,-635.756,
		0.557, 1.738, -255.159,
		0.00159, 0 ,1
	};

	// These values are experimentally finded.
	int transx = 719;
	int transy = 256;
	int overlapWidth = 307;

	double homogeneous_transition_array[] =
	{
		1,0,719,
		0,1,256,
		0,0,1
	};
	cv::Mat homogeneous_transition_matrix(3, 3, CV_64F, homogeneous_transition_array);
	cv::Mat homograpy_matrix(3, 3,CV_64F, homograpy_array);
	homograpy_matrix = homogeneous_transition_matrix * homograpy_matrix;
	
	double width = 640.0f;
	double height = 480.0f;
	
	cv::Vec3d p[4];
	p[0] <<  0,0,1 ;
	p[1] << width,0,1 ;
	p[2] << width,height,1 ;
	p[3] << 0,height,1 ;

	double minx,miny,maxx,maxy;
	minx = miny = 987654321.0f;
	maxx = maxy = -1.0f;
	for (int i = 0; i < 4; i++) {
		cv::Mat temp = (homograpy_matrix*p[i]);
		temp /= temp.row(2);
		std::cout << temp << std::endl;
		minx = std::min(minx, temp.at<double>(0, 0));
		miny = std::min(miny, temp.at<double>(1, 0));

		maxx = std::max(maxx, temp.at<double>(0, 0));
		maxy = std::max(maxy, temp.at<double>(1, 0));
	}
	double Pers_width = std::abs(minx - maxx); // 704.923
	double Pers_height = std::abs(miny - maxy); // 834.24

	cv::Mat warp_img2(cv::Size(2000, 2000), img2.type());
	cv::Mat warp_img3(cv::Size(2000, 2000), img3.type());

	cv::warpPerspective(img2, warp_img2, homograpy_matrix, cv::Size(2000,2000));

	cv::Mat mosaiced_img = cv::Mat(cv::Size(2000, 2000), CV_8UC3);
	
	cv::warpPerspective(img3, warp_img3, homogeneous_transition_matrix, cv::Size(2000, 2000));

	cv::Mat mask1 = cv::Mat::zeros(2000, 2000, CV_32F);
	cv::Mat mask2 = cv::Mat::zeros(2000, 2000, CV_32F);

	cv::rectangle(mask1, cv::Rect(0, 0, transx+overlapWidth/2, 2000) , cv::Scalar(1.0f), cv::FILLED);
	cv::rectangle(mask2, cv::Rect(overlapWidth / 2 + transx, transy, 2000 - (overlapWidth / 2 + transx), 2000), cv::Scalar(1.0f), cv::FILLED);


	// Image Mosaicing
	for (int x = transx; x < transx + overlapWidth; x++) {
		for (int y = 0; y < 2000; y++) {
			mask2.at<float>(y, x) = 1.0f / (overlapWidth) * (x - transx);
			mask1.at<float>(y, x) = 1.0f - mask2.at<float>(y, x);
		}
	}

	for (int y = 0; y < 2000; y++) {
		for (int x = 0; x < 2000; x++) {
			if (warp_img2.at<cv::Vec3b>(y, x)[0] == 0 &&
				warp_img2.at<cv::Vec3b>(y, x)[1] == 0 &&
				warp_img2.at<cv::Vec3b>(y, x)[2] == 0) {
				mosaiced_img.at<cv::Vec3b>(y, x) = warp_img3.at<cv::Vec3b>(y, x);
			}
			else if(warp_img3.at<cv::Vec3b>(y, x)[0] == 0 &&
				warp_img3.at<cv::Vec3b>(y, x)[1] == 0 &&
				warp_img3.at<cv::Vec3b>(y, x)[2] == 0)
				mosaiced_img.at<cv::Vec3b>(y, x) = warp_img2.at<cv::Vec3b>(y, x);
			else {
				mosaiced_img.at<cv::Vec3b>(y, x) = warp_img2.at<cv::Vec3b>(y, x)*mask1.at<float>(y, x)
					+ warp_img3.at<cv::Vec3b>(y, x)*mask2.at<float>(y, x);
			}
			
		}
	}



	cv::imshow("mosaiced_img", mosaiced_img);
	cv::waitKey(0);
	return 0;
}