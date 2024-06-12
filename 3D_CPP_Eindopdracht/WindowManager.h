#pragma once
#include <GLFW/glfw3.h>
#include <stdexcept>

class WindowManager {
public:
	static WindowManager& getInstance() {
		static WindowManager instance;
		return instance;
	}

	GLFWwindow* getWindow() const {
		return window;
	}

private:
	GLFWwindow* window;

	WindowManager() {
		// Initialize GLFW
		if (!glfwInit()) {
			throw std::runtime_error("Failed to initialize GLFW");
		}

		// Create the window
		window = glfwCreateWindow(1920, 1080, "Window", nullptr, nullptr);
		if (!window) {
			glfwTerminate();
			throw std::runtime_error("Failed to create GLFW window");
		}

		glfwMakeContextCurrent(window);
	}

	~WindowManager() {
		// Terminate GLFW
		glfwDestroyWindow(window);
		glfwTerminate();
	}

	// Delete copy constructor and assignment operator to prevent copies
	WindowManager(const WindowManager&) = delete;
	WindowManager& operator=(const WindowManager&) = delete;
};