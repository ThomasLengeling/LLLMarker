/*

Thomas Sanchez Lengeling
May, 2018

LLL
*/

#pragma once

#include "ofMain.h"
#include "ofxNetwork.h"

#include <ctime>
#include <iostream>
#include <vector>
#include <algorithm>
#include <map>

#include <opencv2/aruco.hpp>
#include <opencv2/calib3d.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

//addons
#include "ofxCv.h"
#include "ofxOpenCv.h"
#include "ofxDatGui.h"

#include "gui.h"
#include "MarkerAruco.h"
#include "KnobAruco.h"
#include "Common.h"
#include "Block.h"

#define  GRID_WIDTH   19
#define  GRID_HEIGHT  13

#define CAM_WIDTH  1920
#define CAM_HEIGHT 1080

#define MAX_MARKERS 1000


class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();

		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void mouseEntered(int x, int y);
		void mouseExited(int x, int y);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);

		//system values
		bool 	mDebug;

		//GUI
		ofxDatButtonRef mBDebugVideo;
		ofxDatButtonRef mBShowGrid;
		ofxDatButtonRef mBCalibrateVideo;

		ofxDatSliderRef mGridSpaceX;
		ofxDatSliderRef mGridSpaceY;

		ofxDatSliderRef mGridStartX;
		ofxDatSliderRef mGridStartY;

		ofxDatSliderRef mGridGapX;
		ofxDatSliderRef mGridGapY;

		bool mDrawGUI;

		void setupGUI();
		void updateGUI();
		void drawGUI();


		//Calibrate
		void setupCalibration();

		bool readDetectorParameters(std::string filename, cv::Ptr< cv::aruco::DetectorParameters> & params);
		bool saveCameraParams(const std::string &filename, cv::Size imageSize,
                             float aspectRatio, int flags,
                             const cv::Mat & cameraMatrix, const cv::Mat & distCoeffs,
                             double totalAvgErr);



		//check if found marker
		std::vector< MarkerAruco > mMarkers;
		bool		    mVideoMarkers;

		void 			setupGridPos();

		void 			updateGrid();
		void 			recordGrid();
		bool			mRecordOnce;

		//clean Detection
		void            setupCleaner();
		void 			cleanDetection();
		int 			mWindowIterMax;
		int				mWindowCounter;

		std::vector< std::vector< Block > > mControids;

		void          	drawArucoMarkers();

		int foundMarkers;

		//knob
		KnobArucoRef  mKnobAmenitie;
		void          setupKnob();

		//blocks
		void          setupBlocks();



		//Video grabber
		ofVideoGrabber 		vidGrabber;
		ofFbo				mFlipFbo;

		ofImage				vidImg;
		cv::Mat				vidMat;

		std::vector<int> 	ids;
		std::vector< std::vector<cv::Point2f> > corners;
		std::vector< std::vector<cv::Point2f> > rejected;

		std::vector< glm::vec2 > centroid;
		std::vector< int > tagsIds;

		std::vector< Block > mControid;

		std::vector< Block>  mBlocks;

		std::vector<int> mFullIds;

		//aruco
		cv::Ptr<cv::aruco::Board> board;
		cv::VideoCapture inputVideo;
		cv::Ptr<cv::aruco::Dictionary> dictionary;
		cv::Ptr<cv::aruco::DetectorParameters> detectorParams;

		int mMinFoundId;
		int mMaxFoundId;

		//send commands
		ofxUDPManager udpConnection;
		void setupConnection();
		std::string mUDPHeader;
		ofFile mTypeFile;

};
