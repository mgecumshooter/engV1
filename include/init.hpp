#pragma once

#include <vector>
#include <SDL3/SDL_main.h>
#include <SDL3/SDL.h>
#include <SDL3/SDL_gpu.h>

using namespace std;
using namespace basicRend;

int wHeight = 600;
int wWidth = 800;

Mat4 projection = Mat4::ortho(0, wWidth, wHeight, 0, -1, 1);


vector<Vertex> vertices = {
  {0.f, -150.f, 0.f, 1.f, 0.f, 0.f, 1.f},
  {-150.f, 150.f, 0.f, 0.f, 1.f, 0.f, 1.f},
  {150.f, 150.f, 0.f, 0.f, 0.f, 1.f, 1.f}
};

vector<uint16_t> indices = {0, 1, 2};

SDL_GPUBufferCreateInfo vertexInfo{};
SDL_GPUBufferCreateInfo indexInfo{};
SDL_GPUTransferBufferCreateInfo transferInfo{};

SDL_GPUBuffer* vertexBuffer;
SDL_GPUBuffer* indexBuffer;
SDL_GPUTransferBuffer* transferBuffer;

SDL_GPUGraphicsPipeline* graphicsPipeline;

void init(){
  SDL_Init(SDL_INIT_VIDEO);

  window = SDL_CreateWindow("painis", wWidth, wHeight, SDL_WINDOW_VULKAN);
  device = SDL_CreateGPUDevice(SDL_GPU_SHADERFORMAT_SPIRV, true, NULL);

  SDL_ClaimWindowForGPUDevice(device, window);

  vertexInfo.size = vertices.size() * sizeof(Vertex);
  vertexInfo.usage = SDL_GPU_BUFFERUSAGE_VERTEX;

  vertexBuffer = SDL_CreateGPUBuffer(device, &vertexInfo);

  indexInfo.size = indices.size() * sizeof(uint16_t);
  indexInfo.usage = SDL_GPU_BUFFERUSAGE_INDEX;

  indexBuffer = SDL_CreateGPUBuffer(device, &indexInfo);

  transferInfo.size = (vertices.size() * sizeof(Vertex)) + (indices.size() * sizeof(uint16_t));
  transferInfo.usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD;

  transferBuffer = SDL_CreateGPUTransferBuffer(device, &transferInfo);

  size_t vertexCodeSize;
  void* vertexCode = SDL_LoadFile("shaders/vertex.spv", &vertexCodeSize);

  SDL_GPUShaderCreateInfo vertexShaderInfo{};
  vertexShaderInfo.code = (Uint8*)vertexCode;
  vertexShaderInfo.code_size = vertexCodeSize;
  vertexShaderInfo.entrypoint = "main";
  vertexShaderInfo.format = SDL_GPU_SHADERFORMAT_SPIRV;
  vertexShaderInfo.stage = SDL_GPU_SHADERSTAGE_VERTEX;
  vertexShaderInfo.num_samplers = 0;
  vertexShaderInfo.num_storage_buffers = 0;
  vertexShaderInfo.num_storage_textures = 0;
  vertexShaderInfo.num_uniform_buffers = 1;

  SDL_GPUShader* vertexShader = SDL_CreateGPUShader(device, &vertexShaderInfo);

  SDL_free(vertexCode);


  size_t fragmentCodeSize;
  void* fragmentCode = SDL_LoadFile("shaders/fragment.spv", &fragmentCodeSize);

  SDL_GPUShaderCreateInfo fragmentInfo{};
  fragmentInfo.code = (Uint8*)fragmentCode;
  fragmentInfo.code_size = fragmentCodeSize;
  fragmentInfo.entrypoint = "main";
  fragmentInfo.format = SDL_GPU_SHADERFORMAT_SPIRV;
  fragmentInfo.stage = SDL_GPU_SHADERSTAGE_FRAGMENT;
  fragmentInfo.num_samplers = 0;
  fragmentInfo.num_storage_buffers = 0;
  fragmentInfo.num_storage_textures = 0;
  fragmentInfo.num_uniform_buffers = 0;

  SDL_GPUShader* fragmentShader = SDL_CreateGPUShader(device, &fragmentInfo);

  SDL_free(fragmentCode);


  SDL_GPUGraphicsPipelineCreateInfo pipelineInfo{};

  pipelineInfo.vertex_shader = vertexShader;
  pipelineInfo.fragment_shader = fragmentShader;

  pipelineInfo.primitive_type = SDL_GPU_PRIMITIVETYPE_TRIANGLELIST;

  SDL_GPUVertexBufferDescription vertexBufferDescriptions[1];
  vertexBufferDescriptions[0].slot = 0;
  vertexBufferDescriptions[0].input_rate = SDL_GPU_VERTEXINPUTRATE_VERTEX;
  vertexBufferDescriptions[0].instance_step_rate = 0;
  vertexBufferDescriptions[0].pitch = sizeof(Vertex);

  pipelineInfo.vertex_input_state.num_vertex_buffers = 1;
  pipelineInfo.vertex_input_state.vertex_buffer_descriptions = vertexBufferDescriptions;

  SDL_GPUVertexAttribute vertexAttributes[2];
  vertexAttributes[0].buffer_slot = 0;
  vertexAttributes[0].location = 0;
  vertexAttributes[0].format = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT3;
  vertexAttributes[0].offset = 0;

  vertexAttributes[1].buffer_slot = 0;
  vertexAttributes[1].location = 1;
  vertexAttributes[1].format = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT4;
  vertexAttributes[1].offset = sizeof(float) * 3;

  pipelineInfo.vertex_input_state.num_vertex_attributes = 2;
  pipelineInfo.vertex_input_state.vertex_attributes = vertexAttributes;


  SDL_GPUColorTargetDescription colorTargetDescriptions[1];
  colorTargetDescriptions[0] = {};
  colorTargetDescriptions[0].format = SDL_GetGPUSwapchainTextureFormat(device, window);

  pipelineInfo.target_info.num_color_targets = 1;
  pipelineInfo.target_info.color_target_descriptions = colorTargetDescriptions;


  graphicsPipeline = SDL_CreateGPUGraphicsPipeline(device, &pipelineInfo);

  SDL_ReleaseGPUShader(device, vertexShader);
  SDL_ReleaseGPUShader(device, fragmentShader);
  
}
