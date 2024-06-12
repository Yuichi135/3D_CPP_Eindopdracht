#pragma once
#include "Component.h"
#include "Object.h"
#include "OceanComponent.h"
#include <glm/gtc/matrix_transform.hpp>

class PhysicsComponent : public Component
{
	std::shared_ptr<OceanComponent> ocean;
	glm::vec3 positionOld = glm::vec3(FLT_MAX);
	glm::vec3 acceleration = glm::vec3(0.0f);
	glm::vec3 angularVelocity = glm::vec3(0.0f);

	int resolution;

public:
	PhysicsComponent(std::shared_ptr<OceanComponent> ocean, int resolution = 2);
	~PhysicsComponent();

	void applyForce(const glm::vec3& force);
	virtual void update(float deltaTime) override;
};