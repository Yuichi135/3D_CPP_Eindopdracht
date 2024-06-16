#include "MovementComponent.h"

#include "Object.h"
#include <iostream>
#include <GLFW/glfw3.h>
#include "WindowManager.h"

const float speedIncrement = 0.5f;
const float AngleIncrement = 0.025f;
const float maxSpeed = 1.0f;
const float minSpeed = -0.1f;
const float maxWheelAngle = 0.015f;


MovementComponent::MovementComponent(float speed) : speed(speed)
{
	window = WindowManager::getInstance().getWindow();
}

void MovementComponent::update(float deltaTime)
{
	if (direction == FLT_MAX)
		direction = glm::radians(0.0f);

	if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
		wheelDirection += AngleIncrement * deltaTime;
	if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
		wheelDirection -= AngleIncrement * deltaTime;
	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
		speed += speedIncrement * deltaTime;
	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
		speed -= speedIncrement * deltaTime;

	if (wheelDirection > maxWheelAngle)
		wheelDirection = maxWheelAngle;
	if (wheelDirection < -maxWheelAngle)
		wheelDirection = -maxWheelAngle;

	if (speed > maxSpeed)
		speed = maxSpeed;
	if (speed < minSpeed)
		speed = minSpeed;

	direction += wheelDirection;

	glm::vec3 movement(0.0f);
	movement.x = speed * std::cos(direction + glm::radians(90.0f));
	movement.z = speed * std::sin(direction + glm::radians(90.0f));

	parentObject->position += movement * deltaTime;
}
