#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>

#include <sys/types.h>
#include <netinet/in.h>
#include <inttypes.h>

#include "exporttimtobmp.h"
#include "types.h" 

#define TIM_TYPE_4BIT (8)
#define TIM_TYPE_8BIT_PALETTE (9)
#define TIM_TYPE_TRUE_COLOR (2) 
#define BIT_5_TO_8(x) ((x) << 3 | (x) >> 2)

static int write_bmp(const char *filename, int width, int height, char *rgb);

void exportTimToBmp(void* buffer, char* output) {
	int offset = 0; 
	tim_header_t header = *((tim_header_t*)((int)buffer + offset)); 
	offset += sizeof(tim_header_t); 

	fprintf(stderr, "magic %x, type %x, length %i, u1 %x, u2 %x, palette_colors %x, nb_palettes %x\n", header.magic, header.type, header.length, header.unknown1, header.unknown2, header.palette_colors, header.nb_palettes); 

	fprintf(stderr, "size head %i\n", sizeof(tim_header_t)); 

	if(header.magic != 0x10) {
		fprintf(stderr, "magic number fail.\n"); 
		exit(1); 
	}

	if(header.type != TIM_TYPE_8BIT_PALETTE) {
		fprintf(stderr, "Type not supported.\n"); 
		exit(1); 
	}

	int colorOffset = offset; 
	high_color_t* colors = (high_color_t*) ((int)buffer + offset); 
	offset += header.palette_colors * header.nb_palettes * sizeof(high_color_t); 

	fprintf(stderr, "size color %i\n", sizeof(high_color_t)); 

	fprintf(stderr, "size offset %x\n", offset); 

	//image header
	tim_image_head_t image_info = *(tim_image_head_t*) ( (int)buffer + offset); 
	offset += sizeof(tim_image_head_t); 

	fprintf(stderr, "size = %i, x = %i, y = %i, dim = (%i, %i).\n", image_info.size, image_info.x, image_info.y, image_info.width * 2, image_info.height); 


	u8* pixel = (u8*)((int)buffer + offset); 

	high_color_t* paletteA = (high_color_t*) ((int)buffer + colorOffset); 
	high_color_t* paletteB = 0;	
	if(header.nb_palettes == 2) 
		paletteB = (high_color_t*) ((int)buffer + colorOffset + sizeof(high_color_t) * header.palette_colors); 	

	//create Image
	char* rgb = (char*) malloc(image_info.size * 3);
	int rgbIndex = 0; 
	int x = 0; 
	for(int i = 0; i != image_info.size; i++) { 
		high_color_t color; 

		x = (i % (image_info.width * 2)); 
		if(x < image_info.width || header.nb_palettes == 1 || !paletteB)
			color = paletteA[ pixel[i] ];
		else 
			color = paletteB[ pixel[i] ];

		char c; 
		c = (char)(color.R);
		rgb[rgbIndex++] = BIT_5_TO_8(c); 
		c = (char)(color.G);
		rgb[rgbIndex++] = BIT_5_TO_8(c); 
		c = (char)(color.B);
		rgb[rgbIndex++] = BIT_5_TO_8(c); 

		//rgb[rgbIndex++] = BIT_5_TO_8( (color ) & 0x1F ); 
		//rgb[rgbIndex++] = BIT_5_TO_8( (color>>5 ) & 0x1F ); 
		//rgb[rgbIndex++] = BIT_5_TO_8( (color>>10) & 0x1F ); 
	}

	write_bmp(output, image_info.width * 2, image_info.height, rgb); 

	free(rgb); 
} 


struct BMPHeader
{
	char bfType[2];       /* "BM" */
	int bfSize;           /* Size of file in bytes */
	int bfReserved;       /* set to 0 */
	int bfOffBits;        /* Byte offset to actual bitmap data (= 54) */
	int biSize;           /* Size of BITMAPINFOHEADER, in bytes (= 40) */
	int biWidth;          /* Width of image, in pixels */
	int biHeight;         /* Height of images, in pixels */
	short biPlanes;       /* Number of planes in target device (set to 1) */
	short biBitCount;     /* Bits per pixel (24 in this case) */
	int biCompression;    /* Type of compression (0 if no compression) */
	int biSizeImage;      /* Image size, in bytes (0 if no compression) */
	int biXPelsPerMeter;  /* Resolution in pixels/meter of display device */
	int biYPelsPerMeter;  /* Resolution in pixels/meter of display device */
	int biClrUsed;        /* Number of colors in the color table (if 0, use maximum allowed by biBitCount) */
	int biClrImportant;   /* Number of important colors.  If 0, all colors are important */
};

static int write_bmp(const char *filename, int width, int height, char *rgb)
{
    int i, j, ipos;
    int bytesPerLine;
    unsigned char *line;

    FILE *file;
    struct BMPHeader bmph;

    /* The length of each line must be a multiple of 4 bytes */

    bytesPerLine = (3 * (width + 1) / 4) * 4;

    strcpy(bmph.bfType, "BM");
    bmph.bfOffBits = 54;
    bmph.bfSize = bmph.bfOffBits + bytesPerLine * height;
    bmph.bfReserved = 0;
    bmph.biSize = 40;
    bmph.biWidth = width;
    bmph.biHeight = height;
    bmph.biPlanes = 1;
    bmph.biBitCount = 24;
    bmph.biCompression = 0;
    bmph.biSizeImage = bytesPerLine * height;
    bmph.biXPelsPerMeter = 0;
    bmph.biYPelsPerMeter = 0;
    bmph.biClrUsed = 0;       
    bmph.biClrImportant = 0; 

    file = fopen (filename, "wb");
    if (file == NULL) return(0);
  
    fwrite(&bmph.bfType, 2, 1, file);
    fwrite(&bmph.bfSize, 4, 1, file);
    fwrite(&bmph.bfReserved, 4, 1, file);
    fwrite(&bmph.bfOffBits, 4, 1, file);
    fwrite(&bmph.biSize, 4, 1, file);
    fwrite(&bmph.biWidth, 4, 1, file);
    fwrite(&bmph.biHeight, 4, 1, file);
    fwrite(&bmph.biPlanes, 2, 1, file);
    fwrite(&bmph.biBitCount, 2, 1, file);
    fwrite(&bmph.biCompression, 4, 1, file);
    fwrite(&bmph.biSizeImage, 4, 1, file);
    fwrite(&bmph.biXPelsPerMeter, 4, 1, file);
    fwrite(&bmph.biYPelsPerMeter, 4, 1, file);
    fwrite(&bmph.biClrUsed, 4, 1, file);
    fwrite(&bmph.biClrImportant, 4, 1, file);
  
    line = (unsigned char*) malloc(bytesPerLine);
    if (line == NULL)
    {
        fprintf(stderr, "Can't allocate memory for BMP file.\n");
        return(0);
    }

    for (i = height - 1; i >= 0; i--)
    {
        for (j = 0; j < width; j++)
        {
            ipos = 3 * (width * i + j);
            line[3*j] = rgb[ipos + 2];
            line[3*j+1] = rgb[ipos + 1];
            line[3*j+2] = rgb[ipos];
        }
        fwrite(line, bytesPerLine, 1, file);
    }

    free(line);
    fclose(file);

    return(1);
}
