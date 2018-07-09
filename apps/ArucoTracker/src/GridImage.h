#pragma once
/*

Thomas Sanchez Lengeling
May, 2018

LLL
*/

#include "ofMain.h"

#include <algorithm>
#include <iostream>
#include <vector>

#include <opencv2/calib3d.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

// addons
#include "ofxCv.h"
#include "ofxOpenCv.h"

class GridImage;
typedef std::shared_ptr<GridImage> GridImageRef;

class GridImage{
public:
    GridImage(glm::vec2 dims);

    static GridImageRef create(glm::vec2 dims) {
        return std::make_shared<GridImage>(dims);
    }

    void setupCam(int id);
    void setupVideo(std::string name);

    //cam
    ofVideoGrabber    & getCam(){return mCam;}
    ofVideoPlayer     & getVideo(){return mVideoInput;}


    bool updateImage();
    void drawImage(int x, int y, int w, int h);
    void drawImage(int x, int y);

    ofPixels  & getImgPixels();


    // clean Image
    void adjustGamma(cv::Mat &img, float gamma);

    void toogleCrop(){mActivateCrop =! mActivateCrop;}
    void toogleCam(){mActivateCam =! mActivateCam;}

    void cropImg(cv::Mat & inputVideo);

    void drawCropImg();
    void drawCropRoi();

    cv::Mat getCropMat(){return mCropMat;}

    void setCropUp(glm::vec2 up){ mCornerUp = up; }
    void setCropDown(glm::vec2 down){ mCornerDown = down; }
    void setCropDisp(glm::vec2 dis) { mDisp = dis; }

    glm::vec2 getCropUp(){return mCornerUp;}
    glm::vec2 getCropDown(){return mCornerDown;}
    glm::vec2 getCropDisp(){return mDisp;}

    void setId(int id){mId = id;}

    void updateDetectImg(ofImage & img){mVidImg = img;}
    ofImage & getImg(){return mVidImg;}

    ofTexture & getFboTexture(){return mFboResolution.getTexture();}

private:
    //input stream
    ofVideoGrabber      mCam;
    int                 mCamId;
    ofPixels            mPixs;

    ofVideoPlayer       mVideoInput;
    std::string         mVideoName;

    ofImage             mVidImg;
    cv::Mat             mVidMat;

    bool mActivateCam;

    //crop
    bool mActivateCrop;

    cv::Rect mRoi;
    glm::vec2 mCornerUp;
    glm::vec2 mCornerDown;
    glm::vec2 mDisp;

    cv::Mat mCropMat;

    glm::vec2 mDim;
    glm::vec2 mLength;

    ofFbo     mFboResolution;

    int mId;
    //img correction
    float mGamma;
};
