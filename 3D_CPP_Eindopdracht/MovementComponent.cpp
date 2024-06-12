#include "MovementComponent.h"

#include "Object.h"
#include <iostream>
#include <GLFW/glfw3.h>
#include "WindowManager.h"

MovementComponent::MovementComponent(float speed) : speed(speed)
{
	window = WindowManager::getInstance().getWindow();;
}

void MovementComponent::update(float deltaTime)
{
	if (direction == FLT_MAX)
		direction = glm::radians(0.0f);


	if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
		wheelDirection += 0.025f * deltaTime;
	if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
		wheelDirection -= 0.025f * deltaTime;
	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
		speed += 0.5f * deltaTime;
	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
		speed -= 0.5f * deltaTime;

	// Is zolaag omdat het radians zijn maar heb geen zin om het telkens te berekenen
	if (wheelDirection > 0.015f)
		wheelDirection = 0.015f;
	if (wheelDirection < -0.015f)
		wheelDirection = -0.015f;

	if (speed > 1.5f)
		speed = 1.0f;
	if (speed < -0.1f)
		speed = 0.1f;

	direction += wheelDirection;

	glm::vec3 movement(0.0f);
	movement.x = speed * std::cos(direction + glm::radians(90.0f));
	movement.z = speed * std::sin(direction + glm::radians(90.0f));

	parentObject->position += movement * deltaTime;
}
