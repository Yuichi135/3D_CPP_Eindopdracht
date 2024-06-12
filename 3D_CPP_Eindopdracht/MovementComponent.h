#pragma once

#include "Component.h"
#include <cfloat>
#include <GLFW/glfw3.h>

class MovementComponent : public Component {
	GLFWwindow* window;

public:
	float direction = FLT_MAX;
	float speed;
	float wheelDirection;

	MovementComponent(float speed);
	~MovementComponent() = default;

	virtual void update(float deltaTime) override;
};