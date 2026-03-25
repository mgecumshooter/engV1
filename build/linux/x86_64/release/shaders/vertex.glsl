#version 460

layout (location = 0) in vec3 a_position;
layout (location = 1) in vec4 a_color;
layout (location = 2) in vec2 a_texCoord;

layout(set = 1, binding = 0) uniform Constants {
    mat4 u_model;
} pc;

layout (location = 0) out vec4 v_color;
layout (location = 1) out vec2 v_texCoord;

void main(){
    gl_Position = pc.u_model * vec4(a_position, 1.0f);

    v_color = a_color;
	v_texCoord = a_texCoord;
}
