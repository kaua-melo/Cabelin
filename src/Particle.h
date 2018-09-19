
#ifndef _PARTICLE // if this class hasn't been defined, the program can define it
#define _PARTICLE // by using this if statement you prevent the class to be called more than once which would confuse the compiler

#include "ofMain.h" // we need to include this to have a reference to the openFrameworks framework

class Particle
{
	public:
		// particle's position
		ofVec2f pos;
	
		// radius
		int r;

		// friction with the medium
		// Should be from 0 to 1
		float friction;
		
		// Coefficient of restitution
		// cr = 1 -> the particle returns at the same speed after an impact.
		// cr = 0 -> the particle do not return.
		float cr = 1;


		// color of the particle
		ofColor c = ofColor(0);

		// acc-vel
		ofVec2f acc;
		ofVec2f vel;
    
        // maximum speed our particle will be able to move
		float speedLimit;

        // CONSTRUCTOR
		Particle(ofVec2f _pos,    // position of the particle
                 int r,           // radius of the particle
                 float f,         // friction with the medium
                 int sl,          // speed limit
                 ofColor color);  // color
    

		void update();
		void draw();
		void addForce(ofVec2f f);
		void boundaries();
		void boundaries2();    
		void setColor(ofColor color);
		void setCoefficientOfRestitution(float _rigidity);
		void setSpeedLimit(int vl);
        void changeRadius(int newRadius);
        void changeFriction(float newFriction);
        void changeSpeedLimit(float newSpeedLimit);

    
	private:
};
#endif
