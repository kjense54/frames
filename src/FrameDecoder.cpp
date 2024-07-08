#include "FrameDecoder.h"
#include <string>

// Open input file, allocate format context
 AVFormatContext* FrameDecoder::makeFormatCtx(const char* input_file) {
  AVFormatContext* format_ctx = nullptr;
  if (avformat_open_input(&format_ctx, input_file, nullptr, nullptr) < 0) {
      throw std::runtime_error(std::string("Failed to open input file: ") + input_file);
  }
  return format_ctx;
}

// Decoder 
AVCodecContext* FrameDecoder::makeCodecCtx(AVFormatContext* format_ctx) {
  // Find stream info
  if (avformat_find_stream_info(format_ctx, nullptr) < 0) {
    throw std::runtime_error("Failed to find stream info");
  }

  // Find video stream
  int video_stream_idx = -1;
  for (unsigned int i = 0; i < format_ctx->nb_streams; i++) {
    if (format_ctx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
      video_stream_idx = i;
      break;
    }
  }
  if (video_stream_idx == -1) {
    throw std::runtime_error("Failed to find video stream");
  }

  // Get codec params and context
  AVCodecParameters* codecpar = format_ctx->streams[video_stream_idx]->codecpar;
  const AVCodec* codec = avcodec_find_decoder(codecpar->codec_id);
  if (!codec) {
    throw std::runtime_error("Failed to find codec");
  }

  AVCodecContext* codec_ctx = avcodec_alloc_context3(codec);
    if (!codec_ctx) {
      throw std::runtime_error("Failed to create codec context");
    }

  if (avcodec_parameters_to_context(codec_ctx, codecpar) < 0) {
    avcodec_free_context(&codec_ctx);
      throw std::runtime_error("Failed to copy codec params");
  }

  //std::cout << "name: " << codec->name << std::endl;
  
  // Open codec
  if (avcodec_open2(codec_ctx, codec, nullptr) < 0) {
      throw std::runtime_error("Failed to open codec");
  }

  return codec_ctx;
}

// allocate frame 
AVFrame* FrameDecoder::makeFrame() {
  AVFrame* frame = av_frame_alloc();
  if (!frame) {
    throw std::runtime_error("Failed to allocate frame");
  }
  return frame;
}

// allocate packet
AVPacket* FrameDecoder::makePacket() {
  AVPacket* packet = av_packet_alloc();
  if (!packet) {
    throw std::runtime_error("Failed to allocate packet");
  }
  return packet;
}

// Constructor & Destructor
FrameDecoder::FrameDecoder(const char* img_file) {
  format_ctx = makeFormatCtx(img_file);
  codec_ctx = makeCodecCtx(format_ctx);
  frame = makeFrame();
  packet = makePacket();
}

FrameDecoder::~FrameDecoder() {
  av_packet_free(&packet);
  av_frame_free(&frame);
  avcodec_free_context(&codec_ctx);
  avformat_close_input(&format_ctx);
  avformat_free_context(format_ctx);
}

// read frames
Image FrameDecoder::next() {
  if (av_read_frame(format_ctx, packet) < 0) {
    throw std::runtime_error("Failed to read frame");
  }

  if (avcodec_send_packet(codec_ctx, packet) < 0) {
    throw std::runtime_error("Failed to send packet");
  }

  int ret = avcodec_receive_frame(codec_ctx, frame);
  if (ret < 0 && ret != AVERROR(EAGAIN) && ret != AVERROR_EOF) {
    throw std::runtime_error("Failed to receive frame");
  }

  // print basic frame properties
  std::cout << "Frame " << codec_ctx->frame_number
    << " (type " << av_get_picture_type_char(frame->pict_type)
    << ", size " << packet->size
    << " bytes, format " << av_get_pix_fmt_name(codec_ctx->pix_fmt)
    << ")" << std::endl;

  size_t width = frame->width;
  size_t height = frame->height;
  size_t stride = frame->linesize[0];
	std::vector<uint8_t>yData(frame->data[0], frame->data[0] + stride * height);
	std::vector<uint8_t>uData(frame->data[1] , frame->data[1] + yData.size() / 4); 
	std::vector<uint8_t>vData(frame->data[2] , frame->data[2] + uData.size()); 

	std::cout << "data: " << frame->side_data << " y: " << yData.size() << " u: " << uData.size()  << " v: " << vData.size() << std::endl;
  return { width, height, stride, yData, uData , vData};
}
