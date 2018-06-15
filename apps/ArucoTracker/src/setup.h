
void ofApp::setupConnection() {
    ofLog(OF_LOG_NOTICE) << "setup UDP connection " << std::endl;
    ofxUDPSettings settings;
    settings.sendTo("172.20.10.2", 15800);
    settings.blocking = false;
    udpConnection.Setup(settings);

    if(mDebug){
        string message = "connected to Aruco Detector";
        udpConnection.Send(message.c_str(), message.length());
    }
}
//-----------------------------------------------------------------------------
void ofApp::setupBlocks(){
    for(int i = 0; i < MAX_MARKERS; i++){
      Block block;
      block.mIdType.first = i;
      mBlocks.push_back(block);
    }


}
//-----------------------------------------------------------------------------
void ofApp::setupGridPos(){

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
//-----------------------------------------------------------------------------
void ofApp::setupKnob(){
    ofLog(OF_LOG_NOTICE)<< "setup knob" << std::endl;
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

//-----------------------------------------------------------------------------
void ofApp::setupCalibration() {

  int markersX = GRID_WIDTH;
  int markersY = GRID_HEIGHT;
  float markerLength = 0.0162;     // 0.0165
  float markerSeparation = 0.0042; // 0045
  int dictionaryId = 11;
  std::string outputFile = "./cal.txt";

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

  detectorParams = cv::aruco::DetectorParameters::create();

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

mVideoCapture = 0;

      vidGrabber.setDeviceID(0);
      vidGrabber.setDesiredFrameRate(60);
      vidGrabber.initGrabber(CAM_WIDTH, CAM_HEIGHT);

      gridMovie.load("grid_02.mov");
      gridMovie.setLoopState(OF_LOOP_NORMAL);
      gridMovie.play();


  vidImg.allocate(CAM_WIDTH, CAM_HEIGHT, OF_IMAGE_COLOR);

  // inputVideo.open(camId);
  // if (!inputVideo.isOpened()) { // check if we succeeded
  //    std::cout << "error input cam" << std::endl;
  // }

  std::cout << "loading input video" << std::endl;

  dictionary = cv::aruco::getPredefinedDictionary(
      cv::aruco::PREDEFINED_DICTIONARY_NAME(dictionaryId));

  // create board object
  cv::Ptr<cv::aruco::GridBoard> gridboard = cv::aruco::GridBoard::create(
      markersX, markersY, markerLength, markerSeparation, dictionary);
  board = gridboard.staticCast<cv::aruco::Board>();

  // collected frames for calibration
  std::vector<std::vector<std::vector<cv::Point2f>>> allCorners;
  std::vector<std::vector<int>> allIds;


}

//-----------------------------------------------------------------------------
void ofApp::setupCleaner(){
    ofLog(OF_LOG_NOTICE)<< "setup clean" << std::endl;
    // cleaner
    mWindowCounter = 0;
    mWindowIterMax = 20;

    // record grid
    mRecordOnce = true;
}


//-----------------------------------------------------------------------------
