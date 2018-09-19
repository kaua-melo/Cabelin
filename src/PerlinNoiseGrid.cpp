
#include "PerlinNoiseGrid.h"
#include "ofApp.h" // We need this in order to access ofApp. We access it on the suicide function, take a look at that.


// ==================================================================================
// CONSTRUCTOR ======================================================================
PerlinNoiseGrid::PerlinNoiseGrid(ofVec2f _pos, ofVec2f _gridDimensions, int _xNcells, int _yNcells, ofVec3f _increment, float _zTime, string _name, int _index)
{    
        name           = _name;
        index          = _index;
        pos            = _pos;
        gridDimensions = _gridDimensions;           
        
        // Setting the dimensions of one single cell inside of the grid
        cellDimensions.set(gridDimensions.x/_xNcells, 
                           gridDimensions.y/_yNcells);
            

        xNcells = _xNcells;
        yNcells = _yNcells;
    
        // We are setting 'isGUIVisible' and 'isGridVisible' to false here because
        //  when the program runs 'xNcells.addListener(this, &PerlinNoiseGrid::nColumnsChanged);'
        //  it calls the 'nColumnsChanged' function, which calls the 'update' function, which 
        //  calls the 'draw' function. Since the PerlinNoise objects are created outside the 'draw()' 
        //  function of the ofApp.cpp, it's not possible to draw anything on the screen yet. And if you 
        //  are trying to use ofFbo, you'll have some problems picking colors and such.
        // Therefore, 'isGUIVisible' and 'isGridVisible' are set to false here, but are set to true 
        //  after the declaration of the event listeners to xNcells and yNcells.
        isGUIVisible           = false;
        isGridVisible          = false;

        // When the xNcells change, call nColumnsChanged()
        xNcells.addListener(this, &PerlinNoiseGrid::nColumnsChanged);
    
        // When the yNcells change, call nLinesChanged()        
        yNcells.addListener(this, &PerlinNoiseGrid::nLinesChanged);
    
        gui.setup();    
        gui.setName(name);
        gui.add(xNcells.setup("N cells X", _xNcells, 1, 100));
        gui.add(yNcells.setup("N cells Y", _yNcells, 1, 100));
        gui.add(xIncrement.setup("xIncrement", _increment.x, 0, 0.2));
        gui.add(yIncrement.setup("yIncrement", _increment.y, 0, 0.2));
        gui.add(zIncrement.setup("zIncrement", _increment.z, 0, 0.05));    
        gui.add(force.setup("Force", 0.3, 0, 10));    
        gui.add(angle.setup("Main Direction", 0, 0, 360));    
        gui.add(aperture.setup("Aperture", 45, 0, 360));    
        gui.add(visibility.setup("Hide grid", false));       
	    gui.add(deleteGrid.setup("Destroy Grid", false));  
    
        deleteGrid.addListener(this, &PerlinNoiseGrid::suicide);
        visibility.addListener(this, &PerlinNoiseGrid::hideGrid);
    
        gui.setPosition(pos.x, pos.y-gui.getHeight());
        

        zTime = _zTime;
    
        vectorLength  = ofVec2f(0,0).distance(cellDimensions);
        vectorLength /= 2.5;
        
    
    
        // MOVING AND RESIZING THE GRID =====================================
            centerButtonRadius = 20;
            resizeButtonRadius = 10;
    
            isGUIVisible           = true;
            isGridVisible          = true;
            isMoving               = false;    
            isResizingWidth        = false;    
            isResizingHeight       = false;    

            isMousePressed = false;

            // Positioning the buttons ---------------------------------
            centerButton.set(pos.x + gridDimensions.x/2, 
                             pos.y + gridDimensions.y/2);

            rightBorderButton.set(pos.x + gridDimensions.x, 
                                  pos.y + gridDimensions.y/2);
    
            bottomBorderButton.set(pos.x + gridDimensions.x/2, 
                                   pos.y + gridDimensions.y);    
    
            // ----------------------------------------------------------
    
    
            // loading and setting the font to be used 
            verdana.load("verdana.ttf", 15, true, true);
            verdana.setLineHeight(34.0f);
            verdana.setLetterSpacing(1.035);    
        // ==================================================================
    
        gridLinesColor = ofColor(150);       
}
// ==================================================================================
// ==================================================================================

