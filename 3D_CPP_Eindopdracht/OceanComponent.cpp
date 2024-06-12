#include "OceanComponent.h"
#include <iostream>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/norm.hpp>
#include <thread>

#include <immintrin.h>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

// Debug
//#include "Object.h"
//#include "CubeComponent.h"

glm::vec2 rotateVector(const glm::vec2& vec, float angle);

float OceanComponent::waveSpeedMultiplier = 1.0f;

OceanComponent::OceanComponent(glm::vec3& cameraPos, int size) : cameraPos(&cameraPos), size(size), vbo(nullptr)
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

	// Werkt goed
	waveParams.push_back(GerstnerWaveParams{ 1.5f, 0.3f, 0.1f, 1.0f, rotateVector(glm::vec2(1.0f, 0.0f), 0.0f), 2.0f });
	waveParams.push_back(GerstnerWaveParams{ 1.2f, 0.2f, 0.1f, 3.4f, rotateVector(glm::vec2(1.0f, 0.0f), 60.0f), 1.8f });
	waveParams.push_back(GerstnerWaveParams{ 0.9f, 0.2f, 0.16f, 1.0f, rotateVector(glm::vec2(1.0f, 0.0f), 90.0f), 1.0f });
	waveParams.push_back(GerstnerWaveParams{ 0.8f, 0.3f, 0.2f, 1.0f, rotateVector(glm::vec2(1.0f, 0.0f), 17.0f), 1.5f });
	waveParams.push_back(GerstnerWaveParams{ 0.7f, 0.3f, 0.3f, 1.0f, rotateVector(glm::vec2(1.0f, 0.0f), -38.0f), 1.5f });

	waveParams.push_back(GerstnerWaveParams{ 0.2f, 0.4f, 0.85f, 1.0f, rotateVector(glm::vec2(1.0f, 0.0f), 59.0f), 2.5f });
	waveParams.push_back(GerstnerWaveParams{ 0.15f, 0.2f, 0.7f, 1.0f, rotateVector(glm::vec2(1.0f, 0.0f), 20.0f), 3.0f });
	waveParams.push_back(GerstnerWaveParams{ 0.1f, 0.2f, 0.7f, 1.0f, rotateVector(glm::vec2(1.0f, 0.0f), -20.0f), 4.0f });


	// waveParams moet een meervoud van 8 zijn vanwege SIMD
	int wavesToAdd = (8 - (waveParams.size() % 8)) % 8;

	for (int i = 0; i < wavesToAdd; i++)
		waveParams.push_back(GerstnerWaveParams{ 0.0f, 0.0f, 1.0f, 0.0f, rotateVector(glm::vec2(1.0f, 0.0f), 0.0f), 1.0f });


	setUpNormals();
	setUpVertices();
}

OceanComponent::~OceanComponent()
{
}

