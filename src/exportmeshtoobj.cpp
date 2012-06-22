#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <assert.h>

#include "exportmeshtoobj.h"

#define FLOAT "%1.20f "
#define FV  "v "  FLOAT FLOAT FLOAT "\n"
#define FVN "vn " FLOAT FLOAT FLOAT "\n"
#define FVT "vt " FLOAT FLOAT "\n"

#define snormalize(x) ((x)/((float)0x7FFF))
#define cnormalize(x) ((x)/((float)0xFF))

void exportMeshToObj(void* buffer, char* output) {
	int blob = (int)buffer;  
	tmd_header_t* header = (tmd_header_t*) blob; 
	blob += sizeof(tmd_header_t);  

	int vi  = 1; 
	int vti = 1; 
	int vni = 1; 

	FILE* file = fopen ( output , "wt" );

	fprintf(stderr, "id %x, unknown %x, count %x\n", header->id, header->unknown, header->num_objects); 
	assert(header->id == 0x41); 

	tmd_object_t* objects = (tmd_object_t*) blob; 
	int firstObject = blob; 

	for(int iobject = 0; iobject != header->num_objects; iobject++) {		
		fprintf(stderr, "  %i\n", iobject); 
		tmd_object_t obj = objects[iobject]; 

		int primoffset = firstObject + obj.primitive_offset; 
		int vertoffset = firstObject + obj.vertex_offset;
		int normoffset = firstObject + obj.normal_offset;
		
		tmd_vertex_t*      vertice    = (tmd_vertex_t*)(vertoffset);
		tmd_vertex_t*      normals    = (tmd_vertex_t*)(normoffset);		
		tmd_txprimitive_t* primitives = (tmd_txprimitive_t*)primoffset; 

		fprintf(file, "o object_%i\n", iobject); 

		for(int ipri = 0; ipri != obj.primitive_count; ipri++) { 
			tmd_txprimitive_t primitive = primitives[ipri]; 

			tmd_prim_header_t head = primitive.header; 
			tmd_txtriangle_t  tri  = primitive.triangle; 
			
			assert(head.type == 0x34);

			int vertexindice[] = { tri.v0, tri.v1, tri.v2 }; 
			for(int i = 0; i != 3; i++) {
				tmd_vertex_t v = vertice[ vertexindice[i] ];
				assert(v.zero == 0); 
				fprintf(file, FV, snormalize(v.x), snormalize(v.y), snormalize(v.z)); 
			}

			int normalindice[] = { tri.n0, tri.n1, tri.n2 }; 
			for(int i = 0; i != 3; i++) {
				tmd_vertex_t n = normals[ normalindice[i] ];
				assert(n.zero == 0); 
				fprintf(file, FVN, snormalize(n.x), snormalize(n.y), snormalize(n.z)); 
			}

			fprintf(file, FVT, cnormalize(tri.tex1.u), cnormalize(tri.tex1.v)); 
			fprintf(file, FVT, cnormalize(tri.tex2.u), cnormalize(tri.tex2.v)); 
			fprintf(file, FVT, cnormalize(tri.tex3.u), cnormalize(tri.tex3.v)); 

			fprintf(file, "f"); 
			for(int i = 0; i != 3; i++) {
				fprintf(file, " %i/%i/%i", vi++, vti++, vni++); 
			}
			fprintf(file, "\n");  
		}
	}
	
	fclose (file); 
}
