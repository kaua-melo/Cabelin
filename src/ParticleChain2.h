/*
*   The ParticleChain2.h is different from ParticleChain.h because it makes use
*   of LinkedChain2.h instead of LinkedChain.h.
*   
*   So, with ParticleChain2.h you can use damping connections between the particles.
*   That allow us to create a more rigid hair and still have is moving free on the medium.
*/


#ifndef _PARTICLECHAIN2 // if this class hasn't been defined, the program can define it
#define _PARTICLECHAIN2 // by using this if statement you prevent the class to be called more than once which would confuse the compiler

#include "ofMain.h"   // we need to include this to have a reference to the openFrameworks framework
#include "LinkedParticle2.h" // we need to include this since we will inherit from Particle


                    
class ParticleChain2 
{
	public:
        // Length of the chain
        float length;
		
        // Number of particles
        int np;
    
        // Array(vector) of particles
		vector<LinkedParticle2>  particles;
    
        // The head of the chain. 
        // The position of the first particle.
        ofVec2f head;
    
        // How thick is the chain?
        int thickness;
    
        ofColor c;
        
    
        // Constructor
        ParticleChain2(float _length,       // length of the chain
                       ofVec2f pos,         // position of the head (first particle of the chain)
                       int _thickness,      // thickness of the chain 
                       int _np,             // number of particles
                       float f,             // friction
                       int sl,              // speedLimit
                       ofColor color,       // color
                       float stiffness,     // stiffness of the connection (k from F=kx)
                       float _damping       // damping coefficient
                      );
    
        void update();
        void drawP();      //draw particles
    
        // There is a limit of thickness that openFrameworks can print.
        // If you increase the thickness of the chain and the thickness 
        //  printed ins't thicker, then you should print with drawThickL()
        //  instead of drawThinL().    
        void drawThinL();  //draw thin line
        void drawThickL(); //draw thick line
    
    
    
        void changeThickness(int newThickness); // change thickness of the chain
        
        // change the friction of the particles with the medium    
        void changeFriction(float newFriction); 
        void changeSpeedLimit(float newSpeedLimit); 
        void changeStiffness(float newStiffness); 
        void changeDamping(float newDamping); 
        void changeColor(ofColor newC);

    
        void debug();
    
        
	private:
};
#endif
