// 정점 셰이더(Vertex Shader) 는 OpenGl 파이프라인의 첫 번째 단계로, 주어진 각 정점을 처리한다
// GLSL(OpenGL Shading Language) 3.3 버전을 사용하겠다는 선언
#version 330 core
// 각각 0,1,2 번째 위치 속성
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;

// fragment 셰이더에 전달될 색상 데이터와 텍스처 좌표 데이터
out vec2 TexCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

// 정점 셰이더의 메인 함수, 각 정점마다 전부 실행
void main(void)
{
	gl_Position = projection * view * model * vec4(aPos, 1.0);
	TexCoord = vec2(aTexCoord.x, aTexCoord.y);
}