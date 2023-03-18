#pragma once
#include<iostream>
#include<GL/glew.h>
#include<GLFW/glfw3.h>
#include<GL/GL.h>
//#include "Sphere.h"

class Color {
public:
	GLfloat red;
	GLfloat green;
	GLfloat blue;
public:
	Color();
	Color(GLfloat red, GLfloat green, GLfloat blue);
};

class Cube{
private:
	int width;   // 가로
	int depth;   // 세로
	int height;  // 높이
	Color color;

public:
	Cube();
	Cube(int width, int depth, int height, Color color);
	void drawLine();
	void drawCube();
	void draw();
};

static float rx, ry, rz;
static float rEarth, rMoon;
static bool filled = 0;

void ErrorCallback(int error, const char* description);
void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mode);
void SolarSystem(GLFWwindow* window);
void Render(GLFWwindow* window);
void glInit();