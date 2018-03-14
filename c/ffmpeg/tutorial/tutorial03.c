// tutorial03.c
// A pedagogical video player that will stream through every video frame as fast as it can
// and play audio (out of sync).
//
// This tutorial was written by Stephen Dranger (dranger@gmail.com).
//
// Code based on FFplay, Copyright (c) 2003 Fabrice Bellard, 
// and a tutorial by Martin Bohme (boehme@inb.uni-luebeckREMOVETHIS.de)
// Tested on Gentoo, CVS version 5/01/07 compiled with GCC 4.1.1
//
// Use the Makefile to build all examples.
//
// Run using
// tutorial03 myvideofile.mpg
//
// to play the stream on your screen.


#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>

#include <SDL.h>
#include <SDL_thread.h>

#ifdef __MINGW32__
#undef main /* Prevents SDL from overriding main() */
#endif

#include <stdio.h>

#define SDL_AUDIO_BUFFER_SIZE 1024
#define MAX_AUDIO_FRAME_SIZE 192000

//gcc -o tutorial03 tutorial03.c -lavutil -lavformat -lavcodec -lz -lavutil -lm -lpthread -lswresample -lswscale `sdl-config --cflags --libs`

typedef struct PacketQueue {
  AVPacketList *first_pkt, *last_pkt;
  int nb_packets;
  // size表示我们从packet->size中得到的字节数 
  int size;
  SDL_mutex *mutex;
  SDL_cond *cond;
} PacketQueue;

PacketQueue audioq;

int quit = 0;

void packet_queue_init(PacketQueue *q) {
  memset(q, 0, sizeof(PacketQueue));
  q->mutex = SDL_CreateMutex();
  q->cond = SDL_CreateCond();
}
int packet_queue_put(PacketQueue *q, AVPacket *pkt) {

  AVPacketList *pkt1;
  if(av_dup_packet(pkt) < 0) {
    return -1;
  }
  pkt1 = av_malloc(sizeof(AVPacketList));
  if (!pkt1)
    return -1;
  pkt1->pkt = *pkt;
  pkt1->next = NULL;
  
  
  SDL_LockMutex(q->mutex);
  
  // 从队尾加入
  if (!q->last_pkt)
    q->first_pkt = pkt1;
  else
    q->last_pkt->next = pkt1;

  q->last_pkt = pkt1;
  q->nb_packets++;
  q->size += pkt1->pkt.size;
  // 发起信号通知有数据
  SDL_CondSignal(q->cond);
  
  SDL_UnlockMutex(q->mutex);
  return 0;
}

static int packet_queue_get(PacketQueue *q, AVPacket *pkt, int block)
{
  AVPacketList *pkt1;
  int ret;
  
  SDL_LockMutex(q->mutex);
  
  for(;;) {
    
    if(quit) {
      ret = -1;
      break;
    }

    pkt1 = q->first_pkt;
    if (pkt1) {
    	q->first_pkt = pkt1->next;
      	
		if (!q->first_pkt)
			q->last_pkt = NULL;

      	q->nb_packets--;
      	q->size -= pkt1->pkt.size;
      	*pkt = pkt1->pkt;
      	av_free(pkt1);
      	ret = 1;
      	break;
    } else if (!block) {
      	ret = 0;
      	break;
    } else {
		// SDL_CondWait 的行为如下：
		// 1. unlock the mutex specified by mutex;
		// 2. block the calling thread until another thread signals the condition variable
		// 3. relock mutex
      	SDL_CondWait(q->cond, q->mutex);
    }
  }

  SDL_UnlockMutex(q->mutex);
  return ret;
}

