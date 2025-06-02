#pragma once
#ifndef ARCBALL_H
#define ARCBALL_H

#include <string>
#include <vector>

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

/**
 * This class manages the camera interaction and motions using an ArcBall (from https://graphicsinterface.org/wp-content/uploads/gi1992-18.pdf).
 * By Christina Pilip
 */
class ArcBall
{
public:
	ArcBall();
	virtual ~ArcBall();

	// This starts the tracking motion according to inital mouse positions
	void startRotation(double mousex, double mousey, int windowWidth, int windowHeight);

	// This generates a rotation based on the mouse displacement on screen w.r.t. to the initial mouse position on click
	void updateRotation(double mousex, double mousey, int windowWidth, int windowHeight);

	// This computes the vector position on the sphere
	glm::vec3 computeVecFromMousePos(double mousex, double mousey, int windowWidth, int windowHeight);
	
	// The rotation matrix to change the camera's view
	glm::mat4 R;

	// Parameters to tune the speed of rotation
	double fit;
	double gain;

private:
	// Stores the initial rotation matrix, prior to the update
	glm::mat4 Rmem;

	glm::vec3 p0;
	glm::vec3 p1;
};

#endif
