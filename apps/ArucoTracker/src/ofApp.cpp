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

  setupDetection();

  setupCalibration();

  setupVideo();

  setupGridPos();

  setupCleaner();

  setupConnection();

  setupERICS();

  setupKnob();

  setupBlocks();

  setupGUI();

  ofLog(OF_LOG_NOTICE) << "finished setup" << std::endl;
}

void ofApp::cleanDetection() {
  if (mWindowCounter >= mWindowIterMax) {
    mWindowCounter = 0;

    if (mFullIds.size() > 0) {

      // clasical probabilty of ocurance

      for (auto &mk : mMarkers) {
        mk->resetProba();
      }

      // calculate the frequency of ocurance
      for (auto &blocks : mTmpBlocks) {
        for (auto &block : blocks) {

          glm::vec2 blockPos = block->getPos();

          int k = 0;
          for (auto &mk : mMarkers) {
            glm::vec2 boardPos = mk->getPos();
            float dis = ofDist(blockPos.x, blockPos.y, boardPos.x, boardPos.y);
            if (dis >= 0 && dis <= RAD_DETECTION) {
              mIdsCounter[k] = block->getMarkerId(); // block.mId
              mk->incProba();
              // not sure i need it break;
            }
            k++;
          }
        }
      }

      // send upd data and activations;
      int i = 0;
      int j = 0;
      std::string enables = "e";
      std::string ids = "i";
      std::string fileIds = "";
      for (auto &mk : mMarkers) {
        float proba = mk->getProba(mWindowIterMax);
        enables += " ";
        ids += " ";
        if (proba >= 1.0 / (float)mWindowIterMax) {
          mk->enableOn();
          mk->setMarkerId(mIdsCounter[i]);
          mk->updateId(mIdsCounter[i]);

          enables += "1";
          ids += to_string(mk->getIdTypePair().first);

          if (i < GRID_HEIGHT * GRID_WIDTH) {
            fileIds += to_string(mk->getIdTypePair().first);
            fileIds += " ";
          }

          // find id and update it;
          for (auto &blocks : mBlocks) {
            int makerId = mk->getIdTypePair().first;
            if (blocks->getMarkerId() == makerId) {
              mk->updateType(blocks->getType());
            }
          }

          if (mEnableKnob) {
            if (mk->getBlockType() == BlockType::knobStatic) {
              std::string msg = "skn " + to_string(mk->getIdTypePair().first) +
                                " " + to_string(static_cast<int>(
                                          mKnobAmenitie->getType().getType())) +
                                " " + to_string(mk->getPos().x) + " " +
                                to_string(mk->getPos().y);

              // cout << "sent: " << msg << std::endl;
              udpConnection.Send(msg.c_str(), msg.length());
            }
          }

        } else {
          mk->enableOff();
          mk->setMarkerId(-1);

          enables += "-1";
          ids += "-1";

          if (i < GRID_HEIGHT * GRID_WIDTH) {
            fileIds += "-1";
            fileIds += " ";
          }
        }

        j++;
        if (j >= GRID_WIDTH) {
          j = 0;
          fileIds += "\n";
        }

        i++;
      }

      ofFile mTypeFile;
      mTypeFile.open("ids.asc", ofFile::WriteOnly);
      mTypeFile << mUDPHeader << fileIds;
      udpConnection.Send(mUDPHeader.c_str(), mUDPHeader.length());

      // send udp with on or off
      udpConnection.Send(enables.c_str(), enables.length());
      udpConnection.Send(ids.c_str(), ids.length());

      /// blocks
      {
        std::string types = "type \n";
        for (auto &mk : mMarkers) {
          int t = 0;
          t = static_cast<int>(mk->getIdTypePair().second);
          types += to_string(t);
          types += " ";
        }
        // send udp with on or off
        udpConnection.Send(ids.c_str(), ids.length());
      }
    }

    // done activation and disactivation

    mTmpBlocks.clear();

    // ofLog(OF_LOG_NOTICE) << "min " << mArucoDetector->getMinId() << " max "
    //                     << mArucoDetector->getMaxId();
  }
  mWindowCounter++;
}

