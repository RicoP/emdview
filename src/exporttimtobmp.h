#ifndef EXPORTTIMTOBMP_H
#define EXPORTTIMTOBMP_H

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

#pragma pack(1)
typedef struct {
	unsigned short R : 5; 
	unsigned short G : 5; 
	unsigned short B : 5;
	unsigned char A : 1;
} high_color_t; 
//typedef unsigned short high_color_t; 

void exportTimToBmp(void* buffer, char* output); 
#endif 
