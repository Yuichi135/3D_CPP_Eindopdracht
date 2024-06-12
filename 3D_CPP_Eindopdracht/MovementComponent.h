#pragma once

#include "Component.h"
#include <cfloat>

class MovementComponent : public Component {
public:
	float direction = FLT_MAX;
	float speed;

	MovementComponent(float speed);
	~MovementComponent() = default;

	virtual void update(float deltaTime) override;
};