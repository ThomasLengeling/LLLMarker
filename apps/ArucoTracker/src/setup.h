void ofApp::setupValues() {
  // num Inputs
  mNumInputs        = NUM_CAM_INPUTS;

  mDrawGUI          = true;
  mSortMarkers      = false;
  mRefimentDetector = true;
  mCurrentInputIdx  = 0;
  mTotalMaxMarkers  = 0;

  //load imputs for #cameras
  if(mNumInputs > 1){
    ofFile file("griddef.json");
    if (file.exists()) {
      ofJson js;
      file >> js;
      int j = 0;
      for (auto & grid : js) {
        std::string inputGrid("grid" + to_string(j));
        mGridSizes.push_back( glm::vec2(grid[inputGrid]["x"], grid[inputGrid]["y"] ) );
        mMaxMarkers.push_back( grid[inputGrid]["max"]);
        mTotalMaxMarkers += grid[inputGrid]["max"].get<int>();
        ofLog(OF_LOG_NOTICE) <<j<<": "<<glm::vec2(grid[inputGrid]["x"], grid[inputGrid]["y"] ) <<" "<< grid[inputGrid]["max"]<<std::endl;
        j++;
      }
      ofLog(OF_LOG_NOTICE) << "Done grid values JSON";
    } else {
      ofLog(OF_LOG_NOTICE) << "File does not exist griddef.json";
    }
  }

  //load for GUI interaction
  if(INPUT_KNOB == 1){
    ofLog(OF_LOG_NOTICE) << "Setting up grid gui definition";
    ofFile file("griddef.json");
    if (file.exists()) {
      ofJson gridjs;
      file >> gridjs;
      std::string inputGrid("grid_gui");

      mGridSizes.push_back( glm::vec2(gridjs[inputGrid]["x"], gridjs[inputGrid]["y"] ) );
      mMaxMarkers.push_back( gridjs[inputGrid]["max"]);
      mTotalMaxMarkers += gridjs[inputGrid]["max"].get<int>();

      ofLog(OF_LOG_NOTICE) << glm::vec2(gridjs[inputGrid]["x"], gridjs[inputGrid]["y"] ) <<" "<< gridjs[inputGrid]["max"]<<std::endl;

      ofLog(OF_LOG_NOTICE) << "Done grid values JSON";
    } else {
      ofLog(OF_LOG_NOTICE) << "File does not exist griddef.json";
    }
  }

  ofLog(OF_LOG_NOTICE) << "Max Grid: " << mFullGridDim.x << " " << mFullGridDim.y;

  // load video first
  mTotalMarkers = 0;

  ofLog(OF_LOG_NOTICE) << "done setup init";
}

