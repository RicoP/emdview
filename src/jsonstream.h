#ifndef JSONSTREAM_H 
#define JSONSTREAM_H 

#include <stdio.h> 
#include <stdlib.h> 

class JsonStream {
private:	
	int state; 
	int indentation;  
	FILE* stream; 

public:
	JsonStream(FILE* stream); 

	void beginDoc(); 
	void beginString(); 

	void endDoc(); 
	void endString(); 

	void putText(char* text);
	void putInteger(int nr); 
	void putFloat(float f); 
	void putNew(); 
};

#endif 

