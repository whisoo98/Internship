#if 0
#include <iostream>
#include <GL/glew.h>
#include <GL/gl.h>
#include <GLFW/glfw3.h>
#include <opencv2/opencv.hpp>
#include <opencv2/aruco.hpp>

const float width = 640.f;
const float height = 480.f;
const float near = 0.1f;
const float far = 1000.f;
float camMat_[9] = { 640.f, 0.f, 320.f, 0.f, 640.f, 240.f, 0.f, 0.f, 1.f };
float distCoeffs_[5] = { 0.f, 0.f, 0.f, 0.f, 0.f };
cv::Mat camMat(3, 3, CV_32FC1, camMat_);
cv::Mat distCoeffs(1, 5, CV_32FC1, distCoeffs_);
float projMat[16];
float* modelViewMat;
cv::Mat frame;
unsigned int texture;
cv::Mat markerImg;
cv::Ptr<cv::aruco::Dictionary> dict = cv::aruco::getPredefinedDictionary(cv::aruco::DICT_6X6_100);
std::vector<int> ids;
std::vector<std::vector<cv::Point2f>> corners;
std::vector<cv::Vec3d> rvecs, tvecs;

float deg0;
float deg1;


void calcModelViewMat() {
	corners.clear(); ids.clear(); rvecs.clear(); tvecs.clear();
	cv::Mat P = cv::Mat::eye(4, 4, CV_32FC1);

	cv::aruco::detectMarkers(frame, dict, corners, ids);
	if (ids.size() == 0) return;

	cv::aruco::estimatePoseSingleMarkers(corners, 1.f, camMat, distCoeffs, rvecs, tvecs);
	cv::Mat R;
	cv::Rodrigues(rvecs[0], R);

	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 3; j++)
			P.at<float>(i, j) = R.at<double>(i, j);
		P.at<float>(i, 3) = tvecs[0][i];
	}
	P.row(1) = -P.row(1);
	P.row(2) = -P.row(2);
	P = P.t();
	modelViewMat = (float*)P.data;
}

void drawFarPlane(float zfar) {
	glColor3f(1.f, 1.f, 1.f);

	float xx = (width >= height) ? 0.5f : 0.5*width / height;
	float yy = (width >= height) ? 0.5*height / width : 0.5f;
	xx *= zfar;
	yy *= zfar;

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_BGR_EXT, GL_UNSIGNED_BYTE, frame.data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	glBegin(GL_QUADS);

	glTexCoord2f(1, 0);
	glVertex3f(-xx, -yy, zfar);

	glTexCoord2f(0, 0);
	glVertex3f(xx, -yy, zfar);

	glTexCoord2f(0, 1);
	glVertex3f(xx, yy, zfar);

	glTexCoord2f(1, 1);
	glVertex3f(-xx, yy, zfar);

	glEnd();
	glDisable(GL_TEXTURE_2D);
}

void drawQuad(float scale, float r, float g, float b) {
	glColor3f(r, g, b);

	glBegin(GL_QUADS);
	glVertex3f(-0.5f*scale, -0.5f*scale, 0);
	glVertex3f(0.5f*scale, -0.5f*scale, 0);
	glVertex3f(0.5f*scale, 0.5f*scale, 0);
	glVertex3f(-0.5f*scale, 0.5f*scale, 0);
	glEnd();
}

void drawObjects() {
	drawQuad(1.f, 1.f, 0.f, 0.f);
	{
		glPushMatrix();
		glRotatef(deg0, 0.f, 0.f, 1.f);
		glTranslatef(2.0f, 0.f, 0.f);
		drawQuad(0.5f, 0.f, 1.f, 0.f);
		{
			glPushMatrix();
			glRotatef(deg1, 0.f, 0.f, 1.f);
			glTranslatef(1.0f, 0.f, 0.f);
			drawQuad(0.2f, 0.f, 0.f, 1.f);
			glPopMatrix();
		}
		glPopMatrix();
	}
}

void Render() {
	glClearColor(0.f, 0.f, 0.f, 1.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf(projMat);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	drawFarPlane(-far);

	glPushMatrix();
	glLoadMatrixf(modelViewMat);
	drawObjects();
	glPopMatrix();
}

void setProjMatrix(cv::Mat camMat, float width, float height, float near, float far, float* mat) {
	float fx = camMat.at<float>(0, 0);
	float fy = camMat.at<float>(1, 1);
	float cx = camMat.at<float>(0, 2);
	float cy = camMat.at<float>(1, 2);

	float left = -cx * near / fx;
	float right = (width - cx)*near / fx;
	float top = cy * near / fy;
	float bottom = -(height - cy)*near / fy;

	mat[0] = 2 * near / (right - left);
	mat[5] = 2 * near / (top - bottom);
	mat[8] = (left + right) / (right - left);
	mat[9] = (top + bottom) / (top - bottom);
	mat[10] = (far + near) / (near - far);
	mat[11] = -1.f;
	mat[14] = 2 * near*far / (near - far);
}

void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if (key == GLFW_KEY_ESCAPE && action == GLFW_RELEASE)
		glfwSetWindowShouldClose(window, true);
}

int main() {
	glfwInit();
	GLFWwindow* window = glfwCreateWindow((int)width, (int)height, "result", NULL, NULL);
	glfwMakeContextCurrent(window);
	glfwSetKeyCallback(window, KeyCallback);
	glfwSwapInterval(1);
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);

	setProjMatrix(camMat, width, height, near, far, projMat);
	glGenTextures(1, &texture);
	cv::VideoCapture cap(0);

	while (!glfwWindowShouldClose(window)) {
		cap.read(frame);
		calcModelViewMat();
		Render();
		glfwPollEvents();
		glfwSwapBuffers(window);
		deg0 += 1.f;
		deg1 += 10.f;
	}

}
#endif