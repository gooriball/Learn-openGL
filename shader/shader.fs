// 프래그먼트 셰이더(Fragment Shader)는 각 프래그먼트(픽셀)에 대해 호출되며, 최종 색상을 계산한다
#version 330 core
// 최종 출력 색상을 저장할 변수, 이 변수의 값이 최종적으로 프래그먼트의 색상이 된다
out vec4 FragColor;

// 정점 셰이더로부터 전달받은 색상 데이터와 텍스처 좌표 데이터
in vec3 ourColor;
in vec2 TexCoord;

// 첫 번째 2D 텍스처 샘플러, 텍스처 유닛0에 바인딩된 텍스처를 참조한다
uniform sampler2D texture1;
// 두 번째 2D 텍스처 샘플러, 텍스처 유닛1에 바인딩된 텍스처를 참조한다
uniform sampler2D texture2;

// 프래그먼트 셰이더의 메인 함수, 각 프래그먼트마다 전부 실행
void main(void)
{
	// texture 함수는 텍스처 샘플러와 텍스처 좌표를 사용하여 텍스처의 색상을 가져온다
	// mix 함수는 두 텍스처의 색상을 80%(1 - 0.2) 와 20% 의 비율로 혼합한다
	FragColor = mix(texture(texture1, TexCoord), texture(texture2, TexCoord), 0.2);
}