// ----------------------------------------------------------------------------------
void PerlinNoiseGrid::hideGrid(bool & b)
{
    isGridVisible = !isGridVisible;
}
// ----------------------------------------------------------------------------------


// ----------------------------------------------------------------------------------
void PerlinNoiseGrid::suicide(bool & b)
{    
    // Get a pointer to the ofApp so you can call any method there :)
    ofApp* app = dynamic_cast<ofApp*>(ofGetAppPtr());
    
    
    // Remove the pointer for this object and also destroy this object
    //app->destroyPgrid(index);
    
    //delete this;
    
    // The methods commented above were crashing the program sometimes I'm not 100% sure why.
    // The solution was to add an int vector in ofApp.h which will contain the indexes
    //  of the PerlinNoiseGrids that needs to be destroid. As you can see below.
    app->pGridToBeDestroyed.push_back(index);      
}
// ----------------------------------------------------------------------------------


// ----------------------------------------------------------------------------------
void PerlinNoiseGrid::nColumnsChanged(int &xNcells)
{    
    cellDimensions.x = gridDimensions.x/xNcells;
    
    vectorLength  = ofVec2f(0,0).distance(cellDimensions);
    vectorLength /= 2.5;  
    
    // Once we create more cells, the 'vectorField' will still 
    //  remain the same size until the update() function is called.
    // Therefore, if the ofApp.cpp tries to apply forces to the 
    //  hair before the 'vectorField' is resized, it might access some 
    //  unexisted position in the Vector (I believe) and might end up applying 
    //  some crazy forces to the hair. To avoid that, we call update() here after
    //  the number of cells were updated.
    update();
}
// ----------------------------------------------------------------------------------

// ----------------------------------------------------------------------------------
void PerlinNoiseGrid::nLinesChanged(int &yNcells)
{
    cellDimensions.y = gridDimensions.y/yNcells;
    
    vectorLength  = ofVec2f(0,0).distance(cellDimensions);
    vectorLength /= 2.5;       
    
    // Once we create more cells, the 'vectorField' will still 
    //  remain the same size until the update() function is called.
    // Therefore, if the ofApp.cpp tries to apply forces to the 
    //  hair before the 'vectorField' is resized, it might access some 
    //  unexisted position in the Vector (I believe) and might end up applying 
    //  some crazy forces to the hair. To avoid that, we call update() here after
    //  the number of cells were updated.
    update();    
}
// ----------------------------------------------------------------------------------



//
// ----------------------------------------------------------------------------------
void PerlinNoiseGrid::changeVisibility()
{    
    isGUIVisible  = !isGUIVisible;
    isGridVisible = isGUIVisible;
}
// ----------------------------------------------------------------------------------

// ----------------------------------------------------------------------------------
void PerlinNoiseGrid::setVisibility(bool v)
{    
    isGUIVisible  = v;
    isGridVisible = v;
}
// ----------------------------------------------------------------------------------



// ----------------------------------------------------------------------------------
void PerlinNoiseGrid::hide(){
    isGridVisible = false;
}
// ----------------------------------------------------------------------------------


// ----------------------------------------------------------------------------------
void PerlinNoiseGrid::show(){
    isGridVisible = true;
}
// ----------------------------------------------------------------------------------


// ----------------------------------------------------------------------------------
void PerlinNoiseGrid::update()
{
    vectorField.clear();
    
    // -------------------------------------------------------------------------  
    // GETTING VALUES FROM THE PERLIN NOISE FUNCTION ----------------------------
	float yTime = 0;
        
	for(int y=0; y<yNcells; y++)
	{
		float xTime = 0;
		//for(int x=0; x<xNcells; x++)
		for(int x=0; x<xNcells; x++)            
		{
			//float noise = ofNoise(xTime, yTime, zTime);
			//float teta = noise*6.28;
			float noise = ofSignedNoise(xTime, yTime, zTime);
            float teta  = ofDegToRad(angle + noise*aperture/2);
            
            if(noise>1)
            {
                cout<< noise <<endl;
            }
            else if(noise<-1)
            {
                cout<< noise <<endl;
            }
            
            vectorField.push_back( ofVec2f(cos(teta), sin(teta)).normalize() );

			//xTime+=increment.x;
			xTime+=xIncrement;            
		}
		//yTime+=increment.y;
		yTime+=yIncrement;        
	}

	// zTime+=increment.z;    
	zTime+=zIncrement;    
    
    // -------------------------------------------------------------------------  

    
    
   
    drawGUI();

    // If the grid is set to be visible, draw it and check for interactions
    if(isGridVisible)
    {                 
            draw();    
            mouseEvents();
    }
}
// ----------------------------------------------------------------------------------

