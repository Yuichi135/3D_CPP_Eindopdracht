#pragma once
#include "Component.h"
#include "Object.h"
#include <glm/gtc/matrix_transform.hpp>

class PhysicsComponent : public Component
{
	glm::vec3 positionOld = glm::vec3(FLT_MAX, FLT_MAX, FLT_MAX);
	glm::vec3 acceleration = glm::vec3(0.0f);

public:
	PhysicsComponent();
	~PhysicsComponent();

	void applyForce(const glm::vec3& force);
	virtual void update(float deltaTime) override;
};