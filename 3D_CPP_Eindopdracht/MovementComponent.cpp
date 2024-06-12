#include "MovementComponent.h"

#include "Object.h"

MovementComponent::MovementComponent(float speed) : speed(speed)
{
}

void MovementComponent::update(float deltaTime)
{
	if (direction == FLT_MAX)
		direction = glm::radians(0.0f);

	glm::vec3 movement(0.0f);
	movement.x = speed * std::cos(direction + glm::radians(90.0f));
	movement.z = speed * std::sin(direction + glm::radians(90.0f));

	parentObject->position += movement * deltaTime;
}
