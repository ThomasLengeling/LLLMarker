#include "ofApp.h"
#include "setup.h"

using namespace std;
using namespace cv;

//--------------------------------------------------------------
void ofApp::setup() {

  // DEBUG
  mDebug = true;
  ofLog(OF_LOG_NOTICE) << "Debuging: " << mDebug << std::endl;

  mMinFoundId = 100;
  mMaxFoundId = 0;

  ofSetBackgroundAuto(false);
  ofSetFrameRate(120);
  ofBackground(0);

  setupCalibration();

  setupGridPos();

  setupGUI();

  setupCleaner();

  setupConnection();

  setupKnob();

  setupBlocks();

  ofLog(OF_LOG_NOTICE) << "finished setup" << std::endl;

  // mUDPHeader += "header \n";
  mUDPHeader = "ncols " + to_string(GRID_WIDTH) + " \n";
  mUDPHeader += "nrows " + to_string(GRID_HEIGHT) + " \n";
  mUDPHeader += "xllcorner " + to_string(0) + " \n";
  mUDPHeader += "yllcorner " + to_string(0) + " \n";
  mUDPHeader += "cellsize " + to_string(1) + " \n";
  mUDPHeader += "NODATA_value " + to_string(-1) + " \n";

  ofLog(OF_LOG_NOTICE) << "Header :" << mUDPHeader << std::endl;

  udpConnection.Send(mUDPHeader.c_str(), mUDPHeader.length());
}

void ofApp::updateGrid() {
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

    m.setRectPos(glm::vec2(x - stepX / 2.0, y - stepY / 2.0),
                 glm::vec2(stepX, stepY));

    m.setPos(glm::vec2(x, y));

    i++;

    if (indeX >= GRID_WIDTH - 1) {
      indeY++;
    }
  }
}

