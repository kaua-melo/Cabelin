
#include "ofApp.h"
#include <iomanip> // Useful to save the files in this format: 000010.png
#include <map>
#include <stdio.h> // In order to use 'remove' (you use this to delete a file)

//--------------------------------------------------------------
void ofApp::setup()
{
	// load the mouse icon image
	//mouseIcon.load("micon.png");

	ofSetFrameRate(60);
	ofBackground(100, 100, 100, 255);

	canvas = new Canvas(ofGetWidth(), ofGetHeight());
	//canvas->setNumberSamples(8); // Setting the Number of Samples on the fbo
	canvas->setNumberSamples(0); // Setting the Number of Samples on the fbo

	bTracker = new BlobTracker(1000, true, false);
	//lobTracker(int mxD, bool calculateVel, bool d); -> (maxDistance, calculateVel?, debug?)

	pGrids.push_back(new PerlinNoiseGrid(ofVec2f(300, 250),			   // Top-lef corner of the grid
										 ofVec2f(400, 300),			   // (gridWidth, gridHeight)
										 10,						   // number of cells horizontally
										 10,						   // number of cells vertically
										 ofVec3f(0.107, 0.107, 0.007), // (xIncrement, yIncrement, zIncrement)
										 0,							   // Where are we on the third dimension
										 "Grid 0",					   // Grid's name
										 0							   // Grid's index
										 ));

	// You gotta update the grids once here to be able to access
	//  vectorField[] in the first update() call
	for (unsigned i = 0; i < pGrids.size(); i++)
	{
		// We need to set these variables to false first and to true later
		//  because the pGrids.update() function calls its draw function,
		//  which can't draw anything yet because the ofApp.draw() function
		//  wasn't called yet. There's not much problem on that unless we are
		//  using ofFbo. Therefore, let's set it to false here and then to true
		//  after the pGrids.update was called.
		pGrids[i]->isGUIVisible = false;
		pGrids[i]->isGridVisible = false;

		pGrids[i]->update();

		pGrids[i]->isGUIVisible = true;
		pGrids[i]->isGridVisible = true;
	}

	lockOfHairs.push_back(new LockOfHair("Hair 0",			// name
										 ofVec2f(200, 200), // position
										 0,					// index
										 *canvas));

	focusOnLockOfHair(0);
	// Make the first LockOfHair Active
	//lockOfHairs[0]->setLockOfHairStatus(true);
	lockOfHairInUse = 0;

	pauseSimulation = false;
	hideGUIS = false;
	saveFrames = false;
	showInstructions = true;
	screenShotNumber = 0;
	firstFrame = true;

	printColorBeingPicked = false;

	// ---------------------------------------------------------------
	// The position of the camera/video
	video_pos.set(20, 120);

	// The dimensions of the camera images to be displayed.
	// width = 640;  //320; //640; // 640;
	// height = 480; //240; //360; // 360;

	video_dimension.set(640, 480); // (640, 480), (320,240),
	camera_resizing_button_radius = 15;
	is_resizing_camera_height = false;
	is_resizing_camera_width = false;

// Sets the verbosity - this can be useful for debugging the video grabber interface. you can set the verbosity and then try initGrabber();
#ifdef _USE_LIVE_VIDEO
	camera.setVerbose(true);

	// There may be cases where it helps to not use a texture in order to save memory or for better performance.
	camera.setUseTexture(false);

	// The default webcam dimensions are: 640x480.
	// If you set it up for something smaller than 640x480, it will be automatically set to 640x480.
	// You can set it to 1280x960 though.
	camera.setup(640, 480);
	//camera.setup(1280, 960);

	// Check what are the camera dimensions:
	//cout<< "WIDTH: "  << camera.getWidth()  << endl;
	//cout<< "HEIGHT: " << camera.getHeight() << endl;

	colorImg.allocate(camera.getWidth(), camera.getHeight());

	//cout<< "colorImg.allocate: " << colorImg.getWidth() << " " << colorImg.getHeight() << endl;

	// Keeping the  aspect ratio
	//width = 640; //width  = 320;
	//height = width/( camera.getWidth() / camera.getHeight() );

#else
	video.load("slidingOver.mp4");
	//video.load("bodyTest.mp4");
	//video.load("velocity.mp4");
	//video.load("fingers.mov");
	//video.load("movingShapes.mp4");

	video.play();
	video.setLoopState(OF_LOOP_NORMAL);

	colorImg.allocate(video.getWidth(), video.getHeight());
	cout << "colorImg.allocate: " << colorImg.getWidth() << " " << colorImg.getHeight() << endl;

	// Keeping the video aspect ratio
	//width = 640; //width  = 320;
	height = width / (video.getWidth() / video.getHeight());

#endif

	threshold = 76;
	saveBackground = true;

	draw_video = true;
	draw_blobs = true;
	// ---------------------------------------------------------------
}

