#pragma once
#include "DrawComponent.h"
#include "Model.h"
#include <memory>

class ModelComponent : public DrawComponent {
private:
	std::shared_ptr<Model> model;
	bool boundingBoxInitialized = false;

	void setBoundingBox();
public:
	ModelComponent(const std::string& filename);
	~ModelComponent() = default;

	virtual void update(float deltaTime) override;
	virtual void draw(glm::mat4 parentMatrix) override;
};
