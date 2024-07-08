#include "Texture.h"

Texture imgToTexture(const Image& image) {
  // create texture for each plane
  GLuint yTexture, uTexture, vTexture;
  glGenTextures(1, &yTexture);
  glGenTextures(1, &uTexture);
  glGenTextures(1, &vTexture);

  // Y plane
  glBindTexture(GL_TEXTURE_2D, yTexture);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, image.stride, image.height, 0, GL_RED, GL_UNSIGNED_BYTE, image.yData.data());
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,  GL_CLAMP_TO_BORDER);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  // U plane
  glBindTexture(GL_TEXTURE_2D, uTexture);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, image.stride / 2, image.height / 2, 0, GL_RED, GL_UNSIGNED_BYTE, image.uData.data());
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  // V plane
  glBindTexture(GL_TEXTURE_2D, vTexture);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, image.stride / 2, image.height / 2, 0, GL_RED, GL_UNSIGNED_BYTE, image.vData.data());
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  return { yTexture, uTexture, vTexture };
}

