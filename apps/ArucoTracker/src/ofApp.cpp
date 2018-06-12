#include "ofApp.h"

using namespace std;
using namespace cv;

//--------------------------------------------------------------
void ofApp::setup() {

  // DEBUG
  mDebug = true;
  std::cout << "Debuging: " << mDebug << std::endl;

  ofSetBackgroundAuto(false);
  ofSetFrameRate(120);
  ofBackground(0);

  mFlipFbo.allocate(CAM_WIDTH, CAM_HEIGHT, GL_RGBA);
  mFlipFbo.begin();
  ofBackground(0);
  ofClear(0);
  mFlipFbo.end();

  setupCalibration();
  setupGUI();

  // cleaner
  mWindowCounter = 0;
  mWindowIterMax = 20;

  // record grid
  mRecordOnce = true;

  setupConnection();

  std::cout << "setup knob" << std::endl;
  mKnobAmenitie = KnobAruco::create();
  ofFile file("gridpos.json");
  if (file.exists()) {
    ofJson js;
    file >> js;
    int i = 0;
    cout << js << endl;
    for (auto &gridPos : js) {

      int type = gridPos[to_string(i)]["type"];

      if (type == BlockType::knobStatic) {
        float posx = gridPos[to_string(i)]["posx"];
        float posy = gridPos[to_string(i)]["posy"];
        mKnobAmenitie->setStaticPos(glm::vec2(posx, posy));
        mKnobAmenitie->setStaticGridId(i);
        MarkerAruco m;
        m.setId(-1);
        m.setRectPos(glm::vec2(posx - 20, posy - 20), glm::vec2(20, 20));
        m.setPos(glm::vec2(posx, posy));
        m.setGridId(i);
        m.setBlockType(BlockType::knobStatic);
        mMarkers.push_back(m);
        std::cout << "knob statics Id:" << i << std::endl;
      }
      if (type == BlockType::knobDynamic) {
        float posx = gridPos[to_string(i)]["posx"];
        float posy = gridPos[to_string(i)]["posy"];
        mKnobAmenitie->setDynamicPos(glm::vec2(posx, posy));
        mKnobAmenitie->setDynamicGridId(i);
        MarkerAruco m;
        m.setId(-1);
        m.setRectPos(glm::vec2(posx - 20, posy - 20), glm::vec2(20, 20));
        m.setPos(glm::vec2(posx, posy));
        m.setGridId(i);
        m.setBlockType(BlockType::knobDynamic);
        mMarkers.push_back(m);
        std::cout << "knob dynamic Id:" << i << std::endl;
      }

      i++;
    }
  } else {
    std::cout << "setup failed Knob" << std::endl;
  }

  for (auto &m : mMarkers) {
    Block block;
    mBlocks.push_back(block);
  }

  std::cout << "finished setup" << std::endl;
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

void ofApp::setupConnection() {
  ofxUDPSettings settings;
  settings.sendTo("172.20.10.2", 15800);
  settings.blocking = false;

  udpConnection.Setup(settings);
  std::cout << "setup UDP connection " << std::endl;

  string message = "connected to Aruco Detector";
  udpConnection.Send(message.c_str(), message.length());
}

void ofApp::setupGUI() {
  mBDebugVideo = ofxDatButton::create();
  mBDebugVideo->button = new ofxDatGuiToggle("Debug View", true);
  mBDebugVideo->button->setPosition(10, 10);
  mBDebugVideo->button->setWidth(100, .4);
  mBDebugVideo->button->onButtonEvent([&](ofxDatGuiButtonEvent v) {
    mBDebugVideo->mActive = !mBDebugVideo->mActive;
  });
  mBDebugVideo->mActive = true;

  mBCalibrateVideo = ofxDatButton::create();
  mBCalibrateVideo->button = new ofxDatGuiButton("Calibrate Camera");
  mBCalibrateVideo->button->setPosition(10, 60);
  mBCalibrateVideo->button->setWidth(100, .4);
  mBCalibrateVideo->button->onButtonEvent([&](ofxDatGuiButtonEvent v) {
    mBCalibrateVideo->mActive = !mBCalibrateVideo->mActive;
  });
  mBCalibrateVideo->mActive = true;

  mBShowGrid = ofxDatButton::create();
  mBShowGrid->button = new ofxDatGuiButton("Show Grid");
  mBShowGrid->button->setPosition(10, 100);
  mBShowGrid->button->setWidth(100, .4);
  mBShowGrid->button->onButtonEvent([&](ofxDatGuiButtonEvent v) {
    mBShowGrid->mActive = !mBShowGrid->mActive;
  });

  int sliderStartX = 150;

  mGridSpaceX = ofxDatSlider::create();
  mGridSpaceX->slider = new ofxDatGuiSlider(
      mGridSpaceX->ofParamInt.set("grid space X", 50, 0, 200));
  mGridSpaceX->slider->setWidth(390, .4);
  mGridSpaceX->slider->setPosition(sliderStartX, 60);

  mGridSpaceX->slider->onSliderEvent([&](ofxDatGuiSliderEvent v) {
    mGridSpaceX->ofParamInt = v.value;
    updateGrid();
  });

  mGridSpaceY = ofxDatSlider::create();
  mGridSpaceY->slider = new ofxDatGuiSlider(
      mGridSpaceY->ofParamInt.set("grid space Y", 50, 0, 200));
  mGridSpaceY->slider->setWidth(390, .4);
  mGridSpaceY->slider->setPosition(sliderStartX, 90);
  mGridSpaceY->slider->onSliderEvent([&](ofxDatGuiSliderEvent v) {
    mGridSpaceY->ofParamInt = v.value;
    ;
    updateGrid();
  });

  mGridStartX = ofxDatSlider::create();
  mGridStartX->slider =
      new ofxDatGuiSlider(mGridStartX->ofParamInt.set("start  X", 50, 0, 500));
  mGridStartX->slider->setWidth(390, .4);
  mGridStartX->slider->setPosition(sliderStartX, 120);
  mGridStartX->slider->onSliderEvent([&](ofxDatGuiSliderEvent v) {
    mGridStartX->ofParamInt = v.value;
    ;
    updateGrid();
  });

  mGridStartY = ofxDatSlider::create();
  mGridStartY->slider =
      new ofxDatGuiSlider(mGridStartY->ofParamInt.set("start  Y", 50, 0, 500));
  mGridStartY->slider->setWidth(390, .4);
  mGridStartY->slider->setPosition(sliderStartX, 150);
  mGridStartY->slider->onSliderEvent([&](ofxDatGuiSliderEvent v) {
    mGridStartY->ofParamInt = v.value;
    ;
    updateGrid();
  });

  mGridGapX = ofxDatSlider::create();
  mGridGapX->slider =
      new ofxDatGuiSlider(mGridGapX->ofParamInt.set("grid gap X", 50, 0, 200));
  mGridGapX->slider->setWidth(390, .4);
  mGridGapX->slider->setPosition(sliderStartX, 180);
  mGridGapX->slider->onSliderEvent([&](ofxDatGuiSliderEvent v) {
    mGridGapX->ofParamInt = v.value;
    ;
    updateGrid();
  });

  mGridGapY = ofxDatSlider::create();
  mGridGapY->slider =
      new ofxDatGuiSlider(mGridGapY->ofParamInt.set("grid gap Y", 50, 0, 200));
  mGridGapY->slider->setWidth(390, .4);
  mGridGapY->slider->setPosition(sliderStartX, 210);
  mGridGapY->slider->onSliderEvent([&](ofxDatGuiSliderEvent v) {
    mGridGapY->ofParamInt = v.value;
    updateGrid();
  });
}

void ofApp::setupCalibration() {

  int markersX = GRID_WIDTH;
  int markersY = GRID_HEIGHT;
  float markerLength = 0.0162;     // 0.0165
  float markerSeparation = 0.0042; // 0045
  int dictionaryId = 11;
  string outputFile = "./cal.txt";

  int calibrationFlags = 0;
  float aspectRatio = 1;

  /*
  if (parser.has("a")) {
    calibrationFlags |= CALIB_FIX_ASPECT_RATIO;
    aspectRatio = parser.get<float>("a");


  if (parser.get<bool>("zt"))
    calibrationFlags |= CALIB_ZERO_TANGENT_DIST;
  if (parser.get<bool>("pc"))
    calibrationFlags |= CALIB_FIX_PRINCIPAL_POINT;
    */

  detectorParams = aruco::DetectorParameters::create();

  // detectorParams->adaptiveThreshWinSizeMin = 5;
  // detectorParams->adaptiveThreshWinSizeMax = 50;
  // detectorParams->adaptiveThreshWinSizeStep = 10;

  detectorParams->perspectiveRemovePixelPerCell = 10;
  detectorParams->perspectiveRemoveIgnoredMarginPerCell = 0.3;

  detectorParams->errorCorrectionRate = 0.55;
  detectorParams->maxErroneousBitsInBorderRate = 0.3;

  detectorParams->minOtsuStdDev = 2;

  bool refindStrategy = false;
  int camId = 0;
  String video;

  vidGrabber.setDeviceID(0);
  vidGrabber.setDesiredFrameRate(60);
  vidGrabber.initGrabber(CAM_WIDTH, CAM_HEIGHT);

  vidImg.allocate(CAM_WIDTH, CAM_HEIGHT, OF_IMAGE_COLOR);

  // inputVideo.open(camId);
  // if (!inputVideo.isOpened()) { // check if we succeeded
  //    std::cout << "error input cam" << std::endl;
  // }

  std::cout << "loading input video" << std::endl;

  dictionary = aruco::getPredefinedDictionary(
      aruco::PREDEFINED_DICTIONARY_NAME(dictionaryId));

  // create board object
  Ptr<aruco::GridBoard> gridboard = aruco::GridBoard::create(
      markersX, markersY, markerLength, markerSeparation, dictionary);
  board = gridboard.staticCast<aruco::Board>();

  // collected frames for calibration
  vector<vector<vector<Point2f>>> allCorners;
  vector<vector<int>> allIds;

  {
    std::cout << "loading gridpos json" << std::endl;
    ofFile file("gridpos.json");
    if (file.exists()) {
      ofJson js;
      file >> js;
      int i = 0;
      for (auto &gridPos : js) {
        MarkerAruco m;
        m.setId(-1);

        int type = gridPos[to_string(i)]["type"];

        if (type == BlockType::grid) {

          float posx = gridPos[to_string(i)]["posx"];
          float posy = gridPos[to_string(i)]["posy"];

          m.setRectPos(glm::vec2(posx - 20, posy - 20), glm::vec2(20, 20));

          m.setPos(glm::vec2(posx, posy));
          m.setGridId(i);
          mMarkers.push_back(m);
        }
        i++;
      }
    } else {
      // fill markers
      // set the max region
      float startGridX = 1280 * 0.05;
      float startGridY = 720 * 0.05;

      float stepX = 50.0;
      float stepY = 50.0;

      float gapX = 3;
      float gapY = 3;

      int numW = GRID_WIDTH;
      int numH = GRID_HEIGHT;

      int maxMarkers = numW * numH;
      int indeY = 0;
      for (int i = 0; i < maxMarkers; i++) {
        MarkerAruco m;
        m.setId(i);

        int indeX = (i % numW);

        float x = indeX * stepX + indeX * gapX + startGridX;
        float y = indeY * stepY + indeY * gapY + startGridY;

        m.setRectPos(glm::vec2(x - stepX / 2.0, y - stepY / 2.0),
                     glm::vec2(stepX, stepY));

        m.setPos(glm::vec2(x, y));
        m.setGridId(i);
        mMarkers.push_back(m);
        if (indeX >= numW - 1) {
          indeY++;
        }
      }
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
            if (dis >= 0 && dis <= 18) {
              idsCounter[k] = centros.mId;
              mk.incProba();
              // not sure i need it break;
            }
            k++;
          }
        }
      }

      int i = 0;
      std::string enables = "e";
      std::string ids = "i";
      for (auto &mk : mMarkers) {
        float proba = mk.getProba(mWindowIterMax);
        enables += " ";
        ids += " ";
        if (proba >= 0.2) {
          mk.enableOn();
          mk.setId(idsCounter[i]);

          enables += "1";
          ids += to_string(mk.getId());

          if (mk.getBlockType() == BlockType::knobStatic) {
            std::string msg =
                "skn " + to_string(mk.getId()) + " " +
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
        }
        i++;
      }

      // send udp with on or off
      udpConnection.Send(enables.c_str(), enables.length());
      udpConnection.Send(ids.c_str(), ids.length());

      /// blocks
      {
        std::string types = "t";
        for (auto &block : mBlocks) {
          types += " ";
          int t = 0;
          t = static_cast<int>(block.mMType.getType());
          types += to_string(t);
        }
        // send udp with on or off
        udpConnection.Send(types.c_str(), types.length());
      }
    }

    // done activation and disactivation

    mControids.clear();
  }
  mWindowCounter++;
}