int audio_decode_frame(AVCodecContext *aCodecCtx, uint8_t *audio_buf, int buf_size) {

  static AVPacket pkt;
  static uint8_t *audio_pkt_data = NULL;
  static int audio_pkt_size = 0;
  static AVFrame frame;

  // len1 表示解码使用的数据在包中的大小
  // data_size 表示实际返回的原始音频数据的大小
  int len1, data_size = 0;

  for(;;) {
    while(audio_pkt_size > 0) {
      int got_frame = 0;
	  /*
	   * int avcodec_decode_audio4(AVCodecContext *avctx,
	   * AVFrame * 	frame,
	   * int * 	got_frame_ptr,
	   * const AVPacket * 	avpkt 
	   * )	
	   *
	   * Decode the audio frame of size avpkt->size from avpkt->data into 
	   * frame.
	   *
	   * Some decoders may support multiple frames in a single AVPacket. 
	   * Such decoders would then just decode the first frame and the 
	   * return value would be less than the packet size. In this case, 
	   * avcodec_decode_audio4 has to be called again with an AVPacket 
	   * containing the remaining data in order to decode the second frame, 
	   * etc... Even if no frames are returned, the packet needs to be 
	   * fed to the decoder with remaining data until it is completely 
	   * consumed or an error occurs.
	   *
	   * Some decoders (those marked with AV_CODEC_CAP_DELAY) have a 
	   * delay between input and output. This means that for some packets 
	   * they will not immediately produce decoded output and need to be 
	   * flushed at the end of decoding to get all the decoded data. 
	   * Flushing is done by calling this function with packets with 
	   * avpkt->data set to NULL and avpkt->size set to 0 until it stops 
	   * returning samples. It is safe to flush even those decoders that 
	   * are not marked with AV_CODEC_CAP_DELAY, then no samples will be 
	   * returned.
	   *
	   * The AVCodecContext MUST have been opened with avcodec_open2() 
	   * before packets may be fed to the decoder.
	   * 
	   * 		avctx	the codec context
	   * [out]	frame	The AVFrame in which to store decoded audio samples. 
	   * 				The decoder will allocate a buffer for the decoded 
	   * 				frame by calling the AVCodecContext.get_buffer2() 
	   * 				callback. When AVCodecContext.refcounted_frames is 
	   * 				set to 1, the frame is reference counted and the 
	   * 				returned reference belongs to the caller. The caller 
	   * 				must release the frame using av_frame_unref() when 
	   * 				the frame is no longer needed. The caller may safely 
	   * 				write to the frame if av_frame_is_writable() returns 
	   * 				1. When AVCodecContext.refcounted_frames is set to 
	   * 				0, the returned reference belongs to the decoder and 
	   * 				is valid only until the next call to this function 
	   * 				or until closing or flushing the decoder. The caller 
	   * 				may not write to it.
	   * [out]	got_frame_ptr	Zero if no frame could be decoded, otherwise 
	   * 						it is non-zero. Note that this field being 
	   * 						set to zero does not mean that an error has 
	   * 						occurred. For decoders with 
	   * 						AV_CODEC_CAP_DELA set, no given decode call 
	   * 						is guaranteed to produce a frame.
	   * [in]	avpkt	The input AVPacket containing the input buffer. At 
	   * 				least avpkt->data and avpkt->size should be set. 
	   * 				Some decoders might also require additional fields 
	   * 				to be set.
	   * */
	  // avcodec_decode_audio2() 的功能就像 avcodec_decode_video2()一样，
	  // 唯一的区别是它的一个包里可能不止一个音频帧。
      len1 = avcodec_decode_audio4(aCodecCtx, &frame, &got_frame, &pkt);

      if(len1 < 0) {
		/* if error, skip frame */
		audio_pkt_size = 0;
		break;
      }

      audio_pkt_data += len1;
      audio_pkt_size -= len1;

      if (got_frame)
      {
          data_size = 
            av_samples_get_buffer_size
            (
                NULL, 
                aCodecCtx->channels,
                frame.nb_samples,
                aCodecCtx->sample_fmt,
                1
            );
          memcpy(audio_buf, frame.data[0], data_size);
      }
      if(data_size <= 0) {
			/* No data yet, get more frames */
			continue;
      }
      /* We have data, return it and come back for more later */
      return data_size;
    }
    if(pkt.data)
      av_free_packet(&pkt);

    if(quit) {
      return -1;
    }

    if(packet_queue_get(&audioq, &pkt, 1) < 0) {
      return -1;
    }

    audio_pkt_data = pkt.data;
    audio_pkt_size = pkt.size;
  }
}

