#include "getskeletondata.h"

emd_skel_relpos_t* getSkeletonData(void* buffer) { 
	int blob = (int)buffer; 

	emd_skel_header_t* header = (emd_skel_header_t*)blob; 
	int offsetToSkel_Relpos = blob + header->relpos_offset; 

	return (emd_skel_relpos_t*)offsetToSkel_Relpos; 
	/*int offsetToSkel_Data   = blob + header->length; 
	blob += sizeof(emd_skel_header_t);

	fprintf(stderr, "header: off: 0x%x, len: %i, count: %i, size: %i\n", header->relpos_offset, header->length, header->count, header->size); 

	assert(header->size == sizeof(emd_skel_anim_t)); 

	if(header->relpos_offset > sizeof(emd_skel_header_t)) {
		fprintf(stderr, "Offset is %i. Don't know what to do with junk after header.\n", header->relpos_offset); 
	}*/
}
