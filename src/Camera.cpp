#include "Camera.h"

Camera::Camera(glm::vec3 position, glm::vec3 up, float yaw, float pitch) : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM)
{
	Position = position;
	WorldUp = up;
	Yaw = yaw;
	Pitch = pitch;
	updateCameraVectors();
}

Camera::Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch) : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM)
{
	Position = glm::vec3(posX, posY, posZ);
	WorldUp = glm::vec3(upX, upY, upZ);
	Yaw = yaw;
	Pitch = pitch;
	updateCameraVectors();
}

glm::mat4 Camera::GetViewMatix()
{
	return (glm::lookAt(Position, Position + Front, Up));
}

void Camera::ProcessKeyboard(Camera_Movement direction, float deltaTime)
{
	float velocity = MovementSpeed * deltaTime;
	if (direction == Camera_Movement::FORWARD)
	{
		Position += Front * velocity;
	}
	if (direction == Camera_Movement::BACKWARD)
	{
		Position -= Front * velocity;
	}
	if (direction == Camera_Movement::LEFT)
	{
		Position -= Right * velocity;
	}
	if (direction == Camera_Movement::RIGHT)
	{
		Position += Right * velocity;
	}
}

void Camera::ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch)
{
	xoffset *= MouseSensitivity;
	yoffset *= MouseSensitivity;

	Yaw += xoffset;
	Pitch += yoffset;

	if (constrainPitch)
	{
		if (Pitch > 89.0f)
		{
			Pitch = 89.0f;
		}
		if (Pitch < -89.0f)
		{
			Pitch = -89.0f;
		}
	}
	updateCameraVectors();
}

void Camera::ProcessMouseScroll(float yoffset)
{
	Zoom -= (float)yoffset;
	if (Zoom < 1.0f)
	{
		Zoom = 1.0f;
	}
	if (Zoom > 45.0f)
	{
		Zoom = 45.0f;
	}
}

void Camera::updateCameraVectors()
{
	glm::vec3 front;
	// xz 평면에서 x 좌표 cos(Yaw), 3d 공간에서의 실제 좌표를 구하기 위해 cos(Pitch) 를 곱해 Pitch에 따른 벡터의 기울어짐을 반영
	front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
	// y 좌표는 sin(Pitch) 만으로 정확히 구할 수 있다
	front.y = sin(glm::radians(Pitch));
	// xz 평면에서 z 좌표 sin(Yaw), 위의 x 성분과 마찬가지로 cos(Pitch) 를 곱해줌으로써 Pitch 에 따라 변화는 z 성분을 정확히 표현할 수 있다
	front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));

	Front = glm::normalize(front);
	// 카메라가 바라보는 방향과 월드 좌표계의 윗 방향을 cross product 하여 카메라의 오른쪽 벡터를 구하고, 그 벡터를 정규화 하여 카메라의 오른쪽 방향 벡터를 얻는다
	// 아직 카메라의 윗 방향을 모르기 때문에 월드 좌표계의 윗 방향 벡터를 사용, cross product 의 결과 카메라의 오른쪽 벡터가 나오는 건 같으나 크기가 다르다
	Right = glm::normalize(glm::cross(Front, WorldUp));
	// 위에서 구한 카메라의 오른쪽 방향 벡터와 카메라가 바라보는 방향 벡터를 cross product 하여 카메라의 윗 벡터를 구하고, 정규화 한다
	Up = glm::normalize(glm::cross(Right, Front));
}