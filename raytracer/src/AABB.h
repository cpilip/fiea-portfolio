#pragma once
#ifndef AABB_H
#define AABB_H

#include "Shape.h"
#include "Material.h"

/**
 * This class should set up an axis aligned bounding box. 
 * By Loïc Nassif 
 * Modified by Christina Pilip
 */
class AABB : public Shape, public std::enable_shared_from_this<AABB>
{
public:
	AABB();
	AABB(glm::vec3 _position, glm::vec3 _size);

	virtual ~AABB();

	void intersect(const std::shared_ptr<Ray> ray, std::shared_ptr<IntersectionData> intersection);

	glm::vec3 minpt;
	glm::vec3 maxpt;
};

#endif