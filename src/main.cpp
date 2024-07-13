#include "Shader.h"
#include "stb_image.h"

#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// 창 크기 조정될 때 호출되는 함수
void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
	glViewport(0, 0, width, height);
}

// 키보드 입력을 처리하는 함수. 'ESC' 가 눌리면 창을 닫도록 설정
void processInput(GLFWwindow *window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, true);
	}
}

int main(void)
{
	// GLFW 라이브러리 초기화
	glfwInit();
	// OpenGL 버전 및 프로파일 설정
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	// Mac OS 용
	#ifdef __APPLE__
		glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	#endif

	// 지정된 크기와 이름으로 창 생성
	GLFWwindow *window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_NAME, NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return (-1);
	}
	// 생성된 창을 현재 컨텍스트로 설정
	glfwMakeContextCurrent(window);

	// GLAD 초기화, OpenGL 함수들을 로드
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return (-1);
	}

	// 창 크기 설정
	glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
	// 창 크기 변경 콜백 함수 등록
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	// Shader 클래스 인스턴스 생성 및 셰이더 프로그램 로드하고 컴파일
	Shader ourShader("./shader/shader.vs", "./shader/shader.fs");

	// 정점 위치, 색상, 텍스처 좌표 설정
	float vertices[] = {
		// 정점 위치          // 색상            // 텍스처 좌표
		 0.5f,  0.5f, 0.0f,  1.0f, 0.0f, 0.0f,  1.0f, 1.0f,
		 0.5f, -0.5f, 0.0f,  0.0f, 1.0f, 0.0f,  1.0f, 0.0f,
		-0.5f, -0.5f, 0.0f,  0.0f, 0.0f, 1.0f,  0.0f, 0.0f,
		-0.5f,  0.5f, 0.0f,  1.0f, 1.0f, 0.0f,  0.0f, 1.0f
	};

	// 사각형을 구성하는 인덱스 배열, 삼각형 2개로 사각형 구성
	unsigned int indices[] = {
		0, 1, 3,
		1, 2, 3
	};

	unsigned int VAO;
	unsigned int VBO;
	unsigned int EBO;
	// VAO 생성하고 바인딩, VAO가 바인딩된 상태에서 모든 VBO 와 EBO 설정이 저장된다
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);
	// VBO 생성하고 바인딩, 정점 데이터를 VBO 에 전송
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	// EBO 생성하고 바인딩, 인덱스 데이터를 EBO 에 전송
	glGenBuffers(1, &EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// 각 정점 속성(위치, 색상, 텍스처 좌표)을 설정
	// 위치 속성(0번 속성)은 3개의 float 값을 사용하며, vertices 배열의 처음부터 시작한다.
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)0);
	glEnableVertexAttribArray(0);

	// 위치 속성(1번 속성)은 3개의 float 값을 사용하며, vertices 배열의 3번째 값부터 시작한다.
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	// 위치 속성(2번 속성)은 2개의 float 값을 사용하며, vertices 배열의 6번째 값부터 시작한다.
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	// 텍스처 객체를 생성하고 바인딩
	unsigned int texture1;
	unsigned int texture2;
	glGenTextures(1, &texture1);
	glBindTexture(GL_TEXTURE_2D, texture1);

	// 텍스처 래핑, S 축과 T 축을 반복한다
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// 텍스처 필터링, 축소 및 확대 필터를 선형으로 설정한다 (선형필터를 사용하면 픽셀 색상을 계산할 때 주변의 텍셀들의 가중 평균값을 사용하여(선형보간) 텍스처가 부드럽게 보인다)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	int width;
	int height;
	int nrChannels;
	// stbi_load 함수를 사용하여 이미지 로드, 이미지 파일을 읽어서 메모리에 로드하고 이미지 데이터의 포인터를 반환
	unsigned char *data = stbi_load("./resources/textures/container.jpg", &width, &height, &nrChannels, 0);
	if (data)
	{
		// 이미지를 텍스처로 전송
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		// mipmap(밉맵) 생성
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
	}
	// 이미지 데이터를 메모리에 로드 완료했으므로 해제하여 메모리 누수 방지
	stbi_image_free(data);

	glGenTextures(1, &texture2);
	glBindTexture(GL_TEXTURE_2D, texture2);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	stbi_set_flip_vertically_on_load(true);
	data = stbi_load("./resources/textures/awesomeface.png", &width, &height, &nrChannels, 0);
	if (data)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
	}
	stbi_image_free(data);

	ourShader.use();

	// texture1 샘플러를 텍스처 유닛 0에 연결
	glUniform1i(glGetUniformLocation(ourShader.ID, "texture1"), 0);
	// texture2 샘플러를 텍스처 유닛 1에 연결
	ourShader.setInt("texture2", 1);

	while (!glfwWindowShouldClose(window))
	{
		processInput(window);

		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		// 텍스처 유닛 0(TEXTURE0) 활성화, TEXTURE0 에 texture1 바인딩 
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture1);
		// 텍스처 유닛 1(TEXTURE1) 활성화, TEXTURE1 에 texture2 바인딩
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, texture2);
		// 텍스처 유닛(TEXTURE0 1 2 ...)은 GPU 에서 텍스처를 처리하기 위한 슬롯이다, 셰이더에서는 텍스처 샘플러 변수(sampler2D) 를 통해 텍스처 유닛을 참조한다

		ourShader.use();
		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);

	glfwTerminate();

	return (0);
}