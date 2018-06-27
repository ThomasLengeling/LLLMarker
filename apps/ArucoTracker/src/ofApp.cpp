#include "ofApp.h"
#include "setup.h"

using namespace std;
using namespace cv;

//--------------------------------------------------------------
void ofApp::setup() {

  // DEBUG
  mDebug = true;
  ofLog(OF_LOG_NOTICE) << "Debuging: " << mDebug << std::endl;

  ofSetBackgroundAuto(false);
  ofBackground(0);

  setupValues();
  setupGridDetector();
  setupDetection();
  setupCalibration();
  setupVideo();
  setupConnection();
  setupERICS();
  setupKnob();
  setupGUI();

  ofLog(OF_LOG_NOTICE) << "finished setup";
}

void ofApp::cleanDetection() {

  // calculate probabilyt and clean noise
  //
  if (mBSingleGrid->isActive()) {
    mGridDetector.at(mCurrentInputIdx)->cleanGrid();
  } else if (mBFullGrid->isActive()) {
    for (auto &gridDetector : mGridDetector) {
      gridDetector->cleanGrid();
    }
  }
}

//--------------------------------------------------------------
void ofApp::update() {

  bool newFrame = false;
  ofPixels pixels;
  std::vector<cv::Mat> vidMatImgs;
  std::vector<ofPixels> pixelsImg;
  std::vector<cv::Mat> imageCopys;
  cv::Mat imageCopy;

  int j = 0;
  for (auto &gridImage : mGridImg) {
    newFrame = gridImage->updateImage();
    if (newFrame) {
      pixelsImg.push_back(gridImage->getImgPixels());
    }
    j++;
  }

  int i = 0;
  cv::Mat copyCrop;
  for (auto &pixs : pixelsImg) {

    // v::Mat imgpixs;
    if (mCurrentInputIdx == i) {
    }
    //
    i++;
  }

  if (mBSingleGrid->isActive()) {
    if (pixelsImg.size() > 0 && mCurrentInputIdx <= pixelsImg.size() &&
        mGridImg.size() > 0 && mCurrentInputIdx <= mGridImg.size()) {
      cv::Mat copyCrop;
      cv::Mat input = ofxCv::toCv(pixelsImg.at(mCurrentInputIdx));
      mGridImg.at(mCurrentInputIdx)->cropImg(input);
      cv::Mat copMat = mGridImg.at(mCurrentInputIdx)->getCropMat();
      copMat.copyTo(copyCrop);

      // calculate Gamma
      mGridImg.at(mCurrentInputIdx)
          ->adjustGamma(copyCrop, mGammaValue->getValue());
      copyCrop.copyTo(imageCopy);
    }
  } else if (mBFullGrid->isActive()) {
    int i = 0;
    for (auto &pixs : pixelsImg) {
      if (pixelsImg.size() > 0 && i <= pixelsImg.size() &&
          mGridImg.size() > 0 && i <= mGridImg.size()) {
        cv::Mat copyCrop;
        cv::Mat input = ofxCv::toCv(pixelsImg.at(i));
        mGridImg.at(i)->cropImg(input);
        cv::Mat copMat = mGridImg.at(i)->getCropMat();
        copMat.copyTo(copyCrop);
        mGridImg.at(i)->adjustGamma(copyCrop, mGammaValue->getValue());
        imageCopys.push_back(copyCrop);
        i++;
      }
    }
  }

  mTotalMarkers = 0;
  if (mBSingleGrid->isActive()) {
    if (!imageCopy.empty()) {

      // detect the markers
      mArucoDetector->detectMarkers(imageCopy);

      // calculate the number of total markers
      mTotalMarkers += mArucoDetector->getNumMarkers();

      // get the marker image output
      vidImg = mArucoDetector->getOfImg();
      vidMat = mArucoDetector->getMatImg();

      // save the positions and id from the detected markers.
      mGridDetector.at(mCurrentInputIdx)
          ->generateMarkers(mArucoDetector->getTagIds(),
                            mArucoDetector->getBoard());

    } else {
      ofLog(OF_LOG_NOTICE) << "empty mat img copy " << std::endl;
    }
  } else if (mBFullGrid->isActive()) {
    if (!imageCopys.empty()) {

      int i = 0;
      for (auto &matcopy : imageCopys) {

        // detect the markers
        mArucoDetector->detectMarkers(matcopy);

        // calculate the number of total markers
        mTotalMarkers += mArucoDetector->getNumMarkers();

        // get the marker image output
        vidImg = mArucoDetector->getOfImg();
        vidMat = mArucoDetector->getMatImg();

        // save the positions and id from the detected markers.
        mGridDetector.at(i)->generateMarkers(mArucoDetector->getTagIds(),
                                             mArucoDetector->getBoard());
        i++;
      }

    } else {
      ofLog(OF_LOG_NOTICE) << "empty mat img copy " << std::endl;
    }
  }
  cleanDetection();

  offScreenInfoGrid();

  // udate
  if (mDrawGUI) {
    updateGUI();
  }
}

//--------------------------------------------------------------
void ofApp::drawArucoMarkers() {}

