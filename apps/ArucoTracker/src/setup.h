
void ofApp::setupConnection() {

  ofxUDPSettings settings;
  settings.sendTo("172.20.10.2", 15800);
  settings.blocking = false;
  udpConnection.Setup(settings);

  if (mDebug) {
    string message = "connected to Aruco Detector";
    udpConnection.Send(message.c_str(), message.length());
  }

  ofLog(OF_LOG_NOTICE) << "setup UDP connection " << std::endl;
}
//-----------------------------------------------------------------------------
void ofApp::setupERICS() {
  // mUDPHeader += "header \n";
  mUDPHeader = "ncols         " + to_string(GRID_WIDTH) + "\n";
  mUDPHeader += "nrows         " + to_string(GRID_HEIGHT) + "\n";
  mUDPHeader += "xllcorner     " + std::string("20.0") + "\n";
  mUDPHeader += "yllcorner     " + std::string("30.0") + "\n";
  mUDPHeader += "cellsize      " + std::string("10.0") + "\n";
  mUDPHeader += "NODATA_value  " + std::string("-1") + "\n";

  udpConnection.Send(mUDPHeader.c_str(), mUDPHeader.length());

  ofLog(OF_LOG_NOTICE) << "Header :" << mUDPHeader << std::endl;
}
//-----------------------------------------------------------------------------
void ofApp::setupBlocks() {
  for (int i = 0; i < MAX_MARKERS; i++) {
    BlockRef block = Block::create();
    block->setMarkerId(i);
    mBlocks.push_back(block);
  }

  ofLog(OF_LOG_NOTICE) << "setup blocks " << std::endl;
}
//-----------------------------------------------------------------------------
void ofApp::setupGridPos() {
  ofLog(OF_LOG_NOTICE) << "loading gridpos json";
  ofFile file("gridpos.json");
  if (file.exists()) {
    ofJson js;
    file >> js;
    int i = 0;
    for (auto &gridPos : js) {
      MarkerArucoRef m = MarkerAruco::create();
      m->setMarkerId(-1);

      int type = gridPos[to_string(i)]["type"];

      if (type == BlockType::grid) {

        float posx = gridPos[to_string(i)]["posx"];
        float posy = gridPos[to_string(i)]["posy"];

        m->setRectPos(glm::vec2(posx - 20, posy - 20), glm::vec2(20, 20));

        m->setPos(glm::vec2(posx, posy));
        m->setGridId(i);
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
      MarkerArucoRef m = MarkerAruco::create();
      m->setMarkerId(i);

      int indeX = (i % numW);

      float x = indeX * stepX + indeX * gapX + startGridX;
      float y = indeY * stepY + indeY * gapY + startGridY;

      m->setRectPos(glm::vec2(x - stepX / 2.0, y - stepY / 2.0),
                    glm::vec2(stepX, stepY));

      m->setPos(glm::vec2(x, y));
      m->setGridId(i);
      mMarkers.push_back(m);
      if (indeX >= numW - 1) {
        indeY++;
      }
    }
  }
}
//-----------------------------------------------------------------------------
void ofApp::setupKnob() {

  mKnobAmenitie = KnobAruco::create();
  mEnableKnob = false;
  if (mEnableKnob) {

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
          MarkerArucoRef m = MarkerAruco::create();
          m->setMarkerId(-1);
          m->setRectPos(glm::vec2(posx - 20, posy - 20), glm::vec2(20, 20));
          m->setPos(glm::vec2(posx, posy));
          m->setGridId(i);
          m->setBlockType(BlockType::knobStatic);
          mMarkers.push_back(m);
          std::cout << "knob statics Id:" << i << std::endl;
        }
        if (type == BlockType::knobDynamic) {
          float posx = gridPos[to_string(i)]["posx"];
          float posy = gridPos[to_string(i)]["posy"];
          mKnobAmenitie->setDynamicPos(glm::vec2(posx, posy));
          mKnobAmenitie->setDynamicGridId(i);
          MarkerArucoRef m = MarkerAruco::create();
          m->setMarkerId(-1);
          m->setRectPos(glm::vec2(posx - 20, posy - 20), glm::vec2(20, 20));
          m->setPos(glm::vec2(posx, posy));
          m->setGridId(i);
          m->setBlockType(BlockType::knobDynamic);
          mMarkers.push_back(m);
          std::cout << "knob dynamic Id:" << i << std::endl;
        }

        i++;
      }
      ofLog(OF_LOG_NOTICE) << "setup knob" << std::endl;
    } else {
      ofLog(OF_LOG_NOTICE) << "setup fail knob" << std::endl;
    }
  }
}

//-----------------------------------------------------------------------------
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

  mGammaValue = ofxDatSlider::create();
  mGammaValue->slider =
      new ofxDatGuiSlider(mGammaValue->ofParam.set("gamma", 0.87, 0, 2));
  mGammaValue->slider->setWidth(390, .4);
  mGammaValue->slider->setPosition(sliderStartX, 60);

  mGammaValue->slider->onSliderEvent(
      [&](ofxDatGuiSliderEvent v) { mGammaValue->ofParam = v.value; });
}
//-----------------------------------------------------------------------------
void ofApp::setupDetection() {
  mArucoDetector = Detector::create();
  ofLog(OF_LOG_NOTICE) << "setup detector" << std::endl;
}

//-----------------------------------------------------------------------------
void ofApp::setupCalibration() {
  mArucoDetector->setupCalibration(GRID_WIDTH, GRID_HEIGHT);
  ofLog(OF_LOG_NOTICE) << "setup calibration" << std::endl;
}
//-----------------------------------------------------------------------------
void ofApp::setupVideo() {
  // load video first
  mVideoCapture = 0;

  vidGrabber.setDeviceID(0);
  vidGrabber.setDesiredFrameRate(60);
  vidGrabber.initGrabber(CAM_WIDTH, CAM_HEIGHT);

  gridMovie.load("grid_05.mov");
  gridMovie.setLoopState(OF_LOOP_NORMAL);
  gridMovie.play();

  vidImg.allocate(CAM_WIDTH, CAM_HEIGHT, OF_IMAGE_COLOR);

  ofLog(OF_LOG_NOTICE) << "setup video" << std::endl;
}

//-----------------------------------------------------------------------------
void ofApp::setupCleaner() {

  // cleaner
  mWindowCounter = 0;
  mWindowIterMax = 10;

  // record grid
  mRecordOnce = true;

  ofLog(OF_LOG_NOTICE) << "setup clean" << std::endl;
}

//-----------------------------------------------------------------------------
