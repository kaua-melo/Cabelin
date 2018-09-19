
#include "LockOfHair.h"
#include "ofApp.h" // We need this in order to access ofApp. We access it on the suicide function, take a look at that.


// CONSTRUCTOR ======================================================================
LockOfHair::LockOfHair(string _name, ofVec2f pos, int _index, Canvas &_canvas)
{
    nHairs = 0;
    name   = _name;
    index  = _index;
    
    isLockOfHairActive = false;    
    areHairsVisible    = true;
    
    gui.setup();
    gui.setName(name);    
	gui.add(addHair.setup("Are you adding hair?", false));  
    
    beforeAddHair.setName("BEFORE ADDING HAIR");
        beforeAddHair.add(length.set("Length", 25, 1, 500));   
        beforeAddHair.add(lengthRandomness.set("Length Randomness", 0, 0, 100));    
        beforeAddHair.add(np.set("Number of Particles", 5, 2, 12)); 
    gui.add(beforeAddHair);
    
    
    afterAddHair.setName("AFTER HAIR IS ADDED");
        afterAddHair.add(color.set("Color", ofColor(0), ofColor(0, 0), ofColor(255, 255)));  
        afterAddHair.add(colorRandomness.set("Color Randomness", 0, 0, 1));    
        afterAddHair.add(thickness.set("Thickness", 1, 1, 7));        
        afterAddHair.add(friction.set("Friction", 0.99, 0.9, 1));    
        afterAddHair.add(speedLimit.set("SpeedLimit", 50, 0, 200));
        afterAddHair.add(stiffness.set("Stiffness", 0.1, 0, 1));
        afterAddHair.add(damping.set("Damping", 0.4, 0, 1));
    gui.add(afterAddHair);

    
    gui.add(pickColor.setup("Pick color manually",false));
	gui.add(addOne.setup("Add one hair per click", true));  

    
    
        squareGroup.setName("SQUARE PARAMETERS");
            squareGroup.add( squareDimensions.set("square dimensions", ofVec2f(50,50), ofVec2f(5,5), ofVec2f(300,300)) );
            squareGroup.add( squareHairDensity.set("N of hairs square", 1, 1, 400) );
        gui.add(squareGroup);      
	gui.add(addSquare.setup("Add a square of hair", false));  
            
    
        circleGroup.setName("CIRCLE PARAMETERS");
            circleGroup.add( circleRadius.set("Radius", 10, 1, 100) );
            circleGroup.add( circleHairDensity.set("N of hairs circle", 1, 1, 400) );
        gui.add(circleGroup);    
	gui.add(addCircle.setup("Add a circle of hair", false));  
    
    
    editing.setName("EDITING");
        editing.add( removeLastHair.set("Undo last hairs added", false) ); 
        editing.add( removeAllHair.set("Remove all hairs", false) );   
    gui.add(editing);
    gui.add(visibility.setup("Hide hairs", false));       
    gui.add( deleteLockOfHair.setup("Delete Lock of Hairs", false) );                    
    
    gui.add(numberOfHairs.setup("Number of hairs", ofToString(nHairs)) );    
    

    
    gui.setPosition(pos.x, pos.y);
    
    
    addHair.addListener(this, &LockOfHair::changedAddHair);
    thickness.addListener(this, &LockOfHair::changedThickness);
    friction.addListener(this, &LockOfHair::changedFriction);
    speedLimit.addListener(this, &LockOfHair::changedSpeedLimit);
    stiffness.addListener(this, &LockOfHair::changedStiffness);
    damping.addListener(this, &LockOfHair::changedDamping);
    color.addListener(this, &LockOfHair::changedColor);
    colorRandomness.addListener(this, &LockOfHair::changedColorRandomness);

    
    removeLastHair.addListener(this, &LockOfHair::undo);
    removeAllHair.addListener(this,  &LockOfHair::clean);
    visibility.addListener(this, &LockOfHair::hideHairs);
    
    deleteLockOfHair.addListener(this, &LockOfHair::suicide);

    
    addOne.addListener(this, &LockOfHair::changedAddOne);
    addSquare.addListener(this, &LockOfHair::changedAddSquare);
    addCircle.addListener(this, &LockOfHair::changedAddCircle);
    
    squareDimensions.addListener(this, &LockOfHair::changedSquareDimensions);
    squareHairDensity.addListener(this, &LockOfHair::changedHairDensity);
    circleHairDensity.addListener(this, &LockOfHair::changedHairDensity);
    length.addListener(this, &LockOfHair::changedHairLengthOrRandomness);
    lengthRandomness.addListener(this, &LockOfHair::changedHairLengthOrRandomness);
    
    
    circleRadius.addListener(this, &LockOfHair::changedCircleDimensions);
    
    isMousePressed = false; 
    isMouseOverOtherGuis = false;
    

    activeLockOfHairColor = ofColor(0,200,50);
    
    if (isLockOfHairActive )
    {
        gui.setHeaderBackgroundColor( activeLockOfHairColor );
    }
    else
    {
        gui.setHeaderBackgroundColor( ofColor(100) );        
    }
    
    
    isGUIVisible  = true;   

    canvas = &_canvas; 
}
// ==================================================================================