void PerlinNoiseGrid::drawGUI()
{
    // Draw GUI
    if(isGUIVisible)
    {
        gui.draw();     
    }
}


// ----------------------------------------------------------------------------------
void PerlinNoiseGrid::mouseEvents()
{
    // MOUSE PRESSED ===========================================================
    if(ofGetMousePressed(0) && !isMousePressed)
    {                
        isMousePressed = true;
        
        ofVec2f mouse = ofVec2f(ofGetMouseX(), ofGetMouseY());
    
        // Clicked over the drag circle
        if(mouse.distance( centerButton ) < centerButtonRadius)        
        {
            isMoving = true;
        }

        // Clicked over the right resize circle
        else if( mouse.distance( rightBorderButton ) < resizeButtonRadius)
        {
            isResizingWidth = true;
        }

        // Clicked over the bottom resize circle
        else if( mouse.distance( bottomBorderButton ) < resizeButtonRadius)
        {        
            isResizingHeight = true;
        }            
    }
    // =========================================================================
    
    
    // MOUSE RELEASED ==========================================================
    else if( !ofGetMousePressed(0) && isMousePressed)
    {
        isMousePressed         = false;
        isMoving               = false;
        isResizingWidth        = false;
        isResizingHeight       = false;            
    }
    // =========================================================================
        
    
    // MOUSE DRAGGED ===========================================================
    if( isMousePressed && (ofGetMouseX()!=ofGetPreviousMouseX() || 
                           ofGetMouseY()!=ofGetPreviousMouseY()) )
    {
        // Get mouse position
        int x = ofGetMouseX();
        int y = ofGetMouseY();
        
        
        // Move the whole grid
        if(isMoving)
        {
            move( ofVec2f(x,y) );
        }

        // Resize the width by the right border
        else if(isResizingWidth)
        {
            rightResize(x);
        }

        // Resize the height by the bottom border
        else if(isResizingHeight)
        {
            bottomResize(y);
        }    
    }
    // =========================================================================   
}
// ----------------------------------------------------------------------------------


void PerlinNoiseGrid::drawButton(ofVec2f p, float r, ofColor c)
{
    ofSetColor(c);
    ofDrawCircle(p.x, p.y, r);       
}


// ----------------------------------------------------------------------------------
void PerlinNoiseGrid::drawControls()
{
    ofFill();
    
    // Draw center button
    drawButton(ofVec2f(pos.x + gridDimensions.x/2, pos.y + gridDimensions.y/2), 
               centerButtonRadius,
               ofColor(0,0,200)
               );    
    
    // Draw right border resize button 
    drawButton(rightBorderButton, resizeButtonRadius, ofColor(0,0,200));   
    
    // Draw bottom border resize button
    drawButton(bottomBorderButton, resizeButtonRadius, ofColor(0,0,200));       
}
// ----------------------------------------------------------------------------------



