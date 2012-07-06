#ifndef EXPORTSKELETONTOJSON_H 
#define EXPORTSKELETONTOJSON_H 

#include "types.h" 
#include <stdio.h>  

typedef struct {
	u16 relpos_offset;/* Relative offset to emd_skel_relpos[] array */
	u16 length; /* Relative offset to emd_skel_data[] array, which is also length of relpos+armature+mesh numbers  */
	u16 count; /* Number of objects in relpos,data2,mesh arrays? */
	u16 size; /* Size of each element in emd_skel_data[] array */
} emd_skel_header_t;

typedef struct {
	s16 x;
	s16 y;
	s16 z;
} emd_skel_relpos_t; /* Relative position of each mesh in the object */

typedef struct {
	u16 mesh_count; /* Number of meshes linked to this one */
	u16 offset; /* Relative offset to mesh numbers (emd_skel_mesh[] array) */
} emd_skel_armature_t;

typedef struct {
	s16 x_offset;	/* distance from reference point */
	s16 y_offset;
	s16 z_offset;

	s16 x_speed;	/* speed at which moving the model */
	s16 y_speed;
	s16 z_speed;

	s16 angles[3*15];	/* angle for each bone/model around x,y,z axis */

	s16 unknown;
} emd_skel_anim_t;

void exportSkeletonToJson(void* buffer, FILE* file);

#endif 

