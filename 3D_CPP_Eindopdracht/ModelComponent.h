#pragma once
#include "DrawComponent.h"
#include "Model.h"

class ModelComponent : public DrawComponent {
private:
	Model model;
	bool boundingBoxInitialized = false;

	void setBoundingBox();
public:
	ModelComponent(const std::string& filename);
	~ModelComponent() = default;

	virtual void update(float deltaTime) override;
	virtual void draw(glm::mat4 parentMatrix) override;
};
