#include "ModelLoader.h"
#include <fstream>
#include <iostream>
#include <vector>
#include <algorithm>
#include <cctype>
#include <string>
#include "TextureCache.h"
#include <glm/gtx/string_cast.hpp>

static std::string replace(std::string str, const std::string& toReplace, const std::string& replacement) {
	size_t index = 0;
	while ((index = str.find(toReplace, index)) != std::string::npos) {
		str.replace(index, toReplace.length(), replacement);
		++index;
	}
	return str;
}

static std::vector<std::string> split(std::string str, const std::string& separator) {
	std::vector<std::string> ret;
	size_t index = 0;
	while ((index = str.find(separator)) != std::string::npos) {
		ret.push_back(str.substr(0, index));
		str = str.substr(index + separator.length());
	}
	ret.push_back(str);
	return ret;
}

static inline std::string toLower(std::string data) {
	std::transform(data.begin(), data.end(), data.begin(),
		[](unsigned char c) { return std::tolower(c); });
	return data;
}

static inline std::string cleanLine(std::string line) {
	line = replace(line, "\t", " ");
	while (line.find("  ") != std::string::npos)
		line = replace(line, "  ", " ");
	if (!line.empty() && line[0] == ' ')
		line = line.substr(1);
	if (!line.empty() && line[line.length() - 1] == ' ')
		line = line.substr(0, line.length() - 1);
	return line;
}

bool ModelLoader::loadModel(const std::string& fileName, Model& model) {
	std::cout << "Loading " << fileName << std::endl;
	std::string dirName = fileName;
	if (dirName.rfind("/") != std::string::npos)
		dirName = dirName.substr(0, dirName.rfind("/"));
	if (dirName.rfind("\\") != std::string::npos)
		dirName = dirName.substr(0, dirName.rfind("\\"));
	if (fileName == dirName)
		dirName = "";

	std::ifstream pFile(fileName.c_str());

	if (!pFile.is_open()) {
		std::cout << "Could not open file " << fileName << std::endl;
		return false;
	}

	Model::ObjGroup* currentGroup = new Model::ObjGroup();
	currentGroup->materialIndex = -1;

	while (!pFile.eof()) {
		std::string line;
		std::getline(pFile, line);
		line = cleanLine(line);
		if (line.empty() || line[0] == '#') //skip empty or commented line
			continue;

		std::vector<std::string> params = split(line, " ");
		params[0] = toLower(params[0]);

		if (params[0] == "v")
			model.vertices.push_back(glm::vec3((float)atof(params[1].c_str()), (float)atof(params[2].c_str()), (float)atof(params[3].c_str())));
		else if (params[0] == "vn")
			model.normals.push_back(glm::vec3((float)atof(params[1].c_str()), (float)atof(params[2].c_str()), (float)atof(params[3].c_str())));
		else if (params[0] == "vt")
			model.texcoords.push_back(glm::vec2((float)atof(params[1].c_str()), (float)atof(params[2].c_str())));
		else if (params[0] == "f") {
			for (size_t ii = 4; ii <= params.size(); ii++) {
				Model::Face face;

				for (size_t i = ii - 3; i < ii; i++) {
					Model::Vertex vertex;
					std::vector<std::string> indices = split(params[i == (ii - 3) ? 1 : i], "/");
					if (indices.size() >= 1) //er is een positie
						vertex.position = atoi(indices[0].c_str()) - 1;
					if (indices.size() == 2) //alleen texture
						vertex.texcoord = atoi(indices[1].c_str()) - 1;
					if (indices.size() == 3) { //v/t/n of v//n
						if (!indices[1].empty())
							vertex.texcoord = atoi(indices[1].c_str()) - 1;
						vertex.normal = atoi(indices[2].c_str()) - 1;
					}
					face.vertices.push_back(vertex);
				}
				currentGroup->faces.push_back(face);
			}
		}
		else if (params[0] == "s") {
			//smoothing groups
		}
		else if (params[0] == "mtllib") {
			loadMaterialFile(dirName + "/" + params[1], dirName, model);
		}
		else if (params[0] == "usemtl") {
			if (!currentGroup->faces.empty())
				model.groups.push_back(currentGroup);
			currentGroup = new Model::ObjGroup();
			currentGroup->materialIndex = -1;

			for (size_t i = 0; i < model.materials.size(); i++) {
				Model::MaterialInfo* info = model.materials[i];
				if (info->name == params[1]) {
					currentGroup->materialIndex = i;
					break;
				}
			}
			if (currentGroup->materialIndex == -1)
				std::cout << "Could not find material name " << params[1] << std::endl;
		}
	}
	model.groups.push_back(currentGroup);

	return true;
}

void ModelLoader::loadMaterialFile(const std::string& fileName, const std::string& dirName, Model& model) {
	std::cout << "Loading " << fileName << std::endl;
	std::ifstream pFile(fileName.c_str());
	if (!pFile.is_open()) {
		std::cout << "Could not open file " << fileName << std::endl;
		return;
	}

	Model::MaterialInfo* currentMaterial = nullptr;

	while (!pFile.eof()) {
		std::string line;
		std::getline(pFile, line);
		line = cleanLine(line);
		if (line.empty() || line[0] == '#') continue;

		std::vector<std::string> params = split(line, " ");
		params[0] = toLower(params[0]);

		if (params[0] == "newmtl") {
			if (currentMaterial != nullptr) {
				model.materials.push_back(currentMaterial);
			}
			currentMaterial = new Model::MaterialInfo();
			currentMaterial->name = params[1];
		}
		else if (params[0] == "map_kd") {
			std::string tex = params[1];
			if (tex.find("/"))
				tex = tex.substr(tex.rfind("/") + 1);
			if (tex.find("\\"))
				tex = tex.substr(tex.rfind("\\") + 1);
			// Texture files must be in a sub-folder "textures"
			currentMaterial->texture = TextureCache::loadTexture(dirName + "/textures/" + tex);
		}
		else if (params[0] == "kd") {
			// diffuse color
			std::string tex = params[1];
			if (tex.find("/"))
				tex = tex.substr(tex.rfind("/") + 1);
			if (tex.find("\\"))
				tex = tex.substr(tex.rfind("\\") + 1);

			// Create 1x1 texture based on diffuse color
			glm::vec3 color = glm::vec3(std::stof(params[1]), std::stof(params[2]), std::stof(params[3]));
			const std::string colorName = glm::to_string(color);
			currentMaterial->texture = TextureCache::loadMaterialTexture(colorName, color);
		}
		else if (params[0] == "ka") {
			//TODO, ambient color
		}
		else if (params[0] == "ks") {
			//TODO, specular color
		}
		else if (
			params[0] == "illum" ||
			params[0] == "map_bump" ||
			params[0] == "map_ke" ||
			params[0] == "map_ka" ||
			params[0] == "map_d" ||
			params[0] == "d" ||
			params[0] == "ke" ||
			params[0] == "ns" ||
			params[0] == "ni" ||
			params[0] == "td" ||
			params[0] == "tf" ||
			params[0] == "tr" ||
			false) {
			//these values are usually not used for rendering at this time, so ignore them
		}
		else
			std::cout << "Didn't parse " << params[0] << " in material file" << std::endl;
	}
	if (currentMaterial != nullptr)
		model.materials.push_back(currentMaterial);
}
