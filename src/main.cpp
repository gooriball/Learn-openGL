#include <spdlog/spdlog.h>
#include <GLFW/glfw3.h>

int main(void)
{
	SPDLOG_INFO("Start program");

	SPDLOG_INFO("Initialize glfw");
	if (!glfwInit())
	{
		const char *description = nullptr;
		glfwGetError(&description);
		SPDLOG_ERROR("failed to initialize glfw: {}", description);
		return (-1);
	}

	SPDLOG_INFO("Create glfw window");
	GLFWwindow *window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_NAME, nullptr, nullptr);
	if (!window)
	{
		SPDLOG_ERROR("failed to create glfw window");
		glfwTerminate();
		return (-1);
	}

	SPDLOG_INFO("Start main loop");
	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents();
	}

	glfwTerminate();

	return (0);
}