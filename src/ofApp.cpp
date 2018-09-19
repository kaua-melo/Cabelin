#include "ofApp.h"
#include <iomanip> // Useful to save the files in this format: 000010.png

//--------------------------------------------------------------
void ofApp::setup()
{    
	// load the mouse icon image
	//mouseIcon.load("micon.png");

    ofSetFrameRate(60);  
    ofBackground(100,100,100,255);    
    
	canvas = new Canvas(ofGetWidth(), ofGetHeight());	
	//canvas->setNumberSamples(8); // Setting the Number of Samples on the fbo 
	canvas->setNumberSamples(0); // Setting the Number of Samples on the fbo 


    pGrids.push_back( new PerlinNoiseGrid( ofVec2f(300,250),    // Top-lef corner of the grid
                                 ofVec2f(400,300),              // (gridWidth, gridHeight)
                                 10,                            // number of cells horizontally
                                 10,                            // number of cells vertically
                                 ofVec3f(0.107,0.107, 0.007),   // (xIncrement, yIncrement, zIncrement)
                                 0,                             // Where are we on the third dimension
                                 "Grid 0",  					// Grid's name
                                 0 							// Grid's index
								)
                    );

	
    // You gotta update the grids once here to be able to access
    //  vectorField[] in the first update() call
    for(unsigned i=0; i<pGrids.size(); i++)
    {
		// We need to set these variables to false first and to true later 
		//  because the pGrids.update() function calls its draw function,
		//  which can't draw anything yet because the ofApp.draw() function 
		//  wasn't called yet. There's not much problem on that unless we are 
		//  using ofFbo. Therefore, let's set it to false here and then to true
		//  after the pGrids.update was called.
		pGrids[i]-> isGUIVisible = false;
        pGrids[i]-> isGridVisible = false;

        pGrids[i]->update();

		pGrids[i]-> isGUIVisible = true;
        pGrids[i]-> isGridVisible = true;		
    }
    
	
    lockOfHairs.push_back(new LockOfHair("Hair 0",             // name
                                         ofVec2f(200,200),     // position
                                         0,                    // index
										 *canvas
                                        ));
    
	
	focusOnLockOfHair(0);
    // Make the first LockOfHair Active
    lockOfHairs[0]->setLockOfHairStatus(true);
    lockOfHairInUse = 0;    
	
	pauseSimulation	= false;
    hideGUIS = false;    		
	saveFrames = false;	
	showInstructions = true;
	screenShotNumber = 0;
	firstFrame = true;	

	printColorBeingPicked = false;
}

