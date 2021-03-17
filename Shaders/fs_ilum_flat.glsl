#version 330 core

in vec3 fragPos;
in vec3 fragNormal;
in vec3 fragColor;

void main(){
   gl_FragColor = vec4(fragColor, 1.0f);
}

