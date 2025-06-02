#pragma once
#ifndef SHAPE_H
#define SHAPE_H

#include <memory>
#include <string>
#include <vector>

#define _USE_MATH_DEFINES
#include <math.h>

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class Ray;
class IntersectionData;
class Material;

/**
 * An abstract class from which any intersectable object should derive from. 
 * Every intersectable has the ability to hold many materials.
 * By Loïc Nassif
 * Modified by Christina Pilip
 */
class Shape
{
public:
	Shape() {}
	virtual ~Shape() {}

	virtual void intersect(const std::shared_ptr<Ray> ray, std::shared_ptr<IntersectionData> intersection) {}

	std::vector<std::shared_ptr<Material>> materials;
	std::string name;
	std::string type;
};

#endif
