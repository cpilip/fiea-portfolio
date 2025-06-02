#include "Plane.h"
#include "Ray.h"
#include "IntersectionData.h"

#include <cmath>

Plane::Plane() :
    normal(glm::vec3(0.0f, 1.0f, 0.0f)),
    position(glm::vec3(0.0f, 0.0f, 0.0f))
{
}

Plane::Plane(glm::vec3 _normal, glm::vec3 _position) :
    normal(_normal),
    position(_position)
{
}

Plane::Plane(glm::vec3 _normal) :
    normal(_normal), position(0.0f, 0.0f, 0.0f)
{
}

Plane::~Plane()
{
}

void Plane::intersect(const std::shared_ptr<Ray> ray, std::shared_ptr<IntersectionData> intersection)
{
	// Objective 4: intersection of ray with plane
	intersection->shape = shared_from_this();

	// Parallel?
	float d = glm::dot(ray->direction, this->normal);

	if (d == 0.0f)
	{
		intersection->t = FLT_MAX;
		return;
	}

	float t = glm::dot(this->position - ray->origin, this->normal) / d;

	// Behind?
	if (t < 0)
	{
		intersection->t = FLT_MAX;
		return;
	}

	intersection->t = t;

	ray->computePoint(intersection->t, intersection->p);

	intersection->n = this->normal;

	// Has checkerboard?
	if (this->materials.size() > 1)
	{
		float chessboard = std::floor(intersection->p.x) + std::floor(intersection->p.z);
		chessboard = chessboard * 0.5f - std::floor(chessboard * 0.5f);

		if (chessboard == 0)
		{
			intersection->material = this->materials.at(0);
		}
		else
		{
			intersection->material = this->materials.at(1);
		}
	}
	else
	{
		intersection->material = this->materials.at(0);
	}
}
