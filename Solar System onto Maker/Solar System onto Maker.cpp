/*
PATH ���� : ������Ʈ �Ӽ� -> ���� �Ӽ� -> ����� ȯ�� ->  C:\opencv-source-4.5.5\build\x86\vc15\bin;%PATH% �Է� �ʼ�
Eigen ���� : ������Ʈ �Ӽ� -> ���� �Ӽ� -> VC++ ���丮 -> ���� ���丮 -> C:\opencv-source\eigen-3.4.0; �Է�
*/
#include<Eigen/Dense>
#include<opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/aruco.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/calib3d.hpp>
#include<iostream>
#include<GL/glew.h>
#include<GLFW/glfw3.h>
#include<GL/GL.h>
#include"Sphere.h"

float width = 640;
float height = 480;
float degreeEarth = 0;
float degreeMoon = 0;
float back = 0;
cv::VideoCapture in_video;

void ErrorCallback(int error, const char* description) {
	std::cerr << "Error: " << description << std::endl;
}
void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if (key == GLFW_KEY_ESCAPE && action == GLFW_RELEASE) {
		glfwSetWindowShouldClose(window, true);
	}
	else if (key == GLFW_KEY_UP && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
		back += 1;
		std::cout << "back " << back << std::endl;
	}
	else if (key == GLFW_KEY_DOWN && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
		back -= 1;
		std::cout << "back " << back << std::endl;
	}
}

void glInit() {
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
}
void DrawLine(int width, int depth, int height, GLfloat red, GLfloat green, GLfloat blue) {
	glColor3f(red, green, blue);
	glBegin(GL_LINES);

	//glBegin(GL_POINTS);

	/*
			_______1_______
		   /|             /|
		  / |            / |
		 2 	5           3  |
		/   |          /   8
	   /____|__4______/    |
	   |    |         |    |
	   |    |____12___|____|
	   |    /         |    /
	   6   /          7   /
	   |  9           |  11
	   | /            | /
	   |/_____10______|/


	*/

	// 1
	glVertex3f(-width / 2.0f, depth / 2.0f, -height / 2.0f);
	glVertex3f(width / 2.0f, depth / 2.0f, -height / 2.0f);

	// 2
	glVertex3f(-width / 2.0f, depth / 2.0f, -height / 2.0f);
	glVertex3f(-width / 2.0f, depth / 2.0f, height / 2.0f);

	// 3
	glVertex3f(width / 2.0f, depth / 2.0f, -height / 2.0f);
	glVertex3f(width / 2.0f, depth / 2.0f, height / 2.0f);

	// 4
	glVertex3f(-width / 2.0f, depth / 2.0f, height / 2.0f);
	glVertex3f(width / 2.0f, depth / 2.0f, height / 2.0f);

	// 5
	glVertex3f(-width / 2.0f, depth / 2.0f, -height / 2.0f);
	glVertex3f(-width / 2.0f, -depth / 2.0f, -height / 2.0f);

	// 6
	glVertex3f(-width / 2.0f, depth / 2.0f, height / 2.0f);
	glVertex3f(-width / 2.0f, -depth / 2.0f, height / 2.0f);

	// 7
	glVertex3f(width / 2.0f, depth / 2.0f, height / 2.0f);
	glVertex3f(width / 2.0f, -depth / 2.0f, height / 2.0f);

	// 8
	glVertex3f(width / 2.0f, depth / 2.0f, -height / 2.0f);
	glVertex3f(width / 2.0f, -depth / 2.0f, -height / 2.0f);

	// 9
	glVertex3f(-width / 2.0f, -depth / 2.0f, -height / 2.0f);
	glVertex3f(-width / 2.0f, -depth / 2.0f, height / 2.0f);

	// 10
	glVertex3f(-width / 2.0f, -depth / 2.0f, height / 2.0f);
	glVertex3f(width / 2.0f, -depth / 2.0f, height / 2.0f);

	// 11
	glVertex3f(width / 2.0f, -depth / 2.0f, -height / 2.0f);
	glVertex3f(width / 2.0f, -depth / 2.0f, height / 2.0f);

	// 12
	glVertex3f(-width / 2.0f, -depth / 2.0f, -height / 2.0f);
	glVertex3f(width / 2.0f, -depth / 2.0f, -height / 2.0f);

	glEnd();
	glFlush();
}

