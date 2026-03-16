#include <iostream>
#include <vector>
#include <array>
#include <cmath>
#include <SDL3/SDL_main.h>
#include <SDL3/SDL.h>
#include <SDL3/SDL_gpu.h>

using namespace std;

struct Mat4{
  array<float, 16> m = {
    1, 0, 0, 0,
    0, 1, 0, 0,
    0, 0, 1, 0,
    0, 0, 0, 1
  };

  static Mat4 translate(float x, float y, float z){
    Mat4 result;
    result.m[12] = x;
    result.m[13] = y;
    result.m[14] = z;
    return result;
  }

  static Mat4 scale(float x, float y, float z){
    Mat4 result;
    result.m[0] = x;
    result.m[5] = y;
    result.m[10] = z;
    return result;
  }

  static Mat4 rotateZ(float deg){
    Mat4 result;
    
    float rads = deg * (M_PI / 180.f);
    float c = cos(rads);
    float s = sin(rads);

    result.m[0] = c;
    result.m[1] = s;
    result.m[4] = -s;
    result.m[5] = c;

    return result;
  }

  static Mat4 ortho(float left, float right, float bottom, float top, float znear, float zfar){
    Mat4 result;

    result.m[0] = 2.f / (right - left);
    result.m[5] = 2.f / (top - bottom);
    result.m[10] = 1.f / (zfar - znear);

    result.m[12] = -(right + left) / (right - left);
    result.m[13] = -(top + bottom) / (top - bottom);
    result.m[14] = -znear / (zfar - znear);
    result.m[15] = 1.f;

    return result;
  }

  Mat4 operator*(const Mat4& other) const{
    Mat4 res;
    for (int col = 0; col < 4; ++col){
      for (int row = 0; row < 4; ++row){
	float sum = 0;
	for (int i = 0; i < 4; ++i){
	  sum += this->m[i * 4 + row] * other.m[col * 4 + i];
	}
	res.m[col * 4 + row] = sum;
      }
    }
    return res;
  }
  
};

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

  int wHeight = 600;
  int wWidth = 800;

  Mat4 projection = Mat4::ortho(0, wWidth, wHeight, 0, -1, 1);


  SDL_Init(SDL_INIT_VIDEO);
  window = SDL_CreateWindow("painis", wWidth, wHeight, SDL_WINDOW_RESIZABLE);
  device = SDL_CreateGPUDevice(SDL_GPU_SHADERFORMAT_SPIRV, true, NULL);

  SDL_ClaimWindowForGPUDevice(device, window);


  // Vertex vertices[]{
  //   {0.0f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f},
  //   {-0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f},
  //   {0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f}
  // };

  vector<Vertex> vertices = {
    {0.f, -150.f, 0.f, 1.f, 0.f, 0.f, 1.f},
    {-150.f, 150.f, 0.f, 0.f, 1.f, 0.f, 1.f},
    {150.f, 150.f, 0.f, 0.f, 0.f, 1.f, 1.f}
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


  SDL_GPUGraphicsPipeline* graphicsPipeline = SDL_CreateGPUGraphicsPipeline(device, &pipelineInfo);

  SDL_ReleaseGPUShader(device, vertexShader);
  SDL_ReleaseGPUShader(device, fragmentShader);
  
  
  bool running = true;
  uint currentVertices = vertices.size();
  uint currentIndices = indices.size();

  bool triangle = false;

  int rotation = 0;
  
  while (running) {

    // EVENTS
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
      switch (event.type) {
      case SDL_EVENT_QUIT:
	running = false;
	break;

      case SDL_EVENT_KEY_DOWN:
	if (event.key.scancode == SDL_SCANCODE_Q) {
	  if (!triangle) {
	    addTriangle(vertices, indices, {-50.f, -50.f}, {-50.f, 50.f}, {50.f, -50.f});
	    addRect(vertices, indices, {-60.f, 60.f}, {60.f, -60.f});
	    triangle = true;
	  } else {
	    popTriangle(vertices, indices);
	    popRect(vertices, indices);
	    triangle = false;
	  }
	}
	break; // Обязательно выходим из KEY_DOWN

      case SDL_EVENT_WINDOW_RESIZED: // Теперь это на своем месте
	wWidth = event.window.data1;
	wHeight = event.window.data2;
	// ОБНОВЛЯЕМ существующую переменную, а не создаем новую!
	projection = Mat4::ortho(0, (float)wWidth, (float)wHeight, 0, -1.f, 1.f);
	break;
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

    Mat4 matrix = Mat4::translate(wWidth / 2.f, wHeight / 2.f, 0.f);
    Mat4 finalMPV = projection * matrix;
    SDL_PushGPUVertexUniformData(commandBuffer, 0, &finalMPV, sizeof(Mat4));

    // SDL_DrawGPUPrimitives(renderPass, 3, 1, 0, 0);
    SDL_DrawGPUIndexedPrimitives(renderPass, 3, 1, 0, 0, 0);

    if (vertices.size() > 3 && indices.size() > 3){
      matrix = Mat4::translate(wWidth * 0.75f, wHeight * 0.25f, 0.f) * Mat4::rotateZ(rotation);
      finalMPV = projection * matrix;
      SDL_PushGPUVertexUniformData(commandBuffer, 0, &finalMPV, sizeof(Mat4));

      SDL_DrawGPUIndexedPrimitives(renderPass, 6, 1, indices.size() - 6, 0, 0);

      // NORMALIZING MATRIX
      // matrix = Mat4::translate(0.f, 0.f, 0.f) * Mat4::rotateZ(-rotation) * Mat4::scale(2.f, 2.f, 2.f);
      matrix = Mat4::translate(wWidth * 0.25f, wHeight * 0.25f, 0.f) * Mat4::scale(fabs(sin(rotation * (M_PI / 180))) + 0.5f, fabs(sin(rotation * (M_PI / 180))) + 0.5f, 1);
      finalMPV = projection * matrix;
      SDL_PushGPUVertexUniformData(commandBuffer, 0, &finalMPV, sizeof(Mat4));

      SDL_DrawGPUIndexedPrimitives(renderPass, 3, 1, indices.size() - 6 - 3, 0, 0);
    }

    
    
    // END OF RENDER PASS
    SDL_EndGPURenderPass(renderPass);

    SDL_SubmitGPUCommandBuffer(commandBuffer);

    rotation++;

  }


  SDL_ReleaseGPUBuffer(device, indexBuffer);
  SDL_ReleaseGPUBuffer(device, vertexBuffer);
  SDL_ReleaseGPUTransferBuffer(device, transferBuffer);
  SDL_ReleaseGPUGraphicsPipeline(device, graphicsPipeline);
  SDL_DestroyGPUDevice(device);
  SDL_DestroyWindow(window);
}