// userdata是SDL的指针
// stream 是音频数据写入的缓冲区指针
// len 是缓冲区的大小
void audio_callback(void *userdata, Uint8 *stream, int len) {

  AVCodecContext *aCodecCtx = (AVCodecContext *)userdata;
  int len1, audio_size;

  // audio_buf 的大小为1.5倍的音频帧的大小
  static uint8_t audio_buf[(MAX_AUDIO_FRAME_SIZE * 3) / 2];
  static unsigned int audio_buf_size = 0;
  static unsigned int audio_buf_index = 0;

  while(len > 0) {
    if(audio_buf_index >= audio_buf_size) {
      /* We have already sent all our data; get more */
      audio_size = audio_decode_frame(aCodecCtx, audio_buf, audio_buf_size);

      if(audio_size < 0) {
		/* If error, output silence */
		audio_buf_size = 1024; // arbitrary?
		memset(audio_buf, 0, audio_buf_size);
      } else {
		audio_buf_size = audio_size;
      }
      	audio_buf_index = 0;
    }

    len1 = audio_buf_size - audio_buf_index;
    if(len1 > len)
      len1 = len;
    memcpy(stream, (uint8_t *)audio_buf + audio_buf_index, len1);
    len -= len1;
    stream += len1;
    audio_buf_index += len1;
  }
}

int main(int argc, char *argv[]) {
  AVFormatContext *pFormatCtx = NULL;
  int             i, videoStream, audioStream;
  AVCodecContext  *pCodecCtx = NULL;
  AVCodec         *pCodec = NULL;
  AVFrame         *pFrame = NULL; 
  AVPacket        packet;
  int             frameFinished;
  //float           aspect_ratio;
  
  AVCodecContext  *aCodecCtx = NULL;
  AVCodec         *aCodec = NULL;

  SDL_Overlay     *bmp = NULL;
  SDL_Surface     *screen = NULL;
  SDL_Rect        rect;
  SDL_Event       event;
  SDL_AudioSpec   wanted_spec, spec;

  struct SwsContext   *sws_ctx            = NULL;
  AVDictionary        *videoOptionsDict   = NULL;
  AVDictionary        *audioOptionsDict   = NULL;

  if(argc < 2) {
    fprintf(stderr, "Usage: test <file>\n");
    exit(1);
  }
  // Register all formats and codecs
  av_register_all();
  
  if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER)) {
    fprintf(stderr, "Could not initialize SDL - %s\n", SDL_GetError());
    exit(1);
  }

  // Open video file
  if(avformat_open_input(&pFormatCtx, argv[1], NULL, NULL)!=0)
    return -1; // Couldn't open file
  
  // Retrieve stream information
  if(avformat_find_stream_info(pFormatCtx, NULL)<0)
    return -1; // Couldn't find stream information
  
  // Dump information about file onto standard error
  av_dump_format(pFormatCtx, 0, argv[1], 0);
  
  // Find the first video stream
  videoStream=-1;
  audioStream=-1;
  for(i=0; i<pFormatCtx->nb_streams; i++) {
    if(pFormatCtx->streams[i]->codec->codec_type==AVMEDIA_TYPE_VIDEO &&
       videoStream < 0) {
      videoStream=i;
    }
    if(pFormatCtx->streams[i]->codec->codec_type==AVMEDIA_TYPE_AUDIO &&
       audioStream < 0) {
      audioStream=i;
    }
  }
  if(videoStream==-1)
    return -1; // Didn't find a video stream
  if(audioStream==-1)
    return -1;
   
  aCodecCtx=pFormatCtx->streams[audioStream]->codec;
  // Set audio settings from codec info
  // 包含在编解码上下文中的所有信息正是我们所需要的用来建立音频的信息。
  // 设置采样率
  wanted_spec.freq = aCodecCtx->sample_rate;
  // 告诉SDL我们将要给的格式。"S16SYS"中的S表示有符号的signed，16表示
  // 每个样本是16位长的，SYS表示大小端的顺序是与使用的系统相同的。这些
  // 格式是由avcodec_decode_audio2为我们给出来的输入音频的格式。
  wanted_spec.format = AUDIO_S16SYS;
  // 音频的通道数
  wanted_spec.channels = aCodecCtx->channels;
  // 表示静音的值。
  wanted_spec.silence = 0;
  // 这是我们想要更多音频的时候，我们想让SDL给出来的音频缓冲区的尺寸。
  // 一个比较合适的值在512到8192之间；ffplay使用1024.
  wanted_spec.samples = SDL_AUDIO_BUFFER_SIZE;
  wanted_spec.callback = audio_callback;
  // 这个是SDL供给回调函数运行的参数。我们将让回调函数得到整个编解码的
  // 上下文。
  wanted_spec.userdata = aCodecCtx;
  
  // 打开音频
  if(SDL_OpenAudio(&wanted_spec, &spec) < 0) {
    fprintf(stderr, "SDL_OpenAudio: %s\n", SDL_GetError());
    return -1;
  }
  // 打开音频编解码器本身
  aCodec = avcodec_find_decoder(aCodecCtx->codec_id);
  if(!aCodec) {
    fprintf(stderr, "Unsupported codec!\n");
    return -1;
  }
  avcodec_open2(aCodecCtx, aCodec, &audioOptionsDict);

  // audio_st = pFormatCtx->streams[index]
  packet_queue_init(&audioq);

  //此函数让音频设备最终开始工作，如果没有立即提供
  //足够的数据，它会播放静音。
  SDL_PauseAudio(0);

  // Get a pointer to the codec context for the video stream
  pCodecCtx=pFormatCtx->streams[videoStream]->codec;
  
  // Find the decoder for the video stream
  pCodec=avcodec_find_decoder(pCodecCtx->codec_id);
  if(pCodec==NULL) {
    fprintf(stderr, "Unsupported codec!\n");
    return -1; // Codec not found
  }
  // Open codec
  if(avcodec_open2(pCodecCtx, pCodec, &videoOptionsDict)<0)
    return -1; // Could not open codec
  
  // Allocate video frame
  pFrame=av_frame_alloc();

  // Make a screen to put our video

