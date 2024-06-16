#include "utest.h"
#include "Model.h"
#include "ModelLoader.h"
#include <iostream>
#include "ModelCache.h"


#ifdef _TESTING_CONFIG

UTEST(ModelCache) {
	ModelCache::clearCache();

	auto model1 = ModelCache::loadModel("models/Testcube.obj");
	ASSERT_TRUE(model1);

	auto model1Again = ModelCache::loadModel("models/Testcube.obj");
	ASSERT_EQ(model1, model1Again);

	auto model2 = ModelCache::loadModel("models/arrow.obj");
	ASSERT_TRUE(model2);
	ASSERT_NE(model1, model2);
}


#endif