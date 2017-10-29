// tutorial01.c
//
// This tutorial was written by Stephen Dranger (dranger@gmail.com).
//
// Code based on a tutorial by Martin Bohme (boehme@inb.uni-luebeckREMOVETHIS.de)
// Tested on Gentoo, CVS version 5/01/07 compiled with GCC 4.1.1

// A small sample program that shows how to use libavformat and libavcodec to
// read video from a file.
//
// Use the Makefile to build all examples.
//
// Run using
//
// tutorial01 myvideofile.mpg
//
// to write the first five frames from "myvideofile.mpg" to disk in PPM
// format.

#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>

#include <stdio.h>

#define PIX_FMT_RGB24 24

void SaveFrame(AVFrame *pFrame, int width, int height, int iFrame) {
  FILE *pFile;
  char szFilename[32];
  int  y;
  
  // Open file
  sprintf(szFilename, "frame%d.ppm", iFrame);
  pFile=fopen(szFilename, "wb");
  if(pFile==NULL)
    return;
  
  // Write header
  fprintf(pFile, "P6\n%d %d\n255\n", width, height);
  
  // Write pixel data
  for(y=0; y<height; y++)
    fwrite(pFrame->data[0]+y*pFrame->linesize[0], 1, width*3, pFile);
  
  // Close file
  fclose(pFile);
}