void ofApp::cleanDetection() {
  if (mWindowCounter >= mWindowIterMax) {
    mWindowCounter = 0;

    if (mFullIds.size() > 0) {

      // clasical probabilty of ocurance
      std::map<int, int> idsCounter;
      std::map<int, int> proCounter;
      std::map<int, int> centerCounter;

      for (int i = 0; i < GRID_WIDTH * GRID_HEIGHT; i++) {
        idsCounter.emplace(i, 0); // mFullIds.at(i), 0);
        proCounter.emplace(i, 0);
      }

      // ids from 0 -1000, max number of counters
      for (int i = 0; i < MAX_MARKERS; i++) {
        centerCounter.emplace(i, 0);
      }

      for (auto &mk : mMarkers) {
        mk.resetProba();
      }

      // calculate the frequency of ocurance
      for (auto &markers : mControids) {
        for (auto &centros : markers) {

          glm::vec2 pos = centros.mPos;

          int k = 0;
          for (auto &mk : mMarkers) {
            glm::vec2 boardPos = mk.getPos();
            float dis = ofDist(pos.x, pos.y, boardPos.x, boardPos.y);
            if (dis >= 0 && dis <= RAD_DETECTION) {
              idsCounter[k] = centros.mId;
              mk.incProba();
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
        float proba = mk.getProba(mWindowIterMax);
        enables += " ";
        ids += " ";
        if (proba >= 0.15) {
          mk.enableOn();
          mk.setId(idsCounter[i]);
          mk.updateId(idsCounter[i]);

          enables += "1";
          ids += to_string(mk.getIdTypePair().first);

          if (i < GRID_HEIGHT * GRID_WIDTH) {
            fileIds += to_string(mk.getIdTypePair().first);
            fileIds += " ";
          }

          // find id and update it;
          for (auto &blocks : mBlocks) {
            if (blocks.mIdType.first == mk.getIdTypePair().first) {
              mk.updateType(blocks.mIdType.second);
            }
          }

          if (mk.getBlockType() == BlockType::knobStatic) {
            std::string msg =
                "skn " + to_string(mk.getIdTypePair().first) + " " +
                to_string(
                    static_cast<int>(mKnobAmenitie->getType().getType())) +
                " " + to_string(mk.getPos().x) + " " + to_string(mk.getPos().y);

            cout << "sent: " << msg << std::endl;
            udpConnection.Send(msg.c_str(), msg.length());
          }

        } else {
          mk.enableOff();
          mk.setId(-1);

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
      mTypeFile.open("ids.acs", ofFile::WriteOnly);
      mTypeFile << mUDPHeader << fileIds;

      // send udp with on or off
      udpConnection.Send(enables.c_str(), enables.length());
      udpConnection.Send(ids.c_str(), ids.length());

      /// blocks
      {
        std::string types = "type \n";
        for (auto &mk : mMarkers) {
          int t = 0;
          t = static_cast<int>(mk.getIdTypePair().second);
          types += to_string(t);
          types += " ";
        }
        // send udp with on or off
        udpConnection.Send(ids.c_str(), ids.length());
      }
    }

    // done activation and disactivation

    mControids.clear();
    /// mMinFoundId = 100;
    // mMaxFoundId = 0;

    std::cout << "min " << mMinFoundId << " max " << mMaxFoundId << std::endl;
  }
  mWindowCounter++;
}

//--------------------------------------------------------------
void ofApp::update() {

  bool newFrame = false;
  ofPixels pixels;
  if (mVideoCapture) {
    vidGrabber.update();
    newFrame = vidGrabber.isFrameNew();
    pixels = vidGrabber.getPixels();
  } else {
    gridMovie.update();
    newFrame = gridMovie.isFrameNew();
    pixels = gridMovie.getPixels();
  }

  if (newFrame) {
    pixels.rotate90(2);
    Mat imageCopy = ofxCv::toCv(pixels);

    // detect markers
    ids.clear();
    corners.clear();
    rejected.clear();
    centroid.clear();
    tagsIds.clear();
    mControid.clear();

    aruco::detectMarkers(imageCopy, dictionary, corners, ids, detectorParams);
    aruco::refineDetectedMarkers(imageCopy, board, corners, ids, rejected);

    if (ids.size() > 0) {

      aruco::drawDetectedMarkers(imageCopy, corners, ids);

      InputArrayOfArrays cornersDetected = corners;
      InputArray idsDetected = ids;

      foundMarkers = cornersDetected.total();

      // analize which markers are activated in the grid.
      for (int i = 0; i < cornersDetected.total(); i++) {
        cv::Mat currentMarker = cornersDetected.getMat(i);
        cv::Point2f cent(0, 0);

        for (int p = 0; p < 4; p++) {
          cent += currentMarker.ptr<cv::Point2f>(0)[p];
        }

        cent = cent / 4.;
        Block cva;
        cva.mPos = glm::vec2(cent.x, cent.y);
        centroid.push_back(glm::vec2(cent.x, cent.y));

        // get ids
        if (idsDetected.total() != 0) {
          int id = idsDetected.getMat().ptr<int>(0)[i];
          tagsIds.push_back(id);
          cva.mId = id;

          if (mMinFoundId > id) {
            mMinFoundId = id;
          }
          if (mMaxFoundId < id) {
            mMaxFoundId = id;
          }
        }

        mControid.push_back(cva);
      }
      mControids.push_back(mControid);
      cleanDetection();

      imageCopy.copyTo(vidMat);

      ofxCv::toOf(vidMat, vidImg.getPixels());
      vidImg.update();
    }
    // std::cout << "got img and aruco" << std::endl;
  }

  // udate
  if (mDrawGUI) {
    updateGUI();
  }
}
//--------------------------------------------------------------
void ofApp::drawArucoMarkers() {}

void ofApp::recordGrid() {
  if (mRecordOnce) {
    if (mControid.size() == mMarkers.size()) {
      std::cout << mControid.size() << " markes = " << GRID_WIDTH * GRID_HEIGHT
                << std::endl;

      // set ids
      mFullIds.clear();
      for (auto &mk : mMarkers) {
        glm::vec2 pos = mk.getPos();
        int k = 0;
        for (auto &centroDet : mControid) {
          glm::vec2 cenPos = centroDet.mPos;
          float dis = ofDist(cenPos.x, cenPos.y, pos.x, pos.y);
          if (dis >= 0.0 && dis <= RAD_DETECTION) {
            mk.setId(tagsIds.at(k));
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
            if (block.mIdType.first == marker.getIdTypePair().first) {
              block.mIdType.first = marker.getIdTypePair().first;
              std::cout << block.mIdType.first << " "
                        << marker.getIdTypePair().first << std::endl;
              break;
            }
            i++;
          }
        }
      }

      std::cout << "num Uniques " << mFullIds.size() << std::endl;

      mRecordOnce = false;
      std::cout << "finished recording" << std::endl;
    }
  }
}

//--------------------------------------------------------------
void ofApp::draw() {
  ofSetColor(0, 0, 0, 255);
  ofRect(0, 0, ofGetWidth(), ofGetHeight());

  if (mBDebugVideo->mActive) {
    if (vidImg.isAllocated()) {

      ofSetColor(255);
      vidImg.draw(0, 0, ofGetWidth(), ofGetHeight());
      // vidGrabber.draw(0, 240, 320, 240);
    }
  }

  if (mBCalibrateVideo->mActive) {
    if (vidImg.isAllocated()) {

      ofSetColor(255);
      vidGrabber.draw(0, 0, 240, 240);
      vidImg.draw(0, 0, ofGetWidth(), ofGetHeight());
    }

    // update block

    {
      int type = 0;
      for (auto &block : mBlocks) {
        int id = block.mIdType.first;
        // get marker Id from knob and update the block
        if (id == mKnobAmenitie->getDynamicId()) {
          block.mIdType.second = mKnobAmenitie->getType().getType();
          type = block.mIdType.second;
          std::cout << "changed id: " << block.mIdType.first << " "
                    << mKnobAmenitie->getType().getType() << " "
                    << mKnobAmenitie->getDynamicGridId() << std::endl;

          // 149
        }
      }
    }

    // update block to the grid
    for (auto &block : mBlocks) {
      int id = block.mIdType.first;

      int roadsId[] = {207, 257, 39,  154, 135, 79,  149, 174, 120, 176, 43,
                       250, 52,  119, 156, 29,  81,  168, 61,  152, 190, 189,
                       150, 187, 167, 185, 247, 227, 181, 124, 85,  179, 140,
                       206, 222, 232, 97,  219, 218, 217, 216, 215, 214, 212,
                       40,  213, 255, 101, 36,  49,  26,  89,  164, 228, 246,
                       183, 245, 201, 215, 261, 95};
      for (int i = 0; i < sizeof(roadsId) / sizeof(roadsId[0]); i++) {
        if (id == roadsId[i]) {
          block.mIdType.second = mKnobAmenitie->getType().getType();
        }
      }

      for (auto &mk : mMarkers) {
        if (mk.getIdTypePair().first == id) {
          mk.updateType(block.mIdType.second);
          break;
        }
      }
    }

    for (auto &mk : mMarkers) {
      glm::vec2 pos = mk.getPos();

      if (mk.isEnable()) {
        ofSetColor(255);
        ofDrawCircle(pos.x, pos.y, 7, 7);
      } else {
        ofSetColor(255, 255, 0);
        ofDrawCircle(pos.x, pos.y, 4, 4);
      }
      ofSetColor(255);
      ofDrawBitmapString(mk.getId(), pos.x - 20, pos.y - 7);

      ofDrawBitmapString(mk.getGridId(), pos.x - 25, pos.y - 17);

      if (mk.getGridId() == mKnobAmenitie->getDynamicGridId()) {

        for (auto &cen : mControid) {
          glm::vec2 cenPos = cen.mPos;
          float dis = ofDist(cenPos.x, cenPos.y, pos.x, pos.y);
          if (dis >= 0.0 && dis < RAD_DETECTION) {
            mKnobAmenitie->setDynamicPos(cenPos);
            mKnobAmenitie->setDynamicId(cen.mId);
            mk.setPos(cenPos);

            ofDrawBitmapString(mk.getGridId(), pos.x - 25, pos.y - 17);

            {
              std::string msg = "dkn " + to_string(cenPos.x) + " " +
                                to_string(cenPos.y) + " " +
                                to_string(mKnobAmenitie->getDynamicId());

              std::cout << msg << std::endl;

              udpConnection.Send(msg.c_str(), msg.length());
            }
            break;
          }
        }
      }
    }
  }

  if (mBShowGrid->mActive) {
  }

  mKnobAmenitie->drawArc();
  mKnobAmenitie->draw();

  // record grid
  recordGrid();

  // draw GUI
  if (mDrawGUI) {
    drawGUI();
  }

  // draw results
  ofSetColor(255);
  ofDrawBitmapString(foundMarkers, ofGetWidth() - 100, 20);
  ofDrawBitmapString(ofGetFrameRate(), ofGetWidth() - 100, 40);
}

//--------------------------------------------------------------
void ofApp::updateGUI() {
  mBDebugVideo->button->update();
  mBShowGrid->button->update();
  mBCalibrateVideo->button->update();

  mGridSpaceX->slider->update();
  mGridSpaceY->slider->update();

  mGridStartX->slider->update();
  mGridStartY->slider->update();

  mGridGapX->slider->update();
  mGridGapY->slider->update();
}

//--------------------------------------------------------------
void ofApp::drawGUI() {
  mBDebugVideo->button->draw();
  mBShowGrid->button->draw();
  mBCalibrateVideo->button->draw();

  mGridSpaceX->slider->draw();
  mGridSpaceY->slider->draw();

  mGridStartX->slider->draw();
  mGridStartY->slider->draw();

  mGridGapX->slider->draw();
  mGridGapY->slider->draw();
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

  if (key == '2') {
    // save json

    ofJson writer;
    int i = 0;
    for (auto &mk : mMarkers) {
      ofJson pt;
      if (i < GRID_WIDTH * GRID_HEIGHT) {
        pt[to_string(i)]["posx"] = mk.getPos().x;
        pt[to_string(i)]["posy"] = mk.getPos().y;
        pt[to_string(i)]["type"] = mk.getBlockType();
        writer.push_back(pt);
      }
      i++;
    }

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
    std::cout << "json write" << std::endl;
    ofSaveJson("gridpos.json", writer);
  }

  if (key == 'v') {
    mVideoCapture = !mVideoCapture;
  }
  if (key == 'd') {
    mDebug = !mDebug;
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
    for (auto &mk : mMarkers) {
      glm::vec2 pos = mk.getPos();
      float dist = ofDist(pos.x, pos.y, x, y);
      if (dist >= 0.0 && dist <= 15) {
        mk.setPos(glm::vec2(x, y));
      }
    }

    // Physical GUI
    {
      glm::vec2 pos = mKnobAmenitie->getStaticPos();

      float dist = ofDist(pos.x, pos.y, x, y);
      if (dist >= 0.0 && dist <= 15) {
        mKnobAmenitie->setStaticPos(glm::vec2(x, y));
      }
    }

    {
      glm::vec2 pos = mKnobAmenitie->getDynamicPos();

      float dist = ofDist(pos.x, pos.y, x, y);
      if (dist >= 0.0 && dist <= 15) {
        mKnobAmenitie->setDynamicPos(glm::vec2(x, y));
      }
    }
    //
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

// Calibrate
//--------------------------------------------------------------
bool ofApp::readDetectorParameters(
    std::string filename, cv::Ptr<cv::aruco::DetectorParameters> &params) {
  FileStorage fs(filename, FileStorage::READ);
  if (!fs.isOpened())
    return false;
  fs["adaptiveThreshWinSizeMin"] >> params->adaptiveThreshWinSizeMin;
  fs["adaptiveThreshWinSizeMax"] >> params->adaptiveThreshWinSizeMax;
  fs["adaptiveThreshWinSizeStep"] >> params->adaptiveThreshWinSizeStep;
  fs["adaptiveThreshConstant"] >> params->adaptiveThreshConstant;
  fs["minMarkerPerimeterRate"] >> params->minMarkerPerimeterRate;
  fs["maxMarkerPerimeterRate"] >> params->maxMarkerPerimeterRate;
  fs["polygonalApproxAccuracyRate"] >> params->polygonalApproxAccuracyRate;
  fs["minCornerDistanceRate"] >> params->minCornerDistanceRate;
  fs["minDistanceToBorder"] >> params->minDistanceToBorder;
  fs["minMarkerDistanceRate"] >> params->minMarkerDistanceRate;
  fs["cornerRefinementMethod"] >> params->cornerRefinementMethod;
  fs["cornerRefinementWinSize"] >> params->cornerRefinementWinSize;
  fs["cornerRefinementMaxIterations"] >> params->cornerRefinementMaxIterations;
  fs["cornerRefinementMinAccuracy"] >> params->cornerRefinementMinAccuracy;
  fs["markerBorderBits"] >> params->markerBorderBits;
  fs["perspectiveRemovePixelPerCell"] >> params->perspectiveRemovePixelPerCell;
  fs["perspectiveRemoveIgnoredMarginPerCell"] >>
      params->perspectiveRemoveIgnoredMarginPerCell;
  fs["maxErroneousBitsInBorderRate"] >> params->maxErroneousBitsInBorderRate;
  fs["minOtsuStdDev"] >> params->minOtsuStdDev;
  fs["errorCorrectionRate"] >> params->errorCorrectionRate;
  return true;
}

//--------------------------------------------------------------
bool ofApp::saveCameraParams(const std::string &filename, cv::Size imageSize,
                             float aspectRatio, int flags,
                             const cv::Mat &cameraMatrix,
                             const cv::Mat &distCoeffs, double totalAvgErr) {

  FileStorage fs(filename, FileStorage::WRITE);
  if (!fs.isOpened())
    return false;

  time_t tt;
  time(&tt);
  struct tm *t2 = localtime(&tt);
  char buf[1024];
  strftime(buf, sizeof(buf) - 1, "%c", t2);

  fs << "calibration_time" << buf;

  fs << "image_width" << imageSize.width;
  fs << "image_height" << imageSize.height;

  if (flags & CALIB_FIX_ASPECT_RATIO)
    fs << "aspectRatio" << aspectRatio;

  if (flags != 0) {
    sprintf(buf, "flags: %s%s%s%s",
            flags & CALIB_USE_INTRINSIC_GUESS ? "+use_intrinsic_guess" : "",
            flags & CALIB_FIX_ASPECT_RATIO ? "+fix_aspectRatio" : "",
            flags & CALIB_FIX_PRINCIPAL_POINT ? "+fix_principal_point" : "",
            flags & CALIB_ZERO_TANGENT_DIST ? "+zero_tangent_dist" : "");
  }

  fs << "flags" << flags;

  fs << "camera_matrix" << cameraMatrix;
  fs << "distortion_coefficients" << distCoeffs;

  fs << "avg_reprojection_error" << totalAvgErr;

  return true;
}
