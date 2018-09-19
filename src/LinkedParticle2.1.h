

/*
*   The difference from this class (LinkedParticle2.h) to LinkedParticle.h
*   is on the kind of connection with the linkedPoints.
*
*   The connection in LinkedParticle2.h has a spring and a damper, while
*   the connection in LinkedParticle.h has only a spring.
*
*
*    The presence of a damper allow us to have the effect shown on this sketch. 
*    The damper restricts how fast a particle can move from its linkedpoint.
*
*
*    by Kauã Melo.
*/



#ifndef _LINKEDPARTICLE2 // if this class hasn't been defined, the program can define it
#define _LINKEDPARTICLE2 // by using this if statement you prevent the class to be called more than once which would confuse the compiler

#include "ofMain.h"   // we need to include this to have a reference to the openFrameworks framework
#include "Particle.h" // we need to include this since we will inherit from Particle




					   // Inherit from particle
class LinkedParticle2 : public Particle
{
	public:
    
        // Each connection has a Spring and a Damper
		struct LinkedPoint
    	{
            // position of the point
        	ofVec2f pos;

			// If attraction point -> The length of the spring when relaxed 
            // If repulsive point  -> The force's Radius of action 
			float length; 
            
            // The Stiffness of the connection. 
            // F=kx. k = Stiffness
            float stiffness;
            
            // Damping coefficient. 
            // If damping = 0, there's no damper
            float damping;
            
            // We gotta track the last distance from the particle to
            // the linkedPoint position so we can apply the damper force.
            float lastDistance;
    	};		

    
        
    
		// Array with all attraction points coordinates
		vector<LinkedPoint>  attractionPoints;

		// Array with all repulsive points coordinates
		vector<LinkedPoint>  repulsivePoints;


        // CONSTRUCTOR
		LinkedParticle2(ofVec2f _pos,      // position of the particle
                        int _r,            // radius
                        float f,           // friction with the medium
                        int sl,            // speed limit
                        ofColor color);    // color


        // ADD, REMOVE, POSITION ATTRACTION POINTS --------------------------------------------------------
        // ------------------------------------------------------------------------------------------------      
            // Add a Attraction Point
            void addAttractionPoint(ofVec2f _pos,            // Position of the point
                                    float _length,           // The length of the spring when relaxed 
                                    float _stiffness,        // Stiffness of the connection (k from F=kx)
                                    float _damping);         // Damping coefficient. Damping = 0 -> no damper
    
    
            // Removes an attraction Point by its index    
            void removeAttractionPoint(int index);
    
            // Set the position of the attractive point. You can use this dynamically
            void setAttractionPointPosition(int index,       // The index of the connection
                                            ofVec2f newPos); // The new position of the attractive point
        // ------------------------------------------------------------------------------------------------
    
    
        // ADD, REMOVE, POSITION REPULSIVE POINTS --------------------------------------------------------
        // ------------------------------------------------------------------------------------------------ 
            // Add a Repulsive Point
            void addRepulsivePoint(ofVec2f _pos,             // Position of the point
                                   float _length,            // The force's Radius of action 
                                   float _stiffness,         // Stiffness of the connection (k from F=kx)
                                   float _damping);          // Damping coefficient. Damping = 0 -> no damper
    
            // Removes a repulsive Point by its index
            void removeRepulsivePoint(int index);
    
            // Set the position of the repulsive point. You can use this dynamically
            void setRepulsivePointPosition(int index,        // The index of the connection
                                           ofVec2f newPos);  // The new position of the attractive point
        // ------------------------------------------------------------------------------------------------    
    
        void update();
        void debug();
        void changeStiffness(float newStiffness);
        void changeDamping(float newDamping);

    
	private:
};
#endif
