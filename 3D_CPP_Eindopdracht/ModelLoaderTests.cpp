#include "utest.h"
#include "Model.h"
#include "ModelLoader.h"


#ifdef _TESTING_CONFIG

UTEST(ModelLoader, loadModel) {
	Model model;
	ModelLoader::loadModel("models/Testcube.obj", model);

	ASSERT_EQ(model.vertices.size(), 8);
	ASSERT_EQ(model.normals.size(), 6);
	ASSERT_EQ(model.texcoords.size(), 14);
	ASSERT_EQ(model.groups.size(), 1);
	ASSERT_EQ(model.materials.size(), 1);
}

UTEST(ModelLoader, loadModelNonexistent) {
	Model model;
	bool loaded = ModelLoader::loadModel("models/NoModel.obj", model);

	ASSERT_FALSE(loaded);
	ASSERT_EQ(model.vertices.size(), 0);
	ASSERT_EQ(model.normals.size(), 0);
	ASSERT_EQ(model.texcoords.size(), 0);
	ASSERT_EQ(model.groups.size(), 0);
	ASSERT_EQ(model.materials.size(), 0);
}

#endif