#include "OBJObject.h"
#include "Window.h"
#include <iostream>
#include <fstream>
#include <limits>
#pragma warning(disable: 4996)

using namespace std;

float pointsize = 0.2f;


OBJObject::OBJObject(const char *filepath) 
{
	toWorld = glm::mat4(1.0f);
	angle = 0.0f;
	VAO = GLuint();
	VBO = GLuint();
	EBO = GLuint();
	Norms = GLuint();

	xavg = 0.0f, yavg = 0.0f, zavg = 0.0f;
	parse(filepath);
	scale();
	center();

	//OpenGL
	// Create buffers/arrays
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);
	glGenBuffers(1, &Norms);

	// Bind the Vertex Array Object first, then bind and set vertex buffer(s) and attribute pointer(s).
	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), &vertices[0], GL_STATIC_DRAW);
	glVertexAttribPointer(0,// This first parameter x should be the same as the number passed into the line "layout (location = x)" in the vertex shader. In this case, it's 0. Valid values are 0 to GL_MAX_UNIFORM_LOCATIONS.
		3, // This second line tells us how any components there are per vertex. In this case, it's 3 (we have an x, y, and z component)
		GL_FLOAT, // What type these components are
		GL_FALSE, // GL_TRUE means the values should be normalized. GL_FALSE means they shouldn't
		3 * sizeof(GLfloat), // Offset between consecutive vertex attributes. Since each of our vertices have 3 floats, they should have the size of 3 floats in between
		(GLvoid*)0); // Offset of the first vertex's component. In our case it's 0 since we don't pad the vertices array with anything.

	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, faces.size() * sizeof(int), &faces[0], GL_STATIC_DRAW);

	glBindVertexArray(Norms);
	glBindBuffer(GL_ARRAY_BUFFER, Norms);
	glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(glm::vec3), &normals[0], GL_STATIC_DRAW);
	glVertexAttribPointer(1,// This first parameter x should be the same as the number passed into the line "layout (location = x)" in the vertex shader. In this case, it's 0. Valid values are 0 to GL_MAX_UNIFORM_LOCATIONS.
		3, // This second line tells us how any components there are per vertex. In this case, it's 3 (we have an x, y, and z component)
		GL_FLOAT, // What type these components are
		GL_FALSE, // GL_TRUE means the values should be normalized. GL_FALSE means they shouldn't
		3 * sizeof(GLfloat), // Offset between consecutive vertex attributes. Since each of our vertices have 3 floats, they should have the size of 3 floats in between
		(GLvoid*)0); // Offset of the first vertex's component. In our case it's 0 since we don't pad the vertices array with anything.
	glEnableVertexAttribArray(1);


	glBindBuffer(GL_ARRAY_BUFFER, 0); // Note that this is allowed, the call to glVertexAttribPointer registered VBO as the currently bound vertex buffer object so afterwards we can safely unbind
	glBindVertexArray(0); // Unbind VAO (it's always a good thing to unbind any buffer/array to prevent strange bugs), remember: do NOT unbind the EBO, keep it bound to this VAO
}

void OBJObject::draw(GLuint shaderProgram)
{
	// Calculate combination of the model (toWorld), view (camera inverse), and perspective matrices
	glm::mat4 MVP = Window::P * Window::V * toWorld;
	// We need to calculate this because as of GLSL version 1.40 (OpenGL 3.1, released March 2009), gl_ModelViewProjectionMatrix has been
	// removed from the language. The user is expected to supply this matrix to the shader when using modern OpenGL.

	GLuint MatrixID = glGetUniformLocation(shaderProgram, "MVP");
	GLuint modelID = glGetUniformLocation(shaderProgram, "model");
	glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);
	glUniformMatrix4fv(modelID, 1, GL_FALSE, &toWorld[0][0]);

	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, faces.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

void OBJObject::parse(const char *filepath) 
{
	cout << "parsing" << endl;

	//std::vector< unsigned int > vertexIndices, normalIndices;
	std::vector< glm::vec3 > temp_vertices;
	std::vector< glm::vec2 > temp_uvs;
	std::vector< glm::vec3 > temp_normals;

	FILE * file = fopen(filepath, "rb");
	if (file == NULL) {
		// just in case the file can't be found or is corrupt
		std::cerr << ("error loading file") << std::endl;
		exit(-1);
	}

	while (true) {
		int c1 = fgetc(file);
		int c2 = fgetc(file);
		float r, g, b;  // vertex color

		if (c1 == EOF) {
			break;
		}

		if (c1 == 'v' && c2 == ' ') {
			glm::vec3 vertex;
			fscanf(file, "%f %f %f %f %f %f\n", &vertex.x, &vertex.y, &vertex.z, &r, &g, &b);
			vertices.push_back(vertex);
		}
		else if (c1 == 'v' && c2 == 'n') {
			glm::vec3 normal;
			fscanf(file, "%f %f %f\n", &normal.x, &normal.y, &normal.z);
			normals.push_back(normal);
		}
		else if (c1 == 'f' || c2 == 'f') {
			int junk = 0;
			int x = 0, y = 0, z = 0;
			fscanf(file, "%d//%d  %d//%d %d//%d\n", &x, &junk, &y, &junk, &z, &junk);
			faces.push_back(x - 1);
			faces.push_back(y - 1);
			faces.push_back(z - 1);
		}
	}
	cout << "done parsing" << endl;
	fclose(file);
}

