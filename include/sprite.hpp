#pragma once
#include <cstdint>
#include <vector>
#include <array>
#include <SDL3/SDL.h>
#include "init.hpp"
#include "vertex.hpp"

struct Sprite {
  float x, y, w, h;
  uint16_t firstIndex;
  std::array<float, 2> velocity = {0.f, 0.f};
  SDL_FColor color {1.0f, 1.0f, 1.0f, 1.0f};
  bool pending_destruction = false;
  bool counted = false;
  SDL_GPUTexture* texture = nullTexture;

  void pushData(std::vector<Vertex>& v, std::vector<uint16_t>& i) {
    uint16_t baseIdx = (uint16_t)v.size();
	firstIndex = i.size();

    v.push_back({x, y, 0, color.r, color.g, color.b, color.a, 0, 0});         // top-left
    v.push_back({x, y + h, 0, color.r, color.g, color.b, color.a, 0, 1});     // bot-left
    v.push_back({x + w, y, 0, color.r, color.g, color.b, color.a, 1, 0});     // top-right
    v.push_back({x + w, y + h, 0, color.r, color.g, color.b, color.a, 1, 1}); // bot-right

    i.push_back(baseIdx + 0); i.push_back(baseIdx + 1); i.push_back(baseIdx + 2);
    i.push_back(baseIdx + 2); i.push_back(baseIdx + 1); i.push_back(baseIdx + 3);
  }
};
