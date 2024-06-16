#pragma once

#include "DrawComponent.h"
#include <vector>
#include <glm/gtc/matrix_transform.hpp>
#include "tigl.h"
using tigl::Vertex;

class OceanComponent : public DrawComponent {
	const glm::vec4 top = glm::vec4(0.67450980392f, 0.94901960784f, 0.89803921568f, 1); // Very light cyan
	//const glm::vec4 middle = glm::vec4(0.16470588235f, 0.69019607843f, 0.54901960784f, 1); // Cyan
	const glm::vec4 middle = glm::vec4(0.06666666666f, 0.40392156862f, 0.54901960784f, 1); // Turquoise 
	const glm::vec4 bottom = glm::vec4(0.01176470588f, 0.15686274509f, 0.21960784313f, 1); // Dark blue 
	const int heightSampleIterations = 5;

	std::vector<std::vector<glm::vec3>> heightMap;
	std::vector<std::vector<glm::vec3>> normals;
	std::vector<Vertex> verts;
	tigl::VBO* vbo;

	glm::vec3* cameraPos;

	struct GerstnerWaveParams {
		float radius = 0.0f;
		float steepness = 0.0f;
		float waveLength = 0.1f;
		float phase = 0.0f;
		glm::vec2 direction = glm::vec2(1.0f, 0.0f);
		float speed = 1.0f;
	};

	float totalWaveHeight = 0.0f;
	int renderDistance = 120;
	int size;
	float phase = 0;
public:
	std::vector<GerstnerWaveParams> waveParams;
	static float waveSpeedMultiplier;

	OceanComponent(glm::vec3& cameraPos, int size);
	~OceanComponent();

	float getHeight(float x, float y);
	glm::vec3 getNormal(float x, float y);

	virtual void draw(glm::mat4 parentMatrix) override;
	virtual void update(float deltaTime) override;
private:
	void setUpVertices();
	void setUpTextureCoordinates();
	void setUpNormals();
	glm::vec4 getColor(float height);
	void calculateLoopBounds(int size, int renderDistance, int& startX, int& endX, int& startY, int& endY);
	glm::vec3 calculateVertex(glm::vec3 pos);
	glm::vec3 calculateNormal(glm::vec3 pos);
};