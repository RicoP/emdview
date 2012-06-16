#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>

#include <sys/types.h>
#include <netinet/in.h>
#include <inttypes.h>

typedef struct {
	unsigned int	magic;	/* Constant = 0x10 */
	unsigned int	type;
	unsigned int	offset;
	unsigned short	unknown1;
	unsigned short	unknown2;
	unsigned short	palette_colors;
	unsigned short	nb_palettes;
} tim_header_t;

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

	*length = len; 
	return buf; 
}

void exportTimToPpm(void* buffer, int offset); 

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
	exportTimToPpm(fileInMem, timStart); 

	return 0; 
}

typedef struct {
	char  A : 1;
	short R : 5; 
	short G : 5; 
	short B : 5;
} true_color_t; 

typedef true_color_t* palette; 

#define TIM_TYPE_4BIT (8)
#define TIM_TYPE_8BIT_PALETTE (9)
#define TIM_TYPE_TRUE_COLOR (2) 

void exportTimToPpm(void* buffer, int start) {
	int offset = start; 
	tim_header_t header = *((tim_header_t*)((int)buffer + offset)); 
	offset += sizeof(tim_header_t); 

	fprintf(stderr, "magic %x, type %x, offset %x, palette_colors %i, nb_palettes %i\n", header.magic, header.type, header.offset, header.palette_colors, header.nb_palettes); 

	if(header.magic != 0x10) {
		fprintf(stderr, "magic number fail.\n"); 
		exit(1); 
	}

	if(header.type != TIM_TYPE_8BIT_PALETTE) {
		fprintf(stderr, "Type not supported.\n"); 
		exit(1); 
	}

	true_color_t* palette = (true_color_t*) ((int)buffer + offset); 
	offset += header.palette_colors * header.nb_palettes * sizeof(true_color_t); 
	fprintf(stderr, "size offset %x\n", offset); 

	//image header
	short* size = (short*) ( (int)buffer + offset); 
	offset += 4; 

	fprintf(stderr, "size = (%i, %i).\n", size[0], size[1]); 
} 