// ----------------------------------------------------------------------------------
void PerlinNoiseGrid::draw()
{             
    ofSetColor(25,25,25, 50);    
    ofFill();
    ofDrawRectangle( pos.x, pos.y, gridDimensions.x, gridDimensions.y );
    ofNoFill();
    

    drawControls();
    
    // Write the name of the grid on its top
    ofSetColor(0);
    verdana.drawString(name, pos.x, pos.y-20);     

    ofNoFill();
    //ofSetColor(0);
    ofSetLineWidth(1);

   
    // Draw borders
    ofSetColor(gridLinesColor);
    ofDrawRectangle(pos.x, pos.y, gridDimensions.x, gridDimensions.y);
    
    // Draw columns
    for(int i=0; i<xNcells-1; i++)        
    {
        ofDrawLine(pos.x + (i+1)*cellDimensions.x, 
                   pos.y, 
                   pos.x + (i+1)*cellDimensions.x, 
                   pos.y + gridDimensions.y 
                  );
    }
    
    // Draw lines
    for(int i=0; i<yNcells-1; i++)
    {
        ofDrawLine(pos.x, 
                   pos.y + (i+1)*cellDimensions.y, 
                   pos.x + gridDimensions.x, 
                   pos.y + (i+1)*cellDimensions.y 
                  );
    }
   
    
    
    // Draw the vectors inside each cell -------------------------------------------   
        //ofFill(); // if you are gonna draw the circles on the middle of the cell
        //ofSetColor(255,0,100);
        ofSetColor(255);

        //ofSetColor(0);
        for(int y=0; y<yNcells; y++)
        {
            for(int x=0; x<xNcells; x++)                
            {
                ofPushMatrix();       
                    ofTranslate(pos.x + x*cellDimensions.x + cellDimensions.x/2, 
                                pos.y + y*cellDimensions.y + cellDimensions.y/2);
                                
                    //ofRotateX(teta);         
                    ofDrawLine(0, 0, 
                               vectorField[x+y*xNcells].x*vectorLength,                                
                               vectorField[x+y*xNcells].y*vectorLength);                
                ofPopMatrix(); 
            }
        }    
    // -----------------------------------------------------------------------------    
    
    
    
    // Draw main direction of the field --------------------------------------------
        ofPath path;
        path.moveTo(pos.x+gridDimensions.x-100,
                    pos.y+gridDimensions.y-100);
    
    
        //path.lineTo(100,
        //            100);
    
        path.lineTo(pos.x+gridDimensions.x-100,
                    pos.y+gridDimensions.y-100);
    
    
    
        path.arc(pos.x+gridDimensions.x-100,   // x center
                 pos.y+gridDimensions.y-100,   // y center
                 50, 50,                      // radius x and y
                 angle-aperture/2,            // angle - start
                 angle+aperture/2);           // angle - finish
    
        path.lineTo(pos.x+gridDimensions.x-100,
                    pos.y+gridDimensions.y-100);    
    
        //path.setColor(ofColor(100,100,100,150));  
        path.setColor(ofColor(100,200,100,150));    

        path.draw();


    
        //Draw Line direction
        ofSetColor(0,255,0);
        ofSetLineWidth(2);  
        ofPushMatrix();    
            ofTranslate(pos.x+gridDimensions.x-100,
                        pos.y+gridDimensions.y-100);
    
            ofRotateZ(angle);
    
            ofDrawLine(0,0, 50, 0);
        ofPopMatrix(); 
        ofSetLineWidth(1);  
    // ----------------------------------------------------------------------------
    
    // Changes back to the same styling state
    ofFill();
}
// ----------------------------------------------------------------------------------



// ----------------------------------------------------------------------------------
void PerlinNoiseGrid::move(ofVec2f newCenter)
{
    // Reposition the center button
    centerButton = newCenter;
    
    // Reposition the top-left corner 
    pos.set(centerButton.x - gridDimensions.x/2,
            centerButton.y - gridDimensions.y/2);
 
    
    // Reposition the rightBorderButton
    rightBorderButton.set(pos.x + gridDimensions.x, 
                          pos.y + gridDimensions.y/2);    
    
    
    // Reposition the bottomBorderButton
    bottomBorderButton.set(pos.x + gridDimensions.x/2, 
                           pos.y + gridDimensions.y);    
}
// ----------------------------------------------------------------------------------




// ----------------------------------------------------------------------------------
void PerlinNoiseGrid::repositionControls()
{
    // Reposition the center button
    centerButton.x = pos.x + gridDimensions.x/2;  
    centerButton.y = pos.y + gridDimensions.y/2;  
    
    // Repositioning the rightBorderButton
    rightBorderButton.x = pos.x + gridDimensions.x;
    rightBorderButton.y = pos.y + gridDimensions.y/2;

    
    // Reposition the bottomBorderButton
    bottomBorderButton.x = pos.x + gridDimensions.x/2;
    bottomBorderButton.y = pos.y + gridDimensions.y;   

    // Resizing the cells
    //cellDimensions.x = gridDimensions.x/xNcells;   
    cellDimensions.x = gridDimensions.x/xNcells;   
    
    cellDimensions.y = gridDimensions.y/yNcells;    

    vectorLength  = ofVec2f(0,0).distance(cellDimensions);
    vectorLength /= 2.5;       
}
// ----------------------------------------------------------------------------------


