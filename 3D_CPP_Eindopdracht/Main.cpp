#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_internal.h>
#include <examples/imgui_impl_glfw.h>
#include <examples/imgui_impl_opengl3.h>
#include "tigl.h"
#include <glm/gtc/matrix_transform.hpp>
#include "CameraController.h"
#include "Object.h"
#include "OceanComponent.h"
#include "CubeComponent.h"
#include "PhysicsComponent.h"
#include <string>
#include "ModelComponent.h"
#include "MovementComponent.h"
#include "WindowManager.h"
#include "utest.h"
#include <fstream>
#include <ctime>


#ifdef _TESTING_CONFIG
UTEST_STATE();
#endif


#pragma comment(lib, "glfw3.lib")
#pragma comment(lib, "glew32s.lib")
#pragma comment(lib, "opengl32.lib")

using tigl::Vertex;

void init();
void initWindow();
void initImGui();
void updateImGui();
void onDestroy();
void update();
void draw();
void createCircleOfBuoys(int count, float radius, std::shared_ptr<OceanComponent> oceanComponent);
void log();

const int oceanSize = 1000;
const glm::vec3 backgroundColor(0.3f, 0.4f, 0.6f);

GLFWwindow* window;
std::shared_ptr<CameraController> cameraController;
double lastFrameTime = 0;
const float PI = std::atan(1.0) * 4;

std::list<std::shared_ptr<Object>> objects;
std::shared_ptr<Object> ocean;
std::shared_ptr<Object> player;