void ofApp::setupConnection() {
  std::string jsonNet = "networkUDP.json";
  ofLog(OF_LOG_NOTICE) << "Setup newtwork: " << jsonNet;
  ofFile file(jsonNet);
  if (file.exists()) {
    ofJson js;
    file >> js;
    int i = 0;
    for (auto &net : js) {
      if (i == 0) {
        mUDPIp = net["network_" + to_string(i)]["ip"];
        mUDPPort = int(net["network_" + to_string(i)]["port"]);
      } else if (i == 1) {
        mUDPRadarIp = net["network_" + to_string(i)]["ip"];
        mUDPRadarPort = int(net["network_" + to_string(i)]["port"]);
      }
      i++;
    }
    ofLog(OF_LOG_NOTICE) << "Loaded: UDP Table:";
    ofLog(OF_LOG_NOTICE) << "IP: " << mUDPIp << " Port: " << mUDPPort;

    ofLog(OF_LOG_NOTICE) << "Loaded: UDP Radar:";
    ofLog(OF_LOG_NOTICE) << "IP: " << mUDPRadarIp << " Port: " << mUDPRadarPort;
  } else {
    mUDPIp = "127.0.0.1";
    mUDPPort = 15800;
    ofLog(OF_LOG_NOTICE) << "fail loading newtwork: " << jsonNet
                         << " Default: " << mUDPIp << " " << mUDPPort;
  }

  // radar
  // ofxUDPSettings settingsRadar;
  // settingsRadar.sendTo(mUDPRadarIp, mUDPRadarPort);
  // settingsRadar.blocking = false;
  // udpConnectionRadar.Setup(settingsRadar);

  // table
  ofxUDPSettings settingsTable;
  settingsTable.sendTo(mUDPIp, mUDPPort);
  settingsTable.blocking = false;
  udpConnection.Setup(settingsTable);

  if (mDebug) {
    string message = "connected to Aruco Detector";
    udpConnection.Send(message.c_str(), message.length());
  }

  ofLog(OF_LOG_NOTICE) << "done setup UDP connection ";
}
//-----------------------------------------------------------------------------
void ofApp::setupERICS() {
  // mUDPHeader += "header \n";
  mUDPHeader = "ncols         " + to_string(mFullGridDim.x) + "\n";
  mUDPHeader += "nrows         " + to_string(mFullGridDim.y) + "\n";
  mUDPHeader += "xllcorner     " + std::string("20.0") + "\n";
  mUDPHeader += "yllcorner     " + std::string("30.0") + "\n";
  mUDPHeader += "cellsize      " + std::string("10.0") + "\n";
  mUDPHeader += "NODATA_value  " + std::string("-1") + "\n";

  udpConnection.Send(mUDPHeader.c_str(), mUDPHeader.length());

  ofLog(OF_LOG_NOTICE) << "Header :" << std::endl << mUDPHeader;
  ofLog(OF_LOG_NOTICE) << "done format";
}