// ==================================================================================
void LockOfHair::clean(bool & b)
{
    if(b)
    {
        hairs.clear();
        nHairs = 0;
        numberOfHairs = ofToString(nHairs);   
        
        removeAllHair = false;
    }
}
// ==================================================================================


// ==================================================================================
void LockOfHair::suicide(bool & b)
{    
    // Get a pointer to the ofApp so you can call any method there :)
    ofApp* app = dynamic_cast<ofApp*>(ofGetAppPtr());
    
    
    // Remove the pointer for this object and also destroy this object
    //app->destroyPgrid(index);
    
    //delete this;
    
    // The methods commented above were crashing the program sometimes I'm not 100% sure why.
    // The solution was to add an int vector in ofApp.h which will contain the indexes
    //  of the PerlinNoiseGrids that needs to be destroid. As you can see below.
    app->lockOfHairToBeDestroyed.push_back(index);      
}
// ==================================================================================


// ==================================================================================
void LockOfHair::undo(bool & b)
{
    if(b)
    {
        if(numberOfHairsAdded.size() > 0)
        {
            // Get the last value of the vector
            int nHairsToRemove = numberOfHairsAdded.back();
            
            // Remove the last value of the vector
            numberOfHairsAdded.pop_back();
            
            for( int i=0; i<nHairsToRemove; i++ )
            {
                hairs.pop_back();              
            }
            
            // number of hairs
            nHairs -= nHairsToRemove;
            numberOfHairs = ofToString(nHairs);  
        }
            
        // If we don't make it false, the user will need to click twice
        //  next time in order to undo once.
        removeLastHair = false;
    }
}
// ==================================================================================






