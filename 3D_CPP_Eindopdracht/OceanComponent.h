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

	int size;
	float phase = 0;
public:
	OceanComponent(int size);
	~OceanComponent();

	virtual void draw() override;
	virtual void update(float deltaTime) override;
private:
	void setUpVertices();
	void setUpTextureCoordinates();
	void setUpNormals();
	glm::vec4 getColor(float height);
};