//--------------------------------------------------------------
void ofApp::update()
{    	
	if(!pauseSimulation)
	{
		// Check if we need to destroy any PerlinNoiseGrid
		while (!pGridToBeDestroyed.empty())
		{
			destroyPgrid( pGridToBeDestroyed.back() );  
			// pop_back = Removes the last element in the vector, effectively reducing the container size by one.
			pGridToBeDestroyed.pop_back();
		}   

		// Check if we need to destroy any LockOfHair
		while (!lockOfHairToBeDestroyed.empty())
		{
			destroyLockOfHair( lockOfHairToBeDestroyed.back() );  
			// pop_back = Removes the last element in the vector, effectively reducing the container size by one.        
			lockOfHairToBeDestroyed.pop_back();
		}       


		// Let the hairs know whether the mouse is over some GUI or not 
		// ---------------------------------------------------------------------
		bool mouseOverGridsGUI = false;
		// Checking if the mouse is over any pGrids GUI
		for(unsigned i=0; i<pGrids.size(); i++)
		{
			if( pGrids[i]->isMouseOverGUI() )
			{
				 mouseOverGridsGUI = true;
				 break;
			}
		}

		bool mouseOverLockOfHairGUI = false;
		// Checking if the mouse is over any LockOfHair GUI
		for(unsigned i=0; i<lockOfHairs.size(); i++)
		{
			if(lockOfHairs[i]->isMouseOverGui())
			{
				mouseOverLockOfHairGUI = true;
				break;
			}
		}

		// Let the LockOfHairs know if the mouse is over any GUI or not
		for(unsigned i=0; i<lockOfHairs.size(); i++)
		{
			lockOfHairs[i]->isMouseOverOtherGuis = (mouseOverGridsGUI || mouseOverLockOfHairGUI);
		}
		// ---------------------------------------------------------------------    



		// APPLYING FORCES FROM PERLIN NOISE FIELD TO THE HAIRS -------------------------------------------------------------
		// ------------------------------------------------------------------------------------------------------------------
		// TODO: The forces should be applyed only if the hairs are visible (?)
		// Go through each LockOfHair on the screen.
		for(unsigned l=0; l<lockOfHairs.size(); l++)
		{
			// Go through each hair 
			for(unsigned int i=0; i<lockOfHairs[l]->hairs.size(); i++)
			{      
				// Go through each particle of the chain.
				// p starts from 1 because the first particle is fixed.
				for(unsigned p=1; p<lockOfHairs[l]->hairs[i].particles.size(); p++)
				{
					// Go through each grid
					for(unsigned j=0; j<pGrids.size(); j++)
					{
						// If the particle is inside the grid
						if(pGrids[j]->isInsideGrid(lockOfHairs[l]->hairs[i].particles[p].pos + canvas->position))
						{
							// x = The column number the particle is over
							int x = int( (lockOfHairs[l]->hairs[i].particles[p].pos.x + canvas->position.x - pGrids[j]->pos.x)/
										pGrids[j]->cellDimensions.x );

							// y = The line number the particle is over
							int y = int( (lockOfHairs[l]->hairs[i].particles[p].pos.y + canvas->position.y - pGrids[j]->pos.y)/
										pGrids[j]->cellDimensions.y );

							lockOfHairs[l]->hairs[i].particles[p].addForce(pGrids[j]->vectorField[x+y*pGrids[j]->xNcells]*pGrids[j]->force*0.1);
						}                               
					}
				}
			}    
		}
		// ------------------------------------------------------------------------------------------------------------------


		// update LockOfHairs
		for(unsigned i=0; i<lockOfHairs.size(); i++)
		{
			lockOfHairs[i]->update();
		}     		
	}
}

