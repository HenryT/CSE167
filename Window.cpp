#include "window.h"
#include "OBJObject.h"

const char* window_title = "GLFW Starter Project";
Cube * cube;
OBJObject * bunny;
OBJObject * currObj;
GLint shaderProgram;
int mode = 0;

// Default camera parameters
glm::vec3 cam_pos(0.0f, 0.0f, 20.0f);		// e  | Position of camera
glm::vec3 cam_look_at(0.0f, 0.0f, 0.0f);	// d  | This is where the camera looks at
glm::vec3 cam_up(0.0f, 1.0f, 0.0f);			// up | What orientation "up" is

glm::vec3 pointLightPositions[] = {
	glm::vec3(0.7f,  0.2f,  2.0f) };

int Window::width;
int Window::height;
double Window::mouseX = 0;
double Window::mouseY = 0;
bool Window::leftMouseReleased = true;
bool Window::rightMouseReleased = true;

glm::mat4 Window::P;
glm::mat4 Window::V;

void Window::initialize_objects()
{
	cube = new Cube();
	bunny = new OBJObject("bunny.obj");
	currObj = bunny;

	// Load the shader program. Similar to the .obj objects, different platforms expect a different directory for files
#ifdef _WIN32 // Windows (both 32 and 64 bit versions)
	shaderProgram = LoadShaders("../shader.vert", "../shader.frag");
#else // Not windows
	shaderProgram = LoadShaders("shader.vert", "shader.frag");
#endif
}

void Window::clean_up()
{
	delete(cube);
	glDeleteProgram(shaderProgram);
}

GLFWwindow* Window::create_window(int width, int height)
{
	// Initialize GLFW
	if (!glfwInit())
	{
		fprintf(stderr, "Failed to initialize GLFW\n");
		return NULL;
	}

	// 4x antialiasing
	glfwWindowHint(GLFW_SAMPLES, 4);

	// Create the GLFW window
	GLFWwindow* window = glfwCreateWindow(width, height, window_title, NULL, NULL);

	// Check if the window could not be created
	if (!window)
	{
		fprintf(stderr, "Failed to open GLFW window.\n");
		glfwTerminate();
		return NULL;
	}

	// Make the context of the window
	glfwMakeContextCurrent(window);

	// Set swap interval to 1
	glfwSwapInterval(1);

	// Get the width and height of the framebuffer to properly resize the window
	glfwGetFramebufferSize(window, &width, &height);
	// Call the resize callback to make sure things get drawn immediately
	Window::resize_callback(window, width, height);

	return window;
}

void Window::resize_callback(GLFWwindow* window, int width, int height)
{
	Window::width = width;
	Window::height = height;
	// Set the viewport size
	glViewport(0, 0, width, height);

	if (height > 0)
	{
		P = glm::perspective(45.0f, (float)width / (float)height, 0.1f, 1000.0f);
		V = glm::lookAt(cam_pos, cam_look_at, cam_up);
	}
}

void Window::idle_callback()
{
	// Call the update function the cube
	cube->update();
}

void Window::display_callback(GLFWwindow* window)
{
	// Clear the color and depth buffers
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Use the shader of programID
	glUseProgram(shaderProgram);

	
	// Directional light
	glUniform1i(glGetUniformLocation(shaderProgram, "dirLight.on"), true);
	glUniform3f(glGetUniformLocation(shaderProgram, "dirLight.direction"), -0.2f, -1.0f, -0.3f);
	glUniform3f(glGetUniformLocation(shaderProgram, "dirLight.ambient"), 0.05f, 0.05f, 0.05f);
	glUniform3f(glGetUniformLocation(shaderProgram, "dirLight.diffuse"), 0.4f, 0.4f, 0.4f);
	glUniform3f(glGetUniformLocation(shaderProgram, "dirLight.specular"), 0.5f, 0.5f, 0.5f);
	
	// Point light 1
	glUniform1i(glGetUniformLocation(shaderProgram, "pointLights[0].on"), true);
	glUniform3f(glGetUniformLocation(shaderProgram, "pointLights[0].position"), pointLightPositions[0].x, pointLightPositions[0].y, pointLightPositions[0].z);
	glUniform3f(glGetUniformLocation(shaderProgram, "pointLights[0].ambient"), 0.05f, 0.05f, 0.05f);
	glUniform3f(glGetUniformLocation(shaderProgram, "pointLights[0].diffuse"), 0.8f, 0.8f, 0.8f);
	glUniform3f(glGetUniformLocation(shaderProgram, "pointLights[0].specular"), 1.0f, 1.0f, 1.0f);
	glUniform1f(glGetUniformLocation(shaderProgram, "pointLights[0].quadratic"), 0.032);

	//Spot Light
	glUniform1i(glGetUniformLocation(shaderProgram, "spotLight.on"), true);
	glUniform1i(glGetUniformLocation(shaderProgram, "spotLight.exponent"), 2);
	//glUniform3f(glGetUniformLocation(shaderProgram, "spotLight.position"), camera.Position.x, camera.Position.y, camera.Position.z);
	//glUniform3f(glGetUniformLocation(shaderProgram, "spotLight.direction"), camera.Front.x, camera.Front.y, camera.Front.z);
	glUniform3f(glGetUniformLocation(shaderProgram, "spotLight.ambient"), 0.0f, 0.0f, 0.0f);
	glUniform3f(glGetUniformLocation(shaderProgram, "spotLight.diffuse"), 0.8f, 0.8f, 0.0f);
	glUniform3f(glGetUniformLocation(shaderProgram, "spotLight.specular"), 0.8f, 0.8f, 0.0f);
	glUniform1f(glGetUniformLocation(shaderProgram, "spotLight.quadratic"), 0.032);
	glUniform1f(glGetUniformLocation(shaderProgram, "spotLight.cutOff"), glm::cos(glm::radians(12.5f)));	

	// Render the cube
	//cube->draw(shaderProgram);
	currObj->draw(shaderProgram);

	// Gets events, including input such as keyboard and mouse or window resizing
	glfwPollEvents();
	// Swap buffers
	glfwSwapBuffers(window);
}

