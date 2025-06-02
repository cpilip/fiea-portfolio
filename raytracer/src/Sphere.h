#pragma once
#ifndef SPHERE_H
#define SPHERE_H

#include "Shape.h"
#include "Material.h"

/**
 * This class should set up a sphere defined by a radius and center position.
 * By Loïc Nassif
 * Modified by Christina Pilip
 */
class Sphere : public Shape, public std::enable_shared_from_this<Sphere>
{
public:
	Sphere();
	Sphere(float _radius, glm::vec3 _center);
	Sphere(float _radius);

	virtual ~Sphere();

	void intersect(const std::shared_ptr<Ray> ray, std::shared_ptr<IntersectionData> intersection);

	float radius;
	glm::vec3 center;
};

#endif
