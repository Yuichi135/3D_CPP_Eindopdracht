#pragma once

#include "Model.h"
#include <string>

class ModelLoader {
public:
	static bool loadModel(const std::string& fileName, Model& model);
private:
	static void loadMaterialFile(const std::string& fileName, const std::string& dirName, Model& model);
};
