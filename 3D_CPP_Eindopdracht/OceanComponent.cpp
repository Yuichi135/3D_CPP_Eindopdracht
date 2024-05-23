#include "OceanComponent.h"
#include <iostream>
#include <glm/gtc/matrix_transform.hpp>
#include <thread>

glm::vec2 rotateVector(const glm::vec2& vec, float angle);

OceanComponent::OceanComponent(int size) : size(size), vbo(nullptr)
{
	heightMap.resize(size, std::vector<glm::vec3>(size, glm::vec3(0.0f)));
	normals.resize(size, std::vector<glm::vec3>(size, glm::vec3(0.0f)));
	verts.reserve(size * size * 4);


	for (int x = 0; x < size; ++x) {
		for (int y = 0; y < size; ++y) {
			heightMap[x][y] = glm::vec3(x - (size / 2), 0.0f, y - (size / 2));
		}
	}

	std::cout << "Created heightmap" << std::endl;

	waveParams.push_back(GerstnerWaveParams{ 1.5f, 0.3f, 0.1f, 1.0f, rotateVector(glm::vec2(1.0f, 0.0f), 0.0f), 2.0f });
	waveParams.push_back(GerstnerWaveParams{ 1.2f, 0.2f, 0.1f, 3.4f, rotateVector(glm::vec2(1.0f, 0.0f), 60.0f), 1.8f });
	waveParams.push_back(GerstnerWaveParams{ 0.9f, 0.2f, 0.16f, 1.0f, rotateVector(glm::vec2(1.0f, 0.0f), 90.0f), 1.0f });
	waveParams.push_back(GerstnerWaveParams{ 0.8f, 0.3f, 0.2f, 1.0f, rotateVector(glm::vec2(1.0f, 0.0f), 17.0f), 1.5f });
	waveParams.push_back(GerstnerWaveParams{ 0.7f, 0.3f, 0.3f, 1.0f, rotateVector(glm::vec2(1.0f, 0.0f), -38.0f), 1.5f });

	waveParams.push_back(GerstnerWaveParams{ 0.2f, 0.4f, 0.85f, 1.0f, rotateVector(glm::vec2(1.0f, 0.0f), 59.0f), 2.5f });
	waveParams.push_back(GerstnerWaveParams{ 0.15f, 0.2f, 0.7f, 1.0f, rotateVector(glm::vec2(1.0f, 0.0f), 20.0f), 3.0f });
	waveParams.push_back(GerstnerWaveParams{ 0.1f, 0.2f, 0.7f, 1.0f, rotateVector(glm::vec2(1.0f, 0.0f), -20.0f), 4.0f });

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
	float p = (height - -3.0f) / (4.0f - -3.0f);

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


glm::vec2 rotateVector(const glm::vec2& vec, float angle) {
	float angleRad = glm::radians(angle);

	// Calculate sine and cosine of the angle
	float cosAngle = cos(angleRad);
	float sinAngle = sin(angleRad);

	glm::vec2 rotatedVec;
	rotatedVec.x = vec.x * cosAngle - vec.y * sinAngle;
	rotatedVec.y = vec.x * sinAngle + vec.y * cosAngle;

	return rotatedVec;
}

void OceanComponent::draw()
{
	tigl::shader->enableTexture(false);
	glm::mat4 modelMatrix = glm::mat4(1.0f);
	modelMatrix = glm::scale(modelMatrix, glm::vec3(1.0f)); // Mogelijk slecht idee
	tigl::shader->setModelMatrix(modelMatrix);
	tigl::drawVertices(GL_QUADS, vbo);
}

void OceanComponent::update(float deltaTime) {
	phase += deltaTime;

	return;

	const size_t numThreads = std::thread::hardware_concurrency(); // Get the number of available CPU cores
	const size_t chunkSize = size / numThreads; // Calculate the chunk size for each thread

	std::vector<std::thread> threads;
	threads.reserve(numThreads);

	for (size_t t = 0; t < numThreads; ++t) {
		threads.emplace_back([=]() {
			const size_t startX = t * chunkSize;
			const size_t endX = (t == numThreads - 1) ? size : (startX + chunkSize);

			for (size_t x = startX; x < endX; ++x) {
				for (size_t y = 0; y < size; ++y) {
					glm::vec3 worldPos(x - (size / 2.0f), 0.0f, y - (size / 2.0f));

					float combinedX = 0;
					float combinedY = 0;
					float combinedHeight = 0;

					for (auto& p : waveParams) {
						const float dotProduct = glm::dot(p.direction, glm::vec2(x, y));
						const float phaseDelta = (p.phase + phase) * p.speed;
						const float angle = p.waveLength * dotProduct + phaseDelta;
						const float cosAngle = cos(angle);
						const float sinAngle = sin(angle);

						combinedX += (p.steepness / p.waveLength) * p.direction.x * cosAngle;
						combinedY += (p.steepness / p.waveLength) * p.direction.y * cosAngle;
						combinedHeight += p.radius * sinAngle;
					}

					glm::vec3 newPos = worldPos + glm::vec3(combinedX, combinedHeight, combinedY);

					// Update vertex position based on the combined Gerstner wave displacements
					heightMap[x][y] = newPos;
				}
			}
			});
	}

	for (auto& thread : threads) {
		thread.join();
	}

	setUpVertices();
}