void Window::key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	// Check for a key press
	if (action == GLFW_PRESS)
	{
		if (key == GLFW_KEY_0) {
			mode = 0;
			glUniform1i(glGetUniformLocation(shaderProgram, "dirLight.on"), false);
			glUniform1i(glGetUniformLocation(shaderProgram, "pointLights[0].on"), false);
			glUniform1i(glGetUniformLocation(shaderProgram, "spotLight.on"), false);
		}

		if (key == GLFW_KEY_1) {
			mode = 1;
			glUniform1i(glGetUniformLocation(shaderProgram, "dirLight.on"), true);
			glUniform1i(glGetUniformLocation(shaderProgram, "pointLights[0].on"), false);
			glUniform1i(glGetUniformLocation(shaderProgram, "spotLight.on"), false);
		}

		if (key == GLFW_KEY_2) {
			mode = 2;
			glUniform1i(glGetUniformLocation(shaderProgram, "dirLight.on"), false);
			glUniform1i(glGetUniformLocation(shaderProgram, "pointLights[0].on"), true);
			glUniform1i(glGetUniformLocation(shaderProgram, "spotLight.on"), false);
		}

		if (key == GLFW_KEY_3) {
			mode = 3;
			glUniform1i(glGetUniformLocation(shaderProgram, "dirLight.on"), false);
			glUniform1i(glGetUniformLocation(shaderProgram, "pointLights[0].on"), false);
			glUniform1i(glGetUniformLocation(shaderProgram, "spotLight.on"), true);
		}

		// Check if escape was pressed
		if (key == GLFW_KEY_ESCAPE)
		{
			// Close the window. This causes the program to also terminate.
			glfwSetWindowShouldClose(window, GL_TRUE);
		}
	}
}

void Window::mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
		glfwGetCursorPos(window, &mouseX, &mouseY);
		leftMouseReleased = false;
	}

	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE) {
		leftMouseReleased = true;
	}

	if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS) {
		glfwGetCursorPos(window, &mouseX, &mouseY);
		rightMouseReleased = false;
	}

	if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_RELEASE) {
		rightMouseReleased = true;
	}
}

void Window::cursor_position_callback(GLFWwindow* window, double xpos, double ypos) {
	//Rotate
	if (!leftMouseReleased) {
		double newX = 0;
		double newY = 0;
		glfwGetCursorPos(window, &newX, &newY);

		glm::vec3 lastPoint = trackballmapping(mouseX, mouseY);
		glm::vec3 curPoint = trackballmapping(newX, newY); // Map the mouse position to a logical sphere location.

		// Handle any necessary mouse movements
		glm::vec3 direction;
		direction = curPoint - lastPoint;
		float velocity = glm::length(direction);

		//For Rotation calculations
		float rot_angle;
		glm::vec3 rot_axis;

		//rotation angle, angle between a and b
		rot_angle = glm::acos((glm::dot(lastPoint, curPoint) / (glm::length(lastPoint) * glm::length(curPoint))));

		//rotation axis, cross product of curPoint and lastPoint
		rot_axis = glm::vec3(((curPoint.y * lastPoint.z) - (lastPoint.y * curPoint.z),
			(curPoint.z * lastPoint.x) - (lastPoint.z * curPoint.x),
			(curPoint.x * lastPoint.y) - (lastPoint.x * curPoint.y)));

		if (mode == 0 && velocity > 0.0001f) // If little movement - do nothing.
		{
			currObj->rotate(rot_angle, rot_axis);
			// Rotate about the axis that is perpendicular to the great circle connecting the mouse movements.
			//rot_angle = velocity * 1;
		}

		lastPoint = curPoint;
	}

	//Translate X/Y
	if (!rightMouseReleased) {
		double newX = 0;
		double newY = 0;
		glfwGetCursorPos(window, &newX, &newY);

		currObj->translateX((-.05f)*(newX - mouseX));
		currObj->translateY((-.05f)*(newY - mouseY));
		
		mouseX = newX;
		mouseY = newY;
	}
}


void Window::scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	//Translate Z
	currObj->translateZ(yoffset);
}

glm::vec3 Window::trackballmapping(double x, double y) {
	glm::vec3 v;
	float d;
	v.x = (2.0 * x - width) / width;
	v.y = (height - 2.0 * y) / height;
	v.z = 0.0;
	d = glm::length(v);
	d = (d<1.0) ? d : 1.0;
	v.z = sqrtf(1.001 - d*d);
	glm::normalize(v); // Still need to normalize, since we only capped d, not v.
	return v;
}