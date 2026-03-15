#include <iostream>
#include <vector>
#include <array>
#include <SDL3/SDL_main.h>
#include <SDL3/SDL.h>
#include <SDL3/SDL_gpu.h>

using namespace std;

struct Vertex{
  float x, y, z;
  float r, g, b, a;
};

void addTriangle(vector<Vertex>& verts, vector<uint16_t>& inds, array<float, 2> first, array<float, 2> second, array<float, 2> third) {
  verts.push_back(Vertex{first[0], first[1], 0, 1, 1, 1, 1});
  verts.push_back(Vertex{second[0], second[1], 0, 1, 1, 1, 1});
  verts.push_back(Vertex{third[0], third[1], 0, 1, 1, 1, 1});

  inds.push_back(static_cast<uint16_t>(verts.size() - 3));
  inds.push_back(static_cast<uint16_t>(verts.size() - 2));
  inds.push_back(static_cast<uint16_t>(verts.size() - 1));
}

void popTriangle(vector<Vertex>& verts, vector<uint16_t>& inds){
  verts.pop_back();
  verts.pop_back();
  verts.pop_back();

  inds.pop_back();
  inds.pop_back();
  inds.pop_back();
}

void addRect(vector<Vertex>& verts, vector<uint16_t>& inds, array<float, 2> first, array<float, 2> second) {
  array<float, 2> upLeft = {first[0], first[1]};
  array<float, 2> botLeft = {first[0], second[1]};
  array<float, 2> upRight = {second[0], first[1]};
  array<float, 2> botRight = {second[0], second[1]};

  verts.push_back(Vertex{upLeft[0], upLeft[1], 0, 1, 1, 1, 1});
  verts.push_back(Vertex{botLeft[0], botLeft[1], 0, 1, 1, 1, 1});
  verts.push_back(Vertex{upRight[0], upRight[1], 0, 1, 1, 1, 1});
  verts.push_back(Vertex{botRight[0], botRight[1], 0, 1, 1, 1, 1});

  inds.push_back(verts.size() - 4);
  inds.push_back(verts.size() - 3);
  inds.push_back(verts.size() - 2);
  inds.push_back(verts.size() - 3);
  inds.push_back(verts.size() - 2);
  inds.push_back(verts.size() - 1);
}

void popRect(vector<Vertex>& verts, vector<uint16_t>& inds){
  verts.pop_back();
  verts.pop_back();
  verts.pop_back();
  verts.pop_back();

  inds.pop_back();
  inds.pop_back();
  inds.pop_back();
  inds.pop_back();
  inds.pop_back();
  inds.pop_back();
}

static SDL_Window* window;
static SDL_GPUDevice* device;

