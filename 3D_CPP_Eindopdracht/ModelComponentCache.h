#pragma once

#include <map>
#include <string>
#include <memory>


class ModelComponent;

class ModelComponentCache {
	static std::map<std::string, std::shared_ptr<ModelComponent>> cache;

public:
	static std::shared_ptr<ModelComponent> loadModel(const std::string& fileName);
};