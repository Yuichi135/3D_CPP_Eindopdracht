#include "CameraController.h"
#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include "WindowManager.h"


void key_Callback(GLFWwindow* window, int key, int scancode, int action, int mods);

CameraController::CameraController()
{
	window = WindowManager::getInstance().getWindow();
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	if (glfwRawMouseMotionSupported())
		glfwSetInputMode(window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);

	glfwSetKeyCallback(window, key_Callback);
}

glm::mat4 CameraController::getMatrix()
{
	glm::mat4 ret(1.0f);
	ret = glm::rotate(ret, rotation.x, glm::vec3(1, 0, 0));
	ret = glm::rotate(ret, rotation.y, glm::vec3(0, 1, 0));
	ret = glm::translate(ret, position);
	return ret;
}

void CameraController::update()
{
	int multiplier = (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS) ? 1 : 10;

	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		move(0, 0.1f * multiplier);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		move(180, 0.1f * multiplier);
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		move(90, 0.1f * multiplier);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		move(-90, 0.1f * multiplier);
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
		moveY(-0.1f * multiplier);
	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
		moveY(0.1f * multiplier);


	double x, y;
	glfwGetCursorPos(window, &x, &y);

	static double lastX = x;
	static double lastY = y;

	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_MIDDLE) == GLFW_PRESS) {
		rotation.x -= (float)(lastY - y) / 100.0f;
		rotation.y -= (float)(lastX - x) / 100.0f;
	}

	lastX = x;
	lastY = y;
}

void CameraController::move(float angle, float fac)
{
	position.x += (float)cos(rotation.y + glm::radians(angle)) * fac;
	position.z += (float)sin(rotation.y + glm::radians(angle)) * fac;
}

void CameraController::moveY(float fac)
{
	position.y += fac;
}

int polygoneMode;
void key_Callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_TAB) != GLFW_PRESS)
		return;

	polygoneMode = (polygoneMode + 1) % 3;

	switch (polygoneMode)
	{
	case(0):
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		break;
	case(1):
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		break;
	case(2):
		glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
		break;
	default:
		break;
	}
}
