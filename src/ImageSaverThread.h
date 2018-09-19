/*
 * ImageSaverThread.h
 *
 *  Created on: Oct 14, 2014
 *      Author: arturo
 *
 * 		Modified by: kauã 2018
 */
#pragma once
#include "ofMain.h"
class ImageSaverThread: public ofThread{
public:
	ImageSaverThread();
	~ImageSaverThread();

	void save(unsigned char * pixels);
	void waitReady();
	void threadedFunction();
	void setDimentions(int w, int h);
	void setFirstFrameNumber(int fn);
	void setSavingStatus(bool s);

private:
	ofPixels pixels;
	ofThreadChannel<unsigned char *> channel;
	ofThreadChannel<bool> channelReady;
	bool firstFrame;

	int width, height;
	int firstFrameNumber;
	bool saving;
};
