#include "Shader.h"

// 이미지 파일을 로드하기 위한 라이브러리
#include "stb_image.h"

#include <iostream>
// OpenGL 함수들을 로드하는 라이브러리, OpenGL 함수의 포인터를 가져온다
#include <glad/glad.h>
// 창 생성 및 입력 처리를 위한 라이브러리
#include <GLFW/glfw3.h>
// 수학 라이브러리들, 행렬과 벡터 계산에 사용
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// 카메라의 위치, 방향, 위쪽 벡터를 정의
glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

// 첫 마우스 입력을 처리하기 위한 플래그
bool firstMouse = true;
// 카메라의 좌우 회전 각도, 초기값 -90도는 카메라가 오른쪽을 바라본다
float yaw = -90.0f;
// 카메라의 상하 회전 각도, 초기값 0도는 카메라가 수평을 바라본다
float pitch = 0.0f;
// 마우스의 마지막 위치, 초기값은 당연히 마우스의 초기 위치(화면의 중앙)
float lastX = WINDOW_WIDTH / 2.0f;
float lastY = WINDOW_HEIGHT / 2.0f;

float fov = 45.0f;

float deltaTime = 0.0f;
float lastFrame = 0.0f;

// 창 크기 조정될 때 호출되는 함수, OpenGL 의 뷰포트를 새 창 크기에 맞게 조정
void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
	glViewport(0, 0, width, height);
}

// 키보드 입력을 처리하는 함수. 'ESC' 가 눌리면 창을 닫도록 설정, 'W' 'A' 'S' 'D' 키로 카메라 이동
void processInput(GLFWwindow *window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, true);
	}
	// 프레임 간의 시간 차이에 비례하여 카메라의 이동 속도를 결정
	float cameraSpeed = static_cast<float>(2.5 * deltaTime);
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
	{
		// cameraFront(카메라가 바라보는 방향) 에 카메라 속도를 곱한 만큼 앞으로 이동
		cameraPos += cameraSpeed * cameraFront;
	}
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
	{
		// cameraFront(카메라 바라보는 방향) 에 카메라 속도를 곱한 만큼 뒤로 이동
        cameraPos -= cameraSpeed * cameraFront;
	}
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
	{
		// 카메라가 바라보는 방향과 카메라의 업 방향을 cross product 를 한 뒤, 정규화 해서 카메라의 오른쪽 방향을 구한다
		// 카메라의 오른쪽 방향에 카메라의 속도를 곱한 만큼 왼쪽으로 이동
        cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
	}
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
	{
		// 카메라가 바라보는 방향과 카메라의 업 방향을 cross product 를 한 뒤, 정규화 해서 카메라의 오른쪽 방향을 구한다
		// 카메라의 오른쪽 방향에 카메라의 속도를 곱한 만큼 오른쪽으로 이동
        cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
	}
}

// 마우스 움직임을 처리하여 카메라의 시점을 변경
// 처음 마우스 움직임이 감지되면 firstMouse 를 false 로 설정하고, 이후 마우스 움직임을 기반으로 'yaw' 와 'pitch' 를 업데이트하여 카메라 방향을 조정
void mouse_callback(GLFWwindow *window, double xposin, double yposin)
{
	float xpos = static_cast<float>(xposin);
	float ypos = static_cast<float>(yposin);

	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	// 마우스의 x 축 이동거리를 계산
	float xoffset = xpos - lastX;
	// 일반적인 좌표에서는 Y축이 아래에서 위로 증가하지만 3D 그래픽스에서는 위에서 아래로 증가하기에 반대로 계산
	float yoffset = lastY - ypos;
	lastX = xpos;
	lastY = ypos;

	// 민감도 설정. 이동거리와 민감도를 곱해서 실제 계산에 쓰일 이동거리로  바꾼다
	float sensitivity = 0.1f;
	xoffset *= sensitivity;
	yoffset *= sensitivity;

	// yaw 와 pitch 를 마우스 이동 거리에 비례하여 업데이트
	// yaw 는 좌우회전이므로 xoffset, pitch 는 상하회전이므로 yoffset
	yaw += xoffset;
	pitch += yoffset;

	if (pitch > 89.0f)
	{
		pitch = 89.0f;
	}
	if (pitch < -89.0f)
	{
		pitch = -89.0f;
	}

	// 카메라의 새로운 방향을 계산하여 카메라가 바라보는 방향을 업데이트
	glm::vec3 front;
	front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	front.y = sin(glm::radians(pitch));
	front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch)) ;
	cameraFront = glm::normalize(front);
}