void DrawQuad(float width, float height, float far, GLfloat red, GLfloat green, GLfloat blue) {
	glColor3f(red, green, blue);
	//glTranslatef(0, 0, back);
	glBegin(GL_QUADS);
	glTexCoord2f(0, 1);
	glVertex3f(-far, -far * height / width, far);
	
	glTexCoord2f(1, 1);
	glVertex3f(far, -far * height / width, far);
	
	glTexCoord2f(1, 0);
	glVertex3f(far, far * height / width, far);
	
	glTexCoord2f(0, 0);
	glVertex3f(-far, far * height / width, far);
	glEnd();
	glFlush();
}

void DrawSolarSystem(int scale) {
	Sphere Sun(5*scale, 36, 18), Earth(3*scale, 36, 18), Moon(1*scale, 36, 18);
	
	glColor3f(1, 0, 0);
	Sun.draw();
	glPushMatrix();
	{
		glColor3f(0, 0, 1);
		glRotatef(degreeEarth, 0.0, 0.0, 1.0);
		glTranslated(20.0, 0.0, 0.0);
		degreeEarth = ((int)degreeEarth + 1) % 360;
		Earth.draw();
		glPushMatrix();
		{
			glColor3f(1, 1, 0);
			glRotatef(degreeMoon, 0.0, 0.0, 1.0);
			glTranslated(10.0, 0.0, 0.0);
			degreeMoon = ((int)degreeMoon + 360/30) % 360;
			Moon.draw();
		}
		glPopMatrix();
	}
	glPopMatrix();
}

