#include "HierachicalShape.h"
#include "Ray.h"
#include "IntersectionData.h"

#include <iostream>

HierachicalShape::HierachicalShape() :
    M(glm::mat4(1.0f)), Minv(glm::mat4(1.0f))
{
    transformRay = std::make_shared<Ray>();
    transformData = std::make_shared<IntersectionData>();
}

HierachicalShape::~HierachicalShape()
{
}

void HierachicalShape::intersect(const std::shared_ptr<Ray> ray, std::shared_ptr<IntersectionData> intersection)
{
    /// Objective 7: intersection of ray with hierarchical shapes
    intersection->shape = shared_from_this();

    // Transform ray into object space
	this->transformRay->origin = glm::vec3(this->Minv * glm::vec4(ray->origin.x, ray->origin.y, ray->origin.z, 1.0f));
	this->transformRay->direction = glm::vec3(this->Minv * glm::vec4(ray->direction.x, ray->direction.y, ray->direction.z, 0.0f));
	
	// Call on children until a leaf is reached
	for (auto& c : children)
	{
		transformData->reset();
		c->intersect(this->transformRay, transformData);

		if (intersection->t > transformData->t)
		{
			intersection->t = transformData->t;
			intersection->n = transformData->n;
			intersection->p = transformData->p;
			intersection->material = transformData->material;
			intersection->shape = transformData->shape;
		}
	}

	if (intersection->t < FLT_MAX)
	{
		// Take transpose of Minv to get normal matrix
		glm::mat4 transpose = glm::transpose(this->Minv);

		intersection->p = glm::vec3(this->M * glm::vec4(intersection->p.x, intersection->p.y, intersection->p.z, 1.0f));
		intersection->n = glm::vec3(glm::normalize(transpose * glm::vec4(intersection->n.x, intersection->n.y, intersection->n.z, 0.0f)));
	}

}