// Big ol bottleneck :(
void OceanComponent::setUpVertices()
{
	verts.clear();

	int startX, endX, startY, endY;
	calculateLoopBounds(size, renderDistance, startX, endX, startY, endY);

	for (int x = startX; x < endX - 1; x++) {
		for (int y = startY; y < endY - 1; y++) {
			////// "Pixalated"
			//glm::vec4 color = getColor(heightMap[x][y].y);
			//// Update vertex positions based on the updated heightMap
			//verts.push_back(Vertex::PCN(heightMap[x][y], color, normals[x][y]));
			//verts.push_back(Vertex::PCN(heightMap[x + 1][y], color, normals[x + 1][y]));
			//verts.push_back(Vertex::PCN(heightMap[x + 1][y + 1], color, normals[x + 1][y + 1]));
			//verts.push_back(Vertex::PCN(heightMap[x][y + 1], color, normals[x][y + 1]));


			// Quads
			verts.push_back(Vertex::PCN(heightMap[x][y], getColor(heightMap[x][y].y), normals[x][y]));
			verts.push_back(Vertex::PCN(heightMap[x + 1][y], getColor(heightMap[x + 1][y].y), normals[x + 1][y]));
			verts.push_back(Vertex::PCN(heightMap[x + 1][y + 1], getColor(heightMap[x + 1][y + 1].y), normals[x + 1][y + 1]));
			verts.push_back(Vertex::PCN(heightMap[x][y + 1], getColor(heightMap[x][y + 1].y), normals[x][y + 1]));


			//// TriangleStrip
			////// FAIL
			//verts.push_back(Vertex::PCN(heightMap[x][y], getColor(heightMap[x][y].y), normals[x][y]));
			//verts.push_back(Vertex::PCN(heightMap[x][y + 1], getColor(heightMap[x][y + 1].y), normals[x][y + 1]));
			//verts.push_back(Vertex::PCN(heightMap[x + 1][y], getColor(heightMap[x + 1][y].y), normals[x + 1][y]));
			//verts.push_back(Vertex::PCN(heightMap[x + 1][y + 1], getColor(heightMap[x + 1][y + 1].y), normals[x + 1][y + 1]));
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

void OceanComponent::calculateLoopBounds(int size, int renderDistance, int& startX, int& endX, int& startY, int& endY)
{
	startX = (size / 2) - renderDistance - cameraPos->x;
	endX = (size / 2) + renderDistance - cameraPos->x;
	startY = (size / 2) - renderDistance - cameraPos->z;
	endY = (size / 2) + renderDistance - cameraPos->z;

	startX = std::max(0, startX);
	endX = std::min(size, endX);
	startY = std::max(0, startY);
	endY = std::min(size, endY);
}

// Reference gebruiken misschien
glm::vec3 OceanComponent::calculateVertex(glm::vec3 pos)
{
	float x = pos.x;
	float y = pos.z;

	__m256 combinedX = _mm256_setzero_ps();
	__m256 combinedY = _mm256_setzero_ps();
	__m256 combinedHeight = _mm256_setzero_ps();

	for (size_t i = 0; i < waveParams.size(); i += 8) {
		// Load wave parameters into SIMD registers
		__m256 pDirectionX = _mm256_set_ps(
			waveParams[i + 7].direction.x, waveParams[i + 6].direction.x, waveParams[i + 5].direction.x,
			waveParams[i + 4].direction.x, waveParams[i + 3].direction.x, waveParams[i + 2].direction.x,
			waveParams[i + 1].direction.x, waveParams[i].direction.x);

		__m256 pDirectionY = _mm256_set_ps(
			waveParams[i + 7].direction.y, waveParams[i + 6].direction.y, waveParams[i + 5].direction.y,
			waveParams[i + 4].direction.y, waveParams[i + 3].direction.y, waveParams[i + 2].direction.y,
			waveParams[i + 1].direction.y, waveParams[i].direction.y);

		__m256 pPhase = _mm256_set_ps(
			waveParams[i + 7].phase, waveParams[i + 6].phase, waveParams[i + 5].phase,
			waveParams[i + 4].phase, waveParams[i + 3].phase, waveParams[i + 2].phase,
			waveParams[i + 1].phase, waveParams[i].phase);

		__m256 pSpeed = _mm256_set_ps(
			waveParams[i + 7].speed, waveParams[i + 6].speed, waveParams[i + 5].speed,
			waveParams[i + 4].speed, waveParams[i + 3].speed, waveParams[i + 2].speed,
			waveParams[i + 1].speed, waveParams[i].speed);

		__m256 pWaveLength = _mm256_set_ps(
			waveParams[i + 7].waveLength, waveParams[i + 6].waveLength, waveParams[i + 5].waveLength,
			waveParams[i + 4].waveLength, waveParams[i + 3].waveLength, waveParams[i + 2].waveLength,
			waveParams[i + 1].waveLength, waveParams[i].waveLength);

		__m256 pSteepness = _mm256_set_ps(
			waveParams[i + 7].steepness, waveParams[i + 6].steepness, waveParams[i + 5].steepness,
			waveParams[i + 4].steepness, waveParams[i + 3].steepness, waveParams[i + 2].steepness,
			waveParams[i + 1].steepness, waveParams[i].steepness);

		__m256 pRadius = _mm256_set_ps(
			waveParams[i + 7].radius, waveParams[i + 6].radius, waveParams[i + 5].radius,
			waveParams[i + 4].radius, waveParams[i + 3].radius, waveParams[i + 2].radius,
			waveParams[i + 1].radius, waveParams[i].radius);

		__m256 x_vec = _mm256_set1_ps(static_cast<float>(x));
		__m256 y_vec = _mm256_set1_ps(static_cast<float>(y));

		// Calculate dotProduct
		__m256 dotProduct = _mm256_add_ps(_mm256_mul_ps(pDirectionX, x_vec), _mm256_mul_ps(pDirectionY, y_vec));

		// Calculate phaseDelta
		__m256 phaseDelta = _mm256_mul_ps(_mm256_add_ps(pPhase, _mm256_set1_ps(phase)), pSpeed);

		// Calculate angle
		__m256 angle = _mm256_add_ps(_mm256_mul_ps(pWaveLength, dotProduct), phaseDelta);

		__m256 cosAngle = _mm256_cos_ps(angle);
		__m256 sinAngle = _mm256_sin_ps(angle);

		// Update combined values
		combinedX = _mm256_add_ps(combinedX, _mm256_mul_ps(_mm256_div_ps(pSteepness, pWaveLength), _mm256_mul_ps(pDirectionX, cosAngle)));
		combinedY = _mm256_add_ps(combinedY, _mm256_mul_ps(_mm256_div_ps(pSteepness, pWaveLength), _mm256_mul_ps(pDirectionY, cosAngle)));
		combinedHeight = _mm256_add_ps(combinedHeight, _mm256_mul_ps(pRadius, sinAngle));
	}

	// Extract results from SIMD registers
	float combinedX_scalar[8];
	float combinedY_scalar[8];
	float combinedHeight_scalar[8];

	_mm256_store_ps(combinedX_scalar, combinedX);
	_mm256_store_ps(combinedY_scalar, combinedY);
	_mm256_store_ps(combinedHeight_scalar, combinedHeight);

	// Sum up the results
	float finalCombinedX = 0;
	float finalCombinedY = 0;
	float finalCombinedHeight = 0;
	for (int i = 0; i < 8; ++i) {
		finalCombinedX += combinedX_scalar[i];
		finalCombinedY += combinedY_scalar[i];
		finalCombinedHeight += combinedHeight_scalar[i];
	}

	glm::vec3 worldPos;

	// Update vertex position based on the combined Gerstner wave displacements
	worldPos.x = x + finalCombinedX;
	worldPos.y = pos.y + finalCombinedHeight;
	worldPos.z = y + finalCombinedY;

	return worldPos;
}

glm::vec3 OceanComponent::calculateNormal(glm::vec3 pos)
{
	float x = pos.x;
	float y = pos.z;

	__m256 combinedNx = _mm256_setzero_ps();
	__m256 combinedNy = _mm256_setzero_ps();

	for (size_t i = 0; i < waveParams.size(); i += 8) {
		// Load wave parameters into SIMD registers
		__m256 pDirectionX = _mm256_set_ps(
			waveParams[i + 7].direction.x, waveParams[i + 6].direction.x, waveParams[i + 5].direction.x,
			waveParams[i + 4].direction.x, waveParams[i + 3].direction.x, waveParams[i + 2].direction.x,
			waveParams[i + 1].direction.x, waveParams[i].direction.x);

		__m256 pDirectionY = _mm256_set_ps(
			waveParams[i + 7].direction.y, waveParams[i + 6].direction.y, waveParams[i + 5].direction.y,
			waveParams[i + 4].direction.y, waveParams[i + 3].direction.y, waveParams[i + 2].direction.y,
			waveParams[i + 1].direction.y, waveParams[i].direction.y);

		__m256 pPhase = _mm256_set_ps(
			waveParams[i + 7].phase, waveParams[i + 6].phase, waveParams[i + 5].phase,
			waveParams[i + 4].phase, waveParams[i + 3].phase, waveParams[i + 2].phase,
			waveParams[i + 1].phase, waveParams[i].phase);

		__m256 pSpeed = _mm256_set_ps(
			waveParams[i + 7].speed, waveParams[i + 6].speed, waveParams[i + 5].speed,
			waveParams[i + 4].speed, waveParams[i + 3].speed, waveParams[i + 2].speed,
			waveParams[i + 1].speed, waveParams[i].speed);

		__m256 pWaveLength = _mm256_set_ps(
			waveParams[i + 7].waveLength, waveParams[i + 6].waveLength, waveParams[i + 5].waveLength,
			waveParams[i + 4].waveLength, waveParams[i + 3].waveLength, waveParams[i + 2].waveLength,
			waveParams[i + 1].waveLength, waveParams[i].waveLength);

		__m256 x_vec = _mm256_set1_ps(static_cast<float>(x));
		__m256 y_vec = _mm256_set1_ps(static_cast<float>(y));

		__m256 dotProduct = _mm256_add_ps(_mm256_mul_ps(pDirectionX, x_vec), _mm256_mul_ps(pDirectionY, y_vec));

		__m256 phaseDelta = _mm256_mul_ps(_mm256_add_ps(pPhase, _mm256_set1_ps(phase)), pSpeed);

		__m256 angle = _mm256_add_ps(_mm256_mul_ps(pWaveLength, dotProduct), phaseDelta);

		__m256 cosAngle = _mm256_cos_ps(angle);

		combinedNx = _mm256_add_ps(combinedNx, _mm256_mul_ps(_mm256_div_ps(pDirectionX, pWaveLength), cosAngle));
		combinedNy = _mm256_add_ps(combinedNy, _mm256_mul_ps(_mm256_div_ps(pDirectionY, pWaveLength), cosAngle));
	}

	float combinedNx_scalar[8];
	float combinedNy_scalar[8];

	_mm256_store_ps(combinedNx_scalar, combinedNx);
	_mm256_store_ps(combinedNy_scalar, combinedNy);

	float finalCombinedNx = 0;
	float finalCombinedNy = 0;
	for (int i = 0; i < 8; ++i) {
		finalCombinedNx += combinedNx_scalar[i];
		finalCombinedNy += combinedNy_scalar[i];
	}

	// Compute the normal
	glm::vec3 normal = glm::normalize(glm::vec3(-finalCombinedNx, 1.0f, -finalCombinedNy));

	return normal;
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

// Raar gedrag bij lange diepe swells
float OceanComponent::getHeight(float x, float y) {
	int iterations = 5;
	glm::vec3 position(x, 0.0f, y);
	glm::vec3 displacement(0.0f, 0.0f, 0.0f);
	float height = 0.0f;

	for (int i = 0; i < iterations; i++) {
		glm::vec3 sample = calculateVertex(position + displacement);

		displacement.x -= sample.x - position.x;
		displacement.z -= sample.z - position.z;

		height = sample.y;

		// Voor debug (Draw moet dan wel VOOR update komen)
		//std::shared_ptr<Object> cube = std::make_shared<Object>();
		//cube->position = glm::vec3(sample);
		//cube->addComponent(std::make_shared<CubeComponent>(1.0f));
		//cube->draw();
	}

	return height;
}

glm::vec3 OceanComponent::getNormal(float x, float y)
{
	return calculateNormal(glm::vec3(x, 0.0f, y));
}

void OceanComponent::draw(glm::mat4 parentMatrix)
{
	tigl::shader->enableTexture(false);
	glm::mat4 modelMatrix = parentMatrix;
	modelMatrix = glm::scale(modelMatrix, glm::vec3(1.0f)); // Mogelijk slecht idee
	tigl::shader->setModelMatrix(modelMatrix);
	tigl::drawVertices(GL_QUADS, vbo);
}

// Todo Don't calculated behind camera
// Todo use heightmap dynamically, not a fixed size (uses a lot of memory)
void OceanComponent::update(float deltaTime) {
	phase += deltaTime * waveSpeedMultiplier;

	// Don't do the calculations if to high
	if (cameraPos->y > renderDistance || cameraPos->y < -renderDistance)
		return;

	int startX, endX, startY, endY;
	calculateLoopBounds(size, renderDistance, startX, endX, startY, endY);

	const size_t numThreads = std::thread::hardware_concurrency();
	const size_t chunkSizeX = (endX - startX) / numThreads;
	const size_t chunkSizeY = (endY - startY) / numThreads;

	std::vector<std::thread> threads;
	threads.reserve(numThreads);

	for (size_t t = 0; t < numThreads; ++t) {
		threads.emplace_back([=]() {
			const size_t threadStartX = startX + t * chunkSizeX;
			const size_t threadEndX = (t == numThreads - 1) ? endX : (startX + (t + 1) * chunkSizeX);

			for (int x = threadStartX; x < threadEndX; ++x) {
				for (int y = startY; y < endY; ++y) {
					heightMap[x][y] = calculateVertex(glm::vec3(x - (size / 2.0f), 0, y - (size / 2.0f)));
				}
			}
			});
	}

	for (auto& thread : threads) {
		thread.join();
	}

	//setUpNormals();
	setUpVertices();
}
