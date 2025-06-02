#pragma once
#ifndef MESH_H
#define MESH_H

#include <string>
#include <memory>

#include "Shape.h"
#include "Material.h"

/**
 * A class to build a triangle mesh.
 * A shape defined by a list of triangles
 * - posBuf should be of length 3*ntris
 * - norBuf should be of length 3*ntris (if normals are available)
 * - texBuf should be of length 2*ntris (if texture coords are available)
 * Modified by Christina Pilip
 */ 
class Mesh : public Shape, public std::enable_shared_from_this<Mesh>
{
public:
	Mesh(const std::string& _meshName);
	Mesh(const std::string& _meshName, glm::vec3 _position);

	virtual ~Mesh();

	void loadOBJ(const std::string &meshName);
	
	void intersect(const std::shared_ptr<Ray> ray, std::shared_ptr<IntersectionData> intersection);

	void translate(glm::vec3 position);

	void scale(float s); // Uniform scale

	std::string filepath;

private:
	std::vector<float> posBuf;
	std::vector<float> norBuf;
	std::vector<float> texBuf;

	int faceCount = 0;
};

#endif
