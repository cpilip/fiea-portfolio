#pragma once
#ifndef LIGHT_H
#define LIGHT_H

#define _USE_MATH_DEFINES
#include <math.h>

#include <string>

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

/**
 * This class should set up lights.
 * By Loïc Nassif
 * Modified by Christina Pilip
 */
class Light
{
public:
	Light() :
		name(""),
		colour(1.0f, 1.0f, 1.0f),
		position(0.0f, 0.0f, 0.0f),
		attenuation(0.0f, 0.0f, 0.0f),
		power(1.0f),
		type("point")
		{}
	virtual ~Light() {}

	std::string name; // Name of the light.
	glm::vec3 colour; // Light colour. Default white.
	glm::vec3 position; // Position of light. Default world origin.
	std::string type; // Type of light. Default point light.
	float power; // Light intensity.

	// Attenuation coefficients
	glm::vec3 attenuation; 
};

#endif