void glTextureMapping(GLuint* tex,cv::Mat& image, float width, float height, float far) {
	
	glColor3f(1, 1, 1);
	float xx = (width >= height) ? 0.5f : 0.5*width / height;
	float yy = (width >= height) ? 0.5*height / width : 0.5f;

	far = -far;
	xx *= far;
	yy *= far;


	glEnable(GL_TEXTURE_2D);
	glGenTextures(1, tex);
	glBindTexture(GL_TEXTURE_2D, *tex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image.cols, image.rows, 0, GL_BGR_EXT, GL_UNSIGNED_BYTE, image.data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glBegin(GL_QUADS);
	
	glTexCoord2f(1, 0);
	glVertex3f(-xx, -yy, far);

	glTexCoord2f(0, 0);
	glVertex3f(xx, -yy, far);

	glTexCoord2f(0, 1);
	glVertex3f(xx, yy, far);

	glTexCoord2f(1, 1);
	glVertex3f(-xx, yy, far);

	glEnd();
	glDisable(GL_TEXTURE_2D);
}
void setProjMatrix(cv::Mat cameraMatrix, float width, float height, float near, float far, float* myProj) {
	float fx = cameraMatrix.at<float>(0, 0);
	float fy = cameraMatrix.at<float>(1, 1);
	float cx = cameraMatrix.at<float>(0, 2);
	float cy = cameraMatrix.at<float>(1, 2);
	float left = (-cx * near) / fx;
	float right = (width - cx)*near / fx;
	float top = cy * near / fy;
	float bottom = -(height - cy)*near / fy;
	for (int i = 0; i < 16; i++) myProj[i] = 0;

	myProj[0] = 2 * near / (right - left);
	myProj[5] = 2 * near / (top - bottom);
	myProj[8] = (left + right) / (right - left);
	myProj[9] = (top + bottom) / (top - bottom);
	myProj[10] = (far + near) / (near - far);
	myProj[11] = -1.0;
	myProj[14] = 2 * near*far / (near - far);
}
void Render(GLFWwindow* window) {
	int scale = 10;
	cv::Mat ProjMat = cv::Mat::zeros(4, 4, CV_32F);
	cv::Ptr<cv::aruco::Dictionary> dict = cv::aruco::getPredefinedDictionary(cv::aruco::DICT_6X6_250);

	cv::FileStorage fs("calibration.yml", cv::FileStorage::READ);
	cv::Mat cameraMatrix = cv::Mat::zeros(3, 3, CV_32F), distCoeffs = cv::Mat::zeros(3, 3, CV_32F);
	fs["camera_matrix"] >> cameraMatrix;
	fs["distortion_coefficients"] >> distCoeffs;

	std::vector<int> ids;
	std::vector<std::vector<cv::Point2f>> corners;
	std::vector< cv::Vec3d > rvecs, tvecs;
	cv::Mat rmat;
	cv::Mat H = cv::Mat::zeros(4, 4, CV_32F);

	cameraMatrix.at<float>(0, 0) = 640.0;
	cameraMatrix.at<float>(1, 1) = 640.0;
	cameraMatrix.at<float>(0, 2) = 320.0;
	cameraMatrix.at<float>(1, 2) = 240.0;
	float near = 0.1;
	float far = 1000.0;
	float myProj[16];
	setProjMatrix(cameraMatrix, width, height, near, far, myProj);

	GLuint tex;
	cv::Mat image;

	in_video.grab();
	in_video.retrieve(image);

	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	glClearColor(0, 0, 0, 1);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glLoadMatrixf(myProj);

	cv::aruco::detectMarkers(image, dict, corners, ids);
	if (ids.size() > 0) {
		cv::aruco::estimatePoseSingleMarkers(corners, scale, cameraMatrix, distCoeffs, rvecs, tvecs);
		for (int i = 0; i < ids.size(); i++) {
			cv::aruco::drawAxis(image, cameraMatrix, distCoeffs, rvecs[i], tvecs[i], 0.1);
		}
	}

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTextureMapping(&tex, image, width, height, far);

	if (ids.size() > 0) {
		// Homogeneous Mat setting
		cv::Rodrigues(rvecs[0], rmat);
		for (int i = 0; i < 3; i++) {
			for (int j = 0; j < 3; j++) {
				H.at<float>(i, j) = (float)rmat.at<double>(i, j);
			}
			H.at<float>(i, 3) = tvecs[0][i];
		}
		H.at<float>(3, 3) = 1;
		H.row(1) = -H.row(1);
		H.row(2) = -H.row(2);
		std::cout << H << std::endl;
		H = H.t();

		float* myModel = (float*)H.data;
		glPushMatrix();
		{
			glLoadMatrixf(myModel);
			DrawSolarSystem(scale/10);
		}
		glPopMatrix();
	}
	
	glFlush();
	glfwSwapBuffers(window);

}
int main(int argc,char *argv[]) {
	glfwSetErrorCallback(ErrorCallback);
	if (!glfwInit()) {
		std::cerr << "Failed to Initialize GLFW." << std::endl;
		return -1;
	}

	GLFWwindow* windowHandler = nullptr;
	windowHandler = glfwCreateWindow((int)width, (int)height, "Hello OpenGL!", NULL, NULL);
	if (windowHandler == NULL) {
		glfwTerminate();
		std::cerr << "Failed to Create GLFW Window." << std::endl;
		return -1;
	}
	glfwMakeContextCurrent(windowHandler);
	glfwSwapInterval(1);
	glfwSetKeyCallback(windowHandler, KeyCallback);

	glInit();
	in_video.open(0);

	/*
	������ ���
	3. Point Cloud ���� ����

	
	*/
	
	//glDisable(GL_TEXTURE_2D);
	while (!glfwWindowShouldClose(windowHandler)) {
		Render(windowHandler);
		//glfwSwapBuffers(windowHandler);
		glfwPollEvents();
	}
	return 0;
}