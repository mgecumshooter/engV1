#version 460

layout (location = 0) in vec4 v_color;
layout (location = 1) in vec2 v_texCoord;

layout (set = 2, binding = 0) uniform sampler2D u_texture;

layout(set = 3, binding = 0) uniform Constants {
    float time;
} u_scene;

layout (location = 0) out vec4 FragColor;

void main(){
	vec4 tex = texture(u_texture, v_texCoord);
	FragColor = tex * v_color;
}
