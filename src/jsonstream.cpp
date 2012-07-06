#include "jsonstream.h" 
#include <stdio.h>
#include <stdlib.h>


#define log(format, args...)  \
        fprintf (stderr, "[DEBUG]" format "\n" ,##args )

#define err(format, args...)  \
        fprintf (stderr, "[ERROR]" format "\n" ,##args ); \
        exit(1)



enum {
	NONE = 0
};

JsonStream::JsonStream(FILE* _stream) : state(NONE), indentation(0), stream(_stream)  {	
}

void JsonStream::beginDoc() {
	if(state != NONE) {
		err("New Doc while State is %i", state); 
	}	

}