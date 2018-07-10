#include "GridImage.h"

using namespace std;
using namespace cv;

GridImage::GridImage(glm::vec2 dims) {
  mDim = dims;
  mLength = glm::vec2(0, 0);
  mGamma = 0.9;
  mActivateCrop = false;
  mActivateCam = true;

  mCornerUp = glm::vec2(100, 100);
  mCornerDown = glm::vec2(300, 300);
  mDisp = glm::vec2(23, 23);

  // mFboResolution.allocate(mDim.x, mDim.y, GL_RGBA);
  // mPixs.allocate(mDim.x, mDim.y, OF_PIXELS_RGBA);

  // clean start with Fbos
  // mFboResolution.begin();
  // ofClear(0, 0, 0, 255);
  // FboResolution.end();
}
//-----------------------------------------------------------------------------
void GridImage::setupCam(int id, int ftps) {
  mCamId = id;
  mCam.setDeviceID(mCamId);
  mCam.setDesiredFrameRate(ftps);
  mCam.initGrabber(mDim.x, mDim.y);

  ofLog(OF_LOG_NOTICE) << "loaded Cam: " << mCamId << " " << mId << " "
                       << mDim.x << " " << mDim.y << "  " << mCam.getWidth();
}
//-----------------------------------------------------------------------------
void GridImage::setupVideo(std::string name) {
  mVideoName = name;
  mVideoInput.load(mVideoName);
  mVideoInput.play();

  ofLog(OF_LOG_NOTICE) << "loaded Video: " << mVideoName << " " << mId;
}
//-----------------------------------------------------------------------------
bool GridImage::updateImage() {
  bool newFrame = false;
  if (mActivateCam) {
    mCam.update();
    newFrame = mCam.isFrameNew();
    if (newFrame) {
      mFboResolution.begin();
      mCam.draw(0, 0, mDim.x, mDim.y);
      mFboResolution.end();
      // ofLog(OF_LOG_NOTICE) << " " << newFrame << " ";
    }
  } else {
    mVideoInput.update();
    newFrame = mVideoInput.isFrameNew();
  }
  return newFrame;
}
//-----------------------------------------------------------------------------
ofPixels &GridImage::getImgPixels() {
  return (mActivateCam) ? mCam.getPixels() : mVideoInput.getPixels();
}
//-----------------------------------------------------------------------------
void GridImage::drawImage(int x, int y, int w, int h) {
  if (mActivateCam) {
    mCam.draw(x, y, w, h);
  } else {
    mVideoInput.draw(x, y, w, h);
  }
}

void GridImage::drawImage(int x, int y) {
  if (mActivateCam) {
    mCam.draw(x, y, mDim.x, mDim.y);
  } else {
    mVideoInput.draw(x, y, mDim.x, mDim.y);
  }
}
//-----------------------------------------------------------------------------
void GridImage::adjustGamma(cv::Mat &img, float gamma = 0.5) {
  mGamma = gamma;
  if (!img.empty()) {
    cv::Mat lookUpTable(1, 256, CV_8U);
    unsigned char *p = lookUpTable.ptr();
    for (int i = 0; i < 256; i++) {
      p[i] = saturate_cast<unsigned char>(pow(i / 255.0, mGamma) * 255.0);
    }
    cv::LUT(img, lookUpTable, img);
  }
}
//-----------------------------------------------------------------------------
void GridImage::cropImg(cv::Mat &inputVideo) {
  mLength.x = mCornerDown.x - mCornerUp.x;
  mLength.y = mCornerDown.y - mCornerUp.y;
  mRoi.x = mCornerUp.x;
  mRoi.y = mCornerUp.y;
  mRoi.width = mLength.x + mDisp.x;
  mRoi.height = mLength.y + mDisp.y;

  // Copy the data into new matrix
  if (mRoi.width < mDim.x && mRoi.height < mDim.y) {
    if (mRoi.x > 0 && mRoi.y > 0 && mRoi.width < mDim.x &&
        mRoi.height < mDim.y) {
      cv::Mat cutMat(inputVideo, mRoi);
      cutMat.copyTo(mCropMat);
    }
  } else {
    inputVideo.copyTo(mCropMat);
  }
}
//-----------------------------------------------------------------------------
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
//-----------------------------------------------------------------------------
void GridImage::drawCropImg() {
  ofImage imgCut;
  ofxCv::toOf(mCropMat, imgCut.getPixels());
  imgCut.update();

  ofSetColor(0, 150, 200, 200);
  imgCut.draw(mCornerUp.x, mCornerUp.y, mLength.x + mDisp.x,
              mLength.y + mDisp.y);

  imgCut.draw(0, 0, 200, 200);
  ofxCv::drawMat(mCropMat, 0, 200, 200, 200);
}