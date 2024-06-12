#pragma once
#include <glm/glm.hpp>

struct GLFWwindow;

class CameraController
{
	GLFWwindow* window;
	glm::vec2 rotation = glm::vec2(0, 0);

	void move(float angle, float fac);
	void moveY(float height);

public:
	CameraController();

	glm::mat4 getMatrix();
	void update();

	glm::vec3 position = glm::vec3(0, 0, 0);
};