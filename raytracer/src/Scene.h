#pragma once
#ifndef SCENE_H
#define SCENE_H

#include <string>
#include <vector>
#include <memory>
#include <random>

#include "Image.h"

class Shape;
class Camera;
class Light;
class Material;
class IntersectionData;
class Ray;

#define _USE_MATH_DEFINES
#include <math.h>

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace std;

/**
 * This class handles the scene.
 * This is where iterating over all the pixels happens (aka the render loop).
 * By Loïc Nassif
 * Modified by Christina Pilip
 */
class Scene
{
public:
	Scene(const std::string SCENE_PATH);
	virtual ~Scene();

	void init();
	void render();
	void addObject(shared_ptr<Shape> shape);
	void addLight(shared_ptr<Light> light);

	// Added for rendering
	void findClosestIntersection(const shared_ptr<Ray>& ray, shared_ptr<IntersectionData> intersection, shared_ptr<IntersectionData> closest);
	glm::vec3 getLightingContribution(const shared_ptr<Ray>& ray, shared_ptr<IntersectionData> intersection, const shared_ptr<IntersectionData>& closest);
	glm::vec3 getShading(const shared_ptr<Ray>& ray, shared_ptr<IntersectionData> intersection, shared_ptr<IntersectionData>& closest, int depth);

	
	int width, height; // Width and height of image in pixels
	float aspect; // Aspect ratio

	shared_ptr<Camera> cam; // The scene's camera
	bool jitter = false; // Whether to jitter samples (for antialiasing)

	glm::vec3 ambient; // Ambient lighting 

	std::vector<shared_ptr<Shape>> shapes; // The scene's objects

	// For random number generation
	std::mt19937_64 rng;

	// Options for various effects
	bool useFresnel = false;
	bool useDOF = false;

	// Number of samples for anti-aliasing and for depth-of-field
	int aaSamples = 0;
	int dofSamples = 0;

private:
	std::vector<shared_ptr<Light>> lights; // The scene's lights

	shared_ptr<Image> image; // The scene's image
	std::string outputFilename; // Scene output filename (should end in .png)
};

#endif
