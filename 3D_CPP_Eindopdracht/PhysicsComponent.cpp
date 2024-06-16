#include "PhysicsComponent.h"
#include "Object.h"
#include <iostream>
#include <tuple>
#include "OceanComponent.h"
#include "ModelComponent.h"

float PhysicsComponent::waterDensityMultiplier = 1.0f;
float PhysicsComponent::gravityMultiplier = 1.0f;

PhysicsComponent::PhysicsComponent(std::shared_ptr<OceanComponent> ocean, int resolution) : ocean(ocean), resolution(resolution) {

}

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

	const int resSqr = resolution * resolution;
	glm::vec3 minValues = std::get<0>(parentObject->boundingBox);
	glm::vec3 maxValues = std::get<1>(parentObject->boundingBox);

	// Calculate step sizes based on resolution
	float stepX = (maxValues.x - minValues.x) / static_cast<float>(resolution - 1);
	float stepZ = (maxValues.z - minValues.z) / static_cast<float>(resolution - 1);

	std::vector<glm::vec3> points;

	// Generate grid points on the bottom face of the bounding box
	for (int i = 0; i < resolution; ++i) {
		for (int j = 0; j < resolution; ++j) {
			glm::vec3 point(
				minValues.x + i * stepX,
				minValues.y,
				minValues.z + j * stepZ
			);
			point += parentObject->centreOffMassOffset;
			points.push_back(point);
		}
	}

	constexpr float waterDensity = 2.0f; // Beter resultaat
	constexpr float gravity = 9.81f * 2;

	glm::vec3 totalBuoyancyForce(0.0f);
	glm::vec3 totalTorque(0.0f);

	std::vector<glm::vec3> normals(resSqr);
	std::vector<glm::vec3> globalPoints(resSqr);
	glm::vec3 averageNormal(0.0f);

	// Transform grid positions to world space and get normals
	for (int i = 0; i < resSqr; i++) {
		glm::vec3 localPointPos = points[i];
		glm::vec4 transformedPoint = parentObject->getModelMatrix() * glm::vec4(localPointPos, 1.0f);
		globalPoints[i] = glm::vec3(transformedPoint);

		glm::vec3 normal = ocean->getNormal(globalPoints[i].x, globalPoints[i].z);
		normals[i] = normal;
		averageNormal += normal;

		float waterLevel = ocean->getHeight(globalPoints[i].x, globalPoints[i].z);

		if (globalPoints[i].y < waterLevel) {
			float submergedVolume = (waterLevel - globalPoints[i].y);
			glm::vec3 buoyancyForce(0.0f, waterDensity * waterDensityMultiplier * submergedVolume * gravity * gravityMultiplier, 0.0f);
			totalBuoyancyForce += buoyancyForce;
		}
	}

	applyForce(totalBuoyancyForce / (float)resSqr);
	applyForce(glm::vec3(0.0f, -gravity * gravityMultiplier, 0.0f));

	glm::vec3 velocity = parentObject->position - positionOld;
	velocity *= 0.98; // shitty drag

	positionOld = parentObject->position;
	parentObject->position += velocity + (acceleration * deltaTime * deltaTime);
	acceleration = glm::vec3(0.0f);

	// Rotation
	averageNormal = glm::normalize(averageNormal / (float)resSqr);

	// Calculate the desired rotation to align the cube's up vector with the average normal
	glm::vec3 upVector(0.0f, 1.0f, 0.0f);
	glm::vec3 axis = glm::cross(upVector, averageNormal);
	float angle = acos(glm::dot(upVector, averageNormal));

	if (glm::length(axis) > 0.0f)
		axis = glm::normalize(axis);

	glm::vec3 desiredRotation = angle * axis;
	glm::vec3 rotationDifference = desiredRotation - parentObject->rotation;

	// Sensitivity
	float proportionalGain = 0.2f;
	parentObject->rotation += proportionalGain * rotationDifference * deltaTime;
}