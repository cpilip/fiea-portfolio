#pragma once
#ifndef MATERIAL_H
#define MATERIAL_H

#define _USE_MATH_DEFINES
#include <math.h>

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

/**
 * Material class to specify surface properties such as diffuse and specular colour. 
 * By Loïc Nassif
 * Modified by Christina Pilip
 */
class Material
{
public:
	Material() : name(""),
				 diffuseColor(glm::vec3(1.0f, 1.0f, 1.0f)),
				 specularColor(glm::vec3(0.0f, 0.0f, 0.0f)),
				 reflectivity(0.0f),
				 transparency(0.0f),
				 indexOfRefraction(0.0f),
				 hardness(0.0f)
	{}

	virtual ~Material() {}

	void reset()
	{
		name = "";
		diffuseColor = glm::vec3(1.0f, 1.0f, 1.0f);
		specularColor = glm::vec3(0.0f, 0.0f, 0.0f);
		reflectivity = 0.0f;
		transparency = 0.0f;
		indexOfRefraction = 0.f;
		hardness = 0.f;
	}

	std::string name;	
	glm::vec3 diffuseColor;	
	glm::vec3 specularColor; 
	float hardness;	// Specular hardness
	int ID = -1; // ID for material to pair with shapes 

	// Reflectivity
	float reflectivity;

	// Transparency (default 0 = none)
	float transparency;

	// Index of refraction (default 0 = none)
	float indexOfRefraction;

	// Absorbency (default 0 = none)
	float absorb;
};

#endif
