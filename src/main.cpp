#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>

#include <sys/types.h>
#include <netinet/in.h>
#include <inttypes.h>

#include "types.h"
#include "exporttimtobmp.h" 

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


int main(int argc, char **argv)
{
	int length;
	u8 *fileInMem;
	char* fileName; 
	bool exportTim = false; 

	if (argc<2) {
		return 1;
	}

	fileName = argv[1]; 

	fileInMem = loadFile(fileName, &length);

	if (fileInMem==NULL) {
		return 1;
	}

	if(argc > 2) {
		if(argv[2][0] == 'e') 
			exportTim = true; 
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
	exportTimToBmp(fileInMem, timStart, fileName); 

	if(exportTim) {
		FILE * pFile;
		pFile = fopen ( "out.tim" , "wb" );
		fwrite (fileInMem + timStart , 1 , length - 16 - timStart , pFile );
		fclose (pFile);
	}

	free(fileInMem); 
	fileInMem = NULL; 

	return 0; 
}