//--------------------------------------------------------------
void ofApp::update()
{
	//  Updating video --------------------------------------
	bool newFrame = false;

#ifdef _USE_LIVE_VIDEO
	camera.update();
	newFrame = camera.isFrameNew(); // if newFrame == false  ->  we don't have a new frame.
#else
	video.update();
	newFrame = video.isFrameNew();
#endif

	if (newFrame)
	{
// Getting the current frame of the video.
#ifdef _USE_LIVE_VIDEO
		colorImg.setFromPixels(camera.getPixels());
#else
		colorImg.setFromPixels(video.getPixels());
#endif

		// The line below outputs the following message on the console: "[notice ] ofxCvColorImage: setFromPixels(): reallocating to match dimensions: 320 240"
		// Not sure how to remove it :/  Someone had the same issue in the forum: https://forum.openframeworks.cc/t/ofxcvcolorimage-resize-reallocation/16870
		// *An idea: Try using ofxCvIamge instead of ofxCvColorImage
		// colorImg.resize(width, height);
		colorImg.resize(video_dimension.x, video_dimension.y);

		// In case we are using the webcam, we should flip the image in order to have like a mirror.
		colorImg.mirror(false, true);

		// grayImg gets the gray scale of the camera's frame.
		grayImg = colorImg;

		// If we press ' ', we save the current background that will be used to compare with the current camera frame.
		if (saveBackground == true)
		{
			cout << "SAVING BACKGROUND !" << endl;
			grayBackground = grayImg;
			saveBackground = false;
		}

		// Gets the difference between the 2 images.
		grayDiff.absDiff(grayBackground, grayImg);

		// Sets the contrast of the image.
		grayDiff.threshold(threshold);

		// Find the blobs in the grayDiff image
		bTracker->findBlobs(grayDiff, 10, video_dimension.x * video_dimension.y, 5, false);

		// (ofxCvGrayscaleImage &input,
		//  int minArea,
		//  int maxArea,
		//  int nConsidered, -> maximum number of blobs to be considered
		//  bool bFindHoles
	}
	// ------------------------------------------------------

	if (!pauseSimulation)
	{
		// Check if we need to destroy any PerlinNoiseGrid
		while (!pGridToBeDestroyed.empty())
		{
			destroyPgrid(pGridToBeDestroyed.back());
			// pop_back = Removes the last element in the vector, effectively reducing the container size by one.
			pGridToBeDestroyed.pop_back();
		}

		// Check if we need to destroy any LockOfHair
		while (!lockOfHairToBeDestroyed.empty())
		{
			destroyLockOfHair(lockOfHairToBeDestroyed.back());
			// pop_back = Removes the last element in the vector, effectively reducing the container size by one.
			lockOfHairToBeDestroyed.pop_back();
		}

		// Let the hairs know whether the mouse is over some GUI or not
		// ---------------------------------------------------------------------
		bool mouseOverGridsGUI = false;
		// Checking if the mouse is over any pGrids GUI
		for (unsigned i = 0; i < pGrids.size(); i++)
		{
			if (pGrids[i]->isMouseOverGUI())
			{
				mouseOverGridsGUI = true;
				break;
			}
		}

		bool mouseOverLockOfHairGUI = false;
		// Checking if the mouse is over any LockOfHair GUI
		for (unsigned i = 0; i < lockOfHairs.size(); i++)
		{
			if (lockOfHairs[i]->isMouseOverGui())
			{
				mouseOverLockOfHairGUI = true;
				break;
			}
		}

		// Let the LockOfHairs know if the mouse is over any GUI or not
		for (unsigned i = 0; i < lockOfHairs.size(); i++)
		{
			lockOfHairs[i]->isMouseOverOtherGuis = (mouseOverGridsGUI || mouseOverLockOfHairGUI);
		}
		// ---------------------------------------------------------------------

		// APPLYING FORCES FROM PERLIN NOISE FIELD TO THE HAIRS -------------------------------------------------------------
		// ------------------------------------------------------------------------------------------------------------------
		// TODO: The forces should be applyed only if the hairs are visible (?)
		// Go through each LockOfHair on the screen.
		for (unsigned l = 0; l < lockOfHairs.size(); l++)
		{
			// Go through each hair
			for (unsigned int i = 0; i < lockOfHairs[l]->hairs.size(); i++)
			{
				// Go through each particle of the chain.
				// p starts from 1 because the first particle is fixed.
				for (unsigned p = 1; p < lockOfHairs[l]->hairs[i].particles.size(); p++)
				{
					// Go through each grid
					for (unsigned j = 0; j < pGrids.size(); j++)
					{
						// If the particle is inside the grid
						if (pGrids[j]->isInsideGrid(lockOfHairs[l]->hairs[i].particles[p].pos + canvas->position))
						{
							// x = The column number the particle is over
							int x = int((lockOfHairs[l]->hairs[i].particles[p].pos.x + canvas->position.x - pGrids[j]->pos.x) /
										pGrids[j]->cellDimensions.x);

							// y = The line number the particle is over
							int y = int((lockOfHairs[l]->hairs[i].particles[p].pos.y + canvas->position.y - pGrids[j]->pos.y) /
										pGrids[j]->cellDimensions.y);

							lockOfHairs[l]->hairs[i].particles[p].addForce(pGrids[j]->vectorField[x + y * pGrids[j]->xNcells] * pGrids[j]->force * 0.1);
						}
					}

					// Go through each blob
					map<int, Blob>::iterator it;
					for (it = bTracker->cBlobs_dict.begin(); it != bTracker->cBlobs_dict.end(); it++)
					{
						// it->first   Blob ID
						// it->second  Blob
						ofVec2f particle = lockOfHairs[l]->hairs[i].particles[p].pos + canvas->position;

						// If the particle is inside the blob
						if (particle.x > video_pos.x + it->second.blob.boundingRect.getLeft() + canvas->position.x &&
							particle.x < video_pos.x + it->second.blob.boundingRect.getRight() + canvas->position.x &&

							particle.y > video_pos.y + it->second.blob.boundingRect.getTop() + canvas->position.y &&
							particle.y < video_pos.y + it->second.blob.boundingRect.getBottom() + canvas->position.y)
						{
							//cout << " ! INSIDE! " << endl;
							// Apply force to the particle HERE YOU SET HOW STRONG THE FORCE SHOULD BE!
							lockOfHairs[l]->hairs[i].particles[p].addForce(it->second.vel * 0.015);

							//cout << "	-vel: " << it->second.vel << endl;
						}
					}
				}
			}
		}
		// ------------------------------------------------------------------------------------------------------------------

		// update LockOfHairs
		for (unsigned i = 0; i < lockOfHairs.size(); i++)
		{
			lockOfHairs[i]->update();
		}
	}

	// Resizing the camera/video in case necessary.
	if (is_resizing_camera_height)
	{
		video_dimension.y = ofGetMouseY() - video_pos.y - canvas->position.y;
		grayImg.resize(video_dimension.x, video_dimension.y);
		grayBackground.resize(video_dimension.x, video_dimension.y);
		grayDiff.resize(video_dimension.x, video_dimension.y);
	}
	if (is_resizing_camera_width)
	{
		video_dimension.x = ofGetMouseX() - video_pos.x - canvas->position.x;
		grayImg.resize(video_dimension.x, video_dimension.y);
		grayBackground.resize(video_dimension.x, video_dimension.y);
		grayDiff.resize(video_dimension.x, video_dimension.y);
	}
}

