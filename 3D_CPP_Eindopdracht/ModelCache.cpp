#include "ModelCache.h"
#include "Model.h"
#include "ModelLoader.h"

std::map<std::string, std::shared_ptr<Model>> ModelCache::cache;

std::shared_ptr<Model> ModelCache::loadModel(const std::string& fileName) {
	auto found = cache.find(fileName);
	if (found != cache.end())
		return found->second;

	auto model = std::make_shared<Model>();
	ModelLoader::loadModel(fileName, *model);
	cache[fileName] = model;
	return model;
}