#ifndef __DARWIN__
        screen = SDL_SetVideoMode(pCodecCtx->width, pCodecCtx->height, 0, 0);
#else
        screen = SDL_SetVideoMode(pCodecCtx->width, pCodecCtx->height, 24, 0);
#endif
  if(!screen) {
    fprintf(stderr, "SDL: could not set video mode - exiting\n");
    exit(1);
  }
  
  // Allocate a place to put our YUV image on that screen
  bmp = SDL_CreateYUVOverlay(pCodecCtx->width,
				 pCodecCtx->height,
				 SDL_YV12_OVERLAY,
				 screen);
  sws_ctx =
    sws_getContext
    (
        pCodecCtx->width,
        pCodecCtx->height,
        pCodecCtx->pix_fmt,
        pCodecCtx->width,
        pCodecCtx->height,
        AV_PIX_FMT_YUV420P,
        SWS_BILINEAR,
        NULL,
        NULL,
        NULL
    );


  // Read frames and save first five frames to disk
  i=0;
  while(av_read_frame(pFormatCtx, &packet)>=0) {
    // Is this a packet from the video stream?
    if(packet.stream_index==videoStream) {
      // Decode video frame
      avcodec_decode_video2(pCodecCtx, pFrame, &frameFinished, 
			   &packet);
      
      // Did we get a video frame?
      if(frameFinished) {
	SDL_LockYUVOverlay(bmp);

	AVPicture pict;
	pict.data[0] = bmp->pixels[0];
	pict.data[1] = bmp->pixels[2];
	pict.data[2] = bmp->pixels[1];

	pict.linesize[0] = bmp->pitches[0];
	pict.linesize[1] = bmp->pitches[2];
	pict.linesize[2] = bmp->pitches[1];

	// Convert the image into YUV format that SDL uses
    sws_scale
    (
        sws_ctx, 
        (uint8_t const * const *)pFrame->data, 
        pFrame->linesize, 
        0,
        pCodecCtx->height,
        pict.data,
        pict.linesize
    );
	
	SDL_UnlockYUVOverlay(bmp);
	
	rect.x = 0;
	rect.y = 0;
	rect.w = pCodecCtx->width;
	rect.h = pCodecCtx->height;
	SDL_DisplayYUVOverlay(bmp, &rect);
	av_free_packet(&packet);
      }
    } else if(packet.stream_index==audioStream) {
	  // 我们没有在把包放到队列里的时候释放它，我们将在解码后来释放它。
      packet_queue_put(&audioq, &packet);
    } else {
      av_free_packet(&packet);
    }

    // Free the packet that was allocated by av_read_frame
    SDL_PollEvent(&event);
    switch(event.type) {
    case SDL_QUIT:
      quit = 1;
      SDL_Quit();
      exit(0);
      break;
    default:
      break;
    }

  }

  // Free the YUV frame
  av_free(pFrame);
  
  // Close the codec
  avcodec_close(pCodecCtx);
  
  // Close the video file
  avformat_close_input(&pFormatCtx);
  
  return 0;
}
