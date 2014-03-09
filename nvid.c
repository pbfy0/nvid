/*
  Copyright (c) 2010 The WebM project authors. All Rights Reserved.

  Use of this source code is governed by a BSD-style license
  that can be found in the LICENSE file in the root of the source
  tree. An additional intellectual property rights grant can be found
  in the file PATENTS.  All contributing project authors may
  be found in the AUTHORS file in the root of the source tree.
 */
 
 
/*
 This is an example of a simple decoder loop. It takes an input file
 containing the compressed data (in IVF format), passes it through the
 decoder, and writes the decompressed frames to disk. Other decoder
 examples build upon this one.
 
 The details of the IVF format have been elided from this example for
 simplicity of presentation, as IVF files will not generally be used by
 your application. In general, an IVF file consists of a file header,
 followed by a variable number of frames. Each frame consists of a frame
 header followed by a variable length payload. The length of the payload
 is specified in the first four bytes of the frame header. The payload is
 the raw compressed data.
 */
#include <os.h>
#include <libndls.h>
#include <nspireio2.h>

FILE            *infile;
nio_console csl;
#define printf(...) nio_printf(&csl, __VA_ARGS__)

//#define VPX_CODEC_DISABLE_COMPAT 1
//#include "vpx/vpx_decoder.h"
//#include "vpx/vp8dx.h"
//#define vpx_interface (vpx_codec_vp8_dx())
//#define clamp(x) (x < 0 ? 0 : (x > 255 ? 255 : x))
 
 
#define IVF_FILE_HDR_SZ  (32)
#define IVF_FRAME_HDR_SZ (12)
 
static unsigned int mem_get_le32(const unsigned char *mem) {
    return (mem[3] << 24)|(mem[2] << 16)|(mem[1] << 8)|(mem[0]);
}
 
static void die(const char *text) {
    uart_printf(text);
    if(text[strlen(text)-1] != '\n')
        uart_printf("\n");
		close(infile);
    exit(EXIT_FAILURE);
}
 
/*static void die_codec(vpx_codec_ctx_t *ctx, const char *s) {
    const char *detail = vpx_codec_error_detail(ctx);
    uart_printf("%s: %s\n", s, vpx_codec_error(ctx));
    if(detail)
        uart_printf("    %s\n",detail);
		close(infile);
    exit(EXIT_FAILURE);
}*/
 
 
int main(int argc, char **argv) {
		uart_printf("asdasdasd\n");
    //vpx_codec_ctx_t  codec;
    int              flags = 0, frame_cnt = 0;
    unsigned char    file_hdr[IVF_FILE_HDR_SZ];
    unsigned char    frame_hdr[IVF_FRAME_HDR_SZ];
    unsigned char    frame[256*1024];
    //vpx_codec_err_t  res;
		
		nio_console csl;
		clrscr();
		// 53 columns, 29 rows. 0px offset for x/y. Background color 0 (black), foreground color 15 (white)
		nio_init(&csl, 53, 29, 0, 0, 0, 15, TRUE);
		nio_fflush(&csl);
		nio_set_default(&csl);
		printf("hello world!");
 
    //(void)res;
    /* Open files */
		if(argc!=2){
			die("Wrong number of arguments.");
			return 0;
		}
		if(!(infile = fopen(argv[1], "rb"))){
			die("Could not open input file");
			return 0;
		}
		//return 0;
 
    /* Read file header */
    if(!(fread(file_hdr, 1, IVF_FILE_HDR_SZ, infile) == IVF_FILE_HDR_SZ
         && file_hdr[0]=='D' && file_hdr[1]=='K' && file_hdr[2]=='I'
         && file_hdr[3]=='F'))
        die("Not an IVF file!");
 
    //printf("Using %s\n",vpx_codec_iface_name(vpx_interface));
    /* Initialize codec */
    //if(vpx_codec_dec_init(&codec, vpx_interface, NULL, flags))
    //    die_codec(&codec, "Failed to initialize decoder");
 
    /* Read each frame */
    while(fread(frame_hdr, 1, IVF_FRAME_HDR_SZ, infile) == IVF_FRAME_HDR_SZ) {
        unsigned int               frame_sz = mem_get_le32(frame_hdr);
        //vpx_codec_iter_t  iter = NULL;
        //vpx_image_t      *img;
 
 
        frame_cnt++;
				uart_printf("x%u\n", frame_sz);
        if(frame_sz > sizeof(frame))
            die("Frame data too big for example code buffer");
				
        if(fread(frame, 1, frame_sz, infile) != frame_sz)
            die("Frame failed to read complete frame");
 
        
    }
		
    printf("Processed %d frames.\n",frame_cnt);
    //if(vpx_codec_destroy(&codec))
//        die_codec(&codec, "Failed to destroy codec");
 
    fclose(infile);
    return EXIT_SUCCESS;
}
