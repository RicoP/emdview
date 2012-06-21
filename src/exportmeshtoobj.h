#ifndef EXPORTMESHTOOBJ_H
#define EXPORTMESHTOOBJ_H

#include "types.h" 


#pragma pack(1)
typedef struct {
	u32 id; /* Constant = 0x41 */
	u32 unknown;
	u32 num_objects; /* Number of objects in file */
} tmd_header_t;

#pragma pack(1)
typedef struct {
	u32 vertex_offset; /* Offset to vertex array coordinates */
	u32 vertex_count; /* Number of vertices */
	u32 normal_offset; /* Offset to normal array coordinates */
	u32 normal_count; /* Number of normals */
	u32 primitive_offset; /* Offsets to primitives */
	u32 primitive_count; /* Number of primitives */
	u32 unknown;
} tmd_object_t;

#pragma pack(1)
typedef struct {
	s16 x;
	s16 y;
	s16 z;
	s16 zero;
} tmd_vertex_t;

#pragma pack(1)
typedef struct {
	u8 unknown0;
	u8 length; /* Length of following primitive in 32bit words */
	u8 unknown1;
	u8 type;	/* primitive type */
} tmd_prim_header_t;

#pragma pack(1)
typedef struct {
	u8 r,g,b,unknown; /* R,G,B triangle color */

	u16 n0; /* Normal index */

	u16 v0; /* Vertex index */
	u16 v1;
	u16 v2;
} tmd_flattriangle_t;

#pragma pack(1)
typedef struct {
	u8 r0,g0,b0,unk0; /* R,G,B vertex color */
	u8 r1,g1,b1,unk1;
	u8 r2,g2,b2,unk2;

	u16 n0; /* Normal index */

	u16 v0; /* Vertex index */
	u16 v1;
	u16 v2;
} tmd_gouraudtriangle_t;

#pragma pack(1)
typedef struct {
	u8 r,g,b,unknown; /* R,G,B quad color */

	u16 n0; /* Normal index */

	u16 v0; /* Vertex index */
	u16 v1;
	u16 v2;
	u16 v3;
} tmd_flatquad_t;

#pragma pack(1)
typedef struct {
	u8 r0,g0,b0,unk0; /* R,G,B vertex color */
	u8 r1,g1,b1,unk1;
	u8 r2,g2,b2,unk2;
	u8 r3,g3,b3,unk3;

	u16 n0; /* Normal index */

	u16 v0; /* Vertex index */
	u16 v1;
	u16 v2;
	u16 v3;

	u16 unk5;
} tmd_gouraudquad_t;

#pragma pack(1)
typedef struct {
	u8 r,g,b,unk; /* R,G,B triangle color */

	u16 n0; /* Normal index */
	u16 v0; /* Vertex index */
	u16 n1;
	u16 v1;
	u16 n2;
	u16 v2;
} tmd_flattriangle2_t;

#pragma pack(1)
typedef struct {
	u8 v; /* textures coordinates */
	u8 u;
} tmd_textureuv_t; 

#pragma pack(1)
typedef struct {
	u16 unknown1;
	tmd_textureuv_t tex1; 
	u16 unknown2;
	tmd_textureuv_t tex2; 
	u16 unknown3;
	tmd_textureuv_t tex3; 

	u16 v0; /* Vertex index */
	u16 n0; /* Normal index */
	u16 v1;
	u16 n1;
	u16 v2;
	u16 n2;
} tmd_txtriangle_t;


#pragma pack(1)
typedef struct {
	u8 r,g,b,unk; /* R,G,B quad color */

	u16 n0; /* Normal index */
	u16 v0; /* Vertex index */
	u16 n1;
	u16 v1;
	u16 n2;
	u16 v2;
	u16 n3;
	u16 v3;
} tmd_flatquad2_t;

void exportMeshToObj(void* buffer, char* output);

#endif 