//--------------------------------------------------------------
void ofApp::draw()
{   
	// It's good to set the color to white before starting drawing in the fbo so you don't get weird results.
	ofSetColor(255,255,255); 
	canvas->fbo.begin();
   		ofClear(255, 255, 255); // , 255);	// clearing/Setting all pixels on the fbo to a color.     

		// If the user has dropped an image onto the window, we should draw it.
		if(bImg.isAllocated())
		{     
			bImg.draw(0,0);
		}
	canvas->fbo.end();


	// Before drawing the hairs, we should set the princtColorBeingPicked to false
	//  since in case the user is picking the color manually, the LockOfHair.cpp will
	//  set it to true and send the color to draw the square
	printColorBeingPicked = false;

	
	// Draw Hairs
	for(unsigned i=0; i<lockOfHairs.size(); i++)
	{
		lockOfHairs[i]->draw();
	}   

	canvas->draw();

		
	// Draw Lock of Hairs GUI (These are being drawn outisde the canvas' fbo we created, 
	//  so we don't save it on the images when start recording all frames)
	for(unsigned i=0; i<lockOfHairs.size(); i++)
	{
		lockOfHairs[i]->drawGUI();
	}   		
		
	if(!pauseSimulation)
	{
		// Update grids
		for(unsigned i=0; i<pGrids.size(); i++)
		{
			pGrids[i]->update();
		}    
	}
		
	if( showInstructions )
	{
		ofSetColor(25,25,25, 150);    
        ofFill();
        ofDrawRectangle(ofGetWidth()-200, 20, 195, 170);

		// Commands
		ofSetColor(255);
		ofDrawBitmapString("G -> ADD GRID", ofGetWidth()-180, 50);
		ofDrawBitmapString("A -> ADD HAIR", ofGetWidth()-180, 75);
		ofDrawBitmapString("H -> HIDE ALL", ofGetWidth()-180, 100);	
		ofDrawBitmapString("Q -> SWITCH LOCKOFHAIR", ofGetWidth()-180, 125);
		ofDrawBitmapString("S -> STOP SIMULATION", ofGetWidth()-180, 150);	
		ofDrawBitmapString("O -> SAVE ALL FRAMES", ofGetWidth()-180, 175);				
	}	

	// Draw square with color being picked by hovering mouse over the picture
	if( printColorBeingPicked )
	{
		ofSetColor( colorBeingPicked );
		ofFill();
        ofDrawRectangle(ofGetWidth()-150, ofGetHeight()-150, 100, 100);
	}



	
	// Save every frame
	if( saveFrames ) //&& bImg.isAllocated())
	{
		if(!firstFrame){
			// wait for the thread to finish saving the
			// previous frame and then unmap it
			saverThread.waitReady();
			pixelBufferBack.unmap();
		}
		firstFrame = false;

		// copy the fbo texture to a buffer
		//fbo.getTexture().copyTo(pixelBufferBack);
		canvas->fbo.getTexture().copyTo(pixelBufferBack);

		// bind and map the buffer as PIXEL_UNPACK so it can be
		// accessed from a different thread  from the cpu
		// and send the memory address to the saver thread
		pixelBufferFront.bind(GL_PIXEL_UNPACK_BUFFER);
		unsigned char * p = pixelBufferFront.map<unsigned char>(GL_READ_ONLY);
		saverThread.save(p);

		// swap the front and back buffer so we are always
		// copying the texture to one buffer and reading
		// back from another to avoid stalls
		swap(pixelBufferBack,pixelBufferFront);
	}	

	// To draw the mouse icon on the screen
	//mouseIcon.draw(mouseX, mouseY);	
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key)
{    
   //cout<< "KeyPressed:"  << key << endl;
    
   switch(key)
   {
	    // o - start saving all frames
	   	case 111:
		{   
			saveFrames = !saveFrames;

			// If we are going to save all frames, let's HIDE EVERYTHING
			if(saveFrames)
			{
				// Increasing the number of samples in the ofFbo so we get better image quality for the hairs (antialiasing)
				//canvas->setNumberSamples(canvas->fbo.maxSamples());
				//saverThread.setSavingStatus(true);

				
				// Hiding everything
				hidePNGRids();
				hideLockOfHairGrids();	
				showInstructions = false;
				saverThread.setFirstFrameNumber( ofGetFrameNum() );

				int w = canvas->width; 	
				int h = canvas->height; 

				saverThread.setDimentions(w,h);
				pixelBufferBack.allocate(w*h*4,GL_DYNAMIC_READ);
				pixelBufferFront.allocate(w*h*4,GL_DYNAMIC_READ);
				firstFrame = true;
			}
			// If we are going to stop saving all frames, let's SHOW EVERYTHING
			else
			{
				showPNGRids();
				showLockOfHairGrids();	
				showInstructions = true;
				
				//saverThread.setSavingStatus(false);
				//cout<< "Saving Status = false!" << endl;

				// Decreasing the number of samples in the ofFbo to zero. We get worse images result, but probably faster processing.
				//canvas->setNumberSamples(0);	// --> This is what breaks the program
				//cout<< "Number of Sample = 0 now" << endl;							
			}
			break;
		}
		
		/* TODO
		// P - ScreenShot
        case 112:
        { 	
			
        }*/
		
		// Q
        case 113:
        { 
			// Change focus to the next LockOfHair
			focusOnLockOfHair( (lockOfHairInUse+1) % lockOfHairs.size() );
			break;
        }
		   
		// S
        case 115:
        { 
			pauseSimulation = !pauseSimulation;
			break;
        }
		   
		// CTRL
        case 768:
        { 
			isCtrlPressed = true;
			break;
        }



		// Z
	   	case 122:
		{ 
			cout<< "Z was pressed! " << endl;

			if(isCtrlPressed)
			{
				if(undoHistory.size()>0)
				{
					bool a = true;
					lockOfHairs[ undoHistory.back() ]->undo(a);
					undoHistory.pop_back();
				}
			}
			break;
		}

		// CTRL + Z -> On the Mac, if I press CTRL + Z, I'll get a key=26
	   	case 26:
		{ 
			if(undoHistory.size()>0)
			{
				bool a = true;
				lockOfHairs[ undoHistory.back() ]->undo(a);
				undoHistory.pop_back();
			}
			break;
		}		
		   
        // A -> Add lock of hair
        case 97:
        {
            int x = ofRandom(100,  ofGetWidth() - 150);
            int y = ofRandom(200, ofGetHeight() - 150);
            
            int newLockOfHairIndex;
            
            if(lockOfHairs.size())
            {
                newLockOfHairIndex = lockOfHairs[lockOfHairs.size()-1]->index + 1;
            }
            else
            {
                newLockOfHairIndex = 0;
            }

            lockOfHairs.push_back(new LockOfHair("Hair " + std::to_string(newLockOfHairIndex), 
                                                 ofVec2f(x,y), 
                                                 newLockOfHairIndex,
												 //fbo
												 *canvas 
												 ));
            
            // The LockOfHair should know about the image in order to pick the color
            //  by hovering over the image.
            lockOfHairs[lockOfHairs.size()-1]->bImg = bImg;    
           
            // Make it Active
			focusOnLockOfHair(newLockOfHairIndex);
            
            break;
        }
        // H
        case 104:
        {
            hideGUIS = !hideGUIS;
			
			// HIDE GUIS
			if(hideGUIS)
			{
				hidePNGRids();
				hideLockOfHairGrids();
			}
			// SHOW GUIS
			else
			{
				showPNGRids();
				showLockOfHairGrids();
			}
            break;
        }
        
        // G - Add another Grid
        case 103:
        {
            // Get a vector containing the top-left position of all grids on the screen
            vector<ofVec2f> pos;
            for(unsigned i=0; i<pGrids.size(); i++)
            {           
                pos.push_back( pGrids[i]->pos );
            }              

            ofVec2f p;
            // Randomly pick the top-left position of the new grid, but
            //  make sure it's not the same of any other grid on the screen.
            do
            {
                int x = ofRandom(100,  ofGetWidth() - 150);
                int y = ofRandom(200, ofGetHeight() - 150);

                p = ofVec2f(x,y);
            }
            // Returns true if pos contain p
            while( (std::find(pos.begin(), pos.end(), p) != pos.end()) );

            
            int newGridIndex;
            if(pGrids.size())
            {
                newGridIndex = pGrids[pGrids.size()-1]->index + 1;
            }
            else
            {
                newGridIndex = 0;
            }

            pGrids.push_back(new PerlinNoiseGrid( p,            // Top-lef corner of the grid
                                 ofVec2f(400,300),              // (gridWidth, gridHeight)
                                 10,                            // number of cells horizontally
                                 10,                            // number of cells vertically
                                 ofVec3f(0.107,0.107, 0.007),   // (xIncrement, yIncrement, zIncrement)
                                 0,                             // Where are we on the third dimension
                                 //"Grid " + std::to_string(pGrids.size()),
                                 "Grid " + std::to_string(newGridIndex),   
                                 newGridIndex
                                )
                            );
            break;
        }

		// m
	   	case 109:
		{
			isMpressed = true;
			break;
		}		
    }
}

