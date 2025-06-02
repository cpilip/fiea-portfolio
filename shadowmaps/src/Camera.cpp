// Modified by Christina Pilip

#include "Camera.h"

using namespace std;
#include <glm/gtx/string_cast.hpp>
#include <iostream>

Camera::Camera(float aspect):lightColor(glm::vec3(1,1,1))
{
	this->setPerspective((float)(45.0 * M_PI / 180.0), aspect, 0.01f, 100.0f);
	this->updateView();
}

Camera::~Camera()
{
}

void Camera::setPerspective(float fov, float aspect, float near, float far) {
	P = glm::mat4(1.0);

	// Projection transformation - single line using using glm::perspective
	P = glm::perspective(fov, aspect, near, far);
}

void Camera::updateView() {
	V = glm::mat4(1.0);

	// Viewing transformation - single line using using glm::lookAt
	V = glm::lookAt(this->position, this->lookAt, this->up);
}


void Camera::draw(const shared_ptr<Program> program, glm::mat4 P, glm::mat4 V, shared_ptr <MatrixStack> M, glm::mat4 LightPV, Axis &axis) {
	program->bind();
	M->pushMatrix();
	
	// Set uniform variables
	glUniformMatrix4fv(program->getUniform("P"), 1, GL_FALSE, &P[0][0]);
	glUniformMatrix4fv(program->getUniform("V"), 1, GL_FALSE, &V[0][0]);
	glUniformMatrix4fv(program->getUniform("M"), 1, GL_FALSE, &M->topMatrix()[0][0]);
	glUniformMatrix4fv(program->getUniform("MinvT"), 1, GL_FALSE, &M->topInvMatrix()[0][0]);
	glUniformMatrix4fv(program->getUniform("lightPV"), 1, GL_FALSE, &LightPV[0][0]);
	
	// Draw the light coordinate frame
	M->pushMatrix();
	M->multMatrix(glm::inverse(this->V)); // World space -> light space
	axis.draw(program, M);
	M->popMatrix();

	// Draw the camera's wirecube
	M->pushMatrix();

	// Draw light frustrum
	M->pushMatrix();
	M->multMatrix(glm::inverse(this->V));
	M->multMatrix(glm::inverse(this->P));
	debugWireCube->draw(program, P, V, M, LightPV);
	M->popMatrix();

	program->unbind();

	M->pushMatrix();	

	// Draw the light view on the near plane of the frustum
	M->pushMatrix();
	M->multMatrix(glm::inverse(this->V));
	M->multMatrix(glm::inverse(this->P));

	glm::mat4 xform = glm::mat4(1.0f);
	xform = glm::translate(xform, glm::vec3(0, 0, -1.0f));
	M->multMatrix(xform);

	debugDepthMapQuad->draw(quadShader, P, V, M, LightPV);
	M->popMatrix();

	M->popMatrix();
	M->popMatrix();
	M->popMatrix();
	
	GLSL::checkError(GET_FILE_LINE);
}