int main(int argc, char **argv) {


  SDL_Init(SDL_INIT_VIDEO);
  window = SDL_CreateWindow("painis", 800, 600, SDL_WINDOW_BORDERLESS);
  device = SDL_CreateGPUDevice(SDL_GPU_SHADERFORMAT_SPIRV, true, NULL);

  SDL_ClaimWindowForGPUDevice(device, window);


  // Vertex vertices[]{
  //   {0.0f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f},
  //   {-0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f},
  //   {0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f}
  // };

  vector<Vertex> vertices = {
    {0.f, 0.5f, 0.f, 1.f, 0.f, 0.f, 1.f},
    {-0.5f, -0.5f, 0.f, 0.f, 1.f, 0.f, 1.f},
    {0.5f, -0.5f, 0.f, 0.f, 0.f, 1.f, 1.f}
  };

  vector<uint16_t> indices = {0, 1, 2};


  SDL_GPUBufferCreateInfo vertexInfo{};
  vertexInfo.size = vertices.size() * sizeof(Vertex);
  vertexInfo.usage = SDL_GPU_BUFFERUSAGE_VERTEX;

  SDL_GPUBuffer* vertexBuffer = SDL_CreateGPUBuffer(device, &vertexInfo);


  SDL_GPUBufferCreateInfo indexInfo{};
  indexInfo.size = indices.size() * sizeof(uint16_t);
  indexInfo.usage = SDL_GPU_BUFFERUSAGE_INDEX;

  SDL_GPUBuffer* indexBuffer = SDL_CreateGPUBuffer(device, &indexInfo);


  SDL_GPUTransferBufferCreateInfo transferInfo{};
  transferInfo.size = (vertices.size() * sizeof(Vertex)) + (indices.size() * sizeof(uint16_t));
  transferInfo.usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD;

  SDL_GPUTransferBuffer* transferBuffer = SDL_CreateGPUTransferBuffer(device, &transferInfo);





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
  vertexShaderInfo.num_uniform_buffers = 0;

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


  SDL_GPUGraphicsPipeline* graphicsPipeline = SDL_CreateGPUGraphicsPipeline(device, &pipelineInfo);

  SDL_ReleaseGPUShader(device, vertexShader);
  SDL_ReleaseGPUShader(device, fragmentShader);
  
  
  bool running = true;
  uint currentVertices = vertices.size();
  uint currentIndices = indices.size();

  bool triangle = false;
  
  while (running) {

    // EVENTS
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
      switch (event.type) {
      case SDL_EVENT_QUIT:
	running = false;
	break;
      case SDL_EVENT_KEY_DOWN:
	switch (event.key.scancode){
	case SDL_SCANCODE_Q:
	  
	  if (!triangle){
	    addTriangle(vertices, indices, {-1.f, 1.f}, {-1.f, 0.5f}, {-0.5f, 1.f});
	    addRect(vertices, indices, {0.5f, 1.f}, {1.f, 0.5f});
	    triangle = true;
	  }else {
	    popTriangle(vertices, indices);
	    popRect(vertices, indices);
	    triangle = false;
	  }
	  
	}
      }
    }
    

    // HANDLING BUFFERS SIZES
    if (vertices.size() != currentVertices){

      SDL_ReleaseGPUBuffer(device, vertexBuffer);
      SDL_ReleaseGPUBuffer(device, indexBuffer);
      SDL_ReleaseGPUTransferBuffer(device, transferBuffer);

      vertexInfo.size = vertices.size() * sizeof(Vertex);
      vertexBuffer = SDL_CreateGPUBuffer(device, &vertexInfo);

      indexInfo.size = indices.size() * sizeof(uint16_t);
      indexBuffer = SDL_CreateGPUBuffer(device, &indexInfo);

      transferInfo.size = (vertices.size() * sizeof(Vertex)) + (indices.size() * sizeof(uint16_t));
      transferBuffer = SDL_CreateGPUTransferBuffer(device, &transferInfo);

      currentVertices = vertices.size();
      currentIndices = indices.size();

    }

    
    // MAPPING TRANSFER BUFFER
    Uint8* data = (Uint8*)SDL_MapGPUTransferBuffer(device, transferBuffer, false);
    
    size_t vertexByteSize = vertices.size() * sizeof(Vertex);
    SDL_memcpy(data, vertices.data(), vertexByteSize);
    
    size_t indexByteSize = indices.size() * sizeof(uint16_t);
    SDL_memcpy(data + vertexByteSize, indices.data(), indexByteSize);
    
    SDL_UnmapGPUTransferBuffer(device, transferBuffer);

    
    
    SDL_GPUCommandBuffer* commandBuffer = SDL_AcquireGPUCommandBuffer(device);
    
    // COPY PASS
    SDL_GPUCopyPass* copyPass = SDL_BeginGPUCopyPass(commandBuffer);
    
    SDL_GPUTransferBufferLocation location{};
    location.transfer_buffer = transferBuffer;
    location.offset = 0;

    SDL_GPUBufferRegion region{};
    region.buffer = vertexBuffer;
    region.size = vertices.size() * sizeof(Vertex);
    region.offset = 0;

    SDL_UploadToGPUBuffer(copyPass, &location, &region, true);

    SDL_GPUTransferBufferLocation indexLocation{};
    indexLocation.transfer_buffer = transferBuffer;
    indexLocation.offset = vertices.size() * sizeof(Vertex);

    SDL_GPUBufferRegion indexRegion{};
    indexRegion.buffer = indexBuffer;
    indexRegion.size = indices.size() * sizeof(uint16_t);
    indexRegion.offset = 0;
    
    // location.offset = vertices.size() * sizeof(Vertex);

    // region.buffer = indexBuffer;
    // region.size = indices.size() * sizeof(uint16_t);
    // region.offset = 0;

    SDL_UploadToGPUBuffer(copyPass, &indexLocation, &indexRegion, true);
    
    SDL_EndGPUCopyPass(copyPass);

    
    SDL_GPUTexture* swapchainTexture;
    uint width, height;
    SDL_WaitAndAcquireGPUSwapchainTexture(commandBuffer, window, &swapchainTexture, &width, &height);

    SDL_GPUColorTargetInfo colorTargetInfo{};
    colorTargetInfo.clear_color = {35.0/255.0, 20.0/255.0, 65.0/255.0, 1.0};
    colorTargetInfo.load_op = SDL_GPU_LOADOP_CLEAR;
    colorTargetInfo.store_op = SDL_GPU_STOREOP_STORE;
    colorTargetInfo.texture = swapchainTexture;


    // RENDER PASS
    SDL_GPURenderPass* renderPass = SDL_BeginGPURenderPass(commandBuffer, &colorTargetInfo, 1, NULL);
    SDL_BindGPUGraphicsPipeline(renderPass, graphicsPipeline);

    SDL_GPUBufferBinding bufferBindings[1];
    bufferBindings[0].buffer = vertexBuffer;
    bufferBindings[0].offset = 0;

    SDL_BindGPUVertexBuffers(renderPass, 0, bufferBindings, 1);

    SDL_GPUBufferBinding indexBinding = {};
    indexBinding.buffer = indexBuffer;
    indexBinding.offset = 0;

    SDL_BindGPUIndexBuffer(renderPass, &indexBinding, SDL_GPU_INDEXELEMENTSIZE_16BIT);

    // SDL_DrawGPUPrimitives(renderPass, 3, 1, 0, 0);
    SDL_DrawGPUIndexedPrimitives(renderPass, indices.size(), 1, 0, 0, 0);

    // END OF RENDER PASS
    SDL_EndGPURenderPass(renderPass);

    SDL_SubmitGPUCommandBuffer(commandBuffer);
  }

  SDL_ReleaseGPUBuffer(device, indexBuffer);
  SDL_ReleaseGPUBuffer(device, vertexBuffer);
  SDL_ReleaseGPUTransferBuffer(device, transferBuffer);
  SDL_ReleaseGPUGraphicsPipeline(device, graphicsPipeline);
  SDL_DestroyGPUDevice(device);
  SDL_DestroyWindow(window);
}
