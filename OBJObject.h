#ifndef OBJOBJECT_H
#define OBJOBJECT_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>

class OBJObject
{
private:
std::vector<unsigned int> indices;
float angle;
void spin(float deg);

public:
	std::vector <glm::vec3> vertices;
	std::vector <glm::vec3> normals;
	std::vector <int> faces;

	GLfloat vertices_gl;
	GLfloat faces_gl;

	GLuint VBO, VAO, EBO, Norms;

	float xavg, yavg, zavg;
	float pointsize;
	OBJObject(const char* filepath);
	OBJObject();
	glm::mat4 toWorld;

	void parse(const char* filepath);
	void center();
	void scale();
	void draw(GLuint shaderProgram);

	//mouse functions
	void mouseRotate();
	void mouseTranslateXY();
	void mouseTranslateZ();

	void update();
	void translateX(float amount);
	void translateY(float amount);
	void translateZ(float amount);
	void scale(float amount);
	void rotate(float angle, glm::vec3 axis);
	void reset();
};

#endif