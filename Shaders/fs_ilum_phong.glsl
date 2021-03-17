#version 330 core

in vec3 fragPos;
in vec3 fragNormal;
in vec3 fragColor;
in vec3 fragLightPos;

vec4 Light(vec3 objectColor, vec3 lightPos);

void main(){
    gl_FragColor = Light(fragColor, fragLightPos);
}

vec4 Light(vec3 objectColor, vec3 lightPos){

    //view,pos,light
    vec3 lightColor = vec3(1.0f,1.0f,1.0f);
    // usually this view will need to be the same of the view proyect
    vec3 viewPos = vec3(0.0f,0.0f,-4.0f);

    //ambient
    float ambientStrength = 0.4;
    vec3 ambient = ambientStrength * lightColor;

    //diffuse
    vec3 normal = normalize(fragNormal);
    vec3 lightDir = normalize(lightPos - fragPos);
    float diff = max(dot(normal,lightDir),0.0);
    vec3 diffuse = diff * lightColor;

    //specular
    float specularStrength = 0.4;
    vec3 viewDir = normalize(viewPos - fragPos);
    vec3 reflectionDir =reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir,reflectionDir),0.0),32);
    vec3 specular = specularStrength * spec * lightColor;

    vec3 res = (ambient + diffuse + specular) * objectColor;

    return vec4(res,1.0);
}