#ifndef IMAGE_H
#define IMAGE_H

#include <iostream>
#include <vector>

struct Image {
  size_t width;
  size_t height;
  size_t stride;
  std::vector<uint8_t> yData;
	std::vector<uint8_t> uData;
	std::vector<uint8_t> vData;
};
#endif
