#include "ofApp.h"

using namespace std;
using namespace cv;

//--------------------------------------------------------------
void ofApp::setup() {
  ofBackground(0);

  setupCalibration();
  setupGUI();

  std::cout << "finished setup" << std::endl;
}

void ofApp::setupGUI() {
  mBDebugVideo = ofxDatButton::create();
  mBDebugVideo->button = new ofxDatGuiButton("Debug View");
  mBDebugVideo->button->setPosition(10, 10);
  mBDebugVideo->button->setWidth(100, .4);
  mBDebugVideo->button->onButtonEvent([&](ofxDatGuiButtonEvent v) {
    mBDebugVideo->mActive = !mBDebugVideo->mActive;
  });

  mBCalibrateVideo = ofxDatButton::create();
  mBCalibrateVideo->button = new ofxDatGuiButton("Calibrate Camera");
  mBCalibrateVideo->button->setPosition(10, 60);
  mBCalibrateVideo->button->setWidth(100, .4);
  mBCalibrateVideo->button->onButtonEvent([&](ofxDatGuiButtonEvent v) {
    mBCalibrateVideo->mActive = !mBCalibrateVideo->mActive;
  });

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
      mGridSpaceX->ofParamInt.set("grid space X", 50, 0, 100));
  mGridSpaceX->slider->setWidth(390, .4);
  mGridSpaceX->slider->setPosition(sliderStartX, 60);

  mGridSpaceX->slider->onSliderEvent([&](ofxDatGuiSliderEvent v) {
    float startGridX = 1280 * 0.35;
    float startGridY = 720 * 0.3;

    float stepX = v.value;
    float stepY = 50.0;

    float gapX = 3;
    float gapY = 3;

    int numW = 19;
    int numH = 13;

    int maxMarkers = numW * numH;
    int i = 0;
    for (auto &m : mMarkers) {

      int indeX = int(i / float(numW));
      int indeY = (i % numH);

      float x = indeX * stepX + indeX * gapX + startGridX;
      float y = indeY * stepY + indeY * gapY + startGridY;

      m.setRectPos(glm::vec2(x - stepX / 2.0, y - stepY / 2.0),
                   glm::vec2(stepX, stepY));

      m.setPos(glm::vec2(x, y));
      i++;
    }
  });

  mGridSpaceY = ofxDatSlider::create();
  mGridSpaceY->slider = new ofxDatGuiSlider(
      mGridSpaceY->ofParamInt.set("grid space Y", 50, 0, 100));
  mGridSpaceY->slider->setWidth(390, .4);
  mGridSpaceY->slider->setPosition(sliderStartX, 90);

  mGridSizeX = ofxDatSlider::create();
  mGridSizeX->slider = new ofxDatGuiSlider(
      mGridSizeX->ofParamInt.set("grid size X", 50, 0, 100));
  mGridSizeX->slider->setWidth(390, .4);
  mGridSizeX->slider->setPosition(sliderStartX, 120);

  mGridSizeY = ofxDatSlider::create();
  mGridSizeY->slider = new ofxDatGuiSlider(
      mGridSizeY->ofParamInt.set("grid size Y", 50, 0, 100));
  mGridSizeY->slider->setWidth(390, .4);
  mGridSizeY->slider->setPosition(sliderStartX, 150);

  mGridGapX = ofxDatSlider::create();
  mGridGapX->slider =
      new ofxDatGuiSlider(mGridGapX->ofParamInt.set("grid gap X", 50, 0, 100));
  mGridGapX->slider->setWidth(390, .4);
  mGridGapX->slider->setPosition(sliderStartX, 180);

  mGridGapY = ofxDatSlider::create();
  mGridGapY->slider =
      new ofxDatGuiSlider(mGridGapY->ofParamInt.set("grid gap Y", 50, 0, 100));
  mGridGapY->slider->setWidth(390, .4);
  mGridGapY->slider->setPosition(sliderStartX, 210);
}