// ==================================================================================
void LockOfHair::mouseEvents()
{
    // MOUSE PRESSED ===========================================================
    if(ofGetMousePressed(0) && !isMousePressed)
    {   
        // getting mouse's position        
        ofVec2f mouse(ofGetMouseX() - canvas->position.x, 
                      ofGetMouseY() - canvas->position.y);

        // PICK COLOR
        // If the user clicked to pick the color and the mouse is over the picture
        if(pickColor && isMouseOverImage())
        {
            // Get the pixel's color the mouse is currently over
            //ofColor a = bImg.getColor(mouse.x  - (ofGetWidth()/2  - bImg.getWidth()/2),
            //                          mouse.y  - (ofGetHeight()/2 - bImg.getHeight()/2)); 
            
            ofColor a = bImg.getColor(mouse.x, mouse.y );

            //Saving the alpha value of the current color so we can keep it after the color was changed.
            float lastAlpha = (float)color->a;

            // Let's keep the same alpha.
            a.a = lastAlpha;

            // Setting the GUI's panel color to the pixel's color the mouse is horing over
            color = a;   
        }
        
        
        // ADD HAIR
        // If the user clicked to add hair and the mouse is not over the HAIR GUI panel
        //else if( addHair && !isMouseOverGui() )
        else if( addHair && !isMouseOverOtherGuis )
        {
            isMousePressed = true;

            // getting mouse's position
            //ofVec2f mouse(ofGetMouseX(), ofGetMouseY());

			// Notifying the ofApp.cpp that this LockOfHair added hair so it can keep track
			//  of the history so that they can undo adding hair actions -----------------------------
			// Get a pointer to the ofApp so you can call any method there :)
			ofApp* app = dynamic_cast<ofApp*>(ofGetAppPtr());

			// Add the index of this LockOfHair to the history since it added hair to the scene.
			app->undoHistory.push_back(index);  
			// ---------------------------------------------------------------------------------------
			

            // Add only One Hair
            if(addOne)
            {                
                // Create a chain
                hairs.push_back(ParticleChain2(length,           // length
                                               mouse,            // head position
                                               thickness,        // thickness of the chain
                                               np,               // number of particles
                                               friction,         // friction with the medium. 1 = no friction / 0 = maximum friction
                                               speedLimit,       // speed limit
                                               color,            // color
                                               stiffness,        // stiffness; 0 = no connection (too flexible) / 1 = rigid connection
                                               damping)          // damping coefficient; 0=no damping / 1= maximum damping   
                               );  

                // locking the first particle 
                hairs[hairs.size()-1].particles[0].removeAttractionPoint(0);        

                // number of hairs
                nHairs++;
                numberOfHairs = ofToString(nHairs);
                
                // Registering how many hairs were added so we can undo this action if needed
                numberOfHairsAdded.push_back(1);
            }

            // Add a square of Hair
            else if(addSquare)
            {
                for(int i=0; i<squareHairDensity; i++)
                {
                    // The color of each hair is going to be slightly different (different brightness according to
                    //      the variable "colorRandomness" present on the GUI                    
                    ofColor c = color;
                    c.setBrightness( c.getBrightness() + ofRandom(-colorRandomness*255,colorRandomness*255) );
                    
                    
                    // Randomly pick a point inside the squareDimenios
                    ofVec2f hPosition(ofRandom(mouse.x-squareDimensions.get().x/2, mouse.x+squareDimensions.get().x/2), 
                                      ofRandom(mouse.y-squareDimensions.get().y/2, mouse.y+squareDimensions.get().y/2));

                    // The length of the hair will be the 'length' [-lengthRandomness,0] % of the length set on the GUI.
                    float l = length + ofRandom(-(lengthRandomness/100)*length, 0);
                    
                    // Create a chain
                    hairs.push_back(ParticleChain2(l,                // length
                                                   hPosition,        // head position
                                                   thickness,        // thickness of the chain
                                                   np,               // number of particles
                                                   friction,         // friction with the medium. 1 = no friction / 0 = maximum friction
                                                   speedLimit,       // speed limit
                                                   c,                // color
                                                   stiffness,        // stiffness; 0 = no connection (too flexible) / 1 = rigid connection
                                                   damping)          // damping coefficient; 0=no damping / 1= maximum damping   
                                   );                  

                    // locking the first particle 
                    hairs[hairs.size()-1].particles[0].removeAttractionPoint(0);        

                    // number of hairs    
                    nHairs++;
                }

                numberOfHairs = ofToString(nHairs);
                
                // Registering how many hairs were added so we can undo this action if needed
                numberOfHairsAdded.push_back(squareHairDensity);
            }

            // Add a circle of hairs
            else if(addCircle)
            {
                for(int i=0; i<circleHairDensity; i++)
                {
                    // The color of each hair is going to be slightly different (different brightness according to
                    //      the variable "colorRandomness" present on the GUI
                    ofColor c = color;
                    c.setBrightness( c.getBrightness() + ofRandom(-colorRandomness*255,colorRandomness*255) );
                    
                    
                    
                    // Randomly pick a point inside the circleRadius            
                    float d     = ofRandom(0,circleRadius);
                    float angle = ofRandom(0,6.28);
                    ofVec2f hPosition( mouse.x + d*cos(angle),
                                       mouse.y + d*sin(angle) );

                    // The length of the hair will be the 'length' [-lengthRandomness,0] % of the length set on the GUI.
                    float l = length + ofRandom(-(lengthRandomness/100)*length, 0);
                    
                    // Create a chain
                    hairs.push_back(ParticleChain2(l,                // length
                                                   hPosition,        // head position
                                                   thickness,        // thickness of the chain
                                                   np,               // number of particles
                                                   friction,         // friction with the medium. 1 = no friction / 0 = maximum friction
                                                   speedLimit,       // speed limit
                                                   c,                // color
                                                   stiffness,        // stiffness; 0 = no connection (too flexible) / 1 = rigid connection
                                                   damping)          // damping coefficient; 0=no damping / 1= maximum damping   
                                   );                  

                    // locking the first particle 
                    hairs[hairs.size()-1].particles[0].removeAttractionPoint(0);        

                    // number of hairs    
                    nHairs++;
                }

                numberOfHairs = ofToString(nHairs);

                // Registering how many hairs were added so we can undo this action if needed
                numberOfHairsAdded.push_back(circleHairDensity);
            }        
        }
    }
    // =========================================================================
    // MOUSE RELEASED ==========================================================
    else if( !ofGetMousePressed(0) && isMousePressed)
    {
        isMousePressed         = false;        
    }
    // =========================================================================             
}
// ==================================================================================


