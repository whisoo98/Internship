/*
PATH ���� : ������Ʈ �Ӽ� -> ���� �Ӽ� -> ����� ȯ�� ->  C:\opencv-source-4.5.5\build\x86\vc15\bin;%PATH% �Է� �ʼ�
Eigen ���� : ������Ʈ �Ӽ� -> ���� �Ӽ� -> VC++ ���丮 -> ���� ���丮 -> C:\opencv-source\eigen-3.4.0; �Է�
*/
#include<Eigen/Dense>
#include<opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include<iostream>
#include<GL/glew.h>
#include<GLFW/glfw3.h>
#include<GL/GL.h>
void ErrorCallback(int error, const char* description) {
	std::cerr << "Error: " << description << std::endl;
}
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
	glfwMakeContextCurrent(windowHandler);
	glfwSwapInterval(1);
	while (!glfwWindowShouldClose(windowHandler)) {
		glfwPollEvents();

	}
	return 0;
}