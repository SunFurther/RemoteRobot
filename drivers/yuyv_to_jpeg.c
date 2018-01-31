#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>


static void yuv422_to_rgb24(unsigned char *yuv422,unsigned char *rgb24,
int width, int height)  
{  
    int x,y;  
    uint8_t *yuv444;  
    yuv444 = (uint8_t *) malloc(sizeof(uint8_t) * width * height * 3);  
    for(x = 0,y = 0;x < width*height*2,y < width*height*3;x+=4,y+=6)  
    {  
        yuv444[y] = yuv422[x];  
        yuv444[y+1] = yuv422[x+1];  
        yuv444[y+2] = yuv422[x+3];  
        yuv444[y+3] = yuv422[x+2];  
        yuv444[y+4] = yuv422[x+1];  
        yuv444[y+5] = yuv422[x+3];  
    }  
    for(x = 0;x < width*height*3;x+=3)  
    {  
        rgb24[x+2] = yuv444[x] + 1.402*(yuv444[x+2] - 128);  
        rgb24[x+1] = yuv444[x]-0.34414*(yuv444[x+1]-128)-0.71414*(yuv444[x+2]-128);  
        rgb24[x] = yuv444[x] + 1.772*(yuv444[x+1] - 128);  
        if (rgb24[x]>255)rgb24[x]=255;  
        if (rgb24[x]<0)rgb24[x]=0;  
        if (rgb24[x+1]>255)rgb24[x+1]=255;  
        if (rgb24[x+1]<0)rgb24[x+1]=0;  
        if (rgb24[x+2]>255)rgb24[x+2]=255;  
        if (rgb24[x+2]<0)rgb24[x+2]=0;  
    }  
    free(yuv444);  
}

//////////////////////////////////////////////////////
static void rgb_to_jpeg(unsigned char *rgb24,unsigned char *jpeg,
 int width, int height )
{
    struct jpeg_compress_struct cinfo;
    struct jpeg_error_mgr jerr;
    JSAMPROW row_pointer[1];  /* pointer to JSAMPLE row[s] */
    int row_stride;    /* physical row width in image buffer */
    JSAMPIMAGE  buffer;
    int band,i,buf_width[3],buf_height[3], mem_size, max_line, counter;
    unsigned char *yuv[3];
    uint8_t *pSrc, *pDst;

    yuv[0] = data;
    yuv[1] = yuv[0] + (image_width * image_height);
    yuv[2] = yuv[1] + (image_width * image_height) /2;

    cinfo.err = jpeg_std_error(&jerr);
    jpeg_create_compress(&cinfo);
    jpeg_stdio_dest(&cinfo, fp);

    cinfo.image_width = image_width;  /* image width and height, in pixels */
    cinfo.image_height = image_height;
    cinfo.input_components = 3;    /* # of color components per pixel */
    cinfo.in_color_space = JCS_RGB;  /* colorspace of input image */

    jpeg_set_defaults(&cinfo);
    jpeg_set_quality(&cinfo, quality, TRUE );

    cinfo.raw_data_in = TRUE;
    cinfo.jpeg_color_space = JCS_YCbCr;
    cinfo.comp_info[0].h_samp_factor = 2;
    cinfo.comp_info[0].v_samp_factor = 1;

    jpeg_start_compress(&cinfo, TRUE);

    buffer = (JSAMPIMAGE) (*cinfo.mem->alloc_small) ((j_common_ptr) &cinfo, JPOOL_IMAGE, 3 * sizeof(JSAMPARRAY));
    for(band=0; band <3; band++)
    {
        buf_width[band] = cinfo.comp_info[band].width_in_blocks * DCTSIZE;
        buf_height[band] = cinfo.comp_info[band].v_samp_factor * DCTSIZE;
        buffer[band] = (*cinfo.mem->alloc_sarray) ((j_common_ptr) &cinfo, JPOOL_IMAGE, buf_width[band], buf_height[band]);
    }
    max_line = cinfo.max_v_samp_factor*DCTSIZE;
    for(counter=0; cinfo.next_scanline < cinfo.image_height; counter++)
    {
        //buffer image copy.
        for(band=0; band <3; band++)
        {
            mem_size = buf_width[band];
            pDst = (uint8_t *) buffer[band][0];
            pSrc = (uint8_t *) yuv[band] + counter*buf_height[band] * buf_width[band];

            for(i=0; i <buf_height[band]; i++)
            {
                memcpy(pDst, pSrc, mem_size);
                pSrc += buf_width[band];
                pDst += buf_width[band];
            }
        }
        jpeg_write_raw_data(&cinfo, buffer, max_line);
    }

    jpeg_finish_compress(&cinfo);
    jpeg_destroy_compress(&cinfo);

    return 0;

}