//--------------------------------------------------------------
void ofApp::draw() {
  ofSetColor(0, 0, 0, 255);
  ofRect(0, 0, ofGetWidth(), ofGetHeight());

  if (mBDebugVideo->isActive()) {

    ofSetColor(255);
    vidImg.draw(0, 0, 1280, 720);

    if (mBEnableCrop->isActive()) {
      mGridImg.at(mCurrentInputIdx)
          ->drawImage(0, 0, ofGetWidth(), ofGetHeight());
    } else {

      int i = 0;
      for (auto &gridImage : mGridImg) {
        if (mCurrentInputIdx == i) {
          ofSetColor(0, 200, 150, 200);
        } else {
          ofSetColor(255, 150);
        }
        gridImage->drawImage(ofGetWidth() - 426, 240 * i, 426, 240);
        i++;
      }
    }
  }

  if (mBDebugGrid->isActive()) {

    ofSetColor(255);
    vidImg.draw(0, 0, 640, 360);
    vidImg.draw(0, 0);

    if (mBSingleGrid->isActive()) {
      mGridDetector.at(mCurrentInputIdx)->drawMarkers();
    } else if (mBFullGrid->isActive()) {
      for (auto gridDetector : mGridDetector) {
        gridDetector->drawMarkers();
      }
    }
  }

  // update blocks and types
  if (mBSingleGrid->isActive()) {
    mGridDetector.at(mCurrentInputIdx)->updateBlockTypes();
  } else if (mBFullGrid->isActive()) {
    for (auto gridDetector : mGridDetector) {
      gridDetector->updateBlockTypes();
    }
  }

  if (mBFullGrid->isActive()) {
    ofSetColor(255);

    vidImg.draw(0, 0, ofGetWidth() - 640, ofGetHeight() - 360);
    mFboFullCam.draw(0, 0, 320, 240);
  }

  if (mEnableKnob) {
    mKnobAmenitie->drawArc();
    mKnobAmenitie->draw();
  }
  // record grid
  recordGrid();

  // draw GUI
  if (mDrawGUI) {
    ofSetColor(255);
    drawGUI();
  }

  if (mBEnableCrop->isActive()) {
    mGridImg.at(mCurrentInputIdx)->drawCropImg();
    mGridImg.at(mCurrentInputIdx)->drawCropRoi();
  }

  // draw results
  {
    int maxM = mGridDetector.at(mCurrentInputIdx)->getMaxMarkers();
    int numM = mGridDetector.at(mCurrentInputIdx)->getNumMarkers();
    glm::vec2 cdim = mGridDetector.at(mCurrentInputIdx)->getDim();
    ofSetColor(255);
    int posx = ofGetWidth() - 230;
    ofDrawBitmapString("Fps: " + to_string(ofGetFrameRate()), posx, 20);
    ofDrawBitmapString("Total Dec: " + to_string(mTotalMarkers), posx, 40);
    ofDrawBitmapString("Max Dec: " + to_string(MAX_MARKER_DETECT), posx, 60);
    ofDrawBitmapString("Inputs: " + to_string(mGridImg.size()), posx, 90);
    ofDrawBitmapString("Current input: " + to_string(mCurrentInputIdx), posx,
                       110);
    ofDrawBitmapString("Max Markers Real: " + to_string(maxM), posx, 130);
    ofDrawBitmapString("Markers: " + to_string(numM), posx, 150);
    ofDrawBitmapString("Dim: " + to_string(int(cdim.x)) + " " +
                           to_string(int(cdim.y)),
                       posx, 170);
    ofDrawBitmapString("Max Markers: " + to_string(int(cdim.x * cdim.y)), posx,
                       190);
    ofDrawBitmapString("Full Dim: " + to_string(int(mFullGridDim.x)) + " " +
                           to_string(int(mFullGridDim.y)),
                       posx, 210);
  }
}

//--------------------------------------------------------------
void ofApp::recordGrid() {
  // updte points
  if (mBSingleGrid->isActive()) {
    mGridDetector.at(mCurrentInputIdx)->recordGrid();
  } else if (mBFullGrid->isActive()) {
    for (auto gridDetector : mGridDetector) {
      gridDetector->recordGrid();
    }
  }
}

void ofApp::offScreenInfoGrid() {
  if (mBSingleGrid->isActive()) {
    mFboGridInfo.begin();
    mGridDetector.at(mCurrentInputIdx)->drawMarkers();
    mFboGridInfo.end();
  }

  if (mBSingleGrid->isActive()) {
    mGridDetector.at(mCurrentInputIdx)->drawMarkers();
  } else if (mBFullGrid->isActive()) {
    for (auto gridDetector : mGridDetector) {
      gridDetector->drawMarkers();
    }
  }
}

//--------------------------------------------------------------
void ofApp::updateGUI() {
  mGammaValue->update();
  mBEnableCrop->update();
  mBEnableVideo->update();

  mBDebugVideo->update();
  mBDebugGrid->update();
  mBSingleGrid->update();
  mBFullGrid->update();
  mBDebugMarkers->update();

  mBGridSelect->update();
}

