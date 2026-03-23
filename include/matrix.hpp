#pragma once
#include <array>
#include <cmath>


struct Mat4{
  std::array<float, 16> m = {
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
