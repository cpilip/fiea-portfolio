#include "Sphere.h"
#include "Ray.h"
#include "IntersectionData.h"

#include <iostream>

Sphere::Sphere() :
    radius(1.0f),
    center(glm::vec3(0.0f, 0.0f, 0.0f))
{
}

Sphere::Sphere(float _radius, glm::vec3 _center) :
    radius(_radius), 
    center(_center)
{
}

Sphere::Sphere(float _radius) :
    radius(_radius),
    center(glm::vec3(0.0f, 0.0f, 0.0f))
{
}

Sphere::~Sphere()
{
}

void Sphere::intersect(const std::shared_ptr<Ray> ray, std::shared_ptr<IntersectionData> intersection)
{
    // Objective 2: intersection of ray with sphere
    intersection->shape = shared_from_this(); 
    
    // Calculate roots
    glm::vec3 oc = this->center - ray->origin;
    float a = glm::dot(ray->direction, ray->direction);
    float b = -2.0f * glm::dot(ray->direction, oc);
    float c = glm::dot(oc, oc) - this->radius * this->radius;
    float discriminant = b * b - 4 * a * c;
    
    if (discriminant < 0) 
    {
        // No roots
        intersection->t = FLT_MAX;
        return;
    }
    else 
    {
        // At least one root, pick one
        float sqrt = std::sqrt(discriminant);
        float x0 = (-b - sqrt) / (2.0f * a);
        float x1 = (-b + sqrt) / (2.0f * a);
        float root = glm::min(x0, x1);

        if (root < 0)
        {
            intersection->t = FLT_MAX;
            return;
        }

        intersection->t = root;

        ray->computePoint(intersection->t, intersection->p);

        intersection->n = glm::normalize(intersection->p - this->center);

        intersection->material = this->materials.at(0);

    }
    
}