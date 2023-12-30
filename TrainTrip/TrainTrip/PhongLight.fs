#version 330 core
out vec4 FragColor;

in vec3 Normal;
in vec3 FragPos;

uniform vec3 lightPos;
uniform vec3 viewPos;
uniform vec3 lightColor;
uniform vec3 objectColor;

//setare variabile uniforme
uniform float ka=0.1;
uniform float kd=0.9;
uniform float n=2;
uniform float ks=0.5;


void main()
{
    vec3 ambient=(lightColor*ka);

    vec3 lightDir=normalize(lightPos-FragPos);
    vec3 diffuse=lightColor*kd*max(dot(Normal,lightDir),0.0);

    vec3 viewDir=normalize(viewPos-FragPos);
    vec3 reflectDir=reflect(-lightDir,normalize(Normal));
    float specFactor=pow(max(dot(viewDir,reflectDir),0.0),n);
    vec3 specular=ks*specFactor*lightColor;

    FragColor = vec4(ambient+diffuse+specular,1.0)*vec4(objectColor, 1.0);
}