#include "Mesh.h"
#include "Ray.h"
#include "IntersectionData.h"

#include <iostream>

#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

Mesh::Mesh(const std::string& _meshName) : filepath(_meshName)
{
	loadOBJ(_meshName);
}

Mesh::Mesh(const std::string& _meshName, glm::vec3 _position) : filepath(_meshName)
{
	loadOBJ(_meshName);
	translate(_position);
}

Mesh::~Mesh()
{
}

void Mesh::translate(glm::vec3 position) 
{
	int j = 0;
	for (int i = 0; i < faceCount; i++) 
	{
		j = 9 * (i+1) - 9;
		// v1
		posBuf[j + 0] = posBuf[j + 0] + position.x;
		posBuf[j + 1] = posBuf[j + 1] + position.y;
		posBuf[j + 2] = posBuf[j + 2] + position.z;

		// v2
		posBuf[j + 3] = posBuf[j + 3] + position.x;
		posBuf[j + 4] = posBuf[j + 4] + position.y;
		posBuf[j + 5] = posBuf[j + 5] + position.z;

		// v3
		posBuf[j + 6] = posBuf[j + 6] + position.x;
		posBuf[j + 7] = posBuf[j + 7] + position.y;
		posBuf[j + 8] = posBuf[j + 8] + position.z;
	}
}

void Mesh::scale(float s) 
{
	for (int i = 0; i < faceCount; i++) 
	{
		posBuf[i] = s * posBuf[i];
	}
}

void Mesh::loadOBJ(const std::string& meshName)
{
	// Load geometry
	tinyobj::attrib_t attrib;
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;
	std::string warnStr, errStr;
	bool rc = tinyobj::LoadObj(&attrib, &shapes, &materials, &warnStr, &errStr, meshName.c_str());
	if(!rc) {
		std::cerr << errStr << std::endl;
	} else {
		// Some OBJ files have different indices for vertex positions, normals,
		// and texture coordinates. For example, a cube corner vertex may have
		// three different normals. Here, we are going to duplicate all such
		// vertices.
		// Loop over shapes
		for(size_t s = 0; s < shapes.size(); s++) {
			// Loop over faces (polygons)
			size_t index_offset = 0;
			for(size_t f = 0; f < shapes[s].mesh.num_face_vertices.size(); f++) {
				size_t fv = shapes[s].mesh.num_face_vertices[f];
				// Loop over vertices in the face.
				for(size_t v = 0; v < fv; v++) {
					// access to vertex
					tinyobj::index_t idx = shapes[s].mesh.indices[index_offset + v];
					posBuf.push_back(attrib.vertices[3*idx.vertex_index+0]);
					posBuf.push_back(attrib.vertices[3*idx.vertex_index+1]);
					posBuf.push_back(attrib.vertices[3*idx.vertex_index+2]);
					if(!attrib.normals.empty()) {
						norBuf.push_back(attrib.normals[3*idx.normal_index+0]);
						norBuf.push_back(attrib.normals[3*idx.normal_index+1]);
						norBuf.push_back(attrib.normals[3*idx.normal_index+2]);
					}
					if(!attrib.texcoords.empty()) {
						texBuf.push_back(attrib.texcoords[2*idx.texcoord_index+0]);
						texBuf.push_back(attrib.texcoords[2*idx.texcoord_index+1]);
					}
				}
				index_offset += fv;
				// per-face material (IGNORE)
				shapes[s].mesh.material_ids[f];
			}
		}
	}
	faceCount = int(posBuf.size() / 9.0f);
}

void Mesh::intersect(const std::shared_ptr<Ray> ray, std::shared_ptr<IntersectionData> intersection)
{
	// Objective 9: intersection of ray with triangle mesh
	intersection->shape = shared_from_this();

	// Grab vertices 
	glm::vec3 v1, v2, v3, normal, p;
	float d, t, i1, i2, i3;

	int j = 0;

	for (int i = 0; i < faceCount; i++)
	{
		v1.x = posBuf.at(j);
		v1.y = posBuf.at(j + 1);
		v1.z = posBuf.at(j + 2);
						 
		v2.x = posBuf.at(j + 3);
		v2.y = posBuf.at(j + 4);
		v2.z = posBuf.at(j + 5);
						 
		v3.x = posBuf.at(j + 6);
		v3.y = posBuf.at(j + 7);
		v3.z = posBuf.at(j + 8);
		j += 9;

		// Calculate normal by taking cross product
		normal = glm::normalize(glm::cross(v2 - v1, v3 - v1));

		// Parallel?
		d = glm::dot(ray->direction, normal);

		if (d == 0.0f)
		{
			continue;
		}

		// Behind?
		t = glm::dot(v1 - ray->origin, normal) / d;
		if (t < 0)
		{
			continue;
		}

		ray->computePoint(t, p);

		// Inside?
		i1 = glm::dot(glm::cross(v2 - v1, p - v1), normal);
		i2 = glm::dot(glm::cross(v3 - v2, p - v2), normal);
		i3 = glm::dot(glm::cross(v1 - v3, p - v3), normal);

		if (i1 > 0 && i2 > 0 && i3 > 0)
		{
			if (intersection->t > t)
			{
				intersection->t = t;
				intersection->p = p;
				intersection->n = normal;
				intersection->material = this->materials.at(0);
			}
		}
	}
}
