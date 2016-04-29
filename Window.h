#ifndef _WINDOW_H_
#define _WINDOW_H_

#include <iostream>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "Cube.h"
#include "shader.h"
#include "OBJObject.h"

class Window
{
public:
	//static OBJObject bunny;
	//static OBJObject bear;
	//static OBJObject dragon;

	static int width;
	static int height;
	static glm::mat4 P;
	static glm::mat4 V;

	static double mouseX;
	static double mouseY;
	static bool leftMouseReleased;
	static bool rightMouseReleased;

	static void initialize_objects();
	static void clean_up();
	static GLFWwindow* create_window(int width, int height);
	static void resize_callback(GLFWwindow* window, int width, int height);
	static void idle_callback();
	static void display_callback(GLFWwindow*);
	static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
	static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
	static void cursor_position_callback(GLFWwindow* window, double xpos, double ypos);
	static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

private:
	static glm::vec3 trackballmapping(double x, double y);
};

#endif
