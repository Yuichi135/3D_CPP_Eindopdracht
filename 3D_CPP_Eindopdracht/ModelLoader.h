#pragma once

#include "Model.h"
#include <string>

class ModelLoader {
public:
	static void loadModel(const std::string& fileName, Model& model);
private:
	static void loadMaterialFile(const std::string& fileName, const std::string& dirName, Model& model);
};
