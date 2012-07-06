#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <assert.h>

#include "exportmeshtoobj.h"
#include "getskeletondata.h"

#define FLOAT "%1.20f "
#define FV  "v "  FLOAT FLOAT FLOAT "\n"
#define FVN "vn " FLOAT FLOAT FLOAT "\n"
#define FVT "vt " FLOAT FLOAT "\n"

#define snormalize(x) ((x)/((float)0x500))
#define cnormalize(x) ((x)/((float)0xFF))

void exportMeshToObj(void* buffer, FILE* file, void* skeletonBuffer) {
	int blob = (int)buffer;  
	tmd_header_t* header = (tmd_header_t*) blob; 
	blob += sizeof(tmd_header_t);  

	int vi  = 1; 
	int vti = 1; 
	int vni = 1; 

	fprintf(stderr, "id %x, unknown %x, count %x\n", header->id, header->unknown, header->num_objects); 
	assert(header->id == 0x41); 

	tmd_object_t* objects = (tmd_object_t*) blob; 
	int firstObject = blob; 	

	emd_skel_relpos_t* skeleton = getSkeletonData(skeletonBuffer); 

	for(int iobject = 0; iobject != header->num_objects; iobject++) {		
		//DEBUG
		//float xoff = 1.4f * iobject; 
		emd_skel_relpos_t bone = skeleton[iobject]; 
		//DEBUG 
		static int nr = 0; 
		fprintf(stderr, "Bone %i: (%i,%i,%i)\n", nr++, bone.x, bone.y, bone.z);


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
				fprintf(file, FV, snormalize(bone.x + v.x), snormalize(bone.y + v.y), snormalize(bone.z + v.z));
			}

			int normalindice[] = { tri.n0, tri.n1, tri.n2 }; 
			for(int i = 0; i != 3; i++) {
				tmd_vertex_t n = normals[ normalindice[i] ];
				assert(n.zero == 0); 
				fprintf(file, FVN, snormalize(n.x), snormalize(n.y), snormalize(n.z)); 
			}

			float Rp = (tri.TSB & 0x1F) * 0.5f; 
			fprintf(stderr, "TSB: %i\n", tri.TSB); 
			tmd_textureuv_t textures[] = { tri.tex1, tri.tex2, tri.tex3 }; 
			for(int i = 0; i != 3; i++) { 
				tmd_textureuv_t tex = textures[i]; 
				fprintf(file, FVT, cnormalize(tex.u) + Rp, 1 - cnormalize(tex.v)); 
			}

			fprintf(file, "f"); 
			for(int i = 0; i != 3; i++) {
				fprintf(file, " %i/%i/%i", vi++, vti++, vni++); 
			}
			fprintf(file, "\n");  
		}
	}
}
