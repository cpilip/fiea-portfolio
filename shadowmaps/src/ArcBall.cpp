// By Christina Pilip

#include "ArcBall.h"
#include "MatrixStack.h"
#include "Program.h"

#include <iostream>
#include <cassert>

#include "GLSL.h"

using namespace std;

ArcBall::ArcBall():R(glm::mat4(1.0)), Rmem(glm::mat4(1.0)), p0(glm::vec3(1.0)), p1(glm::vec3(1.0)), fit(0.5), gain(5)
{
}

ArcBall::~ArcBall()
{
}

glm::vec3 ArcBall::computeVecFromMousePos(double mousex, double mousey, int windowWidth, int windowHeight) {
	// Compute the projection of mouse coords on the arcball
	// Follows pseudocode from paper
	
	float radius = min(windowWidth, windowHeight) / this->fit;
	float radiusSquared = radius * radius;

	// To clip space
	float x = ((float)mousex - (windowWidth / 2.0f));
	float y = ((windowHeight / 2.0f) - (float)mousey);
	float d = x * x + y * y;

	if (d > radiusSquared)
	{
		return glm::normalize(glm::vec3(x, y, 0));
	}
	else
	{
		return glm::normalize(glm::vec3(x, y, sqrt(radiusSquared - d)));
	}
}


double computeVectorAngle(glm::vec3& v1, glm::vec3& v2) {
	double vDot = glm::dot(v1, v2);
	if (vDot < -1.0) vDot = -1.0;
	if (vDot > 1.0) vDot = 1.0;
	return((double)(acos(vDot)));
}

void ArcBall::startRotation(double mousex, double mousey, int windowWidth, int windowHeight) {
	Rmem = R;
	p0 = computeVecFromMousePos(mousex, mousey, windowWidth, windowHeight);
}

void ArcBall::updateRotation(double mousex, double mousey, int windowWidth, int windowHeight) {
	// Computes the rotation update for the view camera
	p1 = computeVecFromMousePos(mousex, mousey, windowWidth, windowHeight);

	glm::vec3 axis = glm::normalize(glm::cross(p0, p1));

	// Was there a very small mouse movement?
	if (axis == glm::vec3(0.0f, 0.0f, 0.0f))
	{
		return;
	}

	float angle = (float)computeVectorAngle(p0, p1) * this->gain;

	glm::quat q = glm::angleAxis(angle, axis);
	glm::mat4 rotationMatrix = glm::mat4_cast(q);

	R = rotationMatrix * Rmem;
}
