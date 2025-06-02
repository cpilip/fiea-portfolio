#include "Scene.h"
#include "Ray.h"
#include "IntersectionData.h"
#include "Camera.h"
#include "Light.h"
#include "Shape.h"

#include <stdlib.h> 

#include <iostream>
#include <chrono>

#define OFFSET_EPSILON 0.001f // For shadow acne, etc.

// Default values, they get overwritten if scene file contains them
Scene::Scene(const std::string name) :
	width(1080), height(720), aspect(1080.0f / 720.0f),
	ambient(0.1f, 0.1f, 0.1f)
{
	outputFilename = name;
}

void Scene::init() 
{
	// Initialize camera	
	cam = make_shared<Camera>();

	// Init image
	image = make_shared<Image>(width, height);

	// Initialize rng with seed
	rng.seed(123456);
}

Scene::~Scene()
{
}

void Scene::addObject(shared_ptr<Shape> shape) 
{
	shapes.push_back(shape);
}

void Scene::addLight(shared_ptr<Light> light) 
{
	lights.push_back(light);
}

/*
 * Find the closest point at which a ray intersects the scene's geometry. If there is no
 * intersection, closest will be unchanged.
 *  
 * \param ray The ray to test.
 * \param intersection An intersection record to use during the test. 
 * \param closest An intersection record that will store the result of the test. Only pass a newly created record.
 */
void Scene::findClosestIntersection(const shared_ptr<Ray>& ray, shared_ptr<IntersectionData> intersection, shared_ptr<IntersectionData> closest)
{
	for (auto& s : this->shapes)
	{
		intersection->reset();
		s->intersect(ray, intersection);

		if (intersection->t < closest->t)
		{
			closest->t = intersection->t;
			closest->n = glm::normalize(intersection->n);
			closest->p = intersection->p;
			closest->material = intersection->material;
			closest->shape = intersection->shape;
		}
	}
}

/*
 * Calculate the lighting contribution at an intersection point.
 *
 * \param ray The ray that hit the intersection point.
 * \param intersection An intersection record to use during shadow-testing. 
 * \param closest An intersection record that contains the intersection point.
 * \return The colour at the intersection point.
 */
glm::vec3 Scene::getLightingContribution(const shared_ptr<Ray>& ray, shared_ptr<IntersectionData> intersection, const shared_ptr<IntersectionData>& closest)
{
	std::shared_ptr<Ray> shadowRay = make_shared<Ray>();
	glm::vec3 colour = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 h = glm::vec3();
	glm::vec3 lightDirection = glm::vec3();
	float attenuation = 1.0f;

	// Calculate each light's contribution
	for (int k = 0; k < lights.size(); k++)
	{
		// Determine light direction (and adjust attenuation for point lights)
		if (lights.at(k)->type == "point")
		{
			float d = glm::length(lights.at(k)->position - closest->p);
			attenuation = 1.0 / (1.0 + lights.at(k)->attenuation.y * d + lights.at(k)->attenuation.z * (d * d));

			lightDirection = glm::normalize(lights.at(k)->position - closest->p);
		}
		else
		{
			lightDirection = -1.0f * lights.at(k)->position;
		}

		// Ambient lighting
		colour.x += ambient.x * closest->material->diffuseColor.x * attenuation;
		colour.y += ambient.y * closest->material->diffuseColor.y * attenuation;
		colour.z += ambient.z * closest->material->diffuseColor.z * attenuation;

		// First, test for shadows
		shadowRay->origin = closest->p + OFFSET_EPSILON * lightDirection;
		shadowRay->direction = lightDirection;
		bool hit = false;

		for (auto& s : shapes)
		{
			intersection->reset();
			s->intersect(shadowRay, intersection);

			if (intersection->t < FLT_MAX)
			{
				hit = true; //No contribution 
				break;
			}
		}

		if (!hit)
		{
			// Diffuse (Lambertian)
			float lambertian = glm::max(glm::dot(closest->n, lightDirection), 0.0f);
			colour.x += (lights.at(k)->colour.x * lights.at(k)->power * lambertian * closest->material->diffuseColor.x * attenuation);
			colour.y += (lights.at(k)->colour.y * lights.at(k)->power * lambertian * closest->material->diffuseColor.y * attenuation);
			colour.z += (lights.at(k)->colour.z * lights.at(k)->power * lambertian * closest->material->diffuseColor.z * attenuation);

			// Specular (Blinn - Phong)
			h = glm::normalize(lightDirection - ray->direction); 
			float specular = pow(glm::max(glm::dot(closest->n, h), 0.0f), closest->material->hardness);
			colour.x += (lights.at(k)->colour.x * lights.at(k)->power * specular * closest->material->specularColor.x * attenuation);
			colour.y += (lights.at(k)->colour.y * lights.at(k)->power * specular * closest->material->specularColor.y * attenuation);
			colour.z += (lights.at(k)->colour.z * lights.at(k)->power * specular * closest->material->specularColor.z * attenuation);
		}
	}

	return colour;
}

