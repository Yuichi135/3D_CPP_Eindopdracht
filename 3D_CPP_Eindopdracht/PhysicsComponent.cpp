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

	glm::vec2 gridPositions[9] = {
		glm::vec2(-1.0f, -1.0f),
		glm::vec2(0.0f, -1.0f),
		glm::vec2(1.0f, -1.0f),
		glm::vec2(-1.0f, 0.0f),
		glm::vec2(0.0f, 0.0f),
		glm::vec2(1.0f, 0.0f),
		glm::vec2(-1.0f, 1.0f),
		glm::vec2(0.0f, 1.0f),
		glm::vec2(1.0f, 1.0f)
	};

	const float waterDensity = 1.0f;
	const float gravity = 9.81f * 2;

	glm::vec3 totalBuoyancyForce(0.0f);
	glm::vec3 totalTorque(0.0f);

	glm::vec3 normals[9];
	glm::vec3 globalPoints[9];
	glm::vec3 averageNormal(0.0f);

	// Transform grid positions to world space and get normals
	for (int i = 0; i < 9; i++) {
		glm::vec3 localPointPos = glm::vec3(gridPositions[i].x, 0.0f, gridPositions[i].y);
		glm::vec4 transformedPoint = parentObject->getModelMatrix() * glm::vec4(localPointPos, 1.0f);
		globalPoints[i] = glm::vec3(transformedPoint);

		glm::vec3 normal = ocean->getNormal(globalPoints[i].x, globalPoints[i].z);
		normals[i] = normal;
		averageNormal += normal;

		float waterLevel = ocean->getHeight(globalPoints[i].x, globalPoints[i].z);

		if (globalPoints[i].y < waterLevel) {
			float submergedVolume = (waterLevel - globalPoints[i].y);
			glm::vec3 buoyancyForce(0.0f, waterDensity * submergedVolume * gravity, 0.0f);
			totalBuoyancyForce += buoyancyForce;
		}
	}

	// Apply buoyancy force
	applyForce(totalBuoyancyForce);
	applyForce(glm::vec3(0.0f, -gravity, 0.0f));

	// Update position
	glm::vec3 velocity = parentObject->position - positionOld;
	velocity *= 0.98f; // Damping

	positionOld = parentObject->position;

	parentObject->position += velocity + (acceleration * deltaTime * deltaTime);

	// Calculate average normal
	averageNormal = glm::normalize(averageNormal / 9.0f);

	// Calculate the desired rotation to align the cube's up vector with the average normal
	glm::vec3 upVector(0.0f, 1.0f, 0.0f);
	glm::vec3 axis = glm::cross(upVector, averageNormal);
	float angle = acos(glm::dot(upVector, averageNormal));

	if (glm::length(axis) > 0.0f) {
		axis = glm::normalize(axis);
	}

	// Calculate the rotation angles
	glm::vec3 desiredRotation = angle * axis;

	// Calculate rotation difference
	glm::vec3 rotationDifference = desiredRotation - parentObject->rotation;

	// Apply proportional control to adjust angular velocity
	float proportionalGain = 0.2f; // Increase this value to increase the strength of the force
	parentObject->rotation += proportionalGain * rotationDifference * deltaTime;

	// Reset acceleration
	acceleration = glm::vec3(0.0f);
}