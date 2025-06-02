#pragma once
#ifndef CAMERA_H
#define CAMERA_H

#include <string>
#include <vector>
#include <memory>

#define _USE_MATH_DEFINES
#include <math.h>

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

/**
 * This should set up the camera.
 * By Alexandre Mercier-Aubin
 * Modified by Christina Pilip
 */
class Camera
{
public:
	Camera() : fovy(90.0f) {};
	virtual ~Camera() {};

	glm::vec3 position = glm::vec3(0, 0, -10);
	glm::vec3 lookAt = glm::vec3(0, 0, 1);
	glm::vec3 up = glm::vec3(0, 1, 0);

	// The vertical fov angle
	float fovy;

	// The focal length of the lens
	float focalLength;

	// The size of the lens
	float aperture;
};

#endif
