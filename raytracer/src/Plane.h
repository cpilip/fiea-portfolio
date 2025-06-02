#pragma once
#ifndef PLANE_H
#define PLANE_H

#include "Shape.h"
#include "Material.h"


/**
 * This class should set up a plane defined by a point and a normal.
 * By Loïc Nassif
 * Modified by Christina Pilip
 */
class Plane : public Shape, public std::enable_shared_from_this<Plane>
{
public:
	Plane();
	Plane(glm::vec3 _normal, glm::vec3 _position);
	Plane(glm::vec3 _normal);

	virtual ~Plane();

	void intersect(const std::shared_ptr<Ray> ray, std::shared_ptr<IntersectionData> intersection);

	glm::vec3 normal;
	glm::vec3 position;
};

#endif
