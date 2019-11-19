#version 330

layout(std140) struct Material{
    vec4 diffuse;
    vec4 specular;
    vec4 emission;
    float ns;
};

#ifdef VERTEX_SHADER

layout(location= 0) in vec3 position1;
layout(location= 1) in vec3 position2;
//layout(location= 2) in vec3 normale;
layout(location= 3) in int  materialIndex;

uniform mat4 mvpMatrix;
uniform float deltaT;

flat out int materialIndex0;

void main()
{
    materialIndex0 = materialIndex;
    vec3 position = (1-deltaT)*position1 + deltaT*position2;
    gl_Position = mvpMatrix * vec4(position, 1);
}
    #endif

    #ifdef FRAGMENT_SHADER

flat in int materialIndex0;

uniform vec4 mesh_color;

layout(std140) uniform mat{
    Material m[3];
};

out vec4 fragment_color;

void main()
{
    vec4 color= mesh_color;
    if (materialIndex0==0) {
        color = vec4(1,0,0,0);
    } else if (materialIndex0==1) {
        color = vec4(0,1,0,0);
    } else {
        color = vec4(0,0,1,0);
    }
    color = m[materialIndex0].diffuse;


    fragment_color= color;
}
    #endif
