#include "OceanComponent.h"
#include <iostream>

OceanComponent::OceanComponent(int size) : size(size), vbo(nullptr)
{
	heightMap.resize(size, std::vector<glm::vec3>(size, glm::vec3(0.0f)));
	normals.resize(size, std::vector<glm::vec3>(size, glm::vec3(0.0f)));
	verts.reserve(size * size * 4);


	for (int x = 0; x < size; ++x) {
		for (int y = 0; y < size; ++y) {
			heightMap[x][y] = glm::vec3(x - (size / 2), sin(x * 0.2) * 3, y - (size / 2));
		}
	}

	std::cout << "Created heightmap" << std::endl;

	setUpVertices();
}

OceanComponent::~OceanComponent()
{
}

void OceanComponent::setUpVertices()
{
	verts.clear();

	for (int x = 0; x < size - 1; x++) {
		for (int y = 0; y < size - 1; y++) {
			//// "Pixalated"
			//glm::vec4 color = getColor(heightMap[x][y].y);
			//// Update vertex positions based on the updated heightMap
			//verts.push_back(Vertex::PCN(heightMap[x][y] * scale, color, normals[x][y]));
			//verts.push_back(Vertex::PCN(heightMap[x + 1][y] * scale, color, normals[x + 1][y]));
			//verts.push_back(Vertex::PCN(heightMap[x + 1][y + 1] * scale, color, normals[x + 1][y + 1]));
			//verts.push_back(Vertex::PCN(heightMap[x][y + 1] * scale, color, normals[x][y + 1]));

			// Quads
			verts.push_back(Vertex::PCN(heightMap[x][y], getColor(heightMap[x][y].y), normals[x][y]));
			verts.push_back(Vertex::PCN(heightMap[x + 1][y], getColor(heightMap[x + 1][y].y), normals[x + 1][y]));
			verts.push_back(Vertex::PCN(heightMap[x + 1][y + 1], getColor(heightMap[x + 1][y + 1].y), normals[x + 1][y + 1]));
			verts.push_back(Vertex::PCN(heightMap[x][y + 1], getColor(heightMap[x][y + 1].y), normals[x][y + 1]));

			//// TriangleStrip
			//// FAIL
			//verts.push_back(Vertex::PCN(heightMap[x][y] * scale, getColor(heightMap[x][y].y), normals[x][y]));
			//verts.push_back(Vertex::PCN(heightMap[x][y + 1] * scale, getColor(heightMap[x][y + 1].y), normals[x][y + 1]));
			//verts.push_back(Vertex::PCN(heightMap[x + 1][y] * scale, getColor(heightMap[x + 1][y].y), normals[x + 1][y]));
			//verts.push_back(Vertex::PCN(heightMap[x + 1][y + 1] * scale, getColor(heightMap[x + 1][y + 1].y), normals[x + 1][y + 1]));
		}
	}

	if (vbo) {
		delete vbo; // Delete the existing VBO to avoid memory leaks
		vbo = nullptr;
	}
	vbo = tigl::createVbo(verts);
}

void OceanComponent::setUpTextureCoordinates()
{
}

void OceanComponent::setUpNormals()
{
}

glm::vec4 OceanComponent::getColor(float height)
{
	return glm::vec4(1.0f);
}

void OceanComponent::draw()
{
	glm::mat4 modelMatrix = glm::mat4(1.0f);
	modelMatrix = glm::scale(modelMatrix, glm::vec3(0.1f)); // Mogelijk slecht idee
	tigl::shader->setModelMatrix(modelMatrix);
	tigl::drawVertices(GL_QUADS, vbo);
}

void OceanComponent::update(float deltaTime) {
	phase += deltaTime;


	for (int x = 0; x < size; ++x) {
		for (int y = 0; y < size; ++y) {
			heightMap[x][y] = glm::vec3(x - (size / 2), sin(x * 0.2f + phase) * 3, y - (size / 2));
		}
	}

	setUpVertices();
}
