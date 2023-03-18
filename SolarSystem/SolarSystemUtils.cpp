#include "utils.hpp"
#include "Sphere.h"
Color::Color() {

}
Color::Color(GLfloat red, GLfloat green, GLfloat blue) {
	this->red = red;
	this->green = green;
	this->blue = blue;
}

//Cube::Cube() {
//
//}

Cube::Cube(int width, int depth, int height, Color color) {
	this->width = width;
	this->depth = depth;
	this->height = height;
	this->color = color;
}
void Cube::draw() {
	if (filled) {
		this->drawCube();
	}
	else {
		this->drawLine();
	}
}
void Cube::drawCube() {
	glColor3f(color.red, color.green, color.blue);
	glBegin(GL_QUADS);

	// top
	glVertex3f(-width / 2.0f, depth / 2.0f, -height / 2.0f);
	glVertex3f(width / 2.0f, depth / 2.0f, -height / 2.0f);
	glVertex3f(width / 2.0f, depth / 2.0f, height / 2.0f);
	glVertex3f(-width / 2.0f, depth / 2.0f, height / 2.0f); 

	// left
	glVertex3f(-width / 2.0f, depth / 2.0f, -height / 2.0f);
	glVertex3f(-width / 2.0f, depth / 2.0f, height / 2.0f);
	glVertex3f(-width / 2.0f, -depth / 2.0f, height / 2.0f);
	glVertex3f(-width / 2.0f, -depth / 2.0f, -height / 2.0f);

	// right
	glVertex3f(width / 2.0f, depth / 2.0f, -height / 2.0f);
	glVertex3f(width / 2.0f, depth / 2.0f, height / 2.0f);
	glVertex3f(width / 2.0f, -depth / 2.0f, height / 2.0f);
	glVertex3f(width / 2.0f, -depth / 2.0f, -height / 2.0f);

	// front 
	glVertex3f(-width / 2.0f, depth / 2.0f, height / 2.0f);
	glVertex3f(width / 2.0f, depth / 2.0f, height / 2.0f);
	glVertex3f(width / 2.0f, -depth / 2.0f, height / 2.0f);
	glVertex3f(-width / 2.0f, -depth / 2.0f, height / 2.0f);

	// back
	glVertex3f(-width / 2.0f, depth / 2.0f, -height / 2.0f);
	glVertex3f(width / 2.0f, depth / 2.0f, -height / 2.0f);
	glVertex3f(width / 2.0f, -depth / 2.0f, -height / 2.0f);
	glVertex3f(-width / 2.0f, -depth / 2.0f, -height / 2.0f);

	// bottom
	glVertex3f(-width / 2.0f, -depth / 2.0f, -height / 2.0f);
	glVertex3f(width / 2.0f, -depth / 2.0f, -height / 2.0f);
	glVertex3f(width / 2.0f, -depth / 2.0f, height / 2.0f);
	glVertex3f(-width / 2.0f, -depth / 2.0f, height / 2.0f);

	glEnd();
	glFlush();
}

void Cube::drawLine() {
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
	glColor3f(color.red, color.green, color.blue);
	glBegin(GL_LINES);

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

void ErrorCallback(int error, const char* description) {
	std::cerr << "Error: " << description << std::endl;
}

void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mode) {
	float delta = 0.5;
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, GLFW_TRUE);
	}
	else if (key == GLFW_KEY_RIGHT && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
		ry += delta;
	}
	else if (key == GLFW_KEY_LEFT && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
		ry -= delta;
	}
	else if (key == GLFW_KEY_UP && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
		rx -= delta;
	}
	else if (key == GLFW_KEY_DOWN && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
		rx += delta;
	}
	else if (key == GLFW_KEY_PERIOD && mode == GLFW_MOD_SHIFT && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
		rz -= delta;
	}
	else if (key == GLFW_KEY_COMMA && mode == GLFW_MOD_SHIFT && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
		rz += delta;
	}
	else if (key == GLFW_KEY_1 && action == GLFW_PRESS) {
		filled = 1;
	}
	else if (key == GLFW_KEY_0 && action == GLFW_PRESS) {
		filled = 0;
	}
}

void SolarSystem(GLFWwindow* window) {
	std::cout << rx<<" "<<ry<<" "<<rz << std::endl;
	Sphere Sun(5, 36, 18), Earth(3, 36, 18), Moon(1, 36, 18);
	Cube SunCube(10, 10, 10, Color(1, 0, 0)), EarthCube(6, 6, 6, Color(0, 0, 1)), MoonCube(2, 2, 2, Color(1, 1, 0));
	
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	glClearColor(0, 0, 0, 1);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glFrustum(-0.1f, 0.1f, -0.05f, 0.05f, 0.1f, 1000.0f);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(0.0, 0.0, -50);
	glRotatef(rx, 1, 0, 0);
	glRotatef(ry, 0, 1, 0);
	glRotatef(rz, 0, 0, 1);

	glColor3f(1, 0, 0);
	//Sun.draw();
	SunCube.draw();
	glPushMatrix();
	{
		glColor3f(0, 0, 1);
		glRotatef(rEarth, 0.0, 0.0, 1.0);
		glTranslatef(20.0, 0.0, 0.0);
		rEarth = ((int)rEarth + 1) % 360;
		//Earth.draw();
		EarthCube.draw();
		glPushMatrix();
		{
			glColor3f(1, 1, 0);
			glRotatef(rMoon, 0.0, 0.0, 1.0);
			glTranslatef(10.0, 0.0, 0.0);
			rMoon = ((int)rMoon + 360 / 30) % 360;
			//Moon.draw();
			MoonCube.draw();
		}
		glPopMatrix();
	}
	glPopMatrix();
	glFlush();
	glfwSwapBuffers(window);
}


void Render(GLFWwindow* window) {
	SolarSystem(window);
}

void glInit() {
	//glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
}