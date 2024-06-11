#include "object.h"
#include "tigl.h"
#include <glm/gtc/matrix_transform.hpp>
#include "DrawComponent.h"
#include <memory>

Object::Object() = default;

Object::~Object() = default;

glm::mat4 Object::getModelMatrix(const glm::mat4& parentMatrix)
{
	glm::mat4 modelMatrix = parentMatrix;
	modelMatrix = glm::translate(modelMatrix, position);
	modelMatrix = glm::rotate(modelMatrix, rotation.x, glm::vec3(1, 0, 0));
	modelMatrix = glm::rotate(modelMatrix, rotation.y, glm::vec3(0, 1, 0));
	modelMatrix = glm::rotate(modelMatrix, rotation.z, glm::vec3(0, 0, 1));
	modelMatrix = glm::scale(modelMatrix, scale);

	return modelMatrix;
}

void Object::addComponent(std::shared_ptr<Component> component)
{
	component->setParentObject(this);
	components.push_back(component);

	if (drawComponent == nullptr)
	{
		drawComponent = std::dynamic_pointer_cast<DrawComponent>(component);
	}

}

void Object::removeComponent(std::shared_ptr<Component> component)
{
	components.remove(component);
}

std::list<std::shared_ptr<Component>> Object::getComponents()
{
	return components;
}

void Object::draw(const glm::mat4& parentMatrix)
{
	if (!drawComponent) return;

	auto modelMatrix = getModelMatrix(parentMatrix);

	tigl::shader->setModelMatrix(modelMatrix);

	tigl::shader->enableTexture(true);

	drawComponent->draw(modelMatrix);

	tigl::shader->enableTexture(false);
}

void Object::update(float deltaTime)
{
	for (auto& c : components)
	{
		c->update(deltaTime);
	}
}
