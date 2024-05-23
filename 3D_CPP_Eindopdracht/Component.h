#pragma once

class Object;

class Component
{
protected:
	Object* parentObject;

public:
	Component();
	virtual ~Component();

	virtual void update(float deltaTime)
	{
	};

	inline void setParentObject(Object* object) { this->parentObject = object; }
};
