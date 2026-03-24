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

static SDL_AudioDeviceID audioDevice = 0;

typedef struct Sound {
  Uint8* wavData;
  Uint32 wavDataLen;
  SDL_AudioStream* stream;
} Sound;

static Sound sounds[4];

static void initSound(const char* file, Sound* sound){
  SDL_AudioSpec spec;

  SDL_LoadWAV(file, &spec, &sound->wavData, &sound->wavDataLen);

  sound->stream = SDL_CreateAudioStream(&spec, NULL);
  SDL_BindAudioStream(audioDevice, sound->stream);
}

bool collision(Sprite* player, Sprite* body){
  bool overlapX = (player->x < body->x + body->w) && (player->x + player->w > body->x);
  bool overlapY = (player->y < body->y + body->h) && (player->y + player->h > body->y);

  return overlapX && overlapY;
}



int main(int argc, char **argv) {

  init();

  audioDevice = SDL_OpenAudioDevice(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, NULL);
  if (audioDevice == 0){
    cerr << "AUDIO DEVICE PROBLEM" << endl;
    exit(1);
  }

  initSound("resources/sounds/music2.wav", &sounds[0]);
  initSound("resources/sounds/jump.wav", &sounds[1]);
  initSound("resources/sounds/coin3.wav", &sounds[2]);
  initSound("resources/sounds/hurt1.wav", &sounds[3]);

  Scene scene;
  Sprite* painis = scene.spawn(150.f, 150.f, 45.f, 45.f);
  // Sprite* rect = scene.spawn(wWidth, wHeight / 2.f, 45.f, wHeight);
  // Sprite* rect2 = scene.spawn(wWidth, wHeight / 2.f, 45.f, wHeight);

  const float pipeWidth = 45.f;
  const float pipeGapY = painis->h * 3;
  const float pipeGapX = painis->w * 7;

  int pipePairsCount = (wWidth / pipeGapX) + 2;

  vector<pair<Sprite*, Sprite*>> pipes;

  for (int i = 0; i < pipePairsCount; ++i){
    float startX = wWidth + (i * pipeGapX);

    Sprite* top = scene.spawn(startX, 0, pipeWidth, wHeight);
    Sprite* bot = scene.spawn(startX, 0, pipeWidth, wHeight);

    top->y = -(wHeight * 0.25f + fmod(rand(), wHeight * 0.5f));
    bot->y = top->y + top->h + pipeGapY;

    pipes.push_back({top, bot});
  }
  
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
	if (event.key.scancode == SDL_SCANCODE_SPACE) {
	  painis->velocity[1] = -5.f;
	  SDL_PutAudioStreamData(sounds[1].stream, sounds[1].wavData, (int)sounds[1].wavDataLen);
	  
	}else if (event.key.scancode == SDL_SCANCODE_E){
	  vertices.clear();
	  indices.clear();
	}
	
	break; // Обязательно выходим из KEY_DOWN

	
      case SDL_EVENT_WINDOW_RESIZED: // Теперь это на своем месте
	wWidth = event.window.data1;
	wHeight = event.window.data2;
	// rect->w = wHeight;
	// ОБНОВЛЯЕМ существующую переменную, а не создаем новую!
	projection = Mat4::ortho(0, (float)wWidth, (float)wHeight, 0, -1.f, 1.f);
	break;

	
      }
    }

    if (SDL_GetAudioStreamQueued(sounds[0].stream) < (int)sounds[0].wavDataLen){
      SDL_PutAudioStreamData(sounds[0].stream, sounds[0].wavData, (int)sounds[0].wavDataLen);
    }

    // rect->x -= 2.f;
    // rect2->y = rect->y - rect->h - painis->h * 3;
    // rect2->x = rect->x;
    // if (rect->x <= -rect->w){
    //   rect->x = wWidth;
    //   rect->y = wHeight * 0.25f + fmod(rand(), wHeight * 0.5f);
    // }
    if (painis->velocity[1] < 20.f) {
      painis->velocity[1] += 0.2f;
    }
    // if (painis->y >= wHeight){
    //   running = false;
    // }
    // if (painis->y <= -painis->h){
    //   running = false;
    // }

    for (auto& pair: pipes){
      Sprite* top = pair.first;
      Sprite* bot = pair.second;

      top->x -= 3.f;
      bot->x -= 3.f;

      if (top->x <= -top->w){
	top->counted = false;
	
	float maxX = 0;
	for (const auto& p: pipes){
	  if (p.first->x > maxX){
	    maxX = p.first->x;
	  }
	}

	top->x = maxX + pipeGapX;
	bot->x = top->x;

	top->y = -(wHeight * 0.25f + fmod(rand(), wHeight * 0.7f));
	bot->y = top->y + top->h + pipeGapY;
      }

      if ((painis->x > top->x + top->w) && !top->counted){
	top->counted = true;
	SDL_PutAudioStreamData(sounds[2].stream, sounds[2].wavData, (int)sounds[2].wavDataLen);
      }

      if ((collision(painis, top) || collision(painis, bot)) || (painis->y >= wHeight || painis->y <= -painis->h)){
	// running = false;

	SDL_PutAudioStreamData(sounds[3].stream, sounds[3].wavData, (int)sounds[3].wavDataLen);
	
	painis->y = wHeight / 2 - painis->h;
	painis->velocity[1] = 0;

	int i = 0;
        for (auto& pp : pipes){
	  i++;
	  float startX = wWidth + (i * pipeGapX);

	  Sprite* Ptop = pp.first;
	  Sprite* Pbot = pp.second;

	  Ptop->x = startX;
	  Pbot->x = Ptop->x;

	  Ptop->y = -(wHeight * 0.25f + fmod(rand(), wHeight * 0.75f));
	  Pbot->y = Ptop->y + Ptop->h + pipeGapY;
	  
	}
      }
    }
    // if (collision(painis, rect)){
    //   running = false;
    // }
    // if (collision(painis, rect2)){
    //   running = false;
    // }

    
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

    Mat4 matrix = Mat4::translate(0.f, 0.f, 0.f);
    Mat4 finalMPV = projection * matrix;
    SDL_PushGPUVertexUniformData(commandBuffer, 0, &finalMPV, sizeof(Mat4));

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