// ==================================================================================
bool LockOfHair::isMouseOverGui()
{
    ofVec2f mouse(ofGetMouseX(), ofGetMouseY());
    
    int GUIborder = 5;
    
    return (mouse.x > gui.getPosition().x - GUIborder && 
            mouse.x < gui.getPosition().x + gui.getWidth() + GUIborder &&
            mouse.y > gui.getPosition().y - GUIborder &&
            mouse.y < gui.getPosition().y + gui.getHeight() + GUIborder
           );
}
// ==================================================================================


// ==================================================================================
bool LockOfHair::isMouseOverImage()
{
    ofVec2f mouse(ofGetMouseX(), ofGetMouseY());
    
    //return (mouse.x  > ofGetWidth()/2  - bImg.getWidth()/2  &&
    //        mouse.x  < ofGetWidth()/2  + bImg.getWidth()/2  &&
    //        mouse.y  > ofGetHeight()/2 - bImg.getHeight()/2 &&
    //        mouse.y  < ofGetHeight()/2 + bImg.getHeight()/2
    //       );

    return (mouse.x > canvas->position.x &&
            mouse.x < canvas->position.x  + bImg.getWidth()  &&
            mouse.y > canvas->position.y &&
            mouse.y < canvas->position.y + bImg.getHeight()
           );
}
// ==================================================================================



// Function that detects the pixel's color that's under the mouse and 
//   draw a square with that color
// This function is called if the pickColor toggle is true.
// ==================================================================================
void LockOfHair::pickColorByHovering()
{ 
    // If the user has dropped an image onto the window AND 
    //  the mouse is hovering over the image AND 
    //  pick color manually is activated.
    if(bImg.isAllocated() && isMouseOverImage() )
    {   
        ofVec2f mouse(ofGetMouseX() - canvas->position.x, 
                      ofGetMouseY() - canvas->position.y);

        // Get the pixel's color the mouse is currently over
        //ofColor a = bImg.getColor(mouse.x  - (ofGetWidth()/2  - bImg.getWidth()/2),
        //                          mouse.y  - (ofGetHeight()/2 - bImg.getHeight()/2));
        ofColor a = bImg.getColor(mouse.x, mouse.y );
            
        //ofSetColor(a);    
        //ofFill();
        //ofDrawRectangle(ofGetWidth()-150, ofGetHeight()-150, 100, 100);

        // Get a pointer to the ofApp so you can call any method there :)
        ofApp* app = dynamic_cast<ofApp*>(ofGetAppPtr());

        app->printColorBeingPicked = true;            
        app->colorBeingPicked = a;
    }    
}
// ==================================================================================



// ==================================================================================
void LockOfHair::update()
{
    if(isLockOfHairActive)
    {
        mouseEvents();        
    }
    
    for(unsigned i=0; i<hairs.size(); i++)
    {
        hairs[i].update();
    }
}
// ==================================================================================



// Draw a preview of the hair(s) that will be added
// ==================================================================================
void LockOfHair::drawHairPreview()
{    
    ofVec2f mouse(ofGetMouseX() - canvas->position.x,
                  ofGetMouseY() - canvas->position.y);
    ofNoFill();


    if(addOne)
    {
        ofSetColor(color); // Hair color
        ofSetLineWidth(thickness); // Setting the thichness of the line to be drawn.

        ofDrawLine(mouse.x,mouse.y,mouse.x,mouse.y+length);
    }
    
    else if(addSquare)
    {        
        ofSetColor(0); // black
        ofDrawRectangle(mouse.x-squareDimensions.get().x/2, 
                        mouse.y-squareDimensions.get().y/2,
                        squareDimensions.get().x, 
                        squareDimensions.get().y);
        

        ofSetColor(color); // Hair color
        ofSetLineWidth(thickness); // Setting the thichness of the line to be drawn.

        for(int i =0; i< squareHairDensity; i++)
        {            
            ofDrawLine( mouse.x + previewPosition[i].x, mouse.y + previewPosition[i].y, 
                        mouse.x + previewPosition[i].x, mouse.y + previewPosition[i].y + previewHairLength[i]);
        }        
    }
    
    else if(addCircle)
    {      
        ofSetColor(0); // black
        ofDrawCircle(mouse.x, mouse.y, circleRadius);
        

        ofSetColor(color); // Hair color
        ofSetLineWidth(thickness); // Setting the thichness of the line to be drawn.

        for(int i =0; i< circleHairDensity; i++)
        {
            ofDrawLine( mouse.x + previewPosition[i].x, mouse.y + previewPosition[i].y, 
                        mouse.x + previewPosition[i].x, mouse.y + previewPosition[i].y + previewHairLength[i]);
        }  
    }    
}
// ==================================================================================



