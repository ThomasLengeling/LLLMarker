/*

Thomas Sanchez Lengeling
May, 2018

LLL
*/

#pragma once

#include "ofMain.h"
#include "ofxNetwork.h"

#include <algorithm>
#include <ctime>
#include <iostream>
#include <map>
#include <vector>

#include "opencv2/stitching.hpp"
#include <opencv2/aruco.hpp>
#include <opencv2/calib3d.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/gpu/gpu.hpp>

// addons
#include "ofxCv.h"
#include "ofxDatGui.h"
#include "ofxOpenCv.h"

#include "Block.h"
#include "Common.h"
#include "Detector.h"
#include "KnobAruco.h"
#include "MarkerAruco.h"
#include "gui.h"
#include "GridImage.h"
#include "GridDetector.h"

#define DEFAULT_GRID_WIDTH  19
#define DEFAULT_GRID_HEIGHT 13

/*
grid 1
     10
     00  -> full 8 x 19 = 104 + 18 + 17 + 16+ 10 + 5 = 170

grid 2
     01
     00  -> full 12 x 19 = 228 +5 = 233

grid 3
     00
     10  -> full 247

grid 4
     00
     01  -> full 169

     TOTAL = 819 markers
*/

#define MAX_MARKER_01 170
#define MAX_MARKER_02 233
#define MAX_MARKER_03 247
#define MAX_MARKER_04 169

#define MAX_MARKER_DETECT 819

#define CAM_WIDTH 1920  //1920
#define CAM_HEIGHT 1080 //1080


class ofApp : public ofBaseApp {

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

  // system values
  bool mDebug;

  // GUI
  ofxDatButtonRef mBDebugVideo;
  ofxDatButtonRef mBDebugGrid;

  ofxDatButtonRef mBSingleGrid;
  ofxDatButtonRef mBFullGrid;

  ofxDatButtonRef mBEnableCrop;
  ofxDatButtonRef mBCalibrateGrid;
  ofxDatButtonRef mBEnableVideo;
  ofxDatButtonRef mBDebugMarkers;

  ofxDatSliderRef mGammaValue;
  ofxDatMatrixRef mBGridSelect;

  bool mDrawGUI;

  void setupGUI();
  void updateGUI();
  void drawGUI();

  void setupValues();

  // Calibrate
  void setupCalibration();
  void setupDetection();

  void updateGrid();
  void recordGrid();
  bool mRecordOnce;

  // clean Detection
  void cleanDetection();

  void drawArucoMarkers();

  //draw grid info
  void offScreenRenderGrid();


  // knob
  KnobArucoRef mKnobAmenitie;
  bool mEnableKnob;
  void setupKnob();


  // 4 camera render
  ofFbo mFboSingle;
  std::vector<ofFbo> mFboGrid;

  bool mSingleCam;

  //grid image
  std::vector<GridImageRef> mGridImg;
  int mNumCam;
  int mCurrentInputIdx;

  cv::Mat videoInputMat;

  ofTexture mCurrentVideo;


  bool mVideoCapture;
  bool mStichImg;
  void setupVideo();




  // aruco etector
  DetectorRef mArucoDetector;
  int mTotalMarkers;

  ofImage vidImg;
  cv::Mat vidMat;

  //inputs
  int mNumInputs;

  glm::vec2 mFullGridDim;
  std::vector<glm::vec2> mGridSizes;
  std::vector<int> mMaxMarkers;

  //
  std::vector<GridDetectorRef> mGridDetector;
  void setupGridDetector();

  // send commands
  ofxUDPManager udpConnection;
  std::string mUDPHeader;
  ofFile mTypeFile;

  void setupConnection();
  void setupERICS();

  // save JSON file
  void saveJSONBlocks();
};
