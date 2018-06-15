#include "Detector.h"

using namespace std;
using namespace cv;

Detector::Detector() {
  mMinFoundId = 1000;
  mMaxFoundId = 0;
}

void Detector::resetMinMax() {
  mMinFoundId = 1000;
  mMaxFoundId = 0;
}

void Detector::setupCalibration(int markersX, int markersY) {
  float markerLength = 0.0162;     // 0.0165
  float markerSeparation = 0.0042; // 0045
  int dictionaryId = 11;
  std::string outputFile = "./cal.txt";

  int calibrationFlags = 0;
  float aspectRatio = 1;

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

  dictionary = cv::aruco::getPredefinedDictionary(
      cv::aruco::PREDEFINED_DICTIONARY_NAME(dictionaryId));

  // create board object
  cv::Ptr<cv::aruco::GridBoard> gridboard = cv::aruco::GridBoard::create(
      markersX, markersY, markerLength, markerSeparation, dictionary);
  board = gridboard.staticCast<cv::aruco::Board>();
}

//---------------------------------------------------------------------------
cv::Mat Detector::detectMarkers(cv::Mat inputVideo) {

  cv::Mat vidMat;
  // detect markers
  std::vector<int> arucoIds;
  std::vector<std::vector<cv::Point2f>> corners;
  std::vector<std::vector<cv::Point2f>> rejected;

  mTagsIds.clear();
  mCentroid.clear();

  aruco::detectMarkers(inputVideo, dictionary, corners, arucoIds,
                       detectorParams);
  aruco::refineDetectedMarkers(inputVideo, board, corners, arucoIds, rejected);

  if (arucoIds.size() > 0) {

    aruco::drawDetectedMarkers(inputVideo, corners, arucoIds);

    InputArrayOfArrays cornersDetected = corners;
    InputArray idsDetected = arucoIds;

    mNumFoundMarkers = cornersDetected.total();

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

      // get ids
      if (idsDetected.total() != 0) {
        int id = idsDetected.getMat().ptr<int>(0)[i];
        mTagsIds.push_back(id);
        cva.mId = id;

        if (mMinFoundId > id) {
          mMinFoundId = id;
        }
        if (mMaxFoundId < id) {
          mMaxFoundId = id;
        }
      }

      mCentroid.push_back(cva);
    }
    mCentroids.push_back(mCentroid);

    // cleanDetection();

    inputVideo.copyTo(vidMat);
  }
  return vidMat;
}
