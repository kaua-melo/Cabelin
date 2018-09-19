/*
 * ImageSaverThread.cpp
 *
 *  Created on: Oct 14, 2014
 *      Author: arturo
 * 
 * 		Modified by: kauã 2018
 * 	
 */

#include "ImageSaverThread.h"

ImageSaverThread::ImageSaverThread()
:firstFrame(true){
	startThread();
	firstFrameNumber = 0;

	this->saving = false;	
}

ImageSaverThread::~ImageSaverThread(){
	channel.close();
	channelReady.close();
	waitForThread(true);
}

void ImageSaverThread::save(unsigned char * pixels){
	// send the pixels to save to the thread
	channel.send(pixels);
}

void ImageSaverThread::waitReady(){
	// wait till the thread is done saving the
	// previous frame
	bool ready;
	channelReady.receive(ready);
}

void ImageSaverThread::setDimentions(int w, int h){
	width  = w;
	height = h;
}

void ImageSaverThread::setFirstFrameNumber(int fn){
	firstFrameNumber = fn;
}

void ImageSaverThread::setSavingStatus(bool s){
	this->saving = s;
}

void ImageSaverThread::threadedFunction(){
	// wait to receive some pixels,
	// save them as jpeg and then tell the main
	// thread that we are done
	// if the channel closes go out of the thread
	unsigned char * p;

	while(channel.receive(p)){ // && this->saving ){
		
		//cout<< "Saving image" << endl;

		//pixels.setFromPixels(p,1024,768,OF_PIXELS_RGB);
		pixels.setFromPixels(p, width, height, OF_PIXELS_RGB);

		std::stringstream a;
		a << setfill ('0') << setw (10) << (ofGetFrameNum()-firstFrameNumber);
		
		// Save the image
		//screen.save("f/"+a.str()+".png");	

		//ofSaveImage(pixels,ofToString(ofGetFrameNum())+".png");
		ofSaveImage(pixels,"f/"+a.str()+".png");
		
		//channelReady.send(true);
		channelReady.send(false);
	}
}