int main(int argc, char *argv[]) {
  AVFormatContext *pFormatCtx = NULL;
  int             i, videoStream;
  AVCodecContext  *pCodecCtx = NULL;
  AVCodec         *pCodec = NULL;
  AVFrame         *pFrame = NULL; 
  AVFrame         *pFrameRGB = NULL;
  AVPacket        packet;
  int             frameFinished;
  int             numBytes;
  uint8_t         *buffer = NULL;

  AVDictionary    *optionsDict = NULL;
  struct SwsContext      *sws_ctx = NULL;
  
  if(argc < 2) {
    printf("Please provide a movie file\n");
    return -1;
  }
  // Register all formats and codecs
  //
  // Initialize libavformat and register all the muxers, 
  // demuxers and protocols.
  av_register_all();
  
  // Open video file
  // 只是读取文件的头部并且把信息保存到AVFormatContext
  //
  // Open an input stream and read the header.
  // The codecs are not opened. The stream must be closed with 
  // avformat_close_input().
  if(avformat_open_input(&pFormatCtx, argv[1], NULL, NULL)!=0)
    return -1; // Couldn't open file
  
  // Retrieve stream information
  // 这个函数为pFormatCtx->streams填充上正确的信息
  if(avformat_find_stream_info(pFormatCtx, NULL)<0)
    return -1; // Couldn't find stream information
  
  // Dump information about file onto standard error
  av_dump_format(pFormatCtx, 0, argv[1], 0);
  
  // Find the first video stream
  videoStream=-1;
  for(i=0; i<pFormatCtx->nb_streams; i++)
    if(pFormatCtx->streams[i]->codec->codec_type==AVMEDIA_TYPE_VIDEO) {
      videoStream=i;
      break;
    }
  if(videoStream==-1)
    return -1; // Didn't find a video stream
  
  // Get a pointer to the codec context for the video stream.
  // codec context contains all the information about the codec 
  // that the stream is using
  // 获得编解码器的指针
  pCodecCtx=pFormatCtx->streams[videoStream]->codec;
  
  // Find the decoder for the video stream
  //
  // Find a registered decoder with a matching codec ID.
  pCodec=avcodec_find_decoder(pCodecCtx->codec_id);
  if(pCodec==NULL) {
    fprintf(stderr, "Unsupported codec!\n");
    return -1; // Codec not found
  }

  // int avcodec_open2(AVCodecContext *avctx, const AVCodec *codec, AVDictionary **options)	
  //
  // Initialize the AVCodecContext to use the given AVCodec.
  // Always call this function before using decoding routines 
  // (such as avcodec_decode_video2()).
  if(avcodec_open2(pCodecCtx, pCodec, &optionsDict)<0)
    return -1; // Could not open codec
  
  // Allocate video frame
  // 用于保存帧 
  pFrame=av_frame_alloc();
  
  // Allocate an AVFrame structure
  // 为原始的帧转换成PPM文件来申请一帧的文件
  pFrameRGB=av_frame_alloc();
  if(pFrameRGB==NULL)
    return -1;
  
  /*
   * 即使我们申请了一帧的内存，当转换的时候，我们仍然需要一个
   * 地方来放置原始的数据。
   * */
  // Determine required buffer size and allocate buffer
  numBytes=avpicture_get_size(PIX_FMT_RGB24, pCodecCtx->width,
			      pCodecCtx->height);
  buffer=(uint8_t *)av_malloc(numBytes*sizeof(uint8_t));

  sws_ctx =
    sws_getContext
    (
        pCodecCtx->width,
        pCodecCtx->height,
        pCodecCtx->pix_fmt,
        pCodecCtx->width,
        pCodecCtx->height,
        PIX_FMT_RGB24,
        SWS_BILINEAR,
        NULL,
        NULL,
        NULL
    );
  
  /*
   * avpicture_fill 把帧和我们新申请的内存来结合。
   * */
  // Assign appropriate parts of buffer to image planes in pFrameRGB
  // Note that pFrameRGB is an AVFrame, but AVFrame is a superset
  // of AVPicture
  avpicture_fill((AVPicture *)pFrameRGB, buffer, PIX_FMT_RGB24,
		 pCodecCtx->width, pCodecCtx->height);
  
  // int av_read_frame(AVFormatContext *s, AVPacket *pkt)	
  // Return the next frame of a stream.
  //
  // This function returns what is stored in the file, and does not 
  // validate that what is there are valid frames for the decoder. It 
  // will split what is stored in the file into frames and return one 
  // for each call. It will not omit invalid data between valid frames 
  // so as to give the decoder the maximum information possible for 
  // decoding.
  //
  // If pkt->buf is NULL, then the packet is valid until the next 
  // av_read_frame() or until avformat_close_input(). Otherwise the 
  // packet is valid indefinitely. In both cases the packet must be 
  // freed with av_packet_unref when it is no longer needed. For video, 
  // the packet contains exactly one frame. For audio, it contains an 
  // integer number of frames if each frame has a known fixed size 
  // (e.g. PCM or ADPCM data). If the audio frames have a variable size 
  // (e.g. MPEG audio), then it contains one frame.
  //
  // pkt->pts, pkt->dts and pkt->duration are always set to correct values 
  // in AVStream.time_base units (and guessed if the format cannot provide 
  // them). pkt->pts can be AV_NOPTS_VALUE if the video format has B-frames, 
  // so it is better to rely on pkt->dts if you do not decompress the payload.
  /*
   * av_read_frame()读取一个包并且把它保存到AVPacket结构体中。
   * */
  // Read frames and save first five frames to disk
  i=0;
  while(av_read_frame(pFormatCtx, &packet)>=0) {
    // Is this a packet from the video stream?
    if(packet.stream_index==videoStream) {
	  /*
	   * avcodec_decode_video2()把包转换为帧。 
	   * */
      // Decode video frame
	  //
	  // int avcodec_decode_video2(AVCodecContext *avctx, AVFrame *picture,
	  // int *got_picture_ptr, const AVPacket *avpkt)	
	  //
	  // Decode the video frame of size avpkt->size from avpkt->data 
	  // into picture. Some decoders may support multiple frames in a 
	  // single AVPacket, such decoders would then just decode the first frame.
	  //
	  // Parameters
	  // 		avctx	the codec context
	  // [out]	picture	The AVFrame in which the decoded video frame will 
	  // 				be stored. Use av_frame_alloc() to get an AVFrame. 
	  // 				The codec will allocate memory for the actual bitmap 
	  // 				by calling the AVCodecContext.get_buffer2() callback. 
	  // 				When AVCodecContext.refcounted_frames is set to 1, 
	  // 				the frame is reference counted and the returned 
	  // 				reference belongs to the caller. The caller must 
	  // 				release the frame using av_frame_unref() when the 
	  // 				frame is no longer needed. The caller may safely 
	  // 				write to the frame if av_frame_is_writable() returns 
	  // 				1. When AVCodecContext.refcounted_frames is set to 0, 
	  // 				the returned reference belongs to the decoder and is 
	  // 				valid only until the next call to this function or 
	  // 				until closing or flushing the decoder. The caller may 
	  // 				not write to it.
	  // [in]	avpkt	The input AVPacket containing the input buffer. You 
	  // 				can create such packet with av_init_packet() and by 
	  // 				then setting data and size, some decoders might in 
	  // 				addition need other fields like flags&AV_PKT_FLAG_KEY.
	  // 				All decoders are designed to use the least fields 
	  // 				possible.
	  // [in,out] got_picture_ptr	Zero if no frame could be decompressed, 
	  // 							otherwise, it is nonzero.
      avcodec_decode_video2(pCodecCtx, pFrame, &frameFinished, 
			   &packet);
      
      // Did we get a video frame?
      if(frameFinished) {
		// Convert the image from its native format to RGB
		//
		//
		// int sws_scale(struct SwsContext *c,
		// const uint8_t *const 	srcSlice[],
		// const int 	srcStride[],
		// int 	srcSliceY,
		// int 	srcSliceH,
		// uint8_t *const 	dst[],
		// const int 	dstStride[] 
		// )	
		//
	    // Scale the image slice in srcSlice and put the resulting scaled 
		// slice in the image in dst. A slice is a sequence of consecutive 
		// rows in an image. Slices have to be provided in sequential order, 
		// either in top-bottom or bottom-top order. If slices are provided 
		// in non-sequential order the behavior of the function is undefined.
		//
		//	Parameters
		//	c			the scaling context previously created with 
		//				sws_getContext()
		//	srcSlice	the array containing the pointers to the planes 
		//				of the source slice
		//	srcStride	the array containing the strides for each plane 
		//				of the source image
		//	srcSliceY	the position in the source image of the slice to 
		//				process, that is the number (counted starting from 
		//				zero) in the image of the first row of the slice
		//	srcSliceH	the height of the source slice, that is the number 
		//				of rows in the slice
		//	dst			the array containing the pointers to the planes of 
		//				the destination image
		//	dstStride	the array containing the strides for each plane of 
		//				the destination image
        sws_scale
        (
            sws_ctx,
            (uint8_t const * const *)pFrame->data,
            pFrame->linesize,
            0,
            pCodecCtx->height,
            pFrameRGB->data,
            pFrameRGB->linesize //linesize : For video, size in bytes of each picture line.
        );
	
	// Save the frame to disk
	if(++i<=5)
		SaveFrame(pFrameRGB, pCodecCtx->width, pCodecCtx->height, i);
      }
    }
    
    // Free the packet that was allocated by av_read_frame
    av_free_packet(&packet);
  }
  
  // Free the RGB image
  av_free(buffer);
  av_free(pFrameRGB);
  
  // Free the YUV frame
  av_free(pFrame);
  
  // Close the codec
  avcodec_close(pCodecCtx);
  
  // Close the video file
  avformat_close_input(&pFormatCtx);
  
  return 0;
}
