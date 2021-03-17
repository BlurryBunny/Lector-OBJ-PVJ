#version 330 core 

layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec3 color;
layout (location = 3) in vec3 normalFace;
layout (location = 4) in vec3 lightPos_layout;

out vec3 fragPos;
out vec3 fragNormal;
out vec3 fragColor;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

vec3 Light(vec3 objectColor, vec3 objectPos, vec3 objectNormal, vec3 lightPos );


void main(){
    //We set the new position of the vertex acording to the movement and focus on the obj.
    gl_Position = projection * view * model * vec4(pos,1.0);
    
    //We cannot modify the original vertex, we only supply each frame the position to the model view
    fragPos = vec3(model * vec4(pos,1.0));
    
    //If we use the same normal and if the object is on movement the ilumnation model not will change.
    //The effect cannot update according the movement so we need to calculate it.
    
    //fragNormal = normal;
    fragNormal = mat3(transpose(inverse(model))) * normal;

    fragColor = Light(color, vec3(model * vec4(pos,1.0)), mat3(transpose(inverse(model))) * normalFace, lightPos_layout);
}

vec3 Light(vec3 objectColor, vec3 objectPos, vec3 objectNormal, vec3 lightPos){

    //view,pos,light
    vec3 lightColor = vec3(1.0f,1.0f,1.0f);
    // usually this view will need to be the same of the view proyect
    vec3 viewPos = vec3(0.0f,0.0f,-5.0f);

    //ambient
    float ambientStrength = 0.4;
    vec3 ambient = ambientStrength * lightColor;

    //diffuse
    vec3 diff_normal = normalize(objectNormal);
    vec3 lightDir = normalize(lightPos - objectPos);
    float diff = max(dot(diff_normal,lightDir),0.0);
    vec3 diffuse = diff * lightColor;

    //specular
    float specularStrength = 0.4;
    vec3 viewDir = normalize(viewPos - objectPos);
    vec3 reflectionDir =reflect(-lightDir, diff_normal);
    float spec = pow(max(dot(viewDir,reflectionDir),0.0),32);
    vec3 specular = specularStrength * spec * lightColor;

    vec3 res = (ambient + diffuse + specular) * objectColor;

    return(res);
}