//-----------------------------------------------------------------------------
void ofApp::setupKnob() {

  mKnobAmenitie = KnobAruco::create();
  mEnableKnob = false;
  if (mEnableKnob) {
    ofFile file("knobpos.json");
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
          m->setRectPos(
              glm::vec2(posx - 20, posy - 20), glm::vec2(posx + 20, posy - 20),
              glm::vec2(posx + 20, posy + 20), glm::vec2(posx - 20, posy + 20));
          m->setPos(glm::vec2(posx, posy));
          m->setGridId(i);
          m->setBlockType(BlockType::knobStatic);
          // mMarkers.push_back(m);
          std::cout << "knob statics Id:" << i << std::endl;
        }
        if (type == BlockType::knobDynamic) {
          float posx = gridPos[to_string(i)]["posx"];
          float posy = gridPos[to_string(i)]["posy"];
          mKnobAmenitie->setDynamicPos(glm::vec2(posx, posy));
          mKnobAmenitie->setDynamicGridId(i);
          MarkerArucoRef m = MarkerAruco::create();
          m->setMarkerId(-1);
          m->setRectPos(
              glm::vec2(posx - 20, posy - 20), glm::vec2(posx + 20, posy - 20),
              glm::vec2(posx + 20, posy + 20), glm::vec2(posx - 20, posy + 20));
          m->setPos(glm::vec2(posx, posy));
          m->setGridId(i);
          m->setBlockType(BlockType::knobDynamic);
          // mMarkers.push_back(m);
          std::cout << "knob dynamic Id:" << i << std::endl;
        }

        i++;
      }
      ofLog(OF_LOG_NOTICE) << "done setup knob";
    } else {
      ofLog(OF_LOG_NOTICE) << "setup fail knob";
    }
  }

  ofLog(OF_LOG_NOTICE) << "Loading cam crop positions "<<mGridImg.size();
  ofFile file("imggui.json");
  if (file.exists()) {
    ofJson cam_gui;
    file >> cam_gui;
    std::string inputImg("cam_gui");

    mGridImg.at(0)->setCropUp(glm::vec2(float(cam_gui[inputImg]["x1"]), float(cam_gui[inputImg]["y1"])));
    mGridImg.at(0)->setCropDown(glm::vec2(cam_gui[inputImg]["x2"], cam_gui[inputImg]["y2"]));
    mGridImg.at(0)->setCropDisp(glm::vec2(cam_gui[inputImg]["disX"], cam_gui[inputImg]["disY"]));
    mGridImg.at(0)->setupCam(cam_gui[inputImg]["camId"]);

    //  float gm = std::stof(cam[inputImg]["gamma"]);
    mGridImg.at(0)->setGamma(0.5);

    ofLog(OF_LOG_NOTICE) << "Set Cam: " << cam_gui[inputImg]["camId"];
  } else {
    ofLog(OF_LOG_NOTICE) << "File does not exist img.json";
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
  mBDebugGrid->button = new ofxDatGuiToggle("Aruco View", false);
  mBDebugGrid->button->setPosition(10, 60);
  mBDebugGrid->button->setWidth(100, .4);
  mBDebugGrid->button->onButtonEvent([&](ofxDatGuiButtonEvent v) {
    mBDebugGrid->mActive = !mBDebugGrid->mActive;
  });

  mBDebugVideoGrid = ofxDatButton::create();
  mBDebugVideoGrid->button = new ofxDatGuiToggle("Grid View", false);
  mBDebugVideoGrid->button->setPosition(10, 110);
  mBDebugVideoGrid->button->setWidth(100, .4);
  mBDebugVideoGrid->button->onButtonEvent([&](ofxDatGuiButtonEvent v) {
    mBDebugVideoGrid->mActive = !mBDebugVideoGrid->mActive;
  });

  mBSingleGrid = ofxDatButton::create();
  mBSingleGrid->button = new ofxDatGuiToggle("Single Input", false);
  mBSingleGrid->button->setPosition(10, 160);
  mBSingleGrid->button->setWidth(100, .4);
  mBSingleGrid->button->onButtonEvent([&](ofxDatGuiButtonEvent v) {
    mBSingleGrid->mActive = !mBSingleGrid->mActive;
  });

  mBFullGrid = ofxDatButton::create();
  mBFullGrid->setActivation(false);
  mBFullGrid->button = new ofxDatGuiToggle("Full Input");
  mBFullGrid->button->setPosition(10, 210);
  mBFullGrid->button->setWidth(100, .4);
  mBFullGrid->button->onButtonEvent([&](ofxDatGuiButtonEvent v) {
    mBFullGrid->mActive = !mBFullGrid->mActive;

    if (mBFullGrid->isActive()) {
      std::cout << "done allocated full img";
    } else {
      std::cout << "done allocated single img";
    }
  });

  mBFullCamView = ofxDatButton::create();
  mBFullCamView->setActivation(false);
  mBFullCamView->button = new ofxDatGuiToggle("Full Cam View");
  mBFullCamView->button->setPosition(10, 260);
  mBFullCamView->button->setWidth(100, .4);
  mBFullCamView->button->onButtonEvent([&](ofxDatGuiButtonEvent v) {
    mBFullCamView->mActive = !mBFullCamView->mActive;
  });

  int sliderStartX = 150;

  mBEnableCrop = ofxDatButton::create();
  mBEnableCrop->setActivation(false);
  mBEnableCrop->button = new ofxDatGuiToggle("Toogle Crop");
  mBEnableCrop->button->setPosition(sliderStartX, 10);
  mBEnableCrop->button->setWidth(390, .4);
  mBEnableCrop->button->onButtonEvent([&](ofxDatGuiButtonEvent v) {
    mBEnableCrop->mActive = !mBEnableCrop->mActive;
    mGridImg.at(mCurrentInputIdx)->toogleCrop();
  });

  mBEnableVideo = ofxDatButton::create();
  mBEnableVideo->setActivation(false);
  mBEnableVideo->button = new ofxDatGuiToggle("Toogle Video");
  mBEnableVideo->button->setPosition(sliderStartX, 60);
  mBEnableVideo->button->setWidth(390, .4);
  mBEnableVideo->button->onButtonEvent([&](ofxDatGuiButtonEvent v) {
    for (auto &gridImage : mGridImg) {
      gridImage->toogleCam();
    }
  });

  mGammaValue = ofxDatSlider::create();
  mGammaValue->slider =
      new ofxDatGuiSlider(mGammaValue->ofParam.set("gamma", 0.65, 0, 2));
  mGammaValue->slider->setWidth(390, .4);
  mGammaValue->slider->setPosition(sliderStartX, 110);
  mGammaValue->slider->onSliderEvent(
      [&](ofxDatGuiSliderEvent v) { mGammaValue->ofParam = v.value; });

  mBDebugMarkers = ofxDatButton::create();
  mBDebugMarkers->setActivation(false);
  mBDebugMarkers->button = new ofxDatGuiToggle("Toogle Markers Draw");
  mBDebugMarkers->button->setPosition(sliderStartX, 160);
  mBDebugMarkers->button->setWidth(390, .4);
  mBDebugMarkers->button->onButtonEvent([&](ofxDatGuiButtonEvent v) {
    for (auto &det : mArucoDetector) {
      det->toggleMarkerInfo();
    }
  });

  mBGridSelect = ofxDatMatrix::create();
  mBGridSelect->matrix = new ofxDatGuiMatrix("Grid Matrix", 2 * 2, true);
  mBGridSelect->matrix->setRadioMode(true);
  mBGridSelect->matrix->setOpacity(0.8);
  mBGridSelect->matrix->setWidth(390, .4);
  mBGridSelect->matrix->setPosition(sliderStartX, 210);
  mBGridSelect->matrix->onMatrixEvent([&](ofxDatGuiMatrixEvent v) {
    mCurrentInputIdx = v.child;
    ofLog(OF_LOG_NOTICE) << "Index: " << mCurrentInputIdx << std::endl;
    mFboSingle.begin();
    ofClear(0, 0, 0, 255);
    mFboSingle.end();
  });

  mBCloseCams = ofxDatButton::create();
  mBCloseCams->button = new ofxDatGuiToggle("Close Cams");
  mBCloseCams->button->setOpacity(0.8);
  mBCloseCams->button->setWidth(390, .4);
  mBCloseCams->button->setPosition(sliderStartX, 260);
  mBCloseCams->button->onButtonEvent([&](ofxDatGuiButtonEvent v) {
    for (auto &gridImage : mGridImg) {
      gridImage->getCam().close();
    }

  });

  int i = 0;
  for (auto &gridImage : mGridImg) {
    gridImage->setupGUISwap(sliderStartX, 290 + 30 * i);
    i++;
  }

  // initial comands for setup
  mBFullGrid->setActivation(false);
  mBDebugVideoGrid->setActivation(false);

  ofLog(OF_LOG_NOTICE) << "done setup gui";
}
//-----------------------------------------------------------------------------
void ofApp::setupDetection() {
  for (int i = 0; i < mNumInputs; i++) {
    QRDetectorRef detector = QRDetector::create();
    detector->setupCalibration(mGridSizes.at(i).x, mGridSizes.at(i).y);
    mArucoDetector.push_back(detector);
  }

  ofLog(OF_LOG_NOTICE) << "done setup detector: " << mArucoDetector.size();
}