//--------------------------------------------------------------
void ofApp::draw()
{
	// It's good to set the color to white before starting drawing in the fbo so you don't get weird results.
	ofSetColor(255, 255, 255);

	canvas->fbo.begin();
	ofClear(255, 255, 255); // , 255);	// clearing/Setting all pixels on the fbo to a color.

	// If the user has dropped an image onto the window, we should draw it.
	if (bImg.isAllocated())
	{
		bImg.draw(0, 0);
	}

	// Drawing the video ----------------------------------
	/*
		#ifdef _USE_LIVE_VIDEO
			//colorImg.resize(width, height);
		#else
			//colorImg.resize(427, 240);
			//colorImg.resize(width, height);
			//colorImg.resize(1000, height);
		#endif          
		*/

	// Drawing all variations of the video/webcam (color, gray, grayDiff)
	if (draw_video)
	{
		ofSetColor(255, 255, 255, 150);
		colorImg.draw(video_pos.x, video_pos.y);
		ofSetColor(255, 255, 255, 255);

		// Drawing the video's resizing controllers
		ofSetColor(180);
		// Bottom Button to resize video/camera
		ofDrawCircle(video_pos.x + video_dimension.x / 2,
					 video_pos.y + video_dimension.y,
					 camera_resizing_button_radius);
		// Right Button to resize video/camera
		ofDrawCircle(video_pos.x + video_dimension.x,
					 video_pos.y + video_dimension.y / 2,
					 camera_resizing_button_radius);
	}
	//grayImg.draw(360,20);
	//grayBackground.draw(20,280);
	//grayDiff.draw(360,280);

	// Draw blobs
	if (draw_blobs)
	{
		bTracker->drawContainers(video_pos.x, video_pos.y); //20, 20 + 360 + 218); // 360,20);
		bTracker->drawVelocities(video_pos.x, video_pos.y); //20, 20 + 360 + 218); // 360,20);
	}
	// ----------------------------------------------------

	canvas->fbo.end();

	// Before drawing the hairs, we should set the princtColorBeingPicked to false
	//  since in case the user is picking the color manually, the LockOfHair.cpp will
	//  set it to true and send the color to draw the square
	printColorBeingPicked = false;

	// Draw Hairs
	for (unsigned i = 0; i < lockOfHairs.size(); i++)
	{
		lockOfHairs[i]->draw();
	}

	canvas->draw();

	// Draw Lock of Hairs GUI (These are being drawn outisde the canvas' fbo we created,
	//  so we don't save it on the images when start recording all frames)
	for (unsigned i = 0; i < lockOfHairs.size(); i++)
	{
		lockOfHairs[i]->drawGUI();
	}

	if (!pauseSimulation)
	{
		// Update grids
		for (unsigned i = 0; i < pGrids.size(); i++)
		{
			pGrids[i]->update();
		}
	}

	// Printing "Instructions"
	if (showInstructions)
	{
		ofSetColor(25, 25, 25, 150);
		ofFill();
		ofDrawRectangle(ofGetWidth() - 250, 20, 230, 275);

		// Commands
		ofSetColor(255);
		ofDrawBitmapString("G -> ADD GRID", ofGetWidth() - 235, 50);
		ofDrawBitmapString("A -> ADD HAIR", ofGetWidth() - 235, 75);
		ofDrawBitmapString("H -> HIDE ALL", ofGetWidth() - 235, 100);
		ofDrawBitmapString("Q -> SWITCH LOCKOFHAIR", ofGetWidth() - 235, 125);
		ofDrawBitmapString("S -> STOP SIMULATION", ofGetWidth() - 235, 150);
		ofDrawBitmapString("O -> SAVE ALL FRAMES", ofGetWidth() - 235, 175);
		ofDrawBitmapString("C -> SHOW/HIDE CAMERA/VIDEO", ofGetWidth() - 235, 200);
		ofDrawBitmapString("B -> SHOW/HIDE BLOBS", ofGetWidth() - 235, 225);
		ofDrawBitmapString("M + mouse -> Move canvas", ofGetWidth() - 235, 250);
		ofDrawBitmapString("Y -> Save the project", ofGetWidth() - 235, 275);
	}

	// Draw square with color being picked by hovering mouse over the picture
	if (printColorBeingPicked)
	{
		ofSetColor(colorBeingPicked);
		ofFill();
		ofDrawRectangle(ofGetWidth() - 150, ofGetHeight() - 150, 100, 100);
	}

	// Save every frame
	if (saveFrames) //&& bImg.isAllocated())
	{
		if (!firstFrame)
		{
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
		unsigned char *p = pixelBufferFront.map<unsigned char>(GL_READ_ONLY);
		saverThread.save(p);

		// swap the front and back buffer so we are always
		// copying the texture to one buffer and reading
		// back from another to avoid stalls
		swap(pixelBufferBack, pixelBufferFront);
	}

	// To draw the mouse icon on the screen
	//mouseIcon.draw(mouseX, mouseY);

	// Report for the video threshold:
	//ofSetHexColor(0xffffff);
	ofSetColor(0);
	stringstream reportStr;
	reportStr << "press ' ' to save Background" << endl
			  << "threshold " << threshold << " (press: arrow up/down)" << endl
			  << "num blobs found " << bTracker->cBlobs.size() << ", fps: " << ofGetFrameRate();
	ofDrawBitmapString(reportStr.str(), 20, 700);
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key)
{
	// cout << "KeyPressed:" << key << endl;

	switch (key)
	{
		// space - save the background so we can compare the frames with it in order to find blobs in the scene.
	case ' ':
	{
		saveBackground = true;
		break;
	}
	// o - start saving all frames
	case 111:
	{
		saveFrames = !saveFrames;

		// If we are going to save all frames, let's HIDE EVERYTHING
		if (saveFrames)
		{
			// Increasing the number of samples in the ofFbo so we get better image quality for the hairs (antialiasing)
			//canvas->setNumberSamples(canvas->fbo.maxSamples());
			//saverThread.setSavingStatus(true);

			// Hiding everything
			hidePNGRids();
			hideLockOfHairGrids();
			showInstructions = false;
			saverThread.setFirstFrameNumber(ofGetFrameNum());

			int w = canvas->width;
			int h = canvas->height;

			saverThread.setDimentions(w, h);
			pixelBufferBack.allocate(w * h * 4, GL_DYNAMIC_READ);
			pixelBufferFront.allocate(w * h * 4, GL_DYNAMIC_READ);
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
		focusOnLockOfHair((lockOfHairInUse + 1) % lockOfHairs.size());
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
		cout << "Z was pressed! " << endl;

		if (isCtrlPressed)
		{
			if (undoHistory.size() > 0)
			{
				bool a = true;
				lockOfHairs[undoHistory.back()]->undo(a);
				undoHistory.pop_back();
			}
		}
		break;
	}

	// CTRL + Z -> On the Mac, if I press CTRL + Z, I'll get a key=26
	case 26:
	{
		if (undoHistory.size() > 0)
		{
			bool a = true;
			lockOfHairs[undoHistory.back()]->undo(a);
			undoHistory.pop_back();
		}
		break;
	}

	// A -> Add lock of hair
	case 97:
	{
		int x = ofRandom(100, ofGetWidth() - 150);
		int y = ofRandom(200, ofGetHeight() - 150);

		int newLockOfHairIndex;

		if (lockOfHairs.size())
		{
			newLockOfHairIndex = lockOfHairs[lockOfHairs.size() - 1]->index + 1;
		}
		else
		{
			newLockOfHairIndex = 0;
		}

		lockOfHairs.push_back(new LockOfHair("Hair " + std::to_string(newLockOfHairIndex),
											 ofVec2f(x, y),
											 newLockOfHairIndex,
											 //fbo
											 *canvas));

		// The LockOfHair should know about the image in order to pick the color
		//  by hovering over the image.
		lockOfHairs[lockOfHairs.size() - 1]->bImg = bImg;

		// Make it Active
		focusOnLockOfHair(newLockOfHairIndex);

		break;
	}
	// H
	case 104:
	{
		hideGUIS = !hideGUIS;

		// HIDE GUIS
		if (hideGUIS)
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
		for (unsigned i = 0; i < pGrids.size(); i++)
		{
			pos.push_back(pGrids[i]->pos);
		}

		ofVec2f p;
		// Randomly pick the top-left position of the new grid, but
		//  make sure it's not the same of any other grid on the screen.
		do
		{
			int x = ofRandom(100, ofGetWidth() - 150);
			int y = ofRandom(200, ofGetHeight() - 150);

			p = ofVec2f(x, y);
		}
		// Returns true if pos contain p
		while ((std::find(pos.begin(), pos.end(), p) != pos.end()));

		int newGridIndex;
		if (pGrids.size())
		{
			newGridIndex = pGrids[pGrids.size() - 1]->index + 1;
		}
		else
		{
			newGridIndex = 0;
		}

		pGrids.push_back(new PerlinNoiseGrid(p,							   // Top-lef corner of the grid
											 ofVec2f(400, 300),			   // (gridWidth, gridHeight)
											 10,						   // number of cells horizontally
											 10,						   // number of cells vertically
											 ofVec3f(0.107, 0.107, 0.007), // (xIncrement, yIncrement, zIncrement)
											 0,							   // Where are we on the third dimension
											 //"Grid " + std::to_string(pGrids.size()),
											 "Grid " + std::to_string(newGridIndex),
											 newGridIndex));
		break;
	}

	// M -> To move the canvas
	case 109:
	{
		isMpressed = true;
		break;
	}

	// N -> To move the camera/video
	case 110:
	{
		isNpressed = true;
		break;
	}

	// up arrow
	case 357:
	{
		threshold++;
		if (threshold > 255)
			threshold = 255;

		cout << "threshold= " << threshold << endl;
		break;
	}

	// down arrow
	case 359:
	{
		threshold--;
		if (threshold < 0)
			threshold = 0;

		cout << "threshold= " << threshold << endl;
		break;
	}

	// C  -> Show/hide camera/video
	case 99:
	{
		draw_video = !draw_video;
		break;
	}

	// B  -> Show/hide blobs
	case 98:
	{
		draw_blobs = !draw_blobs;
		break;
	}

	// Y -> Save the project in XML
	case 121:
	{
		cout << ".k--> Saving the Project!" << endl;

		// This is the name of the separate folder that will contain all the project's files
		string folder = "project/";

		// Saving General info (background image, canvas position, ...) ---------------------------------
		// Background Image
		// Save the image inside the /data directory
		// TODO: YOU SHOULD USE THE ORIGINAL'S IMAGE NAME TO DO THIS!
		if (bImg.isAllocated())
		{
			string bImage_name = "background_image.png";
			bImg.save(folder + bImage_name);
			// Save the name of the picture on the XML
			js["background_image"] = bImage_name;
		}
		else
		{
			js["background_image"] = "";
		}

		// Canvas
		ofJson canvas_js;
		canvas_js["x"] = canvas->position.x;
		canvas_js["y"] = canvas->position.y;
		canvas_js["width"] = canvas->width;
		canvas_js["height"] = canvas->height;

		js["canvas"] = canvas_js;
		js["lock_of_hair_in_focus"] = lockOfHairInUse;
		// ----------------------------------------------------------------------------------------------

		// Let's save all the PerlinNoiseGrids ----------------------------------------------------------
		ofJson pnoise_grids_js;

		// Go through each grid
		for (unsigned j = 0; j < pGrids.size(); j++)
		{
			ofJson pnoise_js;

			// Index
			pnoise_js["index"] = pGrids[j]->index;
			// Name
			pnoise_js["name"] = pGrids[j]->name;

			// Position
			ofJson position_js;
			position_js["x"] = pGrids[j]->pos.x;
			position_js["y"] = pGrids[j]->pos.y;
			pnoise_js["position"] = position_js;

			// Dimension
			ofJson dimension_js;
			dimension_js["x"] = pGrids[j]->gridDimensions.x;
			dimension_js["y"] = pGrids[j]->gridDimensions.y;
			pnoise_js["dimension"] = dimension_js;

			// Number of cells horizontally and vertically
			ofJson ncells_js;
			ncells_js["x"] = int(pGrids[j]->xNcells);
			ncells_js["y"] = int(pGrids[j]->yNcells);
			pnoise_js["number_of_cells"] = ncells_js;

			// Perlin Noise Increments
			ofJson increments_js;
			increments_js["x"] = float(pGrids[j]->xIncrement);
			increments_js["y"] = float(pGrids[j]->yIncrement);
			increments_js["z"] = float(pGrids[j]->zIncrement);
			pnoise_js["increments"] = increments_js;

			// Force
			ofJson force_js;
			force_js["magnitude"] = float(pGrids[j]->force);
			force_js["angle"] = float(pGrids[j]->angle);
			force_js["aperture"] = float(pGrids[j]->aperture);
			pnoise_js["force"] = force_js;

			// Info about the GUI
			ofJson gui_js;
			gui_js["x"] = pGrids[j]->gui.getPosition().x;
			gui_js["y"] = pGrids[j]->gui.getPosition().y;
			pnoise_js["gui"] = gui_js;

			pnoise_grids_js.push_back(pnoise_js);
		}
		js["perlin_noise_grids"] = pnoise_grids_js;
		// ----------------------------------------------------------------------------------------------

		// Let's save all LockOfHair --------------------------------------------------------------------
		ofJson lock_of_hairs_js;

		// Go through each LockOfHair
		for (unsigned j = 0; j < lockOfHairs.size(); j++)
		{
			ofJson lhair_js;

			// Name
			lhair_js["name"] = lockOfHairs[j]->name;
			// Index
			lhair_js["index"] = lockOfHairs[j]->index;

			// Info about the GUI
			ofJson gui_js;
			gui_js["x"] = lockOfHairs[j]->gui.getPosition().x;
			gui_js["y"] = lockOfHairs[j]->gui.getPosition().y;
			lhair_js["gui"] = gui_js;

			// Length of the chains
			ofJson features_js;
			features_js["length"] = float(lockOfHairs[j]->length);
			features_js["length_randomness"] = float(lockOfHairs[j]->lengthRandomness);
			features_js["number_of_particles"] = int(lockOfHairs[j]->np);
			features_js["thickness"] = int(lockOfHairs[j]->thickness);
			features_js["friction"] = float(lockOfHairs[j]->friction);
			features_js["speed_limit"] = float(lockOfHairs[j]->speedLimit);
			features_js["stiffness"] = float(lockOfHairs[j]->stiffness);
			features_js["damping"] = float(lockOfHairs[j]->damping);
			lhair_js["features"] = features_js;

			// Info about the hairs' colors
			ofJson color_js;
			color_js["r"] = int(lockOfHairs[j]->color->r);
			color_js["g"] = int(lockOfHairs[j]->color->g);
			color_js["b"] = int(lockOfHairs[j]->color->b);
			color_js["a"] = int(lockOfHairs[j]->color->a);
			color_js["randomness"] = float(lockOfHairs[j]->colorRandomness);
			lhair_js["color"] = color_js;

			// Info about how you were adding hair
			lhair_js["addone"] = bool(lockOfHairs[j]->addOne);
			lhair_js["addsquare"] = bool(lockOfHairs[j]->addSquare);
			lhair_js["addcircle"] = bool(lockOfHairs[j]->addCircle);

			// Info about adding hair in a Square shape
			ofJson square_js;
			square_js["x"] = float(lockOfHairs[j]->squareDimensions.get().x);
			square_js["y"] = float(lockOfHairs[j]->squareDimensions.get().y);
			square_js["density"] = int(lockOfHairs[j]->squareHairDensity);
			lhair_js["square"] = square_js;

			// Info about adding hair in a Circle shape
			ofJson circle_js;
			circle_js["radius"] = float(lockOfHairs[j]->circleRadius);
			circle_js["density"] = float(lockOfHairs[j]->circleHairDensity);
			lhair_js["circle"] = circle_js;

			// Background image
			// This was already saved on the top of this section. You should access it through there.

			ofJson hairs_js;
			// Saving ParticleChain2
			for (unsigned p = 0; p < lockOfHairs[j]->hairs.size(); p++)
			{
				ofJson hair_js;

				// Length
				hair_js["length"] = lockOfHairs[j]->hairs[p].length;
				// Number of Particles
				hair_js["number_particles"] = int(lockOfHairs[j]->hairs[p].np);

				// The head of the Chain/Hair (Where the hair gets fixed)
				ofJson head_js;
				head_js["x"] = lockOfHairs[j]->hairs[p].head.x;
				head_js["y"] = lockOfHairs[j]->hairs[p].head.y;
				hair_js["head"] = head_js;

				// Thickness and Color should be retrieved from LockOfHair

				hairs_js.push_back(hair_js);
			}

			lhair_js["hairs"] = hairs_js;
			lock_of_hairs_js.push_back(lhair_js);
		}
		// ----------------------------------------------------------------------------------------------

		js["lock_of_hairs"] = lock_of_hairs_js;

		// Saving the file ------------------------------
		string file_name = "last_project.json";

		ofFile file;
		// Before saving the file, let's check whether it already exists.
		// If that's the case, let's delete it.
		if (file.open(folder + file_name))
		{
			// Delete the file in your computer
			file.removeFile(folder + file_name);
		}

		// Saving the json
		ofSaveJson(folder + file_name, js);
		// ----------------------------------------------

		break;
	}

	// U -> Open the saved project
	case 117:
	{

		// This is the name of the separate folder that should contain all the project's files.
		string folder = "project/";
		string file_name = "last_project.json";

		ofFile file(folder + file_name);
		if (file.exists())
		{
			cout << ".k--> Opening Project!" << endl;

			ofJson js;

			file >> js;

			// Setting General info (background image, canvas position, ...) ---------------------------------
			// Background image
			string b_img_name = js["background_image"];
			// In case we have an image saved
			if (b_img_name != "")
			{
				bImg.load(folder + b_img_name);
				background_img_name = b_img_name;
			}

			// Canvas
			// In case there was a picture in the background, set the canvas to its dimension.
			if (b_img_name != "")
			{
				canvas = new Canvas(bImg.getWidth(), bImg.getHeight());
			}
			// Otherwise, set the canvas to the dimensions saved on the json file
			else
			{
				canvas = new Canvas(js["canvas"]["width"], js["canvas"]["height"]);
			}

			//canvas->setNumberSamples(8); // Setting the Number of Samples on the fbo
			canvas->setNumberSamples(0); // Setting the Number of Samples on the fbo
			canvas->setPosition(ofVec2f(js["canvas"]["x"], js["canvas"]["y"]));
			// -----------------------------------------------------------------------------------------------

			// LOAD THE PERLIN NOISE GRIDS -------------------------------------------------------------------
			// First of all, delete all pGrids
			// Clearing the pGridToBeDestroyed, which is a vector with the indexes of all pGrids to be deleted.
			pGridToBeDestroyed.clear();
			// Now you populate it with all pGrids.
			for (unsigned j = 0; j < pGrids.size(); j++)
			{
				pGridToBeDestroyed.push_back(pGrids[j]->index);
			}
			// Destroying all grids
			while (!pGridToBeDestroyed.empty())
			{
				destroyPgrid(pGridToBeDestroyed.back());
				// pop_back = Removes the last element in the vector, effectively reducing the container size by one.
				pGridToBeDestroyed.pop_back();
			}

			// Loop through each Perlin Noise from the json file
			for (auto &pnoise_grid : js["perlin_noise_grids"])
			{
				// Get pGrid position
				ofVec2f pos(pnoise_grid["position"]["x"], pnoise_grid["position"]["y"]);

				// Get pGrid dimensions
				ofVec2f dimensions(pnoise_grid["dimension"]["x"], pnoise_grid["dimension"]["y"]);

				// Get number of cells
				int nXCells = pnoise_grid["number_of_cells"]["x"];
				int nYCells = pnoise_grid["number_of_cells"]["y"];

				// Get increments
				ofVec3f increments(pnoise_grid["increments"]["x"], pnoise_grid["increments"]["y"], pnoise_grid["increments"]["z"]);

				// Get name and index
				string name = pnoise_grid["name"];
				int index = pnoise_grid["index"];

				// Get force information
				float force = pnoise_grid["force"]["magnitude"];
				float angle = pnoise_grid["force"]["angle"];
				float aperture = pnoise_grid["force"]["aperture"];

				// Create a PerlinNoiseGrid
				pGrids.push_back(new PerlinNoiseGrid(pos,		 // Top-lef corner of the grid
													 dimensions, // (gridWidth, gridHeight)
													 nXCells,	// number of cells horizontally
													 nYCells,	// number of cells vertically
													 increments, // (xIncrement, yIncrement, zIncrement)
													 0,			 // Where are we on the third dimension
													 name,		 // Grid's name
													 index		 // Grid's index
													 ));
				pGrids[pGrids.size() - 1]->force = force;
				pGrids[pGrids.size() - 1]->angle = angle;
				pGrids[pGrids.size() - 1]->aperture = aperture;

				// Get and set GUI's position
				pGrids[pGrids.size() - 1]->gui.setPosition(pnoise_grid["gui"]["x"], pnoise_grid["gui"]["y"]);
			}
			// -----------------------------------------------------------------------------------------------

			// LOAD THE LOCK OF HAIRS ------------------------------------------------------------------------
			// First delete all LockOfHairs
			// Clear the lockOfHairToBeDestroyed, which is the vector with the indexes of all lockOfHairToBeDestroyed to be deleted.
			lockOfHairToBeDestroyed.clear();
			// Now you populate it with all lockOhHairs
			for (unsigned j = 0; j < lockOfHairs.size(); j++)
			{
				lockOfHairToBeDestroyed.push_back(lockOfHairs[j]->index);
			}
			// Destroying all grids
			while (!lockOfHairToBeDestroyed.empty())
			{
				destroyLockOfHair(lockOfHairToBeDestroyed.back());
				// pop_back = Removes the last element in the vector, effectively reducing the container size by one.
				lockOfHairToBeDestroyed.pop_back();
			}

			// Loop through each LockOfHair from the json file
			for (auto &lhair_js : js["lock_of_hairs"])
			{
				// Get GUI position
				ofVec2f hair_gui_pos(lhair_js["gui"]["x"], lhair_js["gui"]["y"]);

				// Create the LockOfHair

				lockOfHairs.push_back(new LockOfHair(lhair_js["name"],  // name
													 hair_gui_pos,		// position
													 lhair_js["index"], // index
													 *canvas));

				// Info into the GUI
				lockOfHairs[lockOfHairs.size() - 1]->length = float(lhair_js["features"]["length"]);
				lockOfHairs[lockOfHairs.size() - 1]->lengthRandomness = float(lhair_js["features"]["length_randomness"]);
				lockOfHairs[lockOfHairs.size() - 1]->np = int(lhair_js["features"]["number_of_particles"]);
				lockOfHairs[lockOfHairs.size() - 1]->thickness = float(lhair_js["features"]["thickness"]);
				lockOfHairs[lockOfHairs.size() - 1]->friction = float(lhair_js["features"]["friction"]);
				lockOfHairs[lockOfHairs.size() - 1]->speedLimit = float(lhair_js["features"]["speed_limit"]);
				lockOfHairs[lockOfHairs.size() - 1]->stiffness = float(lhair_js["features"]["stiffness"]);
				lockOfHairs[lockOfHairs.size() - 1]->damping = float(lhair_js["features"]["damping"]);

				// Color
				ofColor c(lhair_js["color"]["r"], lhair_js["color"]["g"], lhair_js["color"]["b"], lhair_js["color"]["a"]);
				lockOfHairs[lockOfHairs.size() - 1]->color = c;
				lockOfHairs[lockOfHairs.size() - 1]->colorRandomness = float(lhair_js["color"]["randomness"]);

				// Info about how the hairs were being added
				lockOfHairs[lockOfHairs.size() - 1]->addOne = bool(lhair_js["addone"]);
				lockOfHairs[lockOfHairs.size() - 1]->addSquare = bool(lhair_js["addsquare"]);
				lockOfHairs[lockOfHairs.size() - 1]->addCircle = bool(lhair_js["addcircle"]);

				// Square
				lockOfHairs[lockOfHairs.size() - 1]->squareDimensions = ofVec2f(float(lhair_js["square"]["x"]), float(lhair_js["square"]["y"]));
				lockOfHairs[lockOfHairs.size() - 1]->squareHairDensity = int(lhair_js["square"]["density"]);

				// Circle
				lockOfHairs[lockOfHairs.size() - 1]->circleRadius = float(lhair_js["circle"]["radius"]);
				lockOfHairs[lockOfHairs.size() - 1]->circleHairDensity = int(lhair_js["circle"]["density"]);

				// Background image
				if (bImg.isAllocated())
				{
					lockOfHairs[lockOfHairs.size() - 1]->bImg = bImg;
				}

				// Creating the hairs/chains
				for (auto &hair_js : lhair_js["hairs"]) // looping through all hairs
				{
					// Length
					float length = float(hair_js["length"]);

					// Number of Particles
					int np = int(hair_js["number_particles"]);

					// Head position
					ofVec2f headPosition(hair_js["head"]["x"], hair_js["head"]["y"]);

					// Create a chain
					lockOfHairs[lockOfHairs.size() - 1]->hairs.push_back(ParticleChain2(length,										// length
																						headPosition,								// head position
																						float(lhair_js["features"]["thickness"]),   // thickness of the chain
																						np,											// number of particles
																						float(lhair_js["features"]["friction"]),	// friction with the medium. 1 = no friction / 0 = maximum friction
																						float(lhair_js["features"]["speed_limit"]), // speed limit
																						c,											// color
																						float(lhair_js["features"]["stiffness"]),   // stiffness; 0 = no connection (too flexible) / 1 = rigid connection
																						float(lhair_js["features"]["damping"]))		// damping coefficient; 0=no damping / 1= maximum damping
					);

					// locking the first particle
					lockOfHairs[lockOfHairs.size() - 1]->hairs[lockOfHairs[lockOfHairs.size() - 1]->hairs.size() - 1].particles[0].removeAttractionPoint(0);

					// Updating number of hairs
					lockOfHairs[lockOfHairs.size() - 1]->nHairs++;
				}
			}
			// Setting the focus on the LockOfHair that was in focus before.
			lockOfHairInUse = js["lock_of_hair_in_focus"];
			focusOnLockOfHair(lockOfHairInUse);
			// -----------------------------------------------------------------------------------------------
		}
		else
		{
			cout << ".k--> The project file could not be open." << endl;
		}
	}
	}
}

//--------------------------------------------------------------
void ofApp::saveScreen(ofImage &img, int x = 0, int y = 0, int w = ofGetWidth(), int h = ofGetHeight(), string fileName = "screenShot.png")
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
	for (unsigned i = 0; i < pGrids.size(); i++)
	{
		pGrids[i]->setVisibility(false);
	}
}

//--------------------------------------------------------------
void ofApp::showPNGRids()
{
	for (unsigned i = 0; i < pGrids.size(); i++)
	{
		pGrids[i]->setVisibility(true);
	}
}

//--------------------------------------------------------------
void ofApp::hideLockOfHairGrids()
{
	// Hide or Show all lockOfHairs GUIS
	for (unsigned i = 0; i < lockOfHairs.size(); i++)
	{
		lockOfHairs[i]->setVisibility(false);
	}
}

//--------------------------------------------------------------
void ofApp::showLockOfHairGrids()
{
	// Hide or Show all lockOfHairs GUIS
	for (unsigned i = 0; i < lockOfHairs.size(); i++)
	{
		lockOfHairs[i]->setVisibility(true);
	}
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key)
{

	switch (key)
	{
	//cout<< "Released: " << key << endl;

	// ctrl - used to detect ctrl + z
	case 768:
	{
		isCtrlPressed = false;
		break;
	}

	// M - used to detect when the user wants to move the background image (fbo) on the screen.
	case 109:
	{
		isMpressed = false;
		break;
	}

	// N - used to detect when the user wants to move the camera/video
	case 110:
	{
		isNpressed = false;
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
	if (isMpressed)
	{
		canvas->setPosition(ofVec2f(canvas->position.x + mouseX - mousePressedPosition.x,
									canvas->position.y + mouseY - mousePressedPosition.y));
		mousePressedPosition.set(x, y);
	}

	if (isNpressed)
	{
		video_pos.set(video_pos.x + mouseX - mousePressedPosition.x,
					  video_pos.y + mouseY - mousePressedPosition.y);
		mousePressedPosition.set(x, y);
	}
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button)
{
	isMousePressed = true;
	mousePressedPosition.set(x, y);

	// Checking if the mouse is over any LockOfHair GUI.
	// Go through each LockOfHair
	for (unsigned i = 0; i < lockOfHairs.size(); i++)
	{
		// If the user clicked over its GUI
		if (lockOfHairs[i]->isMouseOverGui())
		{
			//cout<<"Clicked over: "<< lockOfHairs[i]->index <<endl;

			//  If the lockOfHair currently in use is not the one that was clicked,
			//   it means the user wants to use this LockOfHair now.
			if (lockOfHairs[i]->index != lockOfHairInUse)
			{
				//cout<<"Changing focus to: "<< lockOfHairs[i]->index <<endl;

				focusOnLockOfHair(lockOfHairs[i]->index);
			}
		}
	}

	// Checking whether the user is trying to resize the video/camera
	// Resizing vertically
	if (mousePressedPosition.distance(ofVec2f(video_pos.x + video_dimension.x / 2 + canvas->position.x,
											  video_pos.y + video_dimension.y + canvas->position.y)) < camera_resizing_button_radius)
	{
		is_resizing_camera_height = true;
	}
	// Resizing orizontally
	if (mousePressedPosition.distance(ofVec2f(video_pos.x + video_dimension.x + canvas->position.x,
											  video_pos.y + video_dimension.y / 2 + canvas->position.y)) < camera_resizing_button_radius)
	{
		is_resizing_camera_width = true;
	}
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button)
{
	isMousePressed = false;
	is_resizing_camera_height = false;
	is_resizing_camera_width = false;
}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y)
{
}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y)
{
}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h)
{
	//cout<<"resized Window to: "<<w<<" x "<<h<<endl;
}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg)
{
}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo)
{
	// dragInfo.files is a list of the absolute file paths that were drag-and-dropped onto the window.
	// We are getting the last file dropped onto the window
	bImg.load(dragInfo.files[dragInfo.files.size() - 1]);

	// Allocating space for saving the frames when requested by the user.
	canvas->allocate(bImg.getWidth(), bImg.getHeight());

	// The LockOfHair should know about the image in order to pick the color
	//  by hovering over the image.
	for (unsigned i = 0; i < lockOfHairs.size(); i++)
	{
		lockOfHairs[i]->bImg = bImg;
	}

	// Extract only the name of the picture later here!
	background_img_name = dragInfo.files[dragInfo.files.size() - 1];
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

	for (unsigned i = 0; i < pGrids.size(); i++)
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

	for (unsigned i = 0; i < undoHistory.size(); i++)
	{
		// If we remove a LockOfHair of index 3, all the next LockOfHairs will have their
		//  indexes reduced by one. Therefore, we need to make sure we still have the right
		//  reference to them.
		// If the undoHistory item is 3 and we deleted LockOfHair 2. The Lockof Hair 3 is now
		//  LockOfHair 2 and therefore the undoHistory item should now be 2 as well.
		if (undoHistory[i] > indexRemoved)
		{
			newUndoHistory.push_back(undoHistory[i] - 1);
		}
		// If the undoHistory item is not the LockOfHair we have deleted, we should keep it
		//  on the list. Otherwise, we should remove it (ignore it).
		else if (undoHistory[i] != indexRemoved)
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
	for (unsigned i = index; i < lockOfHairs.size(); i++)
	{
		lockOfHairs[i]->index = i;
		lockOfHairs[i]->name = "Hair " + to_string(i);
		lockOfHairs[i]->gui.setName(lockOfHairs[i]->name);
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
	if (lockOfHairInUse != -1)
	{
		lockOfHairs[lockOfHairInUse]->setLockOfHairStatus(false);
	}

	// Let's focus the new LockOfHair
	lockOfHairInUse = index;
	lockOfHairs[lockOfHairInUse]->setLockOfHairStatus(true);
}