void OBJObject::center() {
	//Find average of all the vertices in each direction
	float xmin = vertices[0].x, xmax = vertices[0].x;
	float ymin = vertices[0].y, ymax = vertices[0].y;
	float zmin = vertices[0].z, zmax = vertices[0].z;

	for (int i = 1; i < vertices.size(); i++) {
		if (vertices[i].x < xmin) {
			xmin = vertices[i].x;
		}
		if (vertices[i].y < ymin) {
			ymin = vertices[i].y;
		}
		if (vertices[i].y < zmin) {
			zmin = vertices[i].z;
		}
		if (vertices[i].x > xmax) {
			xmin = vertices[i].x;
		}
		if (vertices[i].y > ymax) {
			ymin = vertices[i].y;
		}
		if (vertices[i].z > zmax) {
			zmin = vertices[i].z;
		}
	}

	xavg = (xmin + xmax) / 2.0f;
	yavg = (ymin + ymax) / 2.0f;
	zavg = (zmin + zmax) / 2.0f;

	//Loop through and subtract average from each vertex
	for (int i = 0; i < vertices.size(); i++) {
		vertices[i].x -= xavg;
		vertices[i].y -= yavg;
		vertices[i].z -= zavg;
	}
}

//scales object into a 2x2 cube
void OBJObject::scale() {
	center();
	float max = vertices[0].x;

	//find max dimension
	for (int i = 0; i < vertices.size(); i++) {
		if (vertices[i].x > max) {
			max = vertices[i].x;
		}
		if (vertices[i].y > max) {
			max = vertices[i].y;
		}
		if (vertices[i].z > max) {
			max = vertices[i].z;
		}
	}

	//divide each vertex by scalar
	for (int i = 0; i < vertices.size(); i++) {
		vertices[i].x /= max;
		vertices[i].y /= max;
		vertices[i].z /= max;
	}
}

//MOUSE FUNCTIONS
void OBJObject::mouseRotate() {
	int xpos = 0, ypos = 0;
}

void OBJObject::mouseTranslateXY() {
}

void OBJObject::mouseTranslateZ() {

}

//OTHER
void OBJObject::spin(float deg)
{
	this->angle += deg;
	if (this->angle > 360.0f || this->angle < -360.0f) this->angle = 0.0f;
	// This creates the matrix to rotate the cube
	glm::mat4 r = glm::rotate(glm::mat4(1.0f) , this->angle / 180.0f * glm::pi<float>(), glm::vec3(0.0f, 1.0f, 0.0f));
	this->toWorld = r * this->toWorld;

}


void OBJObject::translateX(float amount) {
	glm::vec3 v = glm::vec3(amount, 0.0f, 0.0f);
	this->toWorld = glm::translate(glm::mat4(1.0f) , v) * this->toWorld;
}

void OBJObject::translateY(float amount) {
	glm::vec3 v = glm::vec3(0.0f, amount, 0.0f);
	this->toWorld = glm::translate(glm::mat4(1.0f), v) * this->toWorld;
}

void OBJObject::translateZ(float amount) {
	glm::vec3 v = glm::vec3(0.0f, 0.0f, amount);
	this->toWorld = glm::translate(glm::mat4(1.0f), v) * this->toWorld;
}

void OBJObject::rotate(float angle, glm::vec3 axis) {
	if (angle > 360.0f || angle < -360.0f) angle = 0.0f;
	// This creates the matrix to rotate the cube
	glm::mat4 r = glm::rotate(glm::mat4(1.0f), angle / 180.0f * glm::pi<float>(), axis);
	this->toWorld = r * this->toWorld;
}

void OBJObject::reset() {
	this->toWorld = glm::mat4(1.0f);
}

void OBJObject::scale(float amount) {		
	glm::mat4 scale_m = glm::scale(glm::mat4(1.0f), glm::vec3(amount, amount, amount));

	//translate to origin
	float x = this->toWorld[3][0];
	float y = this->toWorld[3][1];
	float z = this->toWorld[3][2];

	//translateX(x);
	//translateY(y);
	//translateZ(z);

	//scale
	this->toWorld = scale_m * this->toWorld;

	//translate back
	//translateX(-x);
	//translateY(-y);
	//translateZ(-z);
}

void OBJObject::update()
{
	//spin(0.05f);
}