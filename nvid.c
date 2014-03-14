/*
  Copyright (c) 2010 The WebM project authors. All Rights Reserved.

  Use of this source code is governed by a BSD-style license
  that can be found in the LICENSE file in the root of the source
  tree. An additional intellectual property rights grant can be found
  in the file PATENTS.  All contributing project authors may
  be found in the AUTHORS file in the root of the source tree.
 */

#include <os.h>
#include <libndls.h>
#include <nspireio2.h>

#include <stdint.h>

FILE			*infile;

#define VPX_CODEC_DISABLE_COMPAT 1
#include "vpx/vpx_decoder.h"
#include "vpx/vp8dx.h"
#define vpx_interface (vpx_codec_vp8_dx())
#define FRAME_SIZE 256*1024
#define mem_get_le32(mem) ((mem[3]<<24)|(mem[2]<<16)|(mem[1]<<8)|(mem[0]))
 
 
#define IVF_FILE_HDR_SZ  (32)
#define IVF_FRAME_HDR_SZ (12)
 
//static unsigned int mem_get_le32(const uint8_t *mem) {
//	return (mem[3] << 24)|(mem[2] << 16)|(mem[1] << 8)|(mem[0]);
//}
 
static void die(char *text) {
	uart_printf(text);
	if(text[strlen(text)-1] != '\n')
		uart_printf("\n");
	fclose(infile);
	exit(EXIT_FAILURE);
}

static uint8_t clamp(int x){
		return (x < 0 ? 0 : (x > 255 ? 255 : x));
}
 
static void die_codec(vpx_codec_ctx_t *ctx, char *s) {
	const char *detail = vpx_codec_error_detail(ctx);
	uart_printf("%s: %s\n", s, vpx_codec_error(ctx));
	if(detail)
		uart_printf("	%s\n",detail);
	fclose(infile);
	exit(EXIT_FAILURE);
}
 
 
int main(int argc, char **argv) {
	vpx_codec_ctx_t  codec;
	int			  flags = 0, frame_cnt = 0;
	uint8_t	file_hdr[IVF_FILE_HDR_SZ];
	uint8_t	frame_hdr[IVF_FRAME_HDR_SZ];
	uint8_t	*frame = malloc(FRAME_SIZE*sizeof(uint8_t));
	//vpx_codec_err_t  res;
 
	/* Open files */
	if(argc!=2){
		cfg_register_fileext("ivf", "nvid");
		show_msgbox("Info", "File extension registered\n" "To use, open an ivf file.");
		return 0;
	}
	if(!(infile = fopen(argv[1], "rb"))){
		show_msgbox("Error", "Could not open video file.");
		return 2;
	}
	uint8_t color_mode = has_colors;
	int screen_size = SCREEN_BYTES_SIZE;
	uint8_t *frame_buffer = SCREEN_BASE_ADDRESS;
 
	/* Read file header */
	if(!(fread(file_hdr, 1, IVF_FILE_HDR_SZ, infile) == IVF_FILE_HDR_SZ
		 && file_hdr[0]=='D' && file_hdr[1]=='K' && file_hdr[2]=='I'
		 && file_hdr[3]=='F'))
		die("Not an IVF file!");
 
	//printf("Using %s\n",vpx_codec_iface_name(vpx_interface));
	/* Initialize codec */
	if(vpx_codec_dec_init(&codec, vpx_interface, NULL, flags))
		die_codec(&codec, "Failed to initialize decoder");
	
	unsigned int frame_sz;
	uint8_t *output_frame_data = malloc(screen_size);
	/* Read each frame */
	while(fread(frame_hdr, 1, IVF_FRAME_HDR_SZ, infile) == IVF_FRAME_HDR_SZ) {
		frame_sz = mem_get_le32(frame_hdr);
		vpx_codec_iter_t  iter = NULL;
		vpx_image_t	  *img;
  
		frame_cnt++;
		if(frame_sz > FRAME_SIZE)
			die("Frame data too big for example code buffer");
				
		if(fread(frame, 1, frame_sz, infile) != frame_sz)
			die("Failed to read complete frame");
		
		if(vpx_codec_decode(&codec, frame, frame_sz, NULL, 0))
			die_codec(&codec, "Failed to decode frame");
		
		/* Write decoded data to disk */
		while((img = vpx_codec_get_frame(&codec, &iter))) {
			unsigned int x, y;
			unsigned int width = img->d_w;
			unsigned int height = img->d_h;
			uint8_t *ybuf =img->planes[0];
			
			if(color_mode){
				uint8_t *ubuf = img->planes[1];
				uint8_t *vbuf = img->planes[2];
				for(y=0; y < height; y++) {
					for(x=0; x < width; x++) {
						int c = ybuf[x] - 16;
						int d = ubuf[x>>1] - 128;
						int e = vbuf[x>>1] - 128;
						uint8_t r = clamp((298*c+409*e+128) >> 8);
						uint8_t g = clamp((298*c-100*d-208*e+128) >> 8);
						uint8_t b = clamp((298*c+516*d+128) >> 8);
						unsigned short *pixel = (unsigned short *)(output_frame_data+((y*width+x)<<1));
						*pixel = (r>>3)<<11 | (g>>2)<<5 | b>>3;
					}

					ybuf += img->stride[0];
					if(y&1){
						ubuf += img->stride[1];
						vbuf += img->stride[2];
					}
				}
			}else{
				for(y=0; y < height; y++) {
					for(x=0; x < width; x+=2) {
						output_frame_data[(y*width+x)>>1] = (ybuf[x]&0xf0) | (ybuf[x+1]>>4);
					}
					ybuf += img->stride[0];
				}
			}
			memcpy(frame_buffer, output_frame_data, screen_size);
			if(isKeyPressed(KEY_NSPIRE_ESC)){
				vpx_codec_destroy(&codec);
				return 0;
			}
		}
	}
	
	if(vpx_codec_destroy(&codec))
		die_codec(&codec, "Failed to destroy codec");

	fclose(infile);
	return EXIT_SUCCESS;
}
