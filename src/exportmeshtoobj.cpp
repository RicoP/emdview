#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>

#include "exportmeshtoobj.h"

void exportMeshToObj(void* buffer, char* output) {
	int blob = (int)buffer;  
	int start = blob; 
	tmd_header_t* header = (tmd_header_t*) blob; 
	blob += sizeof(tmd_header_t);  

	int vi = 0; 
	int vti = 0; 
	int vni = 0; 

	int startFirstObject = blob; 
	
	fprintf(stderr, "id %x, unknown %x, count %x\n", header->id, header->unknown, header->num_objects); 

	tmd_object_t* objects = (tmd_object_t*) blob; 
	blob += sizeof(tmd_object_t) * header->num_objects;

	int offsetRel = (int)(objects);
	FILE* stream = stdout; 

	for(int i = 0; i != header->num_objects; i++) {
		//FILE* vstream = fopen ( "__v" , "wt" );
		//FILE* tstream = fopen ( "__t" , "wt" );
		//FILE* nstream = fopen ( "__n" , "wt" );
		//FILE* fstream = fopen ( "__f" , "wt" );
		FILE* tstream = stdout; 
		FILE* fstream = stdout; 


		tmd_object_t o = objects[i]; 

		int offset = offsetRel + o.primitive_offset;

		fprintf(stream, "o object_%i\n", i); 

		fprintf(stderr, "%i: po: %i\tpc: %i\tvo: %i\tvc: %i\tno: %i\tnc: %i\tu: %i\n", 
			i, 
			o.primitive_offset, 
			o.primitive_count, 
			o.vertex_offset, 
			o.vertex_count, 
			o.normal_offset, 
			o.normal_count, 
			o.unknown
		);
		
		tmd_prim_header_t* primhead = (tmd_prim_header_t*)(offset); 
		offset += sizeof(tmd_prim_header_t);

		fprintf(stderr, "  length %i, type: %x, u1:%x, u2:%x\n", primhead->length, primhead->type, primhead->unknown0, primhead->unknown1);

		if(primhead->type != 0x34) {
			fprintf(stderr, "ERROR. Type must be 0x34 not %x.", primhead->type); 
			exit(1);
		}

		tmd_txtriangle_t* tri = (tmd_txtriangle_t*)(offset); 
		fprintf(tstream, "vt %f %f\n", tri->tex1.u / 255.0f, tri->tex1.v / 255.0f); 
		fprintf(tstream, "vt %f %f\n", tri->tex2.u / 255.0f, tri->tex2.v / 255.0f); 
		fprintf(tstream, "vt %f %f\n", tri->tex3.u / 255.0f, tri->tex3.v / 255.0f); 

		fprintf(fstream, "f "); 
		fprintf(fstream, "%i/%i/%i ", tri->v0, vti+1, tri->n0); 
		vti++; 
		fprintf(fstream, "%i/%i/%i ", tri->v1, vti+1, tri->n1); 
		vti++;
		fprintf(fstream, "%i/%i/%i\n", tri->v2, vti+1, tri->n2); 
		vti++; 

		//fclose (vstream); 
		//fclose (tstream); 
		//fclose (nstream); 
		//fclose (fstream); 
		//fclose (stream); 
	}

}