//--------------------------------------------------------------
void ofApp::drawGUI() {
  mGammaValue->draw();
  mBEnableCrop->draw();
  mBEnableVideo->draw();

  mBDebugVideo->draw();
  mBDebugGrid->draw();
  mBSingleGrid->draw();
  mBFullGrid->draw();
  mBDebugMarkers->draw();

  mBGridSelect->draw();
}

//--------------------------------------------------------------
void ofApp::saveJSONBlocks() {

  int i = 0;
  if (mEnableKnob) {
    {
      ofJson pt;
      pt[to_string(i)]["posx"] = mKnobAmenitie->getStaticPos().x;
      pt[to_string(i)]["posy"] = mKnobAmenitie->getStaticPos().y;
      pt[to_string(i)]["type"] = BlockType::knobStatic;
      // writer.push_back(pt);
      i++;
    }

    {
      ofJson pt;
      pt[to_string(i)]["posx"] = mKnobAmenitie->getDynamicPos().x;
      pt[to_string(i)]["posy"] = mKnobAmenitie->getDynamicPos().y;
      pt[to_string(i)]["type"] = BlockType::knobDynamic;
      // writer.push_back(pt);
    }
  }
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key) {
  if (key == 'g') {
    mDrawGUI = !mDrawGUI;
  }

  if (key == '1') {

    std::cout << "record grid positions" << std::endl;
  }

  if (key == '5') {
    mGridImg.at(mCurrentInputIdx)
        ->setCropDisp(mGridImg.at(mCurrentInputIdx)->getCropDisp() +
                      glm::vec2(1, 1));
  }

  if (key == '6') {
    mGridImg.at(mCurrentInputIdx)
        ->setCropDisp(mGridImg.at(mCurrentInputIdx)->getCropDisp() -
                      glm::vec2(1, 1));
  }

  if (key == '2') {
    mGridDetector.at(mCurrentInputIdx)->saveGridJson();
    ofLog(OF_LOG_NOTICE) << "saved json grid positions: " << mCurrentInputIdx;

    // knob
    // saveJSONBlocks();
  }

  if (key == '3') {
    ofJson writer;
    int i = 0;
    for (auto &gridImage : mGridImg) {
      ofJson pt;
      std::string inputImg("cam" + to_string(i));
      pt[inputImg]["x1"] = gridImage->getCropUp().x;
      pt[inputImg]["y1"] = gridImage->getCropUp().y;
      pt[inputImg]["x2"] = gridImage->getCropDown().x;
      pt[inputImg]["y2"] = gridImage->getCropDown().y;
      pt[inputImg]["disX"] = gridImage->getCropDisp().x;
      pt[inputImg]["disY"] = gridImage->getCropDisp().y;
      writer.push_back(pt);
      i++;
    }

    ofLog(OF_LOG_NOTICE) << "Image json write";
    ofSaveJson("img.json", writer);
  }

  if (key == 'v') {
    mVideoCapture = !mVideoCapture;
  }
  if (key == 's') {
    mStichImg = !mStichImg;
  }
  if (key == 'd') {
    mDebug = !mDebug;
  }
  if (key == 'f') {
  }
  if (key == 'c') {
  }

  if (key == 'n') {
    mGridDetector.at(mCurrentInputIdx)->toogleDebugGrid();
  }
  if (key == '9') {
    udpConnection.Send(mUDPHeader.c_str(), mUDPHeader.length());
  }
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key) {}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y) {}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button) {
  if (mDebug) {
    mGridDetector.at(mCurrentInputIdx)->setGridPos(glm::vec2(x, y));
  }

  // Physical GUI
  {
    if (mEnableKnob) {
      glm::vec2 posStatic = mKnobAmenitie->getStaticPos();
      float distStatic = ofDist(posStatic.x, posStatic.y, x, y);
      if (distStatic >= 0.0 && distStatic <= 15) {
        mKnobAmenitie->setStaticPos(glm::vec2(x, y));
      }

      glm::vec2 posDynamic = mKnobAmenitie->getDynamicPos();
      float distDynamic = ofDist(posDynamic.x, posDynamic.y, x, y);
      if (distDynamic >= 0.0 && distDynamic <= 15) {
        mKnobAmenitie->setDynamicPos(glm::vec2(x, y));
      }
    }
  }
  //
  if (mBEnableCrop->isActive()) {
    {
      float distUp = ofDist(mGridImg.at(mCurrentInputIdx)->getCropUp().x,
                            mGridImg.at(mCurrentInputIdx)->getCropUp().y, x, y);
      if (distUp >= 0.0 && distUp <= 20) {
        mGridImg.at(mCurrentInputIdx)->setCropUp(glm::vec2(x, y));
      }

      float distDown =
          ofDist(mGridImg.at(mCurrentInputIdx)->getCropDown().x,
                 mGridImg.at(mCurrentInputIdx)->getCropDown().y, x, y);
      if (distDown >= 0.0 && distDown <= 20) {
        mGridImg.at(mCurrentInputIdx)->setCropDown(glm::vec2(x, y));
      }
    }
  }
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button) {}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button) {}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y) {}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y) {}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h) {}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg) {}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo) {}
