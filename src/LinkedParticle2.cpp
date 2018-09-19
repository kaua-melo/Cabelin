
#include "LinkedParticle2.h"




// CONSTRUCTOR =====================================================================
LinkedParticle2::LinkedParticle2(ofVec2f _pos, int _r, float f, int sl, ofColor color) : Particle(_pos, _r, f, sl, color)
{
}
// ==================================================================================



// ==================================================================================
void LinkedParticle2::addAttractionPoint(ofVec2f _pos, float _length, float _stiffness, float _damping)
{   
    // Creating a linked point
	LinkedPoint aux;
	aux.pos          = _pos;
	aux.length       = _length;
	aux.stiffness    = _stiffness;
    aux.damping      = _damping;
    aux.lastDistance = pos.distance(aux.pos);

    // Adding it to the attractionPoints vector
	attractionPoints.push_back(aux);
}
// ==================================================================================


// ==================================================================================
void LinkedParticle2::removeAttractionPoint(int index)
{       
    // Removing attraction point from attractionPoints vector
	attractionPoints.erase(attractionPoints.begin()+index);
}
// ==================================================================================


// ==================================================================================
void LinkedParticle2::setAttractionPointPosition(int index, ofVec2f newPos)
{   
	attractionPoints[index].pos = newPos;
}
// ==================================================================================





// ==================================================================================
void LinkedParticle2::addRepulsivePoint(ofVec2f _pos, float _length, float _stiffness, float _damping)
{   
    // Creating a linked point
	LinkedPoint aux;
	aux.pos          = _pos;
	aux.length       = _length;
	aux.stiffness    = _stiffness;
    aux.damping      = _damping;
    aux.lastDistance = pos.distance(aux.pos);
        
    // Adding it to the repulsivePoints vector
	repulsivePoints.push_back(aux);
}
// ==================================================================================



// ==================================================================================
void LinkedParticle2::removeRepulsivePoint(int index)
{       
    // Removing attraction point from attractionPoints vector
	repulsivePoints.erase(repulsivePoints.begin()+index);
}
// ==================================================================================


// ==================================================================================
void LinkedParticle2::setRepulsivePointPosition(int index, ofVec2f newPos)
{   
	repulsivePoints[index].pos = newPos;
}
// ==================================================================================


// ==================================================================================
// Polymorphism of the update() function from Particle.cpp
void LinkedParticle2::update() 
{   
    // Applying force to attraction points
    for(unsigned int i=0; i<attractionPoints.size(); i++)
    {

        // SPRING -------------------------------------------------------------------
                            // Vector pointing from the particle to the point
            ofVec2f force = attractionPoints[i].pos - pos;

            float distention = force.length()-attractionPoints[i].length;

            if(distention > 0)
            {
                // How much the string was extended
                force.scale(force.length()-attractionPoints[i].length);

                // The strength of the connection/spring should affect the vector
                force *= attractionPoints[i].stiffness;

                addForce( force );
            }
        // --------------------------------------------------------------------------
        
        // DAMPER -------------------------------------------------------------------
            float currentDistance = pos.distance(attractionPoints[i].pos);
        
            if( currentDistance != attractionPoints[i].lastDistance )
            {
                ofVec2f force2 = (attractionPoints[i].pos - pos);
                
                force2.scale(currentDistance - attractionPoints[i].lastDistance);
                
                force2 *= attractionPoints[i].damping;
                    
                addForce( force2 );
            }
        // --------------------------------------------------------------------------
        
        // update the lastDistance 
        attractionPoints[i].lastDistance = currentDistance;
    }
    
    
    
    // Applying force from repulsive points
    for(unsigned int i=0; i<repulsivePoints.size(); i++)
    {
        // SPRING -------------------------------------------------------------------
                             // Vector pointing from the point to the particle
            ofVec2f distance = pos - repulsivePoints[i].pos;

            // If the distance from the particle to the repulsive point
            //  is smaller than the length specified at repulsivePoints[i].length,
            //  a repulsive force should be applied to the particle.
            if(distance.length() < repulsivePoints[i].length)
            {
                float f = ofMap(distance.length(), repulsivePoints[i].length, 0, 0, 1);

                // distance = 0                             ->  f = 1  
                // distance = repulsivePoints[m].lengthf=1  ->  f = 0
                addForce( distance*repulsivePoints[i].stiffness*f );            
            }
        // --------------------------------------------------------------------------

        
        // DAMPER -------------------------------------------------------------------
            float currentDistance = pos.distance(repulsivePoints[i].pos);
        
            if( currentDistance != repulsivePoints[i].lastDistance )
            {
                ofVec2f force2 = (repulsivePoints[i].pos - pos);
                
                force2.scale(currentDistance - repulsivePoints[i].lastDistance);
                
                force2 *= repulsivePoints[i].damping;
                    
                addForce( force2 );
            }
        // --------------------------------------------------------------------------
        
        // update the lastDistance 
        repulsivePoints[i].lastDistance = currentDistance;        
    }    
    
    
    // Calling parent's method
    Particle::update();
}
// ==================================================================================



// ==================================================================================
// Draw the attraction and repulsive points
void LinkedParticle2::debug() 
{   
    ofSetColor(0,0,255);
    
    // Draw each attraction point
    for(unsigned int i=0; i<attractionPoints.size(); i++)
    {
        // Draw circle
        ofDrawCircle(attractionPoints[i].pos.x, attractionPoints[i].pos.y, 5);
        
        // Draw connection line
        ofDrawLine(attractionPoints[i].pos.x, attractionPoints[i].pos.y, pos.x, pos.y);
    }
    
    
        
    // Draw each repulsive point
    for(unsigned int i=0; i<repulsivePoints.size(); i++)
    {
        ofSetColor(255,0,0);

        // Draw circle
        ofDrawCircle(repulsivePoints[i].pos.x, repulsivePoints[i].pos.y, 5);
        
        
        if( pos.distance(repulsivePoints[i].pos) > repulsivePoints[i].length)
        {
            ofSetColor(150);
        }
        // Draw connection line
        ofDrawLine(repulsivePoints[i].pos.x, repulsivePoints[i].pos.y, pos.x, pos.y);
    }
}
// ==================================================================================



// ==================================================================================
void LinkedParticle2::changeStiffness(float newStiffness)
{   
    // attraction points
    for(unsigned int i=0; i<attractionPoints.size(); i++)
    {
        attractionPoints[i].stiffness = newStiffness;
    }
    
        
    // repulsive points
    for(unsigned int i=0; i<repulsivePoints.size(); i++)
    {
        repulsivePoints[i].stiffness = newStiffness;
    }
}
// ==================================================================================




// ==================================================================================
void LinkedParticle2::changeDamping(float newDamping)
{   
    // attraction points
    for(unsigned int i=0; i<attractionPoints.size(); i++)
    {
        attractionPoints[i].damping = newDamping;
    }
    
        
    // repulsive points
    for(unsigned int i=0; i<repulsivePoints.size(); i++)
    {
        repulsivePoints[i].damping = newDamping;
    }
}
// ==================================================================================










