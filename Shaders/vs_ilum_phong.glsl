#version 330 core 

layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec3 color;
layout (location = 3) in vec3 normalFace;
layout (location = 4) in vec3 lightPos;

out vec3 fragPos;
out vec3 fragNormal;
out vec3 fragColor;
out vec3 fragLightPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main(){
    //We set the new position of the vertex acording to the movement and focus on the obj.
    gl_Position = projection * view * model * vec4(pos,1.0);
    
    //We cannot modify the original vertex, we only supply each frame the position to the model view
    fragPos = vec3(model * vec4(pos,1.0));
    
    //If we use the same normal and if the object is on movement the ilumnation model not will change.
    //The effect cannot update according the movement so we need to calculate it.
    
    //fragNormal = normal;
    fragNormal = mat3(transpose(inverse(model))) * normal;
    fragColor = color;
    fragLightPos = lightPos;
}
