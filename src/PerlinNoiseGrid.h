
#ifndef _PERLINNOISEGRID // if this class hasn't been defined, the program can define it
#define _PERLINNOISEGRID // by using this if statement you prevent the class to be called more than once which would confuse the compiler

#include "ofMain.h" // we need to include this to have a reference to the openFrameworks framework
#include "ofxGui.h"


class PerlinNoiseGrid
{
	public:    
        // top-left corner of the grid
        ofVec2f pos;
    
        // Width and Height of the grid
        ofVec2f gridDimensions;
            
        // Width and Height of the cell
        ofVec2f cellDimensions;
    
    
        // This is a workaround for avoiding the particles
        //  go crazy when we change the number of columns or lines
        // If it is True, we avoid applying the forces for just one
        //bool changedNcells;
    
    
        // Take a look at update() and you'll understand what this variable is...
        float zTime;
    
        // A vector of ofVec2f containing the normalized vectors of each cell.
        vector<ofVec2f>  vectorField;
    
        // The length of the vector when drawing inside the cell.
        float vectorLength;
    
        // The name of the grid and GuiPanel
        string name; 
    
        // The index of the grid on the vector 
        int index;
        
        // 
        ofColor gridLinesColor;    
        
    
        // On the GUI panel which will control the variables
        // -------------------------------------------------------------------------------------
            // The gui panel 
            ofxPanel        gui;    

            // Number of cells horizontally and vertically
            ofxIntSlider    xNcells;
            ofxIntSlider    yNcells;

            // x, y and z increment        
            ofxFloatSlider  xIncrement;    
            ofxFloatSlider  yIncrement;    
            ofxFloatSlider  zIncrement;   
    
            // The magnitude of each vector 
            ofxFloatSlider  force;        
    
            // Average direction
            ofxFloatSlider  angle;        

            // Aperture angle
            ofxFloatSlider  aperture;   
    
            // Button to destroy the Grid
            ofxToggle deleteGrid;   
            
            // Button to turn the Grid visible or invisible
            ofxToggle visibility;               
        // -------------------------------------------------------------------------------------
    
    
    
    
        // MOVING AND RESIZING THE GRID --------------------------------------------------------
            // Position of the buttons
            ofVec2f centerButton;        // center of the grid. Used to move the whole grid.
            ofVec2f rightBorderButton;   // right border of the grid. Used to resize the grid's width
            ofVec2f bottomBorderButton;  // bottom border of the grid. Used to resize the grid's height   
    
            // Radius of the buttons
            float centerButtonRadius;    // radius of the center button
            float resizeButtonRadius;    // radius of the resize circles
    
            bool isGUIVisible;           // true -> the GUI will be printed on the screen
            bool isGridVisible;          // true -> the grid will be printed on the screen
            bool isMoving;               // true -> the user is moving the grid. 
            bool isResizingWidth;        // true -> the user is resizing by the right border
            bool isResizingHeight;       // true -> the user is resizing by the bottom border

            bool isMousePressed;         // true -> mouse is being pressed right now
    
            ofTrueTypeFont	verdana;     // used to print the nColumns and nLines of the grid
        // -------------------------------------------------------------------------------------
            
        // CONSTRUCTOR
        PerlinNoiseGrid(ofVec2f _pos,             // top-left corner of the grid
                        ofVec2f _gridDimensions,  // (gridWidth, gridHeight)
                        int _xNcells,             // number of cells horizontally
                        int _yNcells,             // number of cells vertically
                        ofVec3f _increment,       // (xIncrement, yIncrement, zIncrement)
                        float _zTime,             // Where are we on the third dimension
                        string _name,             // name of the grid
                        int _index
                       );
    
        // DESTRUCTOR 
        ~PerlinNoiseGrid();
    
        // Tells ofApp.cpp that this PerlinNoiseGrid should be destroyed.
        void suicide(bool & b);
    
        // Hide grid event
        void hideGrid(bool & b);
        

    
        void update();  // YOU NEED TO CALL UPDATE INSIDE 'void ofApp::draw()' 
                        //  OTHERWISE YOU WON'T BE ABLE TO SEE THE GRID EVEN IF
                        //  isGridVisible == true

        void hide();                // Hide the grid and the menu
        void show();                // Show the grid and the menu
        void changeVisibility();    // If hidden -> show it
                                    // If is visible -> hide it
                                    
        // set the visibility of the GUI                       
        void setVisibility(bool v); // If v==true, set visibility to true
                                    // If v==false, set visibility to false

            
        void draw();
        void drawControls();        // draw center, right and bottom buttons
        void drawButton(ofVec2f p,  // position
                        float r,    // radius
                        ofColor c); // color
        void drawGUI();

    
        void move(ofVec2f newCenter);       // move the grid to the newCenter position
        void rightResize(float newRight);   // resize the grid's width
        void bottomResize(float newBottom); // resize the grid's height
    
        bool isMouseOverGUI(); // true -> the mouse is over the GUI or over the grid resize/move circles

    
        // GUI's listeners -------------------------------------------------------
        void nColumnsChanged(int & xNcells); // changed the number of columns
        void nLinesChanged(int & yNcells);   // changed the number of lines
        // -----------------------------------------------------------------------

    
        void repositionControls();
    
        void mouseEvents(); // checking pressed, dragged and released mouse events.   
    
        bool isInsideGrid(ofVec2f p); // true  -> point is inside the grid. 
                                      // false -> point isn't inside the grid
    
	private:
    
};
#endif
