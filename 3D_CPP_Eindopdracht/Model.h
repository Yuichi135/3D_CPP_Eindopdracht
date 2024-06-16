#pragma once

#include <string>
#include <vector>
#include <list>
#include <glm/glm.hpp>

class Texture;

class Model {
public:
	class Vertex {
	public:
		int position;
		int normal;
		int texcoord;
	};

	class Face {
	public:
		std::list<Vertex> vertices;
	};

	class MaterialInfo {
	public:
		MaterialInfo();
		std::string name;
		Texture* texture;
	};

	class ObjGroup {
	public:
		std::string name;
		int materialIndex;
		std::list<Face> faces;
	};

	std::vector<glm::vec3> vertices;
	std::vector<glm::vec3> normals;
	std::vector<glm::vec2> texcoords;
	std::vector<ObjGroup*> groups;
	std::vector<MaterialInfo*> materials;
};