// ==================================================================================
void LockOfHair::draw()
{   
    //fbo->begin(); 
    canvas->fbo.begin();       
        if(isLockOfHairActive)
        {            
            // Draw preview of hair just if the user is about to add a Hair
            //  and the pickColor is not checked
            if(addHair && !pickColor)
            {
                drawHairPreview();    
            }
            
            // If the pickColor toggle variable is checked
            if(pickColor)
            {
                // Function that detects the pixel's color that's under the mouse and 
                //   draw a square with that color
                canvas->fbo.end();
                pickColorByHovering();
                ofSetColor(255,255,255);
                canvas->fbo.begin();
            }        
        }

        if(areHairsVisible)
        {
            for(unsigned int i=0; i<hairs.size(); i++)
            {
                hairs[i].drawThinL();
            }    
        }
    //fbo->end();
    canvas->fbo.end();
 
    //drawGUI();
}
// ==================================================================================


void LockOfHair::drawGUI()
{
    if(isGUIVisible)
    {
        gui.draw();
    }    
}


// ==================================================================================
void LockOfHair::changedDamping(float & dp)
{
    for(unsigned int i=0; i<hairs.size(); i++)
    {
        hairs[i].changeDamping(dp);
    }
}
// ==================================================================================




// ==================================================================================
void LockOfHair::changedStiffness(float & st)
{
    for(unsigned int i=0; i<hairs.size(); i++)
    {
        hairs[i].changeStiffness(st);
    }
}
// ==================================================================================




// ==================================================================================
void LockOfHair::changedSpeedLimit(float & sl)
{
    for(unsigned int i=0; i<hairs.size(); i++)
    {
        hairs[i].changeSpeedLimit(sl);
    }
}
// ==================================================================================



// ==================================================================================
void LockOfHair::changedFriction(float & f)
{
    for(unsigned int i=0; i<hairs.size(); i++)
    {
        hairs[i].changeFriction(f);
    }
}
// ==================================================================================




// ==================================================================================
void LockOfHair::changedThickness(int & t)
{
    for(unsigned int i=0; i<hairs.size(); i++)
    {
        hairs[i].changeThickness(t);
    }
}
// ==================================================================================




// ==================================================================================
void LockOfHair::changedAddHair(bool & b)
{
    if(b)
    {
        gui.setHeaderBackgroundColor(ofColor(250,0,50));
                
        if(addSquare)
        {
            int aux = squareHairDensity;
            changedHairDensity(aux);
        }
        else if(addCircle)
        {
            int aux = circleHairDensity;
            changedHairDensity(aux);
        }
    }
    else
    {
        if (isLockOfHairActive )
        {
            gui.setHeaderBackgroundColor( activeLockOfHairColor );
        }
        else
        {
            gui.setHeaderBackgroundColor( ofColor(100) );        
        }        
    }
}
// ==================================================================================



// ==================================================================================
void LockOfHair::changedAddOne(bool & b)
{
    // if addOne is true, make the rest false
    if(addOne)
    {
        addSquare = false;
        addCircle = false;
    }
    // if addOne is false and the others are also false, make addOne true 
    else if(!addSquare && !addCircle)
    {
        addOne = true;
    }
}
// ==================================================================================



// ==================================================================================
void LockOfHair::changedAddSquare(bool & b)
{
    // if addSquare is true, make the rest false
    if(addSquare)
    {
        addOne    = false;
        addCircle = false;
        
        calculateHairPreviewPositionAndLength("square");
    }
    // If addSquare is false and the others are also false, make addSquare true 
    //  in order to keep some adding hair modality true.    
    else if(!addOne && !addCircle)
    {
        addSquare = true;
    }    
}
// ==================================================================================



// ==================================================================================
void LockOfHair::changedAddCircle(bool & b)
{
    // if addCircle is true, make the rest false
    if(addCircle)
    {
        addOne    = false;
        addSquare = false;
        
        calculateHairPreviewPositionAndLength("circle");        
    }
    // If addSquare is false and the others are also false, make addCircle true 
    //  in order to keep some adding hair modality true.
    else if(!addOne && !addSquare)
    {
        addCircle = true;
    }        
}
// ==================================================================================


