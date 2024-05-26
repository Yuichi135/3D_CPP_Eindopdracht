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

	int size;
	float phase = 0;
	std::vector<GerstnerWaveParams> waveParams;
public:
	OceanComponent(glm::vec3& cameraPos, int size);
	~OceanComponent();

	float getHeight(int x, int y);

	virtual void draw() override;
	virtual void update(float deltaTime) override;
private:
	void setUpVertices();
	void setUpTextureCoordinates();
	void setUpNormals();
	glm::vec4 getColor(float height);
};