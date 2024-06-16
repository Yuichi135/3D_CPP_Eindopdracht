#include "ModelComponent.h"
#include "ModelLoader.h"
#include <iostream>
#include "tigl.h"
#include "Texture.h"
#include "TextureCache.h"
#include <glm/gtx/string_cast.hpp>
#include "Object.h"

ModelComponent::ModelComponent(const std::string& filename) {
	ModelLoader::loadModel(filename, model);
}

void ModelComponent::update(float deltaTime) {
	if (boundingBoxInitialized)
		return;

	setBoundingBox();
}

void ModelComponent::draw(glm::mat4 parentMatrix) {
	tigl::shader->setModelMatrix(parentMatrix);

	for (const auto& group : model.groups) {
		model.materials[group->materialIndex]->texture->bind();
		tigl::shader->enableTexture(true);
		tigl::begin(GL_TRIANGLES);
		for (auto& face : group->faces) {
			for (const auto& vertex : face.vertices) {
				tigl::addVertex(tigl::Vertex::PTN(model.vertices[vertex.position], model.texcoords[vertex.texcoord], model.normals[vertex.normal]));
			}
		}
		tigl::end();
	}
}

void ModelComponent::setBoundingBox() {
	glm::vec3 minValues = model.vertices[0];
	glm::vec3 maxValues = model.vertices[0];

	for (const auto& v : model.vertices) {
		minValues.x = std::min(minValues.x, v.x);
		minValues.y = std::min(minValues.y, v.y);
		minValues.z = std::min(minValues.z, v.z);

		maxValues.x = std::max(maxValues.x, v.x);
		maxValues.y = std::max(maxValues.y, v.y);
		maxValues.z = std::max(maxValues.z, v.z);
	}

	std::cout << "MAX: " << maxValues.x << "\t" << maxValues.y << "\t" << maxValues.z << std::endl;
	std::cout << "MIN: " << minValues.x << "\t" << minValues.y << "\t" << minValues.z << std::endl;

	parentObject->boundingBox = std::make_tuple(minValues, maxValues);
	boundingBoxInitialized = true;
}
