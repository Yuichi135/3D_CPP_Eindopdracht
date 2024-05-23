#include "PhysicsComponent.h"
#include "Object.h"
#include <iostream>

PhysicsComponent::PhysicsComponent()
{
}

PhysicsComponent::~PhysicsComponent()
{
}

void PhysicsComponent::applyForce(const glm::vec3& force)
{
	acceleration += force;
}

void PhysicsComponent::update(float deltaTime)
{
	if (positionOld == glm::vec3(FLT_MAX, FLT_MAX, FLT_MAX))
		positionOld = parentObject->position;


	std::cout << "OLD: " << positionOld.y << std::endl;
	std::cout << "CURRENT: " << parentObject->position.y << std::endl << std::endl;

	applyForce(glm::vec3(0.0f, -9.81, 0.0f));

	//deltaTime = 1;

	acceleration *= deltaTime * deltaTime;

	glm::vec3 velocity = parentObject->position - positionOld;

	//std::cout << velocity.y << std::endl;

	positionOld = parentObject->position;

	parentObject->position += velocity + acceleration;

	acceleration = glm::vec3(0.0f);
}
