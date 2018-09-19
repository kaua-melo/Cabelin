#include "Canvas.h"
#include "ofApp.h" // We need this in order to access ofApp. We access it on the suicide function, take a look at that.


// CONSTRUCTOR ======================================================================
Canvas::Canvas(int w, int h)
{
    fbo.allocate( w, h, GL_RGB );//, GL_RGBA); 

    position.x = 0;
    position.y = 0;

    width  = w;
    height = h;

    cout<<" .k--> The maximum number of MSAA samples that your graphic card supports: "<< fbo.maxSamples() << endl;
    cout<<" .k--> You can set the ofFbo number of samples on ofApp.cpp at 'canvas->setNumberSamples(0);'" << endl;

}

void Canvas::setNumberSamples(int numSamples)
{
    fbo.allocate( width, height, GL_RGB, numSamples);//, GL_RGBA); 
    cout<<" .k--> ofFbo now has: " << numSamples << " samples" << endl;    
}

		
void Canvas::draw()
{
    fbo.draw(position.x, position.y);
}

void Canvas::allocate(int w, int h)
{
    fbo.allocate( w, h,  GL_RGB );//, GL_RGBA); 
    width  = w;
    height = h;
}


ofVec2f Canvas::getPosition()
{
    return position;
}


void Canvas::setPosition(ofVec2f v)
{
    position = v;
}

