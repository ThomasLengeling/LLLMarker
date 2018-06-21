#include "GridImage.h"

using namespace std;
using namespace cv;

GridImage::GridImage(glm::vec2 dims) {
  mDim = dims;
  mLength = glm::vec2(0, 0);
  mGamma = 0.9;
  mActivateCrop = false;
}

void GridImage::adjustGamma(cv::Mat &img, float gamma = 0.5) {
  mGamma = gamma;
  cv::Mat lookUpTable(1, 256, CV_8U);
  unsigned char *p = lookUpTable.ptr();
  for (int i = 0; i < 256; i++) {
    p[i] = saturate_cast<unsigned char>(pow(i / 255.0, mGamma) * 255.0);
  }
  cv::LUT(img, lookUpTable, img);
}

void GridImage::cropImg(cv::Mat inputVideo) {
  mLength.x = mCornerDown.x - mCornerUp.x;
  mLength.y = mCornerDown.y - mCornerUp.y;
  mRoi.x = mCornerUp.x;
  mRoi.y = mCornerUp.y;
  mRoi.width = mLength.x + mDisp.x;
  mRoi.height = mLength.y + mDisp.y;

  // Copy the data into new matrix
  if (mRoi.x > 0 && mRoi.y > 0 && mRoi.width < mDim.x && mRoi.height < mDim.y) {
    cv::Mat cutMat(inputVideo, mRoi);
    cutMat.copyTo(mCropMat);
  }
}

void GridImage::drawCropRoi() {
  if (mActivateCrop) {
    ofSetColor(0, 200, 125, 50);
  } else {
    ofSetColor(0, 220, 195, 155);
  }
  ofBeginShape();
  ofVertex(mCornerUp);
  ofVertex(glm::vec2(mCornerDown.x, mCornerUp.y));
  ofVertex(mCornerDown);
  ofVertex(glm::vec2(mCornerUp.x, mCornerDown.y));
  ofEndShape();

  ofSetColor(255, 50, 50, 155);
  ofDrawCircle(mCornerDown.x, mCornerDown.y, 5, 5);
  ofDrawCircle(mCornerUp.x, mCornerUp.y, 5, 5);
}

void GridImage::drawCropImg() {
  ofImage imgCut;
  ofxCv::toOf(mCropMat, imgCut.getPixels());
  imgCut.update();

  ofSetColor(0, 150, 200, 200);
  imgCut.draw(mCornerUp.x, mCornerUp.y, mLength.x + mDisp.x,
              mLength.y + mDisp.y);

  imgCut.draw(0, 0, 200, 200);
}