/*
 * Calculate the colour of an intersection point (lighting, material properties, etc.)
 *
 * \param ray The ray that hit the intersection point.
 * \param intersection An intersection record to use for further intersection testing.
 * \param closest An intersection record that contains the intersection point.
 * \param depth How many times a ray has bounced.
 * \return The colour at the intersection point.
 */
glm::vec3 Scene::getShading(const shared_ptr<Ray>& ray, shared_ptr<IntersectionData> intersection, shared_ptr<IntersectionData>& closest, int depth)
{
	std::shared_ptr<Ray> reflectedRay = make_shared<Ray>();
	std::shared_ptr<Ray> refractedRay = make_shared<Ray>();
	std::shared_ptr<IntersectionData> reflectiveIntersection = make_shared<IntersectionData>();
	std::shared_ptr<IntersectionData> refractedIntersection = make_shared<IntersectionData>();
	glm::vec3 colour = glm::vec3(0.0f, 0.0f, 0.0f);
	float absorbDistance = 0.0f;

	if (depth > 3)
	{
		return colour;
	}

	// Objective 3: compute the shaded result for the intersection point
	if (closest->t < FLT_MAX)
	{
		// Light
		glm::vec3 lighting = getLightingContribution(ray, intersection, closest);
		colour.x += lighting.x;
		colour.y += lighting.y;
		colour.z += lighting.z;

		float reflectivity = 1.0f;
		float transmissance = 1.0f;
		
		// Calculate in advance for refraction (used for Fresnel equations)
		float angle = glm::max(glm::min(1.0f, glm::dot(ray->direction, closest->n)), -1.0f);
		float eta1 = 1.0f;
		float eta2 = closest->material->indexOfRefraction;
		glm::vec3 normal = closest->n;

		if (angle < 0.0f)
		{
			angle = -1.0f * angle;
		}
		else
		{
			// If negative, switch indexes of refraction
			std::swap(eta1, eta2);
			normal = -1.0f * closest->n;
		}

		float eta = eta1 / eta2;
		float k = 1 - eta * eta * (1.0f - angle * angle);

		if (useFresnel)
		{
			if (k >= 0.0f)
			{
				// Using the actual equations - this works, but is extremely subtle

				//float angle2 = glm::sqrt(k);
				//float fParallel = ((eta2 * angle) - (eta1 * angle2)) / ((eta2 * angle) + (eta1 * angle2));
				//float fPerpendicular = ((eta1 * angle) - (eta2 * angle2)) / ((eta1 * angle) + (eta2 * angle2));
				//reflectivity = (fParallel * fParallel + fPerpendicular * fPerpendicular) / 2.0f;
				
				// Schlick's approximation - more visible
				float r = (eta1 - eta2) / (eta1 + eta2);
				r = r * r;
				reflectivity = r + (1 - r) * (1 - angle) * (1 - angle) * (1 - angle) * (1 - angle) * (1 - angle);

				transmissance = 1.0f - reflectivity;
			}
		}

		// Reflection
		if (closest->material->reflectivity != 0.0f)
		{
			// Reflect ray
			glm::vec3 reflectedLightDirection = glm::normalize(ray->direction - 2.0f * (glm::dot(closest->n, ray->direction) * closest->n));
			reflectedRay->origin = closest->p + OFFSET_EPSILON * reflectedLightDirection;
			reflectedRay->direction = reflectedLightDirection;

			// Recurse
			findClosestIntersection(reflectedRay, intersection, reflectiveIntersection);
			glm::vec3 shading = getShading(reflectedRay, intersection, reflectiveIntersection, depth + 1);

			if (reflectiveIntersection->t < FLT_MAX)
			{
				if (useFresnel)
				{
					colour.x += (closest->material->reflectivity + (1.0 - closest->material->reflectivity) * reflectivity) * shading.x;
					colour.y += (closest->material->reflectivity + (1.0 - closest->material->reflectivity) * reflectivity) * shading.y;
					colour.z += (closest->material->reflectivity + (1.0 - closest->material->reflectivity) * reflectivity) * shading.z;

				}
				else
				{
					colour.x += closest->material->reflectivity * shading.x;
					colour.y += closest->material->reflectivity * shading.y;
					colour.z += closest->material->reflectivity * shading.z;
				}

			}
		}

		// Add in Beer's law absorbance
		float beersLaw = 1.0f;

		// Refraction
		if (closest->material->transparency > 0.0f)
		{
			if (k >= 0.0f)
			{
				// Refract ray only if there is no total internal reflection
				refractedRay->direction = eta * ray->direction + (eta * angle - glm::sqrt(k)) * normal;
				refractedRay->origin = closest->p + OFFSET_EPSILON * refractedRay->direction;

				// Recurse
				findClosestIntersection(refractedRay, intersection, refractedIntersection);
				glm::vec3 shading = getShading(refractedRay, intersection, refractedIntersection, depth + 1);

				if (closest->material->absorb > 0.0f)
				{
					absorbDistance = glm::distance(closest->p, refractedIntersection->p);
					beersLaw = std::exp(-closest->material->absorb * absorbDistance);
				}

				colour.x += closest->material->transparency * shading.x * transmissance * beersLaw;
				colour.y += closest->material->transparency * shading.y * transmissance * beersLaw;
				colour.z += closest->material->transparency * shading.z * transmissance * beersLaw;
			}
		}
	}

	return colour;
}