// ==================================================================================
void LockOfHair::changedColor(ofColor & c)
{
    for(unsigned int i=0; i<hairs.size(); i++)
    {
        // The color of each hair is going to be slightly different (different brightness according to
        //      the variable "colorRandomness" present on the GUI
        ofColor a = c;
        a.setBrightness( c.getBrightness() + ofRandom(-colorRandomness*255,colorRandomness*255) );
        
        
        hairs[i].changeColor(a);
    }    
}
// ==================================================================================



// ==================================================================================
void LockOfHair::changedColorRandomness(float & r)
{
    for(unsigned int i=0; i<hairs.size(); i++)
    {
        // The color of each hair is going to be slightly different (different brightness according to
        //      the variable "colorRandomness" present on the GUI
        ofColor a = color;
        a.setBrightness( a.getBrightness() + ofRandom(-r*255,r*255) );
        
        hairs[i].changeColor(a);
    }    
}
// ==================================================================================




// ==================================================================================
void LockOfHair::changedHairDensity(int & hd)
{
    if(addSquare)
    {  
        calculateHairPreviewPositionAndLength("square");
    }
    else if(addCircle)
    {
        calculateHairPreviewPositionAndLength("circle");        
    }
}
// ==================================================================================


// ==================================================================================
void LockOfHair::calculateHairPreviewPositionAndLength(string addingMode)
{        
    if(addingMode == "square")
    {            
        // Delete all hair preview positions and the length of the hairs
        //  since we are going to define new positions and lengths
        previewPosition.clear();
        previewHairLength.clear();
        
        
        // Set new hair preview positions
        for(int i=0; i<squareHairDensity; i++)
        {
            int px = ofRandom(-squareDimensions.get().x/2, squareDimensions.get().x/2);
            int py = ofRandom(-squareDimensions.get().y/2, squareDimensions.get().y/2);

            previewPosition.push_back(ofVec2f(px, py));
            
            // The length of the hair will be the 'length' + [-lengthRandomness,0] % of the length set on the GUI.
            previewHairLength.push_back( length + ofRandom(-(lengthRandomness/100)*length, 0) );
        }            
    }
    else if(addingMode == "circle")
    {
        // Delete all hair preview positions and the length of the hairs
        //  since we are going to define new positions and lengths
        previewPosition.clear();
        previewHairLength.clear();        
        
        // Set new hair preview positions
        for(int i=0; i<circleHairDensity; i++)
        {
            // Randomly pick a point inside the circleRadius            
            float d     = ofRandom(0,circleRadius);
            float angle = ofRandom(0,6.28);   
        
            previewPosition.push_back(ofVec2f( d*cos(angle), d*sin(angle)));
            
            // The length of the hair will be the 'length' + [-lengthRandomness,0] % of the length set on the GUI.
            previewHairLength.push_back( length + ofRandom(-(lengthRandomness/100)*length, 0) );            
        }   
    }
}
// ==================================================================================



// ==================================================================================
void LockOfHair::changedSquareDimensions(ofVec2f & d)
{
    calculateHairPreviewPositionAndLength("square");
}
// ==================================================================================


// ==================================================================================
void LockOfHair::changedCircleDimensions(float & r)
{
    calculateHairPreviewPositionAndLength("circle");    
}
// ==================================================================================


// ==================================================================================            
LockOfHair::~LockOfHair(void) 
{
   cout << "~The LockOfHair " << index << " was destroyed" << endl;
}
// ==================================================================================


// ==================================================================================
void LockOfHair::setLockOfHairStatus(bool a)
{
    isLockOfHairActive = a;    

    if (isLockOfHairActive )
    {
        gui.setHeaderBackgroundColor( activeLockOfHairColor );
    }
    else
    {
        gui.setHeaderBackgroundColor( ofColor(100) );        
    }    
}
// ==================================================================================



// ==================================================================================
void LockOfHair::hideHairs(bool & b)
{
    areHairsVisible = !b;
}
// ==================================================================================


// ==================================================================================
void LockOfHair::changedHairLengthOrRandomness(float & l)
{
    if(addSquare)
    {
        calculateHairPreviewPositionAndLength("square");
    }
    else if(addCircle)
    {
        calculateHairPreviewPositionAndLength("circle");        
    }
}
// ==================================================================================


// ==================================================================================
// Set the visibility of the GUI
void LockOfHair::setVisibility(bool v)
{
    isGUIVisible  = v;    
}                                  
// ==================================================================================
