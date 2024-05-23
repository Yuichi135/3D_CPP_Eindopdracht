#pragma once

#include <list>
#include <glm/glm.hpp>
#include <memory>

// Forward declarations
class Component;
class DrawComponent;

class Object
{
	std::shared_ptr<DrawComponent> drawComponent;
	std::list<std::shared_ptr<Component>> components;

public:
	Object();
	~Object();

	glm::vec3 position = glm::vec3(0.0f);
	glm::vec3 rotation = glm::vec3(0.0f);
	glm::vec3 scale = glm::vec3(1.0f);

	void addComponent(std::shared_ptr<Component> component);
	void removeComponent(std::shared_ptr<Component> component);
	std::list<std::shared_ptr<Component>> getComponents();
	void update(float deltaTime);
	void draw(const glm::mat4 & = glm::mat4(1.0f));

	template <class T>
	std::shared_ptr<T> getComponent()
	{
		for (auto& c : components)
		{
			std::shared_ptr<T> t = std::dynamic_pointer_cast<T>(c);
			if (t) return t;
		}
		return nullptr;
	}

	template <class T>
	void removeComponent()
	{
		components.remove_if([&](std::shared_ptr<Component> c)
			{
				std::shared_ptr<T> t = std::dynamic_pointer_cast<T>(c);
				return t != nullptr;
			});
	}
};
