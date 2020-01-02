
#ifndef _BLOB // if this class has't been defined, then we should define it.
#define _BLOB // Here we define it. This is useful so that you prevent the class to be called more than once, which would confuse the compiler.

#include "ofMain.h"
#include "ofxOpenCv.h"

class Blob
{
public:
	// VARIABLES -------------------------------------------------------------
	ofxCvBlob blob;
	int ID;

	ofVec2f p_pos; // Previous position. Used to calculate velocity.
	ofVec2f vel;   // Velocity.

	// ofTrueTypeFont font;	// Font to be used when writing blob's ID on screen.
	float maxVelocity; // Blobs with velocities which are greater than 'maxVelocity'
					   //  will be set to vel zero. This is an attempt to avoid crazy
					   //  velocities which appear sometimes randomly. You are not sure
					   //  why this happen yet.
	// -----------------------------------------------------------------------

	// METHODS ---------------------------------------------------------------
	Blob();			   // Constructor
	Blob(ofxCvBlob b); // Constructor

	void drawContainer(int x, int y, ofTrueTypeFont font);
	void drawVelocity(int x, int y);

	void calcVel(); // Calculate the blob's velocity vector
					// -----------------------------------------------------------------------

private:
};

#endif
