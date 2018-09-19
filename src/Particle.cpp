
#include "Particle.h"

// ==================================================================================
// CONSTRUCTORS =====================================================================
Particle::Particle(ofVec2f _pos, int _r, float f, int sl, ofColor color)
{
	c = color;
	speedLimit = sl;

	pos = _pos;

	r = _r;

	friction = f;

	acc.set(0,0);
	vel.set(0,0);

	//speedLimit is set by default, but you can change it;
	//	cout<<"  k--------------------------------------------------k"<<endl;
	//	cout<<"   The particle's speed limit is: "<<speedLimit<<endl;
    //	cout<<"   Call setSpeedLimit(int vl) to change it"<<endl;
	//	cout<<"   The particle's coefficient of restitution is: "<<cr<<endl;
	//	cout<<"  k--------------------------------------------------k"<<endl;    
}
// ==================================================================================
// ==================================================================================




void Particle::update()
{
	vel += acc;
	vel.limit(speedLimit); // Avoid particle going too fast
						   // In case you're using a chain of particles,
						   //  avoid them going crazy some times...
	pos += vel;

	// add friction
	vel*=friction;

	// do not accumulate the acc
	acc.set(0,0);
	
	// check if the particle is touching the borders
	//boundaries();
}


void Particle::draw()
{
	ofSetColor(c);
	ofDrawEllipse(pos.x, pos.y, 2*r, 2*r);
}


void Particle::addForce(ofVec2f f)
{
	acc += f;
}

void Particle::setColor(ofColor color)
{
	c = color;
}


void Particle::boundaries()
{
	// touching the right side
	if(pos.x + r > ofGetWindowWidth())
	{
		pos.x  = ofGetWindowWidth() - r;
		vel.x *= -cr;
	}
	// touching the left side
	else if(pos.x - r < 0)
	{
		pos.x  =  r;
		vel.x *= -cr;
	}

	// touching the top
	if(pos.y - r < 0)
	{
		pos.y  = r;
		vel.y *= -cr;
	}
	// touching the bottom
	else if(pos.y + r > ofGetWindowHeight())
	{
		pos.y  = ofGetWindowHeight() - r;
		vel.y *= -cr; 
	}
}

void Particle::boundaries2()
{
	// right border
	if(pos.x - r > ofGetWindowWidth())
	{
		pos.x = -r; 
	}
	// left border
	else if(pos.x + r < 0)
	{
		pos.x = ofGetWindowWidth() + r;
	}

	// top border
	if(pos.y + r < 0)
	{
		pos.y = ofGetWindowHeight() + r;
	}
	// touching the bottom
	else if(pos.y - r > ofGetWindowHeight())
	{
		pos.y = -r;
	}
}

void Particle::setSpeedLimit(int vl)
{
	speedLimit = vl;
	cout<<"  k-----------------------------------------k"<<endl;
	cout<<"   The new particle's speed limit is: "<< speedLimit <<endl;
	cout<<"  k-----------------------------------------k"<<endl;
}


void Particle::setCoefficientOfRestitution(float _cr)
{
	cr = _cr;
	cout<<"  k----------------------------------------------------k"<<endl;
	cout<<"   The new particle's coefficient of restitution is: "<< cr <<endl;
	cout<<"  k----------------------------------------------------k"<<endl;
}


void Particle::changeRadius(int newRadius)
{
    r = newRadius;
}


void Particle::changeFriction(float newFriction)
{
    friction = newFriction;
}

void Particle::changeSpeedLimit(float newSpeedLimit)
{
    speedLimit = newSpeedLimit;
}
