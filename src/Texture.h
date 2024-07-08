#ifndef TEXTURE_H
#define TEXTURE_H

#include "../include/glad/glad.h"
#include "Image.h"

// TEXTURE
  struct Texture {
    GLuint yTex;
    GLuint uTex;
    GLuint vTex;
  };

Texture imgToTexture(const Image& image);

#endif
