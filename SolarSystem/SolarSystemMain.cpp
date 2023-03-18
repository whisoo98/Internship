/*
PATH : 프로젝트속성 -> 구성속성 -> 디버깅 -> 환경에 추가  C:\opencv-source-4.5.5\build\x86\vc15\bin;%PATH%
*/
#include<Eigen/Dense>
#include<opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include "utils.hpp"


int main(int argc,char *argv[]) {
	glfwSetErrorCallback(ErrorCallback);
	if (!glfwInit()) {
		std::cerr << "Failed to Initialize GLFW." << std::endl;
		return -1;
	}

	GLFWwindow* windowHandler = nullptr;
	windowHandler = glfwCreateWindow(1000, 500, "Hello OpenGL!", NULL, NULL);
	if (windowHandler == NULL) {
		glfwTerminate();
		std::cerr << "Failed to Create GLFW Window." << std::endl;
		return -1;
	}
	glfwSetKeyCallback(windowHandler, KeyCallback);
	glfwMakeContextCurrent(windowHandler);
	glfwSwapInterval(1);

	glInit();
	//Cube::filled = 0;
	while (!glfwWindowShouldClose(windowHandler)) {
		glfwPollEvents();
		Render(windowHandler);
	}
	return 0;
}