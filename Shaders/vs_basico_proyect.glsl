#version 330 core 

layout (location = 0) in vec3 vertexPos;
layout (location = 1) in vec3 vertexNormal;
layout (location = 2) in vec3 vertexColor;

out vec3 fragColor;

uniform mat4x4 model;
uniform mat4x4 view;
uniform mat4x4 projection;

void main(){
	
	gl_Position = projection * view * model * vec4(vertexPos,1.0);
	fragColor = vertexColor;
}
