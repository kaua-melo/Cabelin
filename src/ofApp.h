#pragma once

#include "ofMain.h"
#include "PerlinNoiseGrid.h"
#include "LockOfHair.h"
#include "ImageSaverThread.h"
#include "Canvas.h"
#include "ofxOpenCv.h"
#include "blobTracker.h"
#include "ofxKinectV2.h"
#include <algorithm> // In order to use (std::find(pos.begin(), pos.end(), p) != pos.end())
					 //  That checks if the vector 'pos' contains  'p'

// You should have only one of the options below uncommented.
//define _USE_LIVE_VIDEO // uncomment this to use a live camera instead of a video file or Kinect.
//#define _USE_VIDEO_FILE // uncomment this to use a pre recorded video.
#define _USE_KINECT // uncomment this to use kinect.

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

	// This will be called every time the maxDistance slider in the GUI is changed.
	// It will set the maxDistance variable from the BlobTracker object.
	// In sum, if the distance between blob A to blob A' (from previous frame) is
	//	smaller than maxDistance, then A and A' are considere the same blob.
	void maxDistanceChanged(int &newMaxDistance);

	vector<PerlinNoiseGrid *> pGrids;

	vector<LockOfHair *> lockOfHairs;

	// The index of the LockOfHairs being used
	//int lockOfHairsSelected;

	// The background image the user is gonna drop onto the window
	ofImage bImg;
	string background_img_name;

	// The poster to be drawn on the right side of the screen.
	//ofImage poster_img;

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

	// This will be used to identify whether we are using
	//  a webcam, a pre recorded video, or a kinect as the input
	//  method.
	// 0 -> pre recorded video
	// 1 -> webcam
	// 2 -> kinect
	int input_modality;

	//#ifdef _USE_LIVE_VIDEO // Let's use the webcam.
	ofVideoGrabber camera;
	//#elif defined _USE_VIDEO_FILE // Let's use the pre-recorded video
	ofVideoPlayer video;
	//#elif defined _USE_KINECT	 // Let's use the kinect
	ofxPanel kinect_gui;

	// GUI for the blob tracker.
	// Here you'll pick the maxNumber of blobs. Min/Max area.
	ofxPanel blob_tracker_gui;
	ofxIntSlider min_area;			// min area to be considered a blob
	ofxIntSlider max_area;			// max area to be considered a blob
	ofxIntSlider max_num_blobs;		// max number of blobs in a scene
	ofxIntSlider threshold;			// threshold to be used in the grayDiff image
	ofxIntSlider maxDistance;		// max distance from one blob to the other in 2 subsequent frames.
									//  If the dist. between two blobs is smaller than this, it means that those
									//  two blobs are the same.
	ofxFloatSlider force_from_blob; // The magnitude of the force that the blobs will apply on the hairs.

	std::vector<std::shared_ptr<ofxKinectV2>> kinects;
	std::vector<ofTexture> texDepth;

	std::size_t currentKinect = 0;
	//#endif

	ofxCvColorImage colorImg;
	ofxCvColorImage colorImg_crop;

	// Vector that contains the indexes of the pGrids that should be destroyed
	// If we don't have this here we can't use the PerlinNoiseGrid class anywhere in our code.
	//vector<int> pGridToBeDestroyed;

	ofxCvGrayscaleImage grayImg, grayBackground, grayDiff;

	ofxCvContourFinder previousContourFinder;
	ofxCvContourFinder contourFinder;

	//int threshold;
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
