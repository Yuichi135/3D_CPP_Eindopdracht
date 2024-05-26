#include "PhysicsComponent.h"
#include "Object.h"
#include <iostream>
#include "OceanComponent.h"
PhysicsComponent::PhysicsComponent(std::shared_ptr<OceanComponent> ocean) : ocean(ocean) {}

PhysicsComponent::~PhysicsComponent() {}

void PhysicsComponent::applyForce(const glm::vec3& force) {
	acceleration += force;
}

void PhysicsComponent::update(float deltaTime) {
	if (positionOld == glm::vec3(FLT_MAX)) {
		// For initialization, set positionOld to the current position
		positionOld = parentObject->position;
		return;
	}

	const float waterDensity = 2.0f;
	const float gravity = 9.81f * 2;

	float submergedVolume = 0.0f;
	float waterLevel = ocean->getHeight(parentObject->position.x, parentObject->position.z);

	if (parentObject->position.y < waterLevel) {
		submergedVolume = (waterLevel - parentObject->position.y);
	}

	glm::vec3 buoyancyForce(0.0f, waterDensity * submergedVolume * gravity, 0.0f);

	applyForce(buoyancyForce);
	applyForce(glm::vec3(0.0f, -gravity, 0.0f));

	glm::vec3 velocity = parentObject->position - positionOld;
	velocity *= 0.98; // shitty drag

	positionOld = parentObject->position;

	parentObject->position += velocity + (acceleration * deltaTime * deltaTime);

	acceleration = glm::vec3(0.0f);
}