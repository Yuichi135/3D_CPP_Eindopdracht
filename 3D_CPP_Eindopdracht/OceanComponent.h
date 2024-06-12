#pragma once

#include "DrawComponent.h"
#include <vector>
#include <glm/gtc/matrix_transform.hpp>
#include "tigl.h"
using tigl::Vertex;

class OceanComponent : public DrawComponent {
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