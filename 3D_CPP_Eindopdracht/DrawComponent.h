#pragma once

#include "Component.h"
#include <glm/gtc/matrix_transform.hpp>

class DrawComponent : public Component
{
public:
	DrawComponent();
	~DrawComponent();
	virtual void draw() = 0;
};
