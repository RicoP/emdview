#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>

#include <sys/types.h>
#include <netinet/in.h>
#include <inttypes.h>

#pragma pack(1)
typedef struct {
	unsigned int	magic;	/* Constant = 0x10 */
	unsigned int	type;
	unsigned int	length;
	unsigned short	unknown1;
	unsigned short	unknown2;
	unsigned short	palette_colors;
	unsigned short	nb_palettes;
} tim_header_t;

#pragma pack(1)
typedef struct {
	unsigned int    size; 
	unsigned short  x; 
	unsigned short  y; 
	unsigned short	width;	/* Width of image in 16-bits words */
	unsigned short	height; /* Height of image in pixels */
} tim_image_head_t;

typedef unsigned char u8; 

u8* loadFile(char *filename, int *length) {
	FILE *fp;
	int len;
	u8 *buf;
	fp = fopen(filename,"rb");
	fseek(fp,0,SEEK_END); //go to end
	len = ftell(fp); //get position at end (length)
	fseek(fp,0,SEEK_SET); //go to beg.
	buf=(u8 *) malloc(len); //malloc buffer
	fread(buf,2,len,fp); //read into buffer
	fclose(fp);
	fp = NULL; 

	*length = len; 
	return buf; 
}

void exportTimToPpm(void* buffer, int offset, char* name); 
int write_bmp(const char *filename, int width, int height, char *rgb);

int main(int argc, char **argv)
{
	int length;
	u8 *fileInMem;
	char* fileName; 

	if (argc<2) {
		return 1;
	}

	fileName = argv[1]; 

	fileInMem = loadFile(fileName, &length);

	if (fileInMem==NULL) {
		return 1;
	}

	int* directory = (int*)( fileInMem + length - 4*4 ); 

	int skeleton = directory[0]; 
	int animation = directory[1]; 
	int mesh = directory[2]; 
	int timStart = (directory[3]);

	fprintf(stderr, "offset skeleton: %x\n",   skeleton);  
	fprintf(stderr, "offset animations: %x\n", animation);
	fprintf(stderr, "offset mesh: %x\n",       mesh); 
	fprintf(stderr, "offset tim: %x\n",    timStart); 


	//TIM Texture
	exportTimToPpm(fileInMem, timStart, fileName); 
	free(fileInMem); 
	fileInMem = NULL; 

	return 0; 
}

#pragma pack(1)
typedef struct {
	unsigned short R : 5; 
	unsigned short G : 5; 
	unsigned short B : 5;
	unsigned char A : 1;
} high_color_t; 
//typedef unsigned short high_color_t; 

#define TIM_TYPE_4BIT (8)
#define TIM_TYPE_8BIT_PALETTE (9)
#define TIM_TYPE_TRUE_COLOR (2) 
#define BIT_5_TO_8(x) ((x) << 3 | (x) >> 2)

void exportTimToPpm(void* buffer, int start, char* filename) {
	int offset = start; 
	fprintf(stderr, "start : %x.\n", start); 

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

	for(int p = 0; p != header.nb_palettes; p++) { 
		char name[11]; //filename have allways the same length xxxxNN.emd or xxNNNN.emd = 10 + terminate
		sprintf(name, "%s", filename); 
		name[6] = 0; // terminate before "emd"
		char destination[256]; 
		sprintf(destination, "%s_%i.bmp", name, p); 

		//create Image
		high_color_t* colors = (high_color_t*) ((int)buffer + colorOffset + header.palette_colors*p); 
		char* rgb = (char*) malloc(image_info.size * 3);
		int rgbIndex = 0; 

		int x = 0; 
		for(int i = 0; i != image_info.size; i++) { 
			x = (i % (image_info.width * 2)); 
			//high_color_t color = colors[ pixel[i] + ((x > 127) ? -128 : 0) ]; 
			high_color_t color = colors[ pixel[i] ]; 
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

		write_bmp(destination, image_info.width * 2, image_info.height, rgb); 

		free(rgb); 
	}
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
	int biClrUsed;        /* Number of colors in the color table (if 0, use 
	maximum allowed by biBitCount) */
	int biClrImportant;   /* Number of important colors.  If 0, all colors 
	are important */
};

int write_bmp(const char *filename, int width, int height, char *rgb)
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