void ofApp::setupCalibration() {

  int markersX = 5;
  int markersY = 5;
  float markerLength = 0.0165;
  float markerSeparation = 0.0045;
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

  bool refindStrategy = false;
  int camId = 0;
  String video;

  vidGrabber.setDeviceID(0);
  vidGrabber.setDesiredFrameRate(60);
  vidGrabber.initGrabber(1280, 720);

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
  Ptr<aruco::Board> board = gridboard.staticCast<aruco::Board>();

  // collected frames for calibration
  vector<vector<vector<Point2f>>> allCorners;
  vector<vector<int>> allIds;
  Size imgSize;

  // fill markers

  // set the max region

  float startGridX = 1280 * 0.05;
  float startGridY = 720 * 0.05;

  float stepX = 50.0;
  float stepY = 50.0;

  float gapX = 3;
  float gapY = 3;

  int numW = 19;
  int numH = 13;

  int maxMarkers = numW * numH;
  for (int i = 0; i < maxMarkers; i++) {
    Marker m;
    m.setId(i);

    int indeX = int(i / float(numW));
    int indeY = (i % numH);

    float x = indeX * stepX + indeX * gapX + startGridX;
    float y = indeY * stepY + indeY * gapY + startGridY;

    m.setRectPos(glm::vec2(x - stepX / 2.0, y - stepY / 2.0),
                 glm::vec2(stepX, stepY));

    m.setPos(glm::vec2(x, y));
    mMarkers.push_back(m);
    std::cout << x << " " << y << std::endl;
  }
}

//--------------------------------------------------------------
void ofApp::update() {

  vidGrabber.update();

  if (vidGrabber.isFrameNew()) {

    Mat imageCopy = ofxCv::toCv(vidGrabber.getPixels());

    // detect markers
    ids.clear();
    corners.clear();
    rejected.clear();
    centroid.clear();
    tagsIds.clear();

    aruco::detectMarkers(imageCopy, dictionary, corners, ids, detectorParams,
                         rejected);

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
        centroid.push_back(glm::vec2(cent.x, cent.y));

        // get ids
        if (idsDetected.total() != 0) {
          int id = idsDetected.getMat().ptr<int>(0)[i];
          tagsIds.push_back(id);
        }
      }
    }

    imageCopy.copyTo(vidMat);

    ofxCv::toOf(vidMat, vidImg.getPixels());
    // std::cout << "got img and aruco" << std::endl;
  }

  // udate
  if (mDrawGUI) {
    updateGUI();
  }
}

//--------------------------------------------------------------
void ofApp::updateGUI() {
  mBDebugVideo->button->update();
  mBShowGrid->button->update();
  mBCalibrateVideo->button->update();

  mGridSpaceX->slider->update();
  mGridSpaceY->slider->update();

  mGridSizeX->slider->update();
  mGridSizeY->slider->update();

  mGridGapX->slider->update();
  mGridGapY->slider->update();
}

//--------------------------------------------------------------
void ofApp::draw() {
  ofBackground(0);

  if (mBDebugVideo->mActive) {
    ofSetColor(255);
    ofxCv::drawMat(vidMat, 0, 0, 1920, 1080);
  }

  /*
    if (vidImg.isAllocated()) {
      ofSetColor(255);
      vidImg.draw(320, 0, 320, 240);
    }
    vidGrabber.draw(0, 240, 320, 240);
    */

  if (mBShowGrid->mActive) {
    ofSetColor(0, 200, 200);
    for (int i = 0; i < centroid.size(); i++) {
      ofDrawCircle(centroid.at(i).x, centroid.at(i).y, 5, 5);
      ofDrawBitmapString(i, centroid.at(i).x, centroid.at(i).y);
      ofDrawBitmapString(tagsIds.at(i), centroid.at(i).x - 7,
                         centroid.at(i).y - 7);
    }

    for (auto &mk : mMarkers) {
      glm::vec2 pos = mk.getPos();

      ofSetColor(50, 100, 220);
      ofDrawCircle(pos.x, pos.y, 5, 5);

      int k = 0;
      for (auto &cen : centroid) {
        float dis = ofDist(cen.x, cen.y, pos.x, pos.y);
        if (dis > 1 && dis < 13) {
          ofSetColor(255, 0, 0);
          ofDrawCircle(pos.x, pos.y, 7, 7);
          mk.setId(tagsIds.at(k));
          break;
        }
        k++;
      }

      ofSetColor(255);
      ofDrawBitmapString(mk.getId(), pos.x - 2, pos.y - 10);

      mk.drawRect();
    }
  }

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

  mGridSizeX->slider->draw();
  mGridSizeY->slider->draw();

  mGridGapX->slider->draw();
  mGridGapY->slider->draw();
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key) {
  if (key = 'g') {
    mDrawGUI = !mDrawGUI;
  }
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key) {}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y) {}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button) {}

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