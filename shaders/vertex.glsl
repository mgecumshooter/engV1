#version 460
layout (location = 0) in vec3 a_position;
layout (location = 1) in vec4 a_color;

// В SDL3 юниформы вершинного шейдера живут в set = 1.
// binding = 0 означает, что мы будем передавать данные в "слот 0".
layout(set = 1, binding = 0) uniform Constants {
    mat4 u_model;
} pc;

layout (location = 0) out vec4 v_color;

void main(){
    gl_Position = pc.u_model * vec4(a_position, 1.0f);
    v_color = a_color;
}