//-----------------------------------------------------------------------------
void ofApp::setupCalibration() {

  ofLog(OF_LOG_NOTICE) << "done setup calibration";
}
//-----------------------------------------------------------------------------
void ofApp::setupVideo() {

  std::string movies[] = {"grid_01.mov", "grid_02.mov", "grid_03.mov",
                          "grid_04.mov"};

  // setup inputs
  ofLog(OF_LOG_NOTICE) << "setting inputs: " << mNumInputs;

  for (int i = 0; i < mNumInputs; i++) {
    GridImageRef gridImage =
        GridImage::create(glm::vec2(CAM_WIDTH, CAM_HEIGHT));
    gridImage->setId(i);
    mGridImg.push_back(gridImage);
  }

  // Print available devices
  ofLog(OF_LOG_NOTICE) << "Cam devices:";
  ofVideoGrabber mVideoGrabber;
  mVideoGrabber.listDevices();

  if(mNumInputs > 1){
    ofLog(OF_LOG_NOTICE) << "Loading cam and crop positions "<<mGridImg.size();
    ofFile file("img.json");
    bool foundFile = false;
    if (file.exists()) {
      ofJson js;
      file >> js;
      int j = 0;
      for (auto &cam : js) {
        std::string inputImg("cam" + to_string(j));
        int camId =  cam[inputImg]["camId"];
        ofLog(OF_LOG_NOTICE)<<"Loading: " << j << ": CamId: " << camId;


        mGridImg.at(j)->setCropUp(glm::vec2(cam[inputImg]["x1"], cam[inputImg]["y1"]));
        mGridImg.at(j)->setCropDown(glm::vec2(cam[inputImg]["x2"], cam[inputImg]["y2"]));
        mGridImg.at(j)->setCropDisp(glm::vec2(cam[inputImg]["disX"], cam[inputImg]["disY"]));

        float gm = float(cam[inputImg]["gamma"]);
        mGridImg.at(j)->setGamma(gm);

        ofLog(OF_LOG_NOTICE) << "Loading cam devices:";
        mGridImg.at(j)->setupCam(camId, CAM_FRAMERATE);

        ofLog(OF_LOG_NOTICE) << "Gamma: " << gm;
        j++;
      }
      ofLog(OF_LOG_NOTICE) << "Done crop values JSON";
      foundFile = true;
    } else {
      ofLog(OF_LOG_NOTICE) << "File does not exist img.json";
    }
  }

  {
    ofLog(OF_LOG_NOTICE) << "Loading Videos:";
    // load video
    int i = 0;
    for (auto &gridImage : mGridImg) {
      gridImage->setupVideo(movies[i]);
      i++;
    }
    ofLog(OF_LOG_NOTICE) << "Done loading video";
  }

  // Mat settings for Aruco detector
  vidImg.allocate(CAM_WIDTH, CAM_HEIGHT, OF_IMAGE_COLOR);

  // fill the fbos with the appropiate dimentions
  mFboSingle.allocate(CAM_WIDTH, CAM_HEIGHT, GL_RGBA);

  // clean start with Fbos
  mFboSingle.begin();
  ofClear(0, 0, 0, 255);
  mFboSingle.end();

  ofLog(OF_LOG_NOTICE) << "done setup video";
}

void ofApp::setupGridDetector() {

  for (int i = 0; i < mNumInputs; i++) {
    ofLog(OF_LOG_NOTICE) << "setup grid: " << i << " " << mGridSizes.at(i);
    GridDetectorRef griD = GridDetector::create(mGridSizes.at(i));
    griD->setId(i);
    griD->setupGridJsonPos("gridpos_0" + to_string(i + 1) + ".json");
    griD->setMaxMarkers(mMaxMarkers.at(i));
    griD->setupCleaner();
    griD->setupBlocks();

    mGridDetector.push_back(griD);

    // allocate fbo for drawing
    ofFbo fboTemp;
    fboTemp.allocate(CAM_WIDTH, CAM_HEIGHT, GL_RGB);
    fboTemp.begin();
    ofClear(0, 0, 0);
    fboTemp.end();
    mFboGrid.push_back(fboTemp);
  }

  ofLog(OF_LOG_NOTICE) << "done setup grid :" << mNumInputs;
}

//-----------------------------------------------------------------------------