int main(int argc, const char* const argv[])
{

#ifdef _TESTING_CONFIG
	return utest_main(argc, argv);
#endif

	init();

	while (!glfwWindowShouldClose(window))
	{
		// Start new ImGui frame.
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
		//ImGui::ShowDemoWindow(0);

		update();
		draw();

		updateImGui();

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	onDestroy();

	return 0;
}

void init()
{
	initWindow();
	initImGui();

	tigl::init();

	glFrontFace(GL_CCW);
	glEnable(GL_DEPTH_TEST);

	tigl::shader->enableLighting(true);
	tigl::shader->setLightCount(1);
	tigl::shader->setLightDirectional(0, true);
	tigl::shader->setLightPosition(0, glm::vec3(0, 1, 0));
	tigl::shader->setLightAmbient(0, glm::vec3(0.5f, 0.5f, 0.5f));
	tigl::shader->setLightDiffuse(0, glm::vec3(0.5f, 0.5f, 0.5f));
	tigl::shader->setLightSpecular(0, glm::vec3(1, 1, 1));
	tigl::shader->setShinyness(0);

	tigl::shader->setFogColor(backgroundColor);
	tigl::shader->setFogLinear(75.0f, 90.0f);
	tigl::shader->enableFog(true);

	tigl::shader->enableColor(true);
	tigl::shader->enableTexture(false);
	tigl::shader->enableColorMult(false);


	cameraController = std::make_shared<CameraController>();
	glPointSize(5.0f);

	std::cout << "Creating ocean" << std::endl;
	ocean = std::make_shared<Object>();
	ocean->position = glm::vec3(0.0f);
	auto oceanComponent = std::make_shared<OceanComponent>(cameraController->position, oceanSize);
	ocean->addComponent(oceanComponent);

	std::cout << "Adding ocean to objects" << std::endl;
	objects.push_back(ocean);

	std::cout << "Creating player" << std::endl;
	player = std::make_shared<Object>();
	player->position = glm::vec3(0.0f, 5.0f, -5.0f);
	player->centreOffMassOffset.y = 0.3f;

	player->addComponent(std::make_shared<ModelComponent>("models/ship-large.obj"));
	player->addComponent(std::make_shared<PhysicsComponent>(oceanComponent, 5));
	player->addComponent(std::make_shared<MovementComponent>());
	objects.push_back(player);

	createCircleOfBuoys(10, 50.0f, oceanComponent);
	createCircleOfBuoys(15, 90.0f, oceanComponent);
}

void initWindow()
{
	window = WindowManager::getInstance().getWindow();
}

void initImGui() {
	std::cout << "Initialized ImGui." << std::endl;

	IMGUI_CHECKVERSION();

	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 130");

	ImGui::StyleColorsDark();
}

void updateImGui() {
	ImGui::SetNextWindowPos(ImVec2(0, 0));
	ImGui::Begin("Debug Window");
	ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
	static bool vsync = true;
	if (ImGui::Checkbox("Use vsync", &vsync))
		glfwSwapInterval(vsync ? 1 : 0);


	if (ImGui::Button("Save.. something"))
		log();


	ImGui::Text("Multipliers");
	ImGui::SliderFloat("Wave speed", &OceanComponent::waveSpeedMultiplier, 0.0f, 2.0f);
	ImGui::SliderFloat("Water Density", &PhysicsComponent::waterDensityMultiplier, 0.1f, 5.0f);
	ImGui::SliderFloat("Gravity", &PhysicsComponent::gravityMultiplier, 0.1f, 5.0f);

	ImGui::Spacing();
	ImGui::Spacing();

	auto movement = player->getComponent<MovementComponent>();
	ImGui::Text("Boat controls");
	ImGui::SliderFloat("Wheel direction", &movement->wheelDirection, -0.015f, 0.015f);
	ImGui::SliderFloat("Direction", &movement->direction, -3.14f, 3.14f);
	ImGui::SliderFloat("Boat speed", &movement->speed, -0.1f, 1.0f);

	ImGui::End();

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

static void onDestroy()
{
	std::cout << "DeInitializing." << std::endl;
	ImGui_ImplGlfw_Shutdown();
	ImGui_ImplOpenGL3_Shutdown();
	ImGui::DestroyContext();
	glfwTerminate();
}

void createCircleOfBuoys(int count, float radius, std::shared_ptr<OceanComponent> oceanComponent) {
	std::cout << "Creating circle of buots " << std::endl;
	for (int i = 0; i < count; ++i) {
		float angle = (2 * PI / count) * i;
		float x = radius * cos(angle);
		float z = radius * sin(angle);

		std::shared_ptr<Object> buoy = std::make_shared<Object>();
		buoy->position = glm::vec3(x, 5.0f, z);
		buoy->centreOffMassOffset.y = -0.3f;

		buoy->addComponent(std::make_shared<ModelComponent>("models/buoy.obj"));
		buoy->addComponent(std::make_shared<PhysicsComponent>(oceanComponent));
		objects.push_back(buoy);
	}
}

void update()
{
	cameraController->update();

	double currentFrameTime = glfwGetTime();
	double deltaTime = currentFrameTime - lastFrameTime;
	lastFrameTime = currentFrameTime;

	// Update objects.
	for (std::shared_ptr<Object>& object : objects)
	{
		object->update(static_cast<float>(deltaTime));
	}
}

void draw()
{
	glClearColor(backgroundColor.x, backgroundColor.y, backgroundColor.z, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	int viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport);
	glm::mat4 projection = glm::perspective(glm::radians(75.0f), viewport[2] / (float)viewport[3], 0.01f, 100.0f);

	tigl::shader->setProjectionMatrix(projection);
	tigl::shader->setViewMatrix(cameraController->getMatrix());
	tigl::shader->setModelMatrix(glm::mat4(1.0f));


	// draw objects.
	for (std::shared_ptr<Object>& object : objects)
	{
		object->draw();
	}
}

void log() {
	// get time
	std::time_t now = std::time(nullptr);
	struct tm timeinfo;
	char buf[80];

	localtime_s(&timeinfo, &now);
	std::strftime(buf, sizeof(buf), "%Y%m%d_%H%M%S", &timeinfo);


	std::string filename = "log_" + std::string(buf) + ".txt";
	std::ofstream logFile(filename);
	if (logFile.is_open()) {
		int counter = 0;
		for (std::shared_ptr<Object>& object : objects)
		{
			counter++;
			logFile << "Object " << counter << ", position: " << "X: " << object->position.x << "\tY: " << object->position.y << "\tZ: " << object->position.z << std::endl;
		}
		logFile.close();
		std::cout << "Log written to file: " << filename << std::endl;
	}
	else {
		std::cerr << "Unable to open log file: " << filename << std::endl;
	}
}