void Scene::render() 
{

	auto start = std::chrono::high_resolution_clock::now();
	std::shared_ptr<IntersectionData> intersection = make_shared<IntersectionData>();
	std::shared_ptr<IntersectionData> closestIntersection = make_shared<IntersectionData>();
	std::shared_ptr<Ray> ray = make_shared<Ray>();

	glm::vec3 camDir = cam->position - cam->lookAt;
	float d = this->cam->focalLength;
	float vh = 2.0f * d * glm::tan(0.5f * (M_PI * cam->fovy / 180.f));
	float vw = aspect * vh;

	// Compute camera basis
	glm::vec3 w = glm::normalize(camDir);
	glm::vec3 u = glm::normalize(glm::cross(cam->up, w));
	glm::vec3 v = glm::cross(w, u);

	glm::vec3 pw = (vw * u) * (1.0f / this->width); // Pixel width
	glm::vec3 ph = (vh * -v) * (1.0f / this->height); // Pixel height

	// Scale basis to get upper left of "frame" to render
	glm::vec3 vp = cam->position - (d * w) - (vw * u) * 0.5f - (vh * -v) * 0.5f;
	glm::vec3 pp = vp + 0.5f * (pw + ph); // Center of upper left pixel
	glm::vec3 colour = glm::vec3();

	// A random number generator for jittering rays - using https://stackoverflow.com/questions/9878965/generate-a-value-between-0-0-and-1-0-using-rand 
	std::uniform_real_distribution<float> gen(0.0f, 0.5f);
	std::uniform_real_distribution<float> genDOF(0.0f, 1.0f);

	int	numRaysPerPixel = 0;
	int	totalAASamples = this->aaSamples * this->aaSamples;
	glm::vec3 dofOffset = glm::vec3();

	// Decide how many rays we want to cast per pixel (for anti-aliasing, DOF, etc.)
	if (this->aaSamples != 0)
	{
		numRaysPerPixel += totalAASamples;
	}
	if (this->dofSamples != 0)
	{
		numRaysPerPixel += this->dofSamples;
	}

	if (this->aaSamples == 0 && this->dofSamples == 0)
	{
		numRaysPerPixel = 1;
	}

	for (int i = 0; i < width; i++)
	{
		for (int j = 0; j < height; j++)
		{
			colour = glm::vec3(0.0f, 0.0f, 0.0f);

			// Objective 8: add in anti-aliasing (chose to do uniform grid)
			// Do some fun stuff to manipulate where to point rays to
			float newi;
			float newj;
			int a, b, c = 0;

			for (int k = 0; k < numRaysPerPixel; k++)
			{
				if (c < totalAASamples)
				{
					a = c % aaSamples;
					b = c / aaSamples;
					c += 1;
				}

				if (jitter && this->aaSamples != 0 && c < totalAASamples) // Jittered AA
				{
					newi = i + (a + gen(rng));
					newj = j + (b + gen(rng));

				}
				else if (this->aaSamples != 0 && c < totalAASamples) // Regular AA
				{
					newi = i + (a + 0.5f);
					newj = j + (b + 0.5f);
				}
				else // Just a regular ray
				{
					newi = i + 0.5f;
					newj = j + 0.5f;
				}

				// Objective 1: generate rays
				glm::vec3 point = pp + (newi * pw) + (newj * ph);

				// Depth-of-field?
				if (this->dofSamples != 0)
				{
					// Offset from camera position by a random x, y in the camera basis
					glm::vec3 su = (u * this->cam->aperture);
					glm::vec3 sv = (v * this->cam->aperture);
					glm::vec3 random = glm::vec3(genDOF(rng), genDOF(rng), 0.0f);
					
					// Point this ray from that position to pixel in frame
					ray->origin = cam->position + (random.x * su) + (random.y * sv);
					ray->direction = glm::normalize(point - ray->origin);
				}
				else
				{
					// Point this ray from camera position to pixel in frame
					ray->origin = cam->position;
					ray->direction = glm::normalize(point - ray->origin);
				}

				// Test for intersection with scene surfaces
				findClosestIntersection(ray, intersection, closestIntersection);

				// Objective 3: compute the shaded result for the intersection point
				glm::vec3 shading = getShading(ray, intersection, closestIntersection, 0);
				colour.x += shading.x;
				colour.y += shading.y;
				colour.z += shading.z;

				closestIntersection->reset();
			}

			// Account for samples
			colour.x = colour.x / numRaysPerPixel;
			colour.y = colour.y / numRaysPerPixel;
			colour.z = colour.z / numRaysPerPixel;

			// Clamp colour values to 1
			colour.r = glm::min(1.0f, colour.r);
			colour.g = glm::min(1.0f, colour.g);
			colour.b = glm::min(1.0f, colour.b);

			// Write pixel colour to image
			colour *= 255;
			image->setPixel(i, j, colour.r, colour.g, colour.b);
		}
	}

	auto stop = std::chrono::high_resolution_clock::now();
	auto duration = std::chrono::duration_cast<std::chrono::minutes>(stop - start);

	std::cout << "Time taken by render: " << duration.count() << " minutes" << std::endl;

	image->writeToFile( outputFilename );
}