//--------------------------------------------------------------
void ofApp::saveScreen(ofImage& img, int x=0, int y=0, int w=ofGetWidth(), int h=ofGetHeight(), string fileName = "screenShot.png")
{
    /* It's possible to save the screen with a simple ofImage method ( grabScreen(x,y,w,h) ), however, it saves a black image
		for some specific window's width and height. I couldn't find how to solve that using ofImage.grabScreen(). The method below is how
		I could make any window's size possible to be saved.

     The ofImage object has two copies of an image. One in the RAM (ofPixels) and one in the GPU (ofTexture)
     The one on RAM is good to manipulate the pixels...
     The one on GPU is faster to render on screen...
     It seems to me that they are not necessarily connected all the time. For example, if you
      do img.getTexture().loadScreenData(300, 300, 200, 200); and try to save the img (img is an ofImage),
      a black image will be show. But if you grab the pixels from Texture first 

       ofPixels pixels;
       img.getTexture().readToPixels(pixels);

     and then you save it in the RAM version: img.setFromPixels(pixels);
     then you can save the image and it looks great :)
    */
    // Allocate memory to store the image on the RAM
    img.allocate(w, h, OF_IMAGE_COLOR);

    // Allocate memory to store the image on the GPU
    img.getTexture().allocate(w, h, GL_RGB); //GL_RGBA

    // Load the screen pixels into the ofTexture
    img.getTexture().loadScreenData(x, y, w, h);

    // These 'pixels' will receive the pixels from the ofTexture
    ofPixels pixels;

	pixels.allocate(w, h, OF_IMAGE_COLOR_ALPHA);

    // Passing the pixels from ofTexture to the 'pixels' object we just created
    img.getTexture().readToPixels(pixels);

    // Setting the image on the RAM with the pixels from its ofTexture
    img.setFromPixels(pixels);

	// mirror (vertical, horizontal)
	img.mirror(true, false);	

    // Save the image
    img.save(fileName);		
}



