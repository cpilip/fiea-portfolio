#include "AABB.h"
#include "Ray.h"
#include "IntersectionData.h"

AABB::AABB() :
    minpt(0.0f, 0.0f, 0.0f),
    maxpt(10.0f, 10.0f, 10.0f)
{
}

AABB::AABB(glm::vec3 position, glm::vec3 size)
{
    minpt = position - size / 2.0f;
    maxpt = position + size / 2.0f;
}

AABB::~AABB()
{
}

void AABB::intersect(const std::shared_ptr<Ray> ray, std::shared_ptr<IntersectionData> intersection)
{
    /// Objective 6: intersection of ray with axis aligned box
    intersection->shape = shared_from_this();

    // Using "slabs", but just doing all the tests to be faster
    float t1 = (this->minpt.x - ray->origin.x) / ray->direction.x;
    float t2 = (this->maxpt.x - ray->origin.x) / ray->direction.x;
    float t3 = (this->minpt.y - ray->origin.y) / ray->direction.y;
    float t4 = (this->maxpt.y - ray->origin.y) / ray->direction.y;
    float t5 = (this->minpt.z - ray->origin.z) / ray->direction.z;
    float t6 = (this->maxpt.z - ray->origin.z) / ray->direction.z;

    // Interval
    float tmin = glm::max(glm::max(glm::min(t1, t2), glm::min(t3, t4)), glm::min(t5, t6));
    float tmax = glm::min(glm::min(glm::max(t1, t2), glm::max(t3, t4)), glm::max(t5, t6));

    // In interval?
    if (tmax < 0)
    {
        intersection->t = FLT_MAX;
        return;
    }

    if (tmin > tmax)
    {
        intersection->t = FLT_MAX;
        return;
    }

    if (tmin < 0)
    {
        tmin = tmax;
    }

    intersection->t = tmin;
    ray->computePoint(intersection->t, intersection->p);

    // Get correct normal
    glm::vec3 normal = glm::vec3(0.0f, 0.0f, 0.0f);
    if (tmin == t1)
    {
        normal.x = -1.0f;
        intersection->n = normal;
    }
    else if (tmin == t3)
    {
        normal.y = -1.0f;
        intersection->n = normal;

    }
    else if (tmin == t5)
    {
        normal.z = -1.0f;
        intersection->n = normal;

    }
    else if (tmin == t2)
    {
        normal.x = 1.0f;
        intersection->n = normal;

    }
    else if (tmin == t4)
    {
        normal.y = 1.0f;
        intersection->n = normal;
    }
    else //t6
    {
        normal.z = 1.0f;
        intersection->n = normal;

    }

    intersection->material = this->materials.at(0);
}
