#include <iostream>
#include <vector>
#include <array>
#include <cmath>
#include <algorithm>
#include <optional>
#include <SDL3/SDL_main.h>
#include <SDL3/SDL.h>
#include <SDL3/SDL_gpu.h>

#include "vertex.hpp"
#include "matrix.hpp"
#include "basicRender.hpp"
#include "sprite.hpp"
#include "scene.hpp"

#include "init.hpp"

using namespace std;
using namespace basicRend;


int main(int argc, char **argv) {

  init();

  Scene scene;
  Sprite* rect = nullptr;
  
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
	  
	  if (!rect){
	    rect = scene.spawn(-50.f, -50.f, 100.f, 100.f);
	  }else {
	    rect->pending_destruction = true;
	    rect = nullptr;
	  }
	  
	}else if (event.key.scancode == SDL_SCANCODE_E){
	  vertices.clear();
	  indices.clear();
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

    scene.cleanup();
    scene.updateGlobalBuffers(vertices, indices);

    // HANDLING BUFFERS SIZES
    if (vertices.size() != currentVertices){

      if (vertices.size() == 0 && indices.size() == 0){
	SDL_ReleaseGPUBuffer(device, vertexBuffer);
	SDL_ReleaseGPUBuffer(device, indexBuffer);
	SDL_ReleaseGPUTransferBuffer(device, transferBuffer);

	vertexInfo.size = 4;
	vertexBuffer = SDL_CreateGPUBuffer(device, &vertexInfo);

	indexInfo.size = 4;
	indexBuffer = SDL_CreateGPUBuffer(device, &indexInfo);

	transferInfo.size = 4;
	transferBuffer = SDL_CreateGPUTransferBuffer(device, &transferInfo);

	currentVertices = vertices.size();
	currentIndices = indices.size();	
      }
      else {
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
    }

    
    // MAPPING TRANSFER BUFFER
    if (vertices.size() != 0 && indices.size() != 0){
      Uint8* data = (Uint8*)SDL_MapGPUTransferBuffer(device, transferBuffer, false);
    
      size_t vertexByteSize = vertices.size() * sizeof(Vertex);
      SDL_memcpy(data, vertices.data(), vertexByteSize);
    
      size_t indexByteSize = indices.size() * sizeof(uint16_t);
      SDL_memcpy(data + vertexByteSize, indices.data(), indexByteSize);
    
      SDL_UnmapGPUTransferBuffer(device, transferBuffer);  
    }
    // END OF MAPPING

    
    SDL_GPUCommandBuffer* commandBuffer = SDL_AcquireGPUCommandBuffer(device);
    
    // COPY PASS
    if (vertices.size() != 0 && indices.size() != 0){
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
     
      SDL_UploadToGPUBuffer(copyPass, &indexLocation, &indexRegion, true);
    
      SDL_EndGPUCopyPass(copyPass);  
    }
    // END OF COPY PASS
    
    
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
    SDL_DrawGPUIndexedPrimitives(renderPass, indices.size(), 1, 0, 0, 0);
    
    
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