//--------------------------------------------------------------
void ofApp::hidePNGRids()
{
 	for(unsigned i=0; i<pGrids.size(); i++)
	{
		pGrids[i]->setVisibility(false);                  
	}    	
}


//--------------------------------------------------------------
void ofApp::showPNGRids()
{
 	for(unsigned i=0; i<pGrids.size(); i++)
	{
		pGrids[i]->setVisibility(true);                  
	}    	
}


//--------------------------------------------------------------
void ofApp::hideLockOfHairGrids()
{
	// Hide or Show all lockOfHairs GUIS            
	for(unsigned i=0; i<lockOfHairs.size(); i++)
	{
		lockOfHairs[i]->setVisibility(false);                  
	}  		
}


//--------------------------------------------------------------
void ofApp::showLockOfHairGrids()
{
	// Hide or Show all lockOfHairs GUIS            
	for(unsigned i=0; i<lockOfHairs.size(); i++)
	{
		lockOfHairs[i]->setVisibility(true);                  
	}  		
}

	
//--------------------------------------------------------------
void ofApp::keyReleased(int key){

   switch(key)
   {
	    //cout<< "Released: " << key << endl;

	   	// ctrl - used to detect ctrl + z
		case 768:
		{
			isCtrlPressed = false;	   
			break;
		}
		
		// m - used to detect when the user wants to move the background image (fbo) on the screen.
	   	case 109:
		{
			isMpressed = false;
		}
   }
}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y)
{

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button)
{
	// Moving Canvas
	if(isMpressed)
	{
		canvas->setPosition(ofVec2f(canvas->position.x + mouseX-mousePressedPosition.x, 
									canvas->position.y + mouseY-mousePressedPosition.y));
		mousePressedPosition.set(x,y);
	}
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button)
{
	isMousePressed = true;
	mousePressedPosition.set(x,y);


    // Checking if the mouse is over any LockOfHair GUI.		
	// Go through each LockOfHair
    for(unsigned i=0; i<lockOfHairs.size(); i++)
    {
        // If the user clicked over its GUI
        if(lockOfHairs[i]->isMouseOverGui())
        {			
			//cout<<"Clicked over: "<< lockOfHairs[i]->index <<endl;	
			
            //  If the lockOfHair currently in use is not the one that was clicked,
            //   it means the user wants to use this LockOfHair now.
            if(lockOfHairs[i]->index != lockOfHairInUse)
            {   
				//cout<<"Changing focus to: "<< lockOfHairs[i]->index <<endl;	
				
				focusOnLockOfHair(lockOfHairs[i]->index);              
            }
        }
    }
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button)
{
	isMousePressed = false;
}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){
	//cout<<"resized Window to: "<<w<<" x "<<h<<endl;

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo)
{ 
    // dragInfo.files is a list of the absolute file paths that were drag-and-dropped onto the window.
    // We are getting the last file dropped onto the window
    bImg.load( dragInfo.files[dragInfo.files.size()-1] );  
	
	// Allocating space for saving the frames when requested by the user.
	canvas->allocate( bImg.getWidth(), bImg.getHeight() );
	
    // The LockOfHair should know about the image in order to pick the color
    //  by hovering over the image.
    for(unsigned i=0; i<lockOfHairs.size(); i++)
    {
       lockOfHairs[i]->bImg = bImg;
    }       
}


//--------------------------------------------------------------
void ofApp::destroyPgrid(int index)
{    
    // Call the object's destructor
    delete pGrids[index];
    
    // Remove the object (actually the pointer 
    //  to the object) from the vector
    pGrids.erase(pGrids.begin() + index);  
        
    
    // Reset the indexes of the grids otherwise you gonna get an error
    //  when trying to delete some grid that doesn't exists or you will end up
    //  deleting other grid.
    // Ex: grid content = 0 1 2 3 4 5 6
    // -----------------------
    //     grid indexes = 0 1 2 3 4 5 6
    //
    // remove index 2
    //
    // new grid content = 0 1 3 4 5 6
    // -----------------------
    //     grid indexes = 0 1 2 3 4 5
    //
    // Therefore if you try to remove index 3, you'll actually delete the grid 4

    for(unsigned i=0; i<pGrids.size(); i++)
    {
        pGrids[i]->index = i;
    }
}