// ----------------------------------------------------------------------------------
void PerlinNoiseGrid::rightResize(float newRight)
{
    // Resizing the grid dimension
    gridDimensions.x = newRight - pos.x;
    
    // All the commented code below is done at repositionControls();
    // Although it's more organized calling that method, it does useless
    //  calculations (for the Y axis, for example)
    // I decided to leave the code below just in case you want make the
    // process faster...    
/*   
    // Reposition the center button
    centerButton.x = pos.x + gridDimensions.x/2;  
    
    
    // Repositioning the rightBorderButton
    rightBorderButton.x = pos.x + gridDimensions.x;

    // Reposition the bottomBorderButton
    bottomBorderButton.x = pos.x + gridDimensions.x/2;
    
    
    // Resizing the cell's width
    cellDimensions.x = gridDimensions.x/xNcells;   
    
    vectorLength  = ofVec2f(0,0).distance(cellDimensions);
    vectorLength /= 2.5;      
*/
    
    repositionControls();    
}
// ----------------------------------------------------------------------------------



// ----------------------------------------------------------------------------------
void PerlinNoiseGrid::bottomResize(float newBottom)
{
    // Resizing the grid dimension
    gridDimensions.y = newBottom - pos.y;
    
    // All the commented code below is done at repositionControls();
    // Although it's more organized calling that method, it does useless
    //  calculations (for the X axis, for example).
    // I decided to leave the code below just in case you want make the
    // process faster...
/*
    // Reposition the center
    centerButton.y = pos.y + gridDimensions.y/2;  
    
    
    // Repositioning the rightBorderButton
    rightBorderButton.y = pos.y + gridDimensions.y/2;
    
    // Reposition the bottomBorderButton
    bottomBorderButton.y = pos.y + gridDimensions.y;   
    
    
    // Resizing the cell's width
    cellDimensions.y = gridDimensions.y/yNcells;    
    
    vectorLength  = ofVec2f(0,0).distance(cellDimensions);
    vectorLength /= 2.5;    
*/
    
    
    repositionControls();
}
// ----------------------------------------------------------------------------------






// ----------------------------------------------------------------------------------
bool PerlinNoiseGrid::isInsideGrid(ofVec2f p)
{
    if(p.x > pos.x   &&   p.x< pos.x + gridDimensions.x  // between the x borders
       &&
       p.y > pos.y   &&   p.y< pos.y + gridDimensions.y) // between the y borders
    {
        return true;
    }
    else
    {
        return false;
    }
}
// ----------------------------------------------------------------------------------

    
// true  -> the mouse is over the GUI or over the grid resize/move circles
// false -> the mouse is not over the GUI nor over the grid resize/move circles
// ----------------------------------------------------------------------------------            
bool PerlinNoiseGrid::isMouseOverGUI()
{
    ofVec2f mouse = ofVec2f(ofGetMouseX(), ofGetMouseY());
    
    int GUIborder = 5;
    
     
    return (
             // Check if the mouse is over the resize/move buttons
             (isGridVisible && 
              (
               (mouse.distance( centerButton )       < centerButtonRadius) || // mouse over the drag circle
               (mouse.distance( rightBorderButton )  < resizeButtonRadius) || // mouse over the right resize circle
               (mouse.distance( bottomBorderButton ) < resizeButtonRadius)    // mouse over the bottom resize circle
              )
             )
            ||
            
             // Check if the mouse is over the GUI. 
             (mouse.x > gui.getPosition().x - GUIborder && 
              mouse.x < gui.getPosition().x + gui.getWidth() + GUIborder &&
              mouse.y > gui.getPosition().y - GUIborder &&
              mouse.y < gui.getPosition().y + gui.getHeight() + GUIborder
             )
           );   
}
// ----------------------------------------------------------------------------------

            
PerlinNoiseGrid::~PerlinNoiseGrid(void) 
{
    //deleteGrid.removeListener(this, &PerlinNoiseGrid::suicide);
    //visibility.removeListener(this, &PerlinNoiseGrid::hideGrid);
    
   cout << "~The grid " << index << " was destroyed" << endl;
}
            
            
            
            
            
            
            
            
            
            


