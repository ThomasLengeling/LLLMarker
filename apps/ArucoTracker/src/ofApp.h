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

#define GRID_WIDTH 19
#define GRID_HEIGHT 13

#define GRID_MAX_WIDTH 32
#define GRID_MAX_HEIGHT 26

#define CAM_WIDTH 1920  //1920
#define CAM_HEIGHT 1080 //1080

#define MAX_MARKERS 1000

#define RAD_DETECTION 35

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
  ofxDatButtonRef mBEnableVideo;
  ofxDatButtonRef mBDebugMarkers;

  ofxDatSliderRef mGammaValue;
  ofxDatMatrixRef mBGridSelect;

  bool mDrawGUI;

  void setupGUI();
  void updateGUI();
  void drawGUI();

  // Calibrate
  void setupCalibration();

  void setupDetection();

  // check if found marker
  std::vector<MarkerArucoRef> mMarkers;
  bool mVideoMarkers;

  void setupGridPos();

  void updateGrid();
  void recordGrid();
  bool mRecordOnce;

  // clean Detection
  void setupCleaner();
  void cleanDetection();
  int mWindowIterMax;
  int mWindowCounter;

  std::map<int, int> mIdsCounter;
  std::map<int, int> mProCounter;
  std::map<int, int> mCenterCounter;

  void drawArucoMarkers();

  //draw grid info
  void offScreenInfoGrid();


  // knob
  KnobArucoRef mKnobAmenitie;
  bool mEnableKnob;
  void setupKnob();

  // blocks
  void setupBlocks();

  // 4 camera render
  ofFbo mFboFullCam;
  ofFbo mFboGridSend;
  ofFbo mFboGridInfo;
  ofFbo mFboGridColor;

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

  // stich images
  bool stichImage(cv::Mat &imgStitch, std::vector<cv::Mat> imgs);


  // aruco etector
  DetectorRef mArucoDetector;
  bool mDebugGrid;

  ofImage vidImg;
  cv::Mat vidMat;


  std::vector<BlockRef> mCurrBlock;
  std::vector<std::vector<BlockRef>> mTmpBlocks;

  std::vector<BlockRef> mBlocks;
  std::vector<int> tagsIds;
  std::vector<int> mFullIds;

  // send commands
  ofxUDPManager udpConnection;
  std::string mUDPHeader;
  ofFile mTypeFile;

  void setupConnection();
  void setupERICS();

  // save JSON file
  void saveJSONBlocks();
};
