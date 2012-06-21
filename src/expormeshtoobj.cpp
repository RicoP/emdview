#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>

#include "exporttimtoobj.h"

void exportTimToObj(void* buffer, char* output) {
	int blob = (int)buffer;  
	emd_model_header_t* header = (emd_model_header_t*) blob; 
	blob += sizeof(emd_model_header_t); 
	
	fprintf(stderr, "length %x, unknown %x, count %x\n", header->lengh, header->unknown, header->obj_count); 
}
