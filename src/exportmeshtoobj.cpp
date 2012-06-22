#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>

#include "exportmeshtoobj.h"

#define FLOAT "%1.20f "
#define FV  "v "  FLOAT FLOAT FLOAT "\n"
#define FVN "vn " FLOAT FLOAT FLOAT "\n"
#define FVT "vt " FLOAT FLOAT "\n"
#define snormalize(x) ((x)/((float)0x7FFF))

void exportMeshToObj(void* buffer, char* output) {
	int blob = (int)buffer;  
	int start = blob; 
	tmd_header_t* header = (tmd_header_t*) blob; 
	blob += sizeof(tmd_header_t);  

	int vi  = 1; 
	int vti = 1; 
	int vni = 1; 

	int offsetRel = blob; //Pointer to first Element is point of reference for vertexoffsets 
	
	fprintf(stderr, "id %x, unknown %x, count %x\n", header->id, header->unknown, header->num_objects); 

	if(header->id != 0x41) {
		fprintf(stderr, "Header must start with 0x41.\n"); 
	}

	tmd_object_t* objects = (tmd_object_t*) blob; 
	blob += sizeof(tmd_object_t) * header->num_objects;


	FILE* file = fopen ( output , "wt" );

	for(int i = 0; i != header->num_objects; i++) {
		tmd_object_t o = objects[i]; 

		int primoffset = offsetRel + o.primitive_offset;
		int vertoffset = offsetRel + o.vertex_offset;
		int normoffset = offsetRel + o.normal_offset;

		tmd_prim_header_t* primhead = (tmd_prim_header_t*)(primoffset); 
		tmd_vertex_t*      vertice  = (tmd_vertex_t*)(vertoffset);
		tmd_vertex_t*      normals  = (tmd_vertex_t*)(normoffset);		

		
		if(primhead->type != 0x34) {
			fprintf(stderr, "ERROR. Type must be 0x34 not %x.", primhead->type); 
			exit(1);
		}

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
		
		fprintf(stderr, "  length %i, type: %x, u1:%x, u2:%x\n", primhead->length, primhead->type, primhead->unknown0, primhead->unknown1);
			
		fprintf(file, "o object_%i\n", i); 

		int t = 0; 
		for(tmd_txtriangle_t* tri = (tmd_txtriangle_t*)(primoffset + sizeof(tmd_prim_header_t));
			t != primhead->length; 
			t++, tri++) 
		{
			int vertexindice[] = { tri->v0, tri->v1, tri->v2 }; 
			for(int i = 0; i != 3; i++) {
				tmd_vertex_t v = vertice[ vertexindice[i] ];
				fprintf(file, FV, snormalize(v.x), snormalize(v.y), snormalize(v.z)); 
			}

			int normalindice[] = { tri->n0, tri->n1, tri->n2 }; 
			for(int i = 0; i != 3; i++) {
				tmd_vertex_t n = normals[ normalindice[i] ];
				fprintf(file, FVN, snormalize(n.x), snormalize(n.y), snormalize(n.z)); 
			}

			fprintf(file, FVT, tri->tex1.u / 255.0f, tri->tex1.v / 255.0f); 
			fprintf(file, FVT, tri->tex2.u / 255.0f, tri->tex2.v / 255.0f); 
			fprintf(file, FVT, tri->tex3.u / 255.0f, tri->tex3.v / 255.0f); 


			fprintf(file, "f"); 
			fprintf(file, " %i/%i/%i", vi++, vti++, vni++); 
			fprintf(file, " %i/%i/%i", vi++, vti++, vni++); 
			fprintf(file, " %i/%i/%i", vi++, vti++, vni++); 
			fprintf(file, "\n");  

			break;
		}

	}
	
	fclose (file); 
}
