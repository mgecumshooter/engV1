#pragma once

#include <SDL3/SDL_main.h>
#include <SDL3/SDL.h>
#include <SDL3/SDL_gpu.h>

namespace basicRend {
  
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

}
