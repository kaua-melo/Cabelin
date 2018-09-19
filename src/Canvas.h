

/*
*   
*/

#ifndef _CANVAS // if this class hasn't been defined, the program can define it
#define _CANVAS // by using this if statement you prevent the class to be called more than once which would confuse the compiler

#include "ofMain.h"   // we need to include this to have a reference to the openFrameworks framework

					  
class Canvas 
{
        // Creating everything on public just to be faster to change some properties directly,
        //  instead of calling a function that calls another function.
	public:
        // CONSTRUCTOR
		Canvas(int w,   // width
               int h);  // height

        void draw();
        void allocate(int w, int h);

        ofVec2f getPosition();
        void setPosition(ofVec2f v);

        void setNumberSamples(int numSamples);

    
        ofFbo fbo;
        int width, height;
        ofVec2f position;
	private:
};
#endif
