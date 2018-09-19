
#ifndef _LOCKOFHAIR // if this class hasn't been defined, the program can define it
#define _LOCKOFHAIR // by using this if statement you prevent the class to be called more than once which would confuse the compiler

#include "ofMain.h" // we need to include this to have a reference to the openFrameworks framework
#include "ofxGui.h"
#include "Canvas.h"
#include "ParticleChain2.h"

class LockOfHair
{
	public:
        // The actual number of hairs
        int nHairs;
        
        // The hairs
        vector<ParticleChain2> hairs;
    
        // This vector contains the number of hairs added 
        // Therefore it will contain a history of how the hair was added
        //  so that we can undo (ctrl+z) those actions.
        vector<int> numberOfHairsAdded;
    
        // This will store the background image information so we can pick
        //  the color of the hairs by hovering over the image instead of just being able to
        //  set the color on the GUI Panel.
        ofImage bImg;   

        // The name of the lock of hair on the GUI panel 
        string name; 
    
        // The index of the lockOfHair on the vector 
        int index;
    
        
        // On the GUI panel which will control the variables
        // --------------------------------------------------------------------- 
            // The gui panel 
            ofxPanel gui;   
                
            ofxToggle addHair;    

            ofParameterGroup beforeAddHair; 
                ofParameter<float> length;            // length of the chain
                ofParameter<float> lengthRandomness;  // goes from 0 to 1. The length of the hair will 
                                                      //  be lengthRandomness % greater or shorter than length
                ofParameter<int> np;                  // number of particles on the chain   
    
    
            ofParameterGroup afterAddHair;
                ofParameter<int>   thickness;        // thickness of the chain
                ofParameter<float> friction;         // friction with the medium. 1 = no friction / 0 = maximum friction
                ofParameter<float> speedLimit;       // speedLimit
                ofParameter<float> stiffness;        // stiffness of the connection (k from F=kx)
                ofParameter<float> damping;          // damping coefficient; 0=no damping / 1= maximum damping       
                ofParameter<ofColor> color;          // color of the chain
                ofParameter<float> colorRandomness;  // goes from 0 to 1. The color of each hair will have a different brightness
                                                     //  based on this variable. The greater the colorRandomness, the greater is the color difference.
                
    
            
            ofxToggle      pickColor;    // true -> Pick color by horvering the mouse over the img
    
            // Ways of adding hair
            ofxToggle      addOne;    // true -> add one hair each time
            ofxToggle      addSquare; // true -> add a square of hair
            ofxToggle      addCircle; // true -> add a circle of hair
    
            ofParameterGroup squareGroup;
                ofParameter<ofVec2f> squareDimensions;
                ofParameter<int>     squareHairDensity;    

            ofParameterGroup circleGroup;
                ofParameter<float>   circleRadius;
                ofParameter<int>     circleHairDensity;
    
    
            ofParameterGroup editing;
                ofParameter<bool>    removeLastHair;
                ofParameter<bool>    removeAllHair;
                // Button to destroy the Grid
                ofxToggle deleteLockOfHair;   

                // Button to turn the Hairs visible or invisible
                ofxToggle visibility;  
    
                ofxLabel             numberOfHairs;  // used to write the number of hair added
                
               
    
/*    
            ofxFloatSlider length;         // length of the chain
            ofxIntSlider   np;             // number of particles on the chain  
            
            ofxIntSlider   thickness;      // thickness of the chain
            ofxFloatSlider friction;       // friction with the medium. 1 = no friction / 0 = maximum friction
            ofxFloatSlider speedLimit;     // speedLimit
            ofxFloatSlider stiffness;      // stiffness of the connection (k from F=kx)
            ofxFloatSlider damping;        // damping coefficient; 0=no damping / 1= maximum damping       
            ofxColorSlider color;          // color of the chain
            ofxLabel       numberOfHairs;  // used to write the number of hair added
    	    ofxButton      removeLastHair; // true -> remove last hair added
            ofxButton      removeAll;      // true -> remove all hair
*/
        // ---------------------------------------------------------------------    
        
        // The color of the header when the LockOfHair is the one being used
        ofColor activeLockOfHairColor;
        
        
        bool isLockOfHairActive;       // true -> the user is using this LockOfHair
        
        bool isMousePressed;
    
        bool isMouseOverOtherGuis;
        
        bool isGUIVisible;           // true -> the GUI will be printed on the screen
        
    
        vector<ofVec2f> previewPosition;  
        vector<float> previewHairLength;  
            
            
        Canvas *canvas;
    
        // Constructor
        LockOfHair(string _name, ofVec2f pos, int _index, Canvas &_canvas);
        
        // DESTRUCTOR 
        ~LockOfHair();        
    
        // Tells ofApp.cpp that this LockOfHair should be destroyed.
        void suicide(bool & b);
        
        void update();
        void draw();
        void drawGUI();
    
        void mouseEvents();     // checking pressed, dragged and released mouse events.   
        bool isMouseOverGui();  // true-> mouse is over the Gui. 
                                // false-> mouse isn't over the gui

                                
        bool areHairsVisible;
    
        bool isMouseOverImage();

    
        // GUI panel events
        void changedAddOne(bool & b);     // 
        void changedAddSquare(bool & b);  // 
        void changedAddCircle(bool & b);  // 
        
    
        void undo(bool & b);  // remove last hair
        void clean(bool & b); // remove all hairs
            
        
        void drawHairPreview();
    
        // GUI panel events
        void changedAddHair(bool & b);
        void changedThickness(int & t);
        void changedFriction(float & f);
        void changedSpeedLimit(float & sl);
        void changedStiffness(float & st);
        void changedDamping(float & dp);
        void changedColor(ofColor & c);
        void changedColorRandomness(float & r);


        void pickColorByHovering();
    
        void changedSquareDimensions(ofVec2f & d);
        void changedCircleDimensions(float & r);

        void changedHairDensity(int & hd);
            
        void calculateHairPreviewPositionAndLength(string addingMode);
        
        void changedHairLengthOrRandomness(float & l);
        
        // Set the status (active/inactive) 
        void setLockOfHairStatus(bool a);
        
        void hideHairs(bool & b);
        
        // Set the visibility of the GUI
        void setVisibility(bool v); // If v==true, set visibility to true
                                    // If v==false, set visibility to false        
    
	private:
};
#endif
