#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <assert.h> 

#include <sys/types.h>
#include <netinet/in.h>
#include <inttypes.h>
#include <unistd.h>    /* for getopt */

#include "types.h"
#include "exporttimtobmp.h" 
#include "exportmeshtoobj.h"  
#include "exportskeletontojson.h" 

u8* loadFile(char *filename, int *length) {
	FILE *fp;
	int len;
	u8 *buf;
	fp = fopen(filename,"rb");
	if(!fp) exit(1); 
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

enum {
	NONE = 0, 
	FROM_EMD  = 1<<0,
	FROM_TIM  = 1<<1,
	FROM_SKEL = 1<<2, 
	TO_TIM    = 1<<3,
	TO_MESH   = 1<<4, 
	TO_OBJ    = 1<<5,
	TO_JSON   = 1<<6,
	TO_BMP    = 1<<7 
};

void replaceExtension(char* input, char* extension, char* destination) {
	//remove everything before the main filename /x/foo.abr -> foo.bar
	for(char* p = input, n = 0; *p; p++, n++) {
		if(*p == '/')
			input = p+1;  
	}

	int lastPoint = -1; 
	for(char* p = input, n = 0; *p; p++, n++) {
		if(*p == '.') 
			lastPoint = n; 
	}

	if(lastPoint == -1) {
		fprintf(stderr, "File %s has no extension.\n", input); 
		exit(1); 
	}

	char buff[256]; 
	sprintf(buff, "%s", input); 
	buff[lastPoint] = 0; //terminate at point

	sprintf(destination, "%s.%s", buff, extension); 
	fprintf(stderr, "%s\n", destination); 
}

int main(int argc, char **argv)
{
	assert(sizeof(tmd_header_t) == 12); 
	assert(sizeof(tmd_object_t) == 28); 
	assert(sizeof(tmd_prim_header_t) == 4); 
	assert(sizeof(tmd_vertex_t) == 8); 
	assert(sizeof(tmd_txtriangle_t) == 24); 

	int length = 0;
	u8* blob = 0;

	char* inputfile = 0; 
	char usePipe = 0; 

	int options = 0; 

	{
		int c; 
		while ( (c = getopt(argc, argv, "i:exbtemopjs")) != -1) {
			switch(c) {
				case 'i':
				fprintf(stderr, "inputfile %s\n", optarg); 
				inputfile = optarg;
				break;

				case 'e': 
				fprintf(stderr, "From EMD\n"); 
				options |= FROM_EMD; 
				break; 

				case 'x': 
				fprintf(stderr, "From TIM\n"); 
				options |= FROM_TIM; 
				break; 

				case 'b': 
				fprintf(stderr, "BMP export\n"); 
				options |= TO_BMP; 
				break; 

				case 't':
				fprintf(stderr, "TIM export\n"); 
				options |= TO_TIM; 
				break; 

				case 'm': 
				fprintf(stderr, "MESH Export\n"); 
				options |= TO_MESH; 
				break; 

				case 'o': 
				fprintf(stderr, "OBJ Export\n"); 
				options |= TO_OBJ; 
				break; 

				case 'p': 
				fprintf(stderr, "use PIPE\n"); 
				usePipe = 1; 
				break; 

				case 's': 
				fprintf(stderr, "From SKEL\n"); 
				options |= FROM_SKEL; 
				break; 

				case 'j': 
				fprintf(stderr, "JSON Export\n"); 
				options |= TO_JSON; 
				break; 
			}
		}
	}

	if(!inputfile) {		
		fprintf(stderr, "No inputfile.\n"); 
		exit(1); 
	}

	if( (options & FROM_EMD) && (options & FROM_TIM) ) {
		fprintf(stderr, "Inputfile can't be tim and emd.\n"); 
		exit(1); 
	}

	blob = loadFile(inputfile, &length);

	if (!blob || !length) {
		return 1;
	}

	char output[256]; 

	if(options & FROM_EMD) { 		
		s32* directory = (int*)( blob + length - 4 * sizeof(s32) ); 

		s32 skelStart = directory[0]; 
		//s32 animation = directory[1]; 
		s32 meshStart = directory[2]; 
		s32 timStart = (directory[3]);

		void* timBlob  = blob + timStart; 
		void* meshBlob = blob + meshStart; 
		void* skelBlob = blob + skelStart; 

		if(options & TO_MESH) {
			FILE* pFile;
			replaceExtension(inputfile, "mesh", output); 
			pFile = fopen ( output , "wb" );
			fwrite (meshBlob, 1 , length - 16 - timStart , pFile );
			fclose (pFile);		
		}

		if(options & TO_TIM) {		
			FILE* pFile;
			replaceExtension(inputfile, "tim", output); 
			pFile = fopen ( output , "wb" );
			fwrite (timBlob, 1 , length - 16 - timStart , pFile );
			fclose (pFile);
		}

		if(options & TO_OBJ) {
			if(!usePipe) {
				replaceExtension(inputfile, "obj", output); 
				FILE* file = fopen( output, "wt" ); 
				exportMeshToObj(meshBlob, file, skelBlob);
				fclose(file); 
			}
			else {	
				exportMeshToObj(meshBlob, stdout, skelBlob);
			}
		}

		//EMD -> BMP 
		if(options & TO_BMP) { 
			replaceExtension(inputfile, "bmp", output); 
			exportTimToBmp(timBlob, output);
		}

		//Skel -> JSON
		if(options & TO_JSON) {	
			if(!usePipe) {
				replaceExtension(inputfile, "json", output); 
				FILE* file = fopen( output, "wt" ); 
				exportSkeletonToJson(skelBlob, file);
				fclose(file); 
			}
			else {	
				exportSkeletonToJson(skelBlob, stdout);
			}
		}
	}
	else if(options & FROM_TIM) {
		//TIM -> BMP 
		if(options & TO_BMP) { 
			replaceExtension(inputfile, "bmp", output); 
			exportTimToBmp(blob, output);
		}
		else {
			fprintf(stderr, "Don't know what to do with TIM file.\n"); 
			exit(1); 
		}
	}

	return 0; 
}

