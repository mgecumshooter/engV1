#pragma once
#define STB_IMAGE_IMPLEMENTATION
#include <SDL3/SDL.h>
#include <SDL3/SDL_gpu.h>
#include <iostream>
#include "stb_image.h"

namespace Loader {
	SDL_GPUTexture* load_texture(SDL_GPUDevice* device, const char* path) {
		int width, height, channels;
		unsigned char* pixels = stbi_load(path, &width, &height, &channels, STBI_rgb_alpha);

		if (!pixels) {
			std::cout << "Failed to load texture: " << path << std::endl;
			return nullptr;
		}

		SDL_GPUTextureCreateInfo tex_info{};
		tex_info.type = SDL_GPU_TEXTURETYPE_2D;
		tex_info.format = SDL_GPU_TEXTUREFORMAT_R8G8B8A8_UNORM;
		tex_info.width = width;
		tex_info.height = height;
		tex_info.layer_count_or_depth = 1;
		tex_info.num_levels = 1;
		tex_info.usage = SDL_GPU_TEXTUREUSAGE_SAMPLER;

		SDL_GPUTexture* texture = SDL_CreateGPUTexture(device, &tex_info);

		Uint32 buffer_size = width * height * 4;
		SDL_GPUTransferBufferCreateInfo transfer_info{};
		transfer_info.size = buffer_size;
		transfer_info.usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD;

		SDL_GPUTransferBuffer* upload_buffer = SDL_CreateGPUTransferBuffer(device, &transfer_info);

		void* map = SDL_MapGPUTransferBuffer(device, upload_buffer, false);
		memcpy(map, pixels, buffer_size);
		SDL_UnmapGPUTransferBuffer(device, upload_buffer);
		stbi_image_free(pixels);

		SDL_GPUCommandBuffer* cmd = SDL_AcquireGPUCommandBuffer(device);
		SDL_GPUCopyPass* copy_pass = SDL_BeginGPUCopyPass(cmd);

		SDL_GPUTextureTransferInfo source{};
		source.transfer_buffer = upload_buffer;
		source.offset = 0;

		SDL_GPUTextureRegion dest{};
		dest.texture = texture;
		dest.w = width;
		dest.h = height;
		dest.d = 1;

		SDL_UploadToGPUTexture(copy_pass, &source, &dest, false);
		SDL_EndGPUCopyPass(copy_pass);
		SDL_SubmitGPUCommandBuffer(cmd);

		SDL_ReleaseGPUTransferBuffer(device, upload_buffer);

		return texture;
	}
}
