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
	setUpNormals();
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
	std::vector<std::vector<glm::vec3>> faceNormals(size - 1, std::vector<glm::vec3>(size - 1, glm::vec3(0.0f)));

	for (int x = 0; x < size - 1; ++x) {
		for (int y = 0; y < size - 1; ++y) {
			glm::vec3 p1 = heightMap[x][y];
			glm::vec3 p2 = heightMap[x + 1][y];
			glm::vec3 p3 = heightMap[x][y + 1];

			glm::vec3 normal = glm::normalize(glm::cross(p1 - p2, p1 - p3));

			faceNormals[x][y] = normal;
		}
	}

	for (int x = 0; x < size; ++x) {
		for (int y = 0; y < size; ++y) {
			glm::vec3 normal(0.0f);
			if (x > 0 && y > 0)
				normal += faceNormals[x - 1][y - 1];
			if (x < size - 1 && y > 0)
				normal += faceNormals[x][y - 1];
			if (x > 0 && y < size - 1)
				normal += faceNormals[x - 1][y];
			if (x < size - 1 && y < size - 1)
				normal += faceNormals[x][y];

			normals[x][y] = glm::normalize(normal);
		}
	}

	std::cout << "Calculated vertex normals" << std::endl;
}

glm::vec4 OceanComponent::getColor(float height)
{
	float p = (height - -3.0f) / (3.0f - -3.0f);

	glm::vec4 top = glm::vec4(0.67450980392f, 0.94901960784f, 0.89803921568f, 1); // Very light cyan
	//glm::vec4 middle = glm::vec4(0.16470588235f, 0.69019607843f, 0.54901960784f, 1); // Cyan
	glm::vec4 middle = glm::vec4(0.06666666666f, 0.40392156862f, 0.54901960784f, 1); // Turquoise 
	glm::vec4 bottom = glm::vec4(0.01176470588f, 0.15686274509f, 0.21960784313f, 1); // Dark blue 

	glm::vec4 color;

	if (p <= 0.8f) {
		// Interpolate between bottom and middle
		float t = p / 0.8f; // Normalize to [0, 1] range
		color = glm::mix(bottom, middle, t);
	}
	else {
		// Interpolate between middle and top
		float t = (p - 0.8f) / 0.2f; // Normalize to [0, 1] range
		color = glm::mix(middle, top, t);
	}

	return color;
}

void OceanComponent::draw()
{
	tigl::shader->enableTexture(false);
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
