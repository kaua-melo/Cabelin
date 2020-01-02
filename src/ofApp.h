#pragma once

#include "ofMain.h"
#include "PerlinNoiseGrid.h"
#include "LockOfHair.h"
#include "ImageSaverThread.h"
#include "Canvas.h"
#include "ofxOpenCv.h"
#include "blobTracker.h"

#include <algorithm> // In order to use (std::find(pos.begin(), pos.end(), p) != pos.end())
					 //  That checks if the vector 'pos' contains  'p'

#define _USE_LIVE_VIDEO // uncomment this to use a live camera instead of a video file.

class ofApp : public ofBaseApp
{
public:
	void setup();
	void update();
	void draw();

	void keyPressed(int key);
	void keyReleased(int key);
	void mouseMoved(int x, int y);
	void mouseDragged(int x, int y, int button);
	void mousePressed(int x, int y, int button);
	void mouseReleased(int x, int y, int button);
	void mouseEntered(int x, int y);
	void mouseExited(int x, int y);
	void windowResized(int w, int h);
	void dragEvent(ofDragInfo dragInfo);
	void gotMessage(ofMessage msg);

	void destroyPgrid(int index);
	void destroyLockOfHair(int index);
	void focusOnLockOfHair(int index);

	void updateUndoHistory(int indexRemoved);

	void hidePNGRids();
	void showPNGRids();
	void hideLockOfHairGrids();
	void showLockOfHairGrids();

	void saveScreen(ofImage &img, int x, int y, int w, int h, string fileName);

	vector<PerlinNoiseGrid *> pGrids;

	vector<LockOfHair *> lockOfHairs;

	// The index of the LockOfHairs being used
	//int lockOfHairsSelected;

	// The background image the user is gonna drop onto the window
	ofImage bImg;
	string background_img_name;

	// left and top border of the image on the window.
	// This will be useful when saving the frames
	int leftBorder, topBorder;

	// Vector that contains the indexes of the pGrids that should be destroyed
	vector<int> pGridToBeDestroyed;

	// Vector that contains the indexes of the LockOfHairs that should be destroyed
	vector<int> lockOfHairToBeDestroyed;

	// This vector will host the sequence of the LockOfHairs's indexes that added hair
	//  to the scene.
	vector<int> undoHistory;

	// Index of the LockOfHair in use
	int lockOfHairInUse;

	// Hide every GUI of the screen
	bool hideGUIS;

	//
	bool showInstructions;

	// Will be used to name the screenShot files
	int screenShotNumber;

	/* Just to let the computer breath a bit.
		   if true  -> keeps calculating everything.
		   if false -> stops calculating*/
	bool pauseSimulation;

	bool saveFrames; // If true -> save every single frame
	bool firstFrame;
	ofBufferObject pixelBufferBack, pixelBufferFront;
	ImageSaverThread saverThread;

	//
	bool printColorBeingPicked;
	ofColor colorBeingPicked;

	// If true, the CTRL is pressed. Will be useful to detect 'CTRL + Z' actions.
	bool isCtrlPressed;

	bool isMpressed; // Will be used to move the Canvas
	bool isNpressed; // Will be used to move the camera/video
	bool isMousePressed;
	ofVec2f mousePressedPosition;

	Canvas *canvas;

	BlobTracker *bTracker;

	// Used to load and show a mouse icon
	//ofImage mouseIcon;

	// From the BLOB TRACKING WITH PERLIN NOISE CODE:
	//------------------------------------------------------------------
	// The dimensions of the camera images to be displayed
	int width,
		height;

#ifdef _USE_LIVE_VIDEO
	ofVideoGrabber camera;
#else
	ofVideoPlayer video;
#endif

	ofxCvColorImage colorImg;
	ofxCvColorImage colorImg_crop;

	// Vector that contains the indexes of the pGrids that should be destroyed
	// If we don't have this here we can't use the PerlinNoiseGrid class anywhere in our code.
	//vector<int> pGridToBeDestroyed;

	ofxCvGrayscaleImage grayImg, grayBackground, grayDiff;

	ofxCvContourFinder previousContourFinder;
	ofxCvContourFinder contourFinder;

	int threshold;
	bool saveBackground;

	bool draw_video;
	bool draw_blobs;

	ofVec2f video_pos;
	ofVec2f video_dimension;
	int camera_resizing_button_radius;
	bool is_resizing_camera_height;
	bool is_resizing_camera_width;
	//------------------------------------------------------------------

	// This will be used to save our project in a XML.
	//ofXml XML;
	// This will be used to save our project in json
	ofJson js;
};