// 마우스 스크롤을 처리하여 카메라의 시야각(FOV)을 조절
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset)
{
	fov -= (float)yoffset;
	if (fov < 1.0f)
	{
		fov = 1.0f;
	}
	if (fov > 45.0f)
	{
		fov = 45.0f;
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
	// 프레임버퍼 크기 변경 콜백을 설정
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	// 마우스 커서 이동 콜백을 설정
	glfwSetCursorPosCallback(window, mouse_callback);
	// 마우스 휠 콜백을 설정
	glfwSetScrollCallback(window, scroll_callback);
	// 마우스 커서를 비활성화(숨김), 'GLFW_CURSOR_DISABLE' 모드는 커서를 중앙에 고정시키고 이동 거리를 추적한다
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// GLAD 초기화, OpenGL 함수들을 로드, 'glfwGetProcAddress' 를 통해 함수 포인터를 얻는다
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return (-1);
	}

	// 깊이 테스트 활성화
	// 깊이 테스트는 렌더링할 떄 깊이 버퍼를 사용하여 각 픽셀의 깊이 값을 비교, 더 가까운 픽셀만 렌더링하도록 한다
	glEnable(GL_DEPTH_TEST);

	// Shader 클래스 인스턴스 생성 및 셰이더 프로그램 로드하고 컴파일
	Shader ourShader("./shader/shader.vs", "./shader/shader.fs");

	// 정점 위치, 텍스처 좌표 설정
	float vertices[] = {
		-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
		0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
		0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
		0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
		0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
		-0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

		-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		-0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

		0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
		0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
		0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
		0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		-0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f
	};

	// 여러 큐브의 위치(중심좌표)를 정의
	glm::vec3 cubePositions[] = {
		glm::vec3( 0.0f,  0.0f,  0.0f),
		glm::vec3( 2.0f,  5.0f, -15.0f),
		glm::vec3(-1.5f, -2.2f, -2.5f),
		glm::vec3(-3.8f, -2.0f, -12.3f),
		glm::vec3( 2.4f, -0.4f, -3.5f),
		glm::vec3(-1.7f,  3.0f, -7.5f),
		glm::vec3( 1.3f, -2.0f, -2.5f),
		glm::vec3( 1.5f,  2.0f, -2.5f),
		glm::vec3( 1.5f,  0.2f, -1.5f),
		glm::vec3(-1.3f,  1.0f, -1.5f)
    };

	unsigned int VAO;
	unsigned int VBO;
	// VAO 생성하고 바인딩, VAO가 바인딩된 상태에서 모든 VBO 와 EBO 설정이 저장된다
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);
	// VBO 생성하고 바인딩, 정점 데이터를 VBO 에 전송
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// 각 정점 속성(위치, 색상, 텍스처 좌표)을 설정
	// 위치 속성(0번 속성)은 3개의 float 값을 사용하며, vertices 배열의 처음부터 시작한다.
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)0);
	glEnableVertexAttribArray(0);

	// 위치 속성(1번 속성)은 3개의 float 값을 사용하며, vertices 배열의 3번째 값부터 시작한다.
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	// 텍스처 객체를 생성하고 바인딩
	unsigned int texture1;
	unsigned int texture2;
	glGenTextures(1, &texture1);
	glBindTexture(GL_TEXTURE_2D, texture1);

	// 텍스처 래핑, S 축과 T 축을 반복한다
	// 텍스처 래핑은 텍스처 좌표가 0.0 에서 1.0 범위를 벗어날 때 텍스처를 어떻게 적용할지를 정의한다
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// 텍스처 필터링, 축소 및 확대 필터를 선형으로 설정한다 (선형필터를 사용하면 픽셀 색상을 계산할 때 주변의 텍셀들의 가중 평균값을 사용하여(선형보간) 텍스처가 부드럽게 보인다)
	// 텍스처 필터링은 텍스처가 확대 또는 축소될 때 픽셀을 어떻게 보간할지 정의한다
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	int width;
	int height;
	int nrChannels;

	stbi_set_flip_vertically_on_load(true);
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
	ourShader.setInt("texture1", 0);
	// texture2 샘플러를 텍스처 유닛 1에 연결
	ourShader.setInt("texture2", 1);

	while (!glfwWindowShouldClose(window))
	{
		float currentFrame = static_cast<float>(glfwGetTime());
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		processInput(window);

		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// 텍스처 유닛 0(TEXTURE0) 활성화, TEXTURE0 에 texture1 바인딩 
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture1);
		// 텍스처 유닛 1(TEXTURE1) 활성화, TEXTURE1 에 texture2 바인딩
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, texture2);
		// 텍스처 유닛(TEXTURE0 1 2 ...)은 GPU 에서 텍스처를 처리하기 위한 슬롯이다, 셰이더에서는 텍스처 샘플러 변수(sampler2D) 를 통해 텍스처 유닛을 참조한다

		ourShader.use();

		glm::mat4 projection = glm::perspective(glm::radians(fov), (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT, 0.1f, 100.0f);
		ourShader.setMat4("projection", projection);

		glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
		ourShader.setMat4("view", view);

		glBindVertexArray(VAO);
		for (unsigned int i = 0; i < 10; ++i)
		{
			glm::mat4 model = glm::mat4(1.0f);
			model = glm::translate(model, cubePositions[i]);
			float angle = 20.0f * i;
			model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
			ourShader.setMat4("model", model);

			glDrawArrays(GL_TRIANGLES, 0, 36);
		}

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);

	glfwTerminate();

	return (0);
}