// If we remove a LockOfHair, we should also remove any record it might have on undoHistory.
// That's what this function does.
//--------------------------------------------------------------
void ofApp::updateUndoHistory(int indexRemoved)
{
	/*
	Suppose the following case, the undoHistory is 00112200
	If we delete the LcokOfHair which has the index = 1, our new 
	  undoHistory should now be 001100.
	The code below makes sure we make that change.
	*/
	
	// Will temporarily hold the new undo history. 
	// It's just more handy to use another vector. You'll understand why
	//  when you look at the code below.
	vector<int> newUndoHistory;	

	for(unsigned i=0; i<undoHistory.size(); i++)
	{
		// If we remove a LockOfHair of index 3, all the next LockOfHairs will have their
		//  indexes reduced by one. Therefore, we need to make sure we still have the right
		//  reference to them.
		// If the undoHistory item is 3 and we deleted LockOfHair 2. The Lockof Hair 3 is now
		//  LockOfHair 2 and therefore the undoHistory item should now be 2 as well.
		if(undoHistory[i] > indexRemoved)
		{
			newUndoHistory.push_back(undoHistory[i]-1);
		}
		// If the undoHistory item is not the LockOfHair we have deleted, we should keep it
		//  on the list. Otherwise, we should remove it (ignore it).
		else if(undoHistory[i] != indexRemoved)
		{
			newUndoHistory.push_back(undoHistory[i]);		
		}
	}		

	undoHistory.clear();
	undoHistory = newUndoHistory;	
}
//--------------------------------------------------------------



//--------------------------------------------------------------
void ofApp::destroyLockOfHair(int index)
{    
    // Call the object's destructor
    delete lockOfHairs[index];
    
    // Remove the object (actually the pointer 
    //  to the object) from the vector
    lockOfHairs.erase(lockOfHairs.begin() + index);  
            
    // Reset the indexes of the grids otherwise you gonna get an error
    //  when trying to delete some grid that doesn't exist or you could also end up
    //  deleting other grid.
	//
    // Ex: lockOfHairs content = 0 1 2 3 4 5 6
    // -----------------------
    //     lockOfHairs indexes = 0 1 2 3 4 5 6
    //
    // remove index 2
    //
    // new lockOfHairs content = 0 1 3 4 5 6
    // -----------------------
    //     lockOfHairs indexes = 0 1 2 3 4 5
    //
    // Therefore if you try to remove index 3, you'll actually delete the lockOfHairs 4
	// The process below then reset the index values of each grid to make sure we have the
	//  following order again:
	//
    // formated lockOfHairs content = 0 1 2 3 4 5
    // -----------------------
    //          lockOfHairs indexes = 0 1 2 3 4 5
	// :
	/*
    for(unsigned i=0; i<lockOfHairs.size(); i++)
    {
        lockOfHairs[i]->index = i;
		lockOfHairs[i]->name = "Hair " + to_string(i);
		lockOfHairs[i]->gui.setName(lockOfHairs[i]->name );   
    }*/
    for(unsigned i=index; i<lockOfHairs.size(); i++)
    {
        lockOfHairs[i]->index = i ;
		lockOfHairs[i]->name = "Hair " + to_string(i);
		lockOfHairs[i]->gui.setName(lockOfHairs[i]->name );   
    }	
	
	// Since we are removing a LockOfHair, we should also remove any record it might
	//  have on undoHistory.
	updateUndoHistory(index);
	
	// 
	focusOnLockOfHair(0);
}


//--------------------------------------------------------------
void ofApp::focusOnLockOfHair(int index)
{
	// if there's some LockOfHair in use, let's unfocus it
	if(lockOfHairInUse != -1)
	{
		lockOfHairs[lockOfHairInUse]->setLockOfHairStatus(false);
	}
	
	// Let's focus the new LockOfHair
	lockOfHairInUse = index;
	lockOfHairs[lockOfHairInUse]->setLockOfHairStatus(true);	
}









