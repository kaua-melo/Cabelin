
#include "ParticleChain2.h"


// ==================================================================================
// CONSTRUCTORS =====================================================================
ParticleChain2::ParticleChain2(float _length, ofVec2f pos, int _thickness, int _np, float f, int sl, ofColor color, float stiffness, float damping)
{
	length    = _length;
    thickness = _thickness;
    np        = _np;
    c = color;
    
    head = pos;
    

    // gap between particles
    float gap = length/(np-1);
            
    // creating the particles
    for(int i=0; i<np; i++)
    {
        particles.push_back( LinkedParticle2(ofVec2f(head.x, head.y +i*gap), // position of the particle
                                             thickness/2,                    // radius of the particle
                                             f,                              // friction with the medium
                                             sl,                             // speed limit
                                             c));                            // color of the particle
    }
    
    
    // CONNECTING THE PARTICLES -------------------------------------------------------
    // --------------------------------------------------------------------------------
        // FIRST PARTICLE
        // addAttractionPoint(ofVec2f _pos, float _length, float _stiffness, float _damping)

        particles[0].addAttractionPoint(particles[1].pos, 
                                        particles[0].pos.distance(particles[1].pos),
                                        stiffness,
                                        damping);
        // PARTICLES FROM THE MIDDLE
        for(int i=1; i<np-1; i++)
        {
            // connecting with left particle
            particles[i].addAttractionPoint(particles[i-1].pos, 
                                        particles[i].pos.distance(particles[i-1].pos),
                                        stiffness,
                                        damping);

            // connecting with right particle
            particles[i].addAttractionPoint(particles[i+1].pos, 
                                        particles[i].pos.distance(particles[i+1].pos),
                                        stiffness,
                                        damping);
        }    

        // LAST PARTICLE
        particles[np-1].addAttractionPoint(particles[np-2].pos, 
                                           particles[np-1].pos.distance(particles[np-2].pos),
                                           stiffness,
                                           damping);    
    // --------------------------------------------------------------------------------
    // --------------------------------------------------------------------------------
}
// ==================================================================================
// ==================================================================================



// ==================================================================================
void ParticleChain2::drawP()
{
    for(unsigned int i=0; i<particles.size(); i++)
    {
        particles[i].draw();
    } 
}
// ==================================================================================



// ==================================================================================
void ParticleChain2::drawThinL()
{
	ofSetColor(c);
    
	ofPolyline line;

	// for first connection (particle 0 and 1)
	line.curveTo(particles[0].pos);
	line.curveTo(particles[0].pos);

	// connections between particles in the middle
	for(int i=1; i<np; i++)
	{
		line.curveTo(particles[i].pos);
	}

	// for last particle
	line.curveTo(particles[np-1].pos);

    
    
	// There are two ways of setting the weight 
	// of a line in OF: ofSetLineWidth(2) and glLineWidth(20);
	// Both of the ways has a weight maximum limit.

    ofSetLineWidth(thickness);
	line.draw();
}
// ==================================================================================



// ==================================================================================
void ParticleChain2::drawThickL()
{
	ofSetColor(c);
    
	ofPolyline line;

	// for first connection (particle 0 and 1)
	line.curveTo(particles[0].pos);
	line.curveTo(particles[0].pos);

	// connections between particles in the middle
	for(int i=1; i<np; i++)
	{
		line.curveTo(particles[i].pos);
	}

	// for last particle
	line.curveTo(particles[np-1].pos);

	//line.draw();

	// There are two ways of setting the weight 
	// of a line in OF: ofSetLineWidth(2) and glLineWidth(20);
	// Both of the ways has a weight maximum limit.
	// Here's what I did to workaround it:
	ofVec2f v = line.getPointAtLength(0);

	int index=0;
	while( index < line.getPerimeter() )
	{
		v = line.getPointAtLength(index);
		ofDrawEllipse(v.x, v.y, thickness, thickness);
		index+=1;
	}

	v = line.getPointAtLength(line.getPerimeter());
	ofDrawEllipse(v.x, v.y, thickness, thickness);
}
// ==================================================================================



// ==================================================================================
void ParticleChain2::debug()
{
    for(unsigned int i=0; i<particles.size(); i++)
    {
        particles[i].debug();
    } 
}
// ==================================================================================



// ==================================================================================
void ParticleChain2::update()
{
    // UPDATING CONNECTIONS -----------------------------------------------------------
    // --------------------------------------------------------------------------------
        // FIRST PARTICLE
        // addAttractionPoint(ofVec2f _pos, float _length, float _stiffness);
        particles[0].setAttractionPointPosition(0, particles[1].pos);

        // PARTICLES FROM THE MIDDLE
        for(int i=1; i<np-1; i++)
        {
            // connection with left particle
            particles[i].setAttractionPointPosition(0, particles[i-1].pos );

            // connection with right particle
            particles[i].setAttractionPointPosition(1, particles[i+1].pos );
        }    

        // LAST PARTICLE
        particles[np-1].setAttractionPointPosition(0, particles[np-2].pos);    
    // --------------------------------------------------------------------------------
    // --------------------------------------------------------------------------------
    
    
    // Updating each particle
    for(unsigned int i=0; i<particles.size(); i++)
    {
        particles[i].update();
    } 
}
// ==================================================================================



// ==================================================================================
void ParticleChain2::changeThickness(int newThickness)
{
    thickness = newThickness;
    for(unsigned int i=0; i<particles.size(); i++)
    {
        particles[i].changeRadius(newThickness);
    }
}
// ==================================================================================


// ==================================================================================
void ParticleChain2::changeFriction(float newFriction)
{
    for(unsigned int i=0; i<particles.size(); i++)
    {
        particles[i].changeFriction(newFriction);
    }
}
// ==================================================================================

// ==================================================================================
void ParticleChain2::changeSpeedLimit(float newSpeedLimit)
{
    for(unsigned int i=0; i<particles.size(); i++)
    {
        particles[i].changeSpeedLimit(newSpeedLimit);
    }
}
// ==================================================================================


// ==================================================================================
void ParticleChain2::changeStiffness(float newStiffness)
{
    for(unsigned int i=0; i<particles.size(); i++)
    {
        particles[i].changeStiffness(newStiffness);
    }
}
// ==================================================================================


// ==================================================================================
void ParticleChain2::changeDamping(float newDamping)
{
    for(unsigned int i=0; i<particles.size(); i++)
    {
        particles[i].changeDamping(newDamping);
    }
}
// ==================================================================================


// ==================================================================================
void ParticleChain2::changeColor(ofColor newC)
{
    c = newC; 
}
// ==================================================================================
