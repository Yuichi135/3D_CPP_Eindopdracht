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

#pragma comment(lib, "glfw3.lib")
#pragma comment(lib, "glew32s.lib")
#pragma comment(lib, "opengl32.lib")

using tigl::Vertex;

void init();
void initWindow();
void initImGui();
void onDestroy();
void update();
void draw();


GLFWwindow* window;
CameraController* cameraController;
const int width = 1900;
const int height = 1000;
double lastFrameTime = 0;

std::list<std::shared_ptr<Object>> objects;
std::shared_ptr<Object> ocean;

int main(void)
{
	init();

	while (!glfwWindowShouldClose(window))
	{
		// Start new ImGui frame.
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		update();
		draw();

		{
			ImGui::SetNextWindowPos(ImVec2(0, 0));
			ImGui::SetNextWindowSize(ImVec2(0, 200));
			ImGui::Begin("Debug Window");
			ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
			static bool vsync = true;
			if (ImGui::Checkbox("Use vsync", &vsync))
			{
				glfwSwapInterval(vsync ? 1 : 0);
			}
			ImGui::End();
		}

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

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

	glEnable(GL_DEPTH_TEST);
	tigl::shader->enableColor(true);
	tigl::shader->enableLighting(true);
	tigl::shader->setLightCount(1);
	tigl::shader->setLightDirectional(0, true);
	tigl::shader->setLightPosition(0, glm::normalize(glm::vec3(1, 1, 1)));
	tigl::shader->setLightAmbient(0, glm::vec3(0.5f, 0.5f, 0.5f));
	tigl::shader->setLightDiffuse(0, glm::vec3(0.5f, 0.5f, 0.5f));
	tigl::shader->setLightSpecular(0, glm::vec3(1, 1, 1));
	tigl::shader->setShinyness(0);


	cameraController = new CameraController(window);
	glPointSize(5.0f);

	std::cout << "Creating ocean" << std::endl;
	ocean = std::make_shared<Object>();
	ocean->position = glm::vec3(0.0f);

	ocean->addComponent(std::make_shared<OceanComponent>(100));

	std::cout << "Adding ocean to objects" << std::endl;
	objects.push_back(ocean);
}

void initWindow()
{
	if (!glfwInit()) throw "Could not initialize glwf";
	window = glfwCreateWindow(width, height, "Hello World", NULL, NULL);

	if (!window)
	{
		glfwTerminate();
		throw "Could not initialize glwf";
	}

	glfwMakeContextCurrent(window);
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

static void onDestroy()
{
	std::cout << "DeInitializing." << std::endl;
	ImGui_ImplGlfw_Shutdown();
	ImGui_ImplOpenGL3_Shutdown();
	ImGui::DestroyContext();
	glfwTerminate();
}

void update()
{
	cameraController->update(window);

	double currentFrameTime = glfwGetTime();
	double deltaTime = currentFrameTime - lastFrameTime;
	lastFrameTime = currentFrameTime;

	// Update objects.
	for (std::shared_ptr<Object>& object : objects)
	{
		object->update(static_cast<float>(deltaTime));
	}
}

void drawGroundPlane() {
	tigl::shader->setModelMatrix(glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -3.0f, 0.0f)));

	tigl::begin(GL_QUADS);

	tigl::addVertex(Vertex::PC(glm::vec3(-10, 0, 10), glm::vec4(1.0f)));
	tigl::addVertex(Vertex::PC(glm::vec3(-10, 0, -10), glm::vec4(1.0f)));
	tigl::addVertex(Vertex::PC(glm::vec3(10, 0, -10), glm::vec4(1.0f)));
	tigl::addVertex(Vertex::PC(glm::vec3(10, 0, 10), glm::vec4(1.0f)));

	tigl::end();
}

void draw()
{
	glClearColor(0.3f, 0.4f, 0.6f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	int viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport);
	glm::mat4 projection = glm::perspective(glm::radians(75.0f), viewport[2] / (float)viewport[3], 0.01f, 100.0f);

	tigl::shader->setProjectionMatrix(projection);
	//tigl::shader->setViewMatrix(camera->getMatrix());
	tigl::shader->setViewMatrix(cameraController->getMatrix());
	tigl::shader->setModelMatrix(glm::mat4(1.0f));

	tigl::shader->enableColor(true);
	tigl::shader->enableLighting(true);
	tigl::shader->enableTexture(false);
	tigl::shader->setLightCount(1);
	tigl::shader->setLightDirectional(0, true);
	tigl::shader->setLightPosition(0, glm::vec3(0.0f, 6.0f, 0.0f));
	tigl::shader->setLightAmbient(0, glm::vec3(1.0f));
	tigl::shader->setLightDiffuse(0, glm::vec3(1.0f));
	tigl::shader->setLightSpecular(0, glm::vec3(0, 0, 0));
	tigl::shader->setShinyness(32.0f);
	tigl::shader->setFogLinear(0, 50);
	tigl::shader->enableColorMult(false);

	tigl::shader->enableFog(true);
	tigl::shader->enableColor(true);
	glEnable(GL_DEPTH_TEST);

	// draw objects.
	for (std::shared_ptr<Object>& object : objects)
	{
		object->draw();
	}

	//drawGroundPlane();
}