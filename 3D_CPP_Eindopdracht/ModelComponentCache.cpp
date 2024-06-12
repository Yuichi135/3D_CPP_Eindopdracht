#include "ModelComponentCache.h"
#include "ModelComponent.h"

std::map<std::string, std::shared_ptr<ModelComponent>> ModelComponentCache::cache;

std::shared_ptr<ModelComponent> ModelComponentCache::loadModel(const std::string& fileName) {
	auto found = cache.find(fileName);
	if (found != cache.end())
		return found->second;
	const auto model = std::make_shared<ModelComponent>(fileName);
	cache[fileName] = model;
	return model;
}