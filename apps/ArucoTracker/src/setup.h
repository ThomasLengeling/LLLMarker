
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

  mBDebugGrid = ofxDatButton::create();
  mBDebugGrid->button = new ofxDatGuiToggle("Grid View", false);
  mBDebugGrid->button->setPosition(10, 60);
  mBDebugGrid->button->setWidth(100, .4);
  mBDebugGrid->button->onButtonEvent([&](ofxDatGuiButtonEvent v) {
    mBDebugGrid->mActive = !mBDebugGrid->mActive;
  });

  mBSingleGrid = ofxDatButton::create();
  mBSingleGrid->button = new ofxDatGuiToggle("Single Input", false);
  mBSingleGrid->button->setPosition(10, 110);
  mBSingleGrid->button->setWidth(100, .4);
  mBSingleGrid->button->onButtonEvent([&](ofxDatGuiButtonEvent v) {
    mBSingleGrid->mActive = !mBSingleGrid->mActive;
  });

  mBFullGrid = ofxDatButton::create();
  mBFullGrid->setActivation(false);
  mBFullGrid->button = new ofxDatGuiToggle("Full Input");
  mBFullGrid->button->setPosition(10, 160);
  mBFullGrid->button->setWidth(100, .4);
  mBFullGrid->button->onButtonEvent([&](ofxDatGuiButtonEvent v) {
    mBFullGrid->mActive = !mBFullGrid->mActive;

    if(mBFullGrid->isActive()){
        mFboGridSend.allocate(CAM_WIDTH * 2, CAM_HEIGHT * 2, GL_RGBA);
        mFboGridInfo.allocate(CAM_WIDTH * 2, CAM_HEIGHT * 2, GL_RGBA);
        mFboGridColor.allocate(CAM_WIDTH * 2, CAM_HEIGHT * 2, GL_RGBA);

        mFboFullCam.allocate(CAM_WIDTH * 2, CAM_HEIGHT * 2, GL_RGB);

        // Mat settings for Aruco detector
        vidImg.allocate(CAM_WIDTH * 2, CAM_HEIGHT * 2, OF_IMAGE_COLOR);

        mFboGridSend.begin();
        ofClear(0, 0, 0, 0);
        mFboGridSend.end();

        mFboGridInfo.begin();
        ofClear(0, 0, 0, 5);
        mFboGridInfo.end();

        mFboGridColor.begin();
        ofClear(0, 0, 0, 0);
        mFboGridColor.end();

        mFboFullCam.begin();
        ofClear(0, 0, 0);
        mFboFullCam.end();

        std::cout<<"allocated full img"<<std::endl;
    }else{
        mFboGridSend.allocate(CAM_WIDTH, CAM_HEIGHT, GL_RGBA);
        mFboGridInfo.allocate(CAM_WIDTH, CAM_HEIGHT, GL_RGBA);
        mFboGridColor.allocate(CAM_WIDTH, CAM_HEIGHT, GL_RGBA);

        mFboFullCam.allocate(CAM_WIDTH, CAM_HEIGHT, GL_RGB);

        // Mat settings for Aruco detector
        vidImg.allocate(CAM_WIDTH, CAM_HEIGHT, OF_IMAGE_COLOR);

        mFboGridSend.begin();
        ofClear(0, 0, 0, 0);
        mFboGridSend.end();

        mFboGridInfo.begin();
        ofClear(0, 0, 0, 0);
        mFboGridInfo.end();

        mFboGridColor.begin();
        ofClear(0, 0, 0, 0);
        mFboGridColor.end();

        mFboFullCam.begin();
        ofClear(0, 0, 0);
        mFboFullCam.end();

        std::cout<<"allocated single img"<<std::endl;
      }
  });

  int sliderStartX = 150;

  mBEnableCrop = ofxDatButton::create();
  mBEnableCrop->setActivation(false);
  mBEnableCrop->button = new ofxDatGuiToggle("Toogle Crop");
  mBEnableCrop->button->setPosition(sliderStartX, 60);
  mBEnableCrop->button->setWidth(390, .4);
  mBEnableCrop->button->onButtonEvent([&](ofxDatGuiButtonEvent v) {
    mBEnableCrop->mActive = !mBEnableCrop->mActive;
    mGridImg.at(mCurrentInputIdx)->toogleCrop();
});

    mBEnableVideo = ofxDatButton::create();
    mBEnableVideo->setActivation(false);
    mBEnableVideo->button = new ofxDatGuiToggle("Toogle Video");
    mBEnableVideo->button->setPosition(sliderStartX, 110);
    mBEnableVideo->button->setWidth(390, .4);
    mBEnableVideo->button->onButtonEvent([&](ofxDatGuiButtonEvent v) {
        for (auto &gridImage : mGridImg) {
            gridImage->toogleCam();
        }
    });



  mGammaValue = ofxDatSlider::create();
  mGammaValue->slider =
      new ofxDatGuiSlider(mGammaValue->ofParam.set("gamma", 0.87, 0, 2));
  mGammaValue->slider->setWidth(390, .4);
  mGammaValue->slider->setPosition(sliderStartX, 160);

  mGammaValue->slider->onSliderEvent(
      [&](ofxDatGuiSliderEvent v) { mGammaValue->ofParam = v.value; });

      mBDebugMarkers = ofxDatButton::create();
      mBDebugMarkers->setActivation(false);
      mBDebugMarkers->button = new ofxDatGuiToggle("Toogle Markers Draw");
      mBDebugMarkers->button->setPosition(sliderStartX, 210);
      mBDebugMarkers->button->setWidth(390, .4);
      mBDebugMarkers->button->onButtonEvent([&](ofxDatGuiButtonEvent v) {
          mArucoDetector->toggleMarkerInfo();
      });

}
//-----------------------------------------------------------------------------
void ofApp::setupDetection() {
  mArucoDetector = Detector::create();
  ofLog(OF_LOG_NOTICE) << "setup detector" << std::endl;
}

//-----------------------------------------------------------------------------
void ofApp::setupCalibration() {
  mArucoDetector->setupCalibration(GRID_WIDTH, GRID_HEIGHT);
  ofLog(OF_LOG_NOTICE) << "setup calibration";
}
//-----------------------------------------------------------------------------
void ofApp::setupVideo() {
    mStichImg = false;
    mDebugGrid = false;

    mVideoCapture = true;

  // load video first
  int numInputs = 4;
  mCurrentInputIdx = 0;
  std::string movies [] = {"grid_05.mov", "grid_05.mov", "grid_05.mov","grid_05.mov"};
  for(int i = 0; i < numInputs; i++){
      GridImageRef gridImage = GridImage::create(glm::vec2(CAM_WIDTH, CAM_HEIGHT));
      mGridImg.push_back(gridImage);
  }

  ofFile file("img.json");
  bool foundFile = false;
  if (file.exists()) {
      ofJson js;
      file >> js;
      int j = 0;
      for (auto & cam : js) {
          std::string inputImg("cam" + to_string(j));
          mGridImg.at(j)->setCropUp(glm::vec2(cam[inputImg]["x1"], cam[inputImg]["y1"]));
          mGridImg.at(j)->setCropDown(glm::vec2(cam[inputImg]["x2"], cam[inputImg]["y2"]));
          mGridImg.at(j)->setCropDisp(glm::vec2(cam[inputImg]["disX"], cam[inputImg]["disY"]));
          j++;
      }
      ofLog(OF_LOG_NOTICE) << "end cam values JSON";
      foundFile = true;
  }else{
      ofLog(OF_LOG_NOTICE) << "file does not exist img.json";
  }

  {
      int i =0;
      for (auto &gridImage : mGridImg) {
          gridImage->setupCam(i);
          gridImage->setupVideo(movies[i]);
          i++;
      }
  }




  // fill the fbos with the appropiate dimentions
  mFboGridSend.allocate(CAM_WIDTH * 2, CAM_HEIGHT * 2, GL_RGBA);
  mFboGridInfo.allocate(CAM_WIDTH * 2, CAM_HEIGHT * 2, GL_RGBA);
  mFboGridColor.allocate(CAM_WIDTH * 2, CAM_HEIGHT * 2, GL_RGBA);
  mFboFullCam.allocate(CAM_WIDTH * 2, CAM_HEIGHT * 2, GL_RGB);

  // Mat settings for Aruco detector
  vidImg.allocate(CAM_WIDTH * 2, CAM_HEIGHT * 2, OF_IMAGE_COLOR);

  // clean start with Fbos
  mFboGridSend.begin();
  ofClear(0, 0, 0, 0);
  mFboGridSend.end();

  mFboGridInfo.begin();
  ofClear(0, 0, 0, 0);
  mFboGridInfo.end();

  mFboGridColor.begin();
  ofClear(0, 0, 0, 0);
  mFboGridColor.end();

  mFboFullCam.begin();
  ofClear(0, 0, 0);
  mFboFullCam.end();

  ofLog(OF_LOG_NOTICE) << "setup video";
}

//-----------------------------------------------------------------------------
void ofApp::setupCleaner() {

  // cleaner
  mWindowCounter = 0;
  mWindowIterMax = 6;

  // record grid
  mRecordOnce = true;

  for (int i = 0; i < GRID_WIDTH * GRID_HEIGHT; i++) {
      mIdsCounter.emplace(i, 0); // mFullIds.at(i), 0);
      mProCounter.emplace(i, 0);
  }

  // ids from 0 -1000, max number of counters
  for (int i = 0; i < MAX_MARKERS; i++) {
      mCenterCounter.emplace(i, 0);
  }

  ofLog(OF_LOG_NOTICE) << "setup clean" << std::endl;
}

//-----------------------------------------------------------------------------
