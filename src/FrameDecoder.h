#ifndef FRAMEDECODER_H
#define FRAMEDECODER_H
#include "Image.h"

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/imgutils.h>
#include <libavutil/avutil.h>
#include <libswscale/swscale.h>
}

class FrameDecoder {
private:
  AVFormatContext* format_ctx;
  AVCodecContext* codec_ctx;
  AVFrame* frame;
  AVPacket* packet;

  static AVFormatContext* makeFormatCtx(const char* input_file);
  static AVCodecContext* makeCodecCtx(AVFormatContext* format_ctx);
  static AVFrame* makeFrame();
  static AVPacket* makePacket();

public:
  FrameDecoder(const char* img_file); 
  ~FrameDecoder();
  Image next();
};
#endif
