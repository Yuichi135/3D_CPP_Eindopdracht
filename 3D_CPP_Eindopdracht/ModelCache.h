#pragma once

#include <string>
#include <map>
#include <memory>

class Model;

class ModelCache {
public:
	static std::shared_ptr<Model> loadModel(const std::string& fileName);
	static void clearCache();
private:
	static std::map<std::string, std::shared_ptr<Model>> cache;
};