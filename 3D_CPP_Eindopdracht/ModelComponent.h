#pragma once
#include "DrawComponent.h"

#include <string>
#include <vector>
#include <list>
#include <tuple>

class Texture;

class ModelComponent : public DrawComponent
{
private:
	class Vertex
	{
	public:
		int position;
		int normal;
		int texcoord;
	};

	class Face
	{
	public:
		std::list<Vertex> vertices;
	};
	class MaterialInfo
	{
	public:
		MaterialInfo();
		std::string name;
		Texture* texture;
	};

	class ObjGroup
	{
	public:
		std::string name;
		int materialIndex;
		std::list<Face> faces;
	};

	std::vector<glm::vec3>	vertices;
	std::vector<glm::vec3>	normals;
	std::vector<glm::vec2>	texcoords;
	std::vector<ObjGroup*> groups;
	std::vector<MaterialInfo*> materials;

	bool boundingBoxInitialized = false;

	void loadMaterialFile(const std::string& fileName, const std::string& dirName);
	void setBoundingBox();
public:
	ModelComponent(const std::string& filename);
	~ModelComponent() = default;

	virtual void update(float deltaTime) override;
	virtual void draw(glm::mat4 parentMatrix) override;
};