#ifndef FRAME_H
#define FRAME_H

#include <iostream>
#include <vector>

struct Frame {
  size_t width;
  size_t height;
  std::vector<uint8_t> yData;
	std::vector<uint8_t> uData;
	std::vector<uint8_t> vData;
	bool eof;
};
#endif