bool ofApp::stichImage(cv::Mat &imgStitch, std::vector<cv::Mat> imgs) {
  bool try_use_gpu = true;
  bool success = true;
  Stitcher::Mode mode = Stitcher::PANORAMA;

  Ptr<Stitcher> stitcher = Stitcher::create(mode, try_use_gpu);
  Stitcher::Status status = stitcher->stitch(imgs, imgStitch);

  if (status != Stitcher::OK) {
    ofLog(OF_LOG_NOTICE) << "Can't stitch images, error code = " << int(status);
    success = false;
  }
  return success;
}
//--------------------------------------------------------------
void ofApp::update() {

  bool newFrame = false;
  ofPixels pixels;
  std::vector<cv::Mat> vidMatImgs;
  std::vector<ofPixels> pixelsImg;

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

  if (pixelsImg.size() > 0 && mCurrentInputIdx <= pixelsImg.size() &&
      mGridImg.size() > 0 && mCurrentInputIdx <= mGridImg.size()) {
    cv::Mat input = ofxCv::toCv(pixelsImg.at(mCurrentInputIdx));
    mGridImg.at(mCurrentInputIdx)->cropImg(input);
    cv::Mat copMat = mGridImg.at(mCurrentInputIdx)->getCropMat();
    copMat.copyTo(copyCrop);
  }

  // cv::Mat copyCrop = mGridImg.at(mCurrentInputIdx)->getCropMat();
  mGridImg.at(mCurrentInputIdx)->adjustGamma(copyCrop, mGammaValue->getValue());
  cv::Mat imageCopy;
  copyCrop.copyTo(imageCopy);

  if (!imageCopy.empty()) {

    mArucoDetector->detectMarkers(imageCopy);

    vidImg = mArucoDetector->getOfImg();
    vidMat = mArucoDetector->getMatImg();

    tagsIds = mArucoDetector->getTagIds();
    mCurrBlock = mArucoDetector->getBoard(); // current ids
    mTmpBlocks.push_back(mCurrBlock);        // array of blocks
  } else {
    ofLog(OF_LOG_NOTICE) << "empty mat img copy " << std::endl;
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

    mFboGridInfo.draw(640 * 2, 0, 640, 360);
    vidImg.draw(0, 0);

    for (auto &mk : mMarkers) {
      glm::vec2 pos = mk->getPos();

      if (mk->isEnable()) {
        ofSetColor(255);
        ofDrawCircle(pos.x, pos.y, 7, 7);
        ofSetColor(200, 80);
        ofDrawCircle(pos.x, pos.y, RAD_DETECTION / 2.0, RAD_DETECTION / 2.0);
      } else {
        ofSetColor(255, 255, 0);
        ofDrawCircle(pos.x, pos.y, 4, 4);
        ofSetColor(255, 355, 100);
        ofDrawCircle(pos.x, pos.y, RAD_DETECTION / 2.0, RAD_DETECTION / 2.0);
      }

      if (mDebugGrid) {
        ofSetColor(0, 100, 200, 100);
        ofDrawCircle(pos.x, pos.y, RAD_DETECTION, RAD_DETECTION);
      }

      ofSetColor(255);
      ofDrawBitmapString(mk->getMarkerId(), pos.x - 20, pos.y - 7);
      ofDrawBitmapString(mk->getGridId(), pos.x - 25, pos.y - 17);
    }
  }

  // update block

  {
    if (mEnableKnob) {
      int type = 0;
      for (auto &block : mBlocks) {
        int id = block->getMarkerId();
        // get marker Id from knob and update the block
        if (id == mKnobAmenitie->getDynamicId()) {
          block->setType(mKnobAmenitie->getType().getType());
          type = block->getType();
        }
      }
    }
  }

  /*
          // update block to the grid
          for (auto &block : mBlocks) {
            int id = block->getMarkerId();

            int roadsId[] = {
                207, 257, 39,  154, 135, 79,  149, 174, 120, 176, 43,  250,
  52,
                119, 156, 29,  81,  168, 61,  152, 190, 189, 150, 187, 167,
  185,
                247, 227, 181, 124, 85,  179, 140, 206, 222, 232, 97,  219,
  218,
                217, 216, 215, 214, 212, 40,  213, 255, 101, 36,  49,  26,
  89,
                164, 228, 246, 183, 245, 201, 215, 261, 95};
            for (int i = 0; i < sizeof(roadsId) / sizeof(roadsId[0]); i++) {
              if (id == roadsId[i]) {
                block->setType(mKnobAmenitie->getType().getType());
              }
            }
          }
        }
      }
*/
  // update blocks and types
  for (auto &block : mBlocks) {
    int id = block->getMarkerId();
    for (auto &mk : mMarkers) {
      if (mk->getIdTypePair().first == id) {
        mk->updateType(block->getType());
        break;
      }
    }
  }

  for (auto &mk : mMarkers) {
    glm::vec2 pos = mk->getPos();
    if (mEnableKnob) {
      if (mk->getGridId() == mKnobAmenitie->getDynamicGridId()) {

        for (auto &cblock : mCurrBlock) {
          glm::vec2 blockPos = cblock->getPos();
          int id = cblock->getMarkerId();
          float dis = ofDist(blockPos.x, blockPos.y, pos.x, pos.y);
          if (dis >= 0.0 && dis < RAD_DETECTION) {
            mKnobAmenitie->setDynamicPos(blockPos);
            mKnobAmenitie->setDynamicId(id);
            mk->setPos(blockPos);

            ofDrawBitmapString(mk->getGridId(), pos.x - 25, pos.y - 17);

            {
              std::string msg = "dkn " + to_string(blockPos.x) + " " +
                                to_string(blockPos.y) + " " +
                                to_string(mKnobAmenitie->getDynamicId());

              // std::cout << msg << std::endl;

              udpConnection.Send(msg.c_str(), msg.length());
            }
            break;
          }
        }
      }
    }
  }

  if (mBFullGrid->isActive()) {
    ofSetColor(255);

    vidImg.draw(0, 0, ofGetWidth(), ofGetHeight());
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
  ofSetColor(255);
  int posx = ofGetWidth() - 200;
  ofDrawBitmapString("Markers: " + to_string(mArucoDetector->getNumMarkers()),
                     posx, 20);
  ofDrawBitmapString("Fps: " + to_string(ofGetFrameRate()), posx, 40);
  ofDrawBitmapString("Inputs: " + to_string(mGridImg.size()), posx, 60);
  ofDrawBitmapString("Current input: " + to_string(mCurrentInputIdx), posx, 80);
}

//--------------------------------------------------------------
void ofApp::recordGrid() {
  if (mRecordOnce) {
    ofLog(OF_LOG_NOTICE) << mCurrBlock.size() << " " << mMarkers.size();
    if (mCurrBlock.size() == mMarkers.size() - 2) {
      ofLog(OF_LOG_NOTICE) << mCurrBlock.size()
                           << " markes = " << GRID_WIDTH * GRID_HEIGHT;

      // set ids
      mFullIds.clear();
      for (auto &mk : mMarkers) {
        glm::vec2 pos = mk->getPos();
        int k = 0;
        for (auto &cblock : mCurrBlock) {
          glm::vec2 cenPos = cblock->getPos();
          float dis = ofDist(cenPos.x, cenPos.y, pos.x, pos.y);
          if (dis >= 0.0 && dis <= RAD_DETECTION) {
            mk->setMarkerId(tagsIds.at(k));
            mFullIds.push_back(tagsIds.at(k));
            // calculate grid positions

            break;
          }
          k++;
        }
      }

      sort(mFullIds.begin(), mFullIds.end());

      std::cout << "full " << mFullIds.size() << std::endl;
      {
        int i = 0;
        for (auto &block : mBlocks) {
          for (auto &marker : mMarkers) {
            if (block->getMarkerId() == marker->getIdTypePair().first) {
              int mkId = marker->getIdTypePair().first;
              block->setMarkerId(mkId);
              std::cout << block->getMarkerId() << " " << mkId << std::endl;
              break;
            }
            i++;
          }
        }
      }

      ofLog(OF_LOG_NOTICE) << "num Uniques: " << mFullIds.size();
      ofLog(OF_LOG_NOTICE) << "finished recording";

      mRecordOnce = false;
    }
  }
}

void ofApp::offScreenInfoGrid() {
  mFboGridInfo.begin();
  for (auto &mk : mMarkers) {
    glm::vec2 pos = mk->getPos();

    if (mk->isEnable()) {
      ofSetColor(255);
      ofDrawCircle(pos.x, pos.y, 7, 7);
    } else {
      ofSetColor(255, 255, 0);
      ofDrawCircle(pos.x, pos.y, 4, 4);
    }

    ofSetColor(255);
    ofDrawBitmapString(mk->getMarkerId(), pos.x - 20, pos.y - 7);
    ofDrawBitmapString(mk->getGridId(), pos.x - 25, pos.y - 17);
  }
  mFboGridInfo.end();
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

  /*
  mGridSpaceX->slider->update();
  mGridSpaceY->slider->update();

  mGridStartX->slider->update();
  mGridStartY->slider->update();

  mGridGapX->slider->update();
  mGridGapY->slider->update();
  */
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
  /*
mGridSpaceX->slider->draw();
mGridSpaceY->slider->draw();

mGridStartX->slider->draw();
mGridStartY->slider->draw();

mGridGapX->slider->draw();
mGridGapY->slider->draw();
*/
}

void ofApp::updateGrid() {
  /*
float startGridX = mGridStartX->ofParamInt;
float startGridY = mGridStartY->ofParamInt;

float stepX = mGridSpaceX->ofParamInt;
float stepY = mGridSpaceY->ofParamInt;

float gapX = mGridGapX->ofParamInt;
float gapY = mGridGapY->ofParamInt;

int maxMarkers = GRID_WIDTH * GRID_HEIGHT;

int i = 0;
int indeY = 0;
for (auto &m : mMarkers) {

  int indeX = (i % (int)GRID_WIDTH);

  float x = indeX * stepX + indeX * gapX + startGridX;
  float y = indeY * stepY + indeY * gapY + startGridY;

  m->setRectPos(glm::vec2(x - stepX / 2.0, y - stepY / 2.0),
                glm::vec2(stepX, stepY));

  m->setPos(glm::vec2(x, y));

  i++;

  if (indeX >= GRID_WIDTH - 1) {
    indeY++;
  }
}
*/
}

//--------------------------------------------------------------
void ofApp::saveJSONBlocks() {
  // save json
  ofJson writer;
  int i = 0;
  for (auto &mk : mMarkers) {
    ofJson pt;
    if (i < GRID_WIDTH * GRID_HEIGHT) {
      pt[to_string(i)]["posx"] = mk->getPos().x;
      pt[to_string(i)]["posy"] = mk->getPos().y;
      pt[to_string(i)]["type"] = mk->getBlockType();
      writer.push_back(pt);
      i++;
    }
  }

  if (mEnableKnob) {
    {
      ofJson pt;
      pt[to_string(i)]["posx"] = mKnobAmenitie->getStaticPos().x;
      pt[to_string(i)]["posy"] = mKnobAmenitie->getStaticPos().y;
      pt[to_string(i)]["type"] = BlockType::knobStatic;
      writer.push_back(pt);
      i++;
    }

    {
      ofJson pt;
      pt[to_string(i)]["posx"] = mKnobAmenitie->getDynamicPos().x;
      pt[to_string(i)]["posy"] = mKnobAmenitie->getDynamicPos().y;
      pt[to_string(i)]["type"] = BlockType::knobDynamic;
      writer.push_back(pt);
    }
  }
  ofLog(OF_LOG_NOTICE) << "json write";
  ofSaveJson("gridpos.json", writer);
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key) {
  if (key == 'g') {
    mDrawGUI = !mDrawGUI;
  }

  if (key == '1') {
    mRecordOnce = true;
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
    saveJSONBlocks();
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
    mDebugGrid = !mDebugGrid;
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
    if (mDebugGrid) {
      for (auto &mk : mMarkers) {
        glm::vec2 pos = mk->getPos();
        float dist = ofDist(pos.x, pos.y, x, y);
        if (dist >= 0.0 && dist <= 15) {
          mk->setPos(glm::vec2(x, y));
        }
      }
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
        float distUp =
            ofDist(mGridImg.at(mCurrentInputIdx)->getCropUp().x,
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
