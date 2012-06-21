#ifndef EXPORTTIMTOOBJ_H
#define EXPORTTIMTOOBJ_H

#include "types.h" 

typedef struct {
	u32 length; /* Section length in bytes */
	u32 unknown;
	u32 obj_count; /* Number of objects in model */
} emd_model_header_t;

typedef struct {
	u32 vertex_offset; /* Offset to vertex data, array of emd_vertex_t */
	u32 vertex_count;  /* Vertex count */
	u32 normal_offset; /* Offset to normal data, array of emd_vertex_t */
	u32 normal_count;  /* Normal count */
	u32 tri_offset; /* Offset to triangle data, array of emd_triangle_t */
	u32 tri_count;  /* Triangle count */
	u32 dummy;
} emd_model_triangles_t;

typedef struct {
	emd_model_triangles_t triangles;
} emd_model_object_t;

typedef struct {
	s16 x; /* Coordinates */
	s16 y;
	s16 z;
	s16 w;    
} emd_vertex_t; /* Normals have same format */


typedef struct {
	u32 unknown; /* id 0x34000609 */

	u8 tu0; /* u,v texture coordinates of vertex 0 */
	u8 tv0;
	u16 clutid; /* Texture clut id, bits 0-5 */
	u8 tu1;
	u8 tv1;
	u16 page;  /* Texture page */
	u8 tu2;
	u8 tv2;
	u16 dummy;

	u16 n0; /* Index of normal data for vertex 0 */
	u16 v0; /* Index of vertex data for vertex 0 */
	u16 n1;
	u16 v1;    
	u16 n2;
	u16 v2;    
} emd_triangle_t; /* Triangle */

void exportTimToObj(void* buffer, char* output);

#endif 