//--------------------------------------------------------------
void ofApp::update() {

  vidGrabber.update();

  if (vidGrabber.isFrameNew()) {

    mFlipFbo.begin();
    vidGrabber.draw(CAM_WIDTH, 0, -CAM_WIDTH, CAM_HEIGHT);
    mFlipFbo.end();

    ofPixels pixels = vidGrabber.getPixels();
    // pixels.mirror(false, true);
    pixels.rotate90(2);

    // pixels.mirror(true, false);

    // ofImage image;
    // image.setFromPixels(pixels);
    // image.rotate90(3);
    // image.mirror(true, false);
    // image.update();
    Mat imageCopy = ofxCv::toCv(pixels);

    // Mat imageCopy = ofxCv::toCv(mFlipFbo.getTexture());
    // Mat imageCopy0 = ofxCv::toCv(vidGrabber.getPixels());
    // Mat imageCopy = ofxCv::toCv(vidGrabber.getPixels());
    // Mat imageCopy;
    // ofxCv::imitate(imageCopy, imageCopy0);
    // cv::flip(imageCopy0, imageCopy, 1);
    // image.update();

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
          if (dis >= 0.0 && dis <= 28) {
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
          block.mId = mMarkers.at(i).getId();
          std::cout << block.mId << " " << mMarkers.at(i).getId() << std::endl;
          i++;
        }
      }

      /*
            std::cout << "ids" << std::endl;
            for (auto &id : mFullIds) {
              std::cout << id << std::endl;
            }
      */
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
    for (auto &block : mBlocks) {
      int id = block.mId;
      // get marker Id from knob and update the block
      if (id == mKnobAmenitie->getDynamicId()) {
        block.mMType = mKnobAmenitie->getType();
        std::cout << block.mId << " " << mKnobAmenitie->getType().getType()
                  << " " << mKnobAmenitie->getDynamicGridId() << std::endl;
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
          if (dis >= 0.0 && dis < 32) {
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

    mKnobAmenitie->drawArc();
  }

  /*
    sort(mControid.begin(), mControid.end(),
         [](const auto &lhs, const auto &rhs) { return lhs.id < rhs.id; });
    int i = 0;
    for (auto &cen : mControid) {
      ofSetColor(i);
      ofDrawCircle(cen.pos.x, cen.pos.y, 3, 3);
      ofDrawBitmapString(i, cen.pos.x - 8, cen.pos.y);
      i++;
    }

    int j = 0;
    for (auto &mk : mMarkers) {
      glm::vec2 pos = mk.getPos();

      ofSetColor(i);
      ofDrawCircle(pos.x, pos.y, 5, 5);
      j++;
    }
    */

  if (mBShowGrid->mActive) {
    ofSetColor(0, 200, 200);
    for (int i = 0; i < centroid.size(); i++) {
      ofDrawCircle(centroid.at(i).x, centroid.at(i).y, 5, 5);
      ofDrawBitmapString(i, centroid.at(i).x, centroid.at(i).y);
      ofDrawBitmapString(tagsIds.at(i), centroid.at(i).x - 7,
                         centroid.at(i).y - 7);
    }

    int r = 0;
    for (auto &mk : mMarkers) {
      glm::vec2 pos = mk.getPos();

      ofSetColor(50, 100, 220);
      ofDrawCircle(pos.x, pos.y, 10, 10);

      int k = 0;

      for (auto &cen : mControid) {
        glm::vec2 cenPos = cen.mPos;
        float dis = ofDist(cenPos.x, cenPos.y, pos.x, pos.y);
        if (dis >= 0.0 && dis < 10) {
          ofSetColor(255, 0, 0);
          ofDrawCircle(pos.x, pos.y, 7, 7);
          mk.setId(tagsIds.at(k));
          r++;
          break;
        }
        k++;
      }

      ofSetColor(255);
      ofDrawBitmapString(mk.getId(), pos.x - 20, pos.y - 7);

      mk.drawRect();
    }

    // std::cout << "registered: " << r << std::endl;
  }

  mKnobAmenitie->draw();

  // record grid
  recordGrid();

  // draw GUI
  if (mDrawGUI) {
    drawGUI();
  }

  // draw results
  // image.copyTo(imageCopy);
  ofSetColor(255);
  ofDrawBitmapString(foundMarkers, ofGetWidth() - 100, 20);
  ofDrawBitmapString(ofGetFrameRate(), ofGetWidth() - 100, 40);
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

  if (key == 'd') {
    mDebug = !mDebug;
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
