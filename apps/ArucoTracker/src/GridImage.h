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

#include "gui.h"

class GridImage;
typedef std::shared_ptr<GridImage> GridImageRef;

class GridImage{
public:
    GridImage(glm::vec2 dims);

    static GridImageRef create(glm::vec2 dims) {
        return std::make_shared<GridImage>(dims);
    }

    //setup video
    void setupCam(int id, int ftps = 30);
    void setupVideo(std::string name);

    //cam
    ofVideoGrabber    & getCam(){return mCam;}
    ofVideoPlayer     & getVideo(){return mVideoInput;}

    int getCamId(){return mCamId;}

    //setup GUI
    void setupGUISwap(float x, float y);
    void drawGUISwap();
    void updateGUISwap();

    //draw update images
    bool updateImage();
    void drawImage(int x, int y, int w, int h);
    void drawImage(int x, int y);

    ofPixels  & getImgPixels();

    void setGamma(float gm){mGamma = gm;}
    float getGamma(){return mGamma;}

    // clean Image
    void adjustGamma(cv::Mat &img);

    void toogleCrop(){mActivateCrop =! mActivateCrop;}
    void toogleCam(){mActivateCam =! mActivateCam;}

    void cropImg(cv::Mat & inputVideo);

    void drawCropImg();
    void drawCropRoi();

    void resetCrop();

    cv::Mat & getCropMat(){return mCropMat;}

    void setCropUp(glm::vec2 up);
    void setCropDown(glm::vec2 down);

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
    int                 mFps;
    int                 mCamId;
    ofPixels            mPixs;

    glm::vec2           mDim;
    glm::vec2           mLength;

    //Resolution of the I
    ofFbo               mFboResolution;

    //swap camId
    ofxDatMatrixRef     mSwapCamId;

    //video player
    ofVideoPlayer       mVideoInput;
    std::string         mVideoName;

    //input image
    ofImage             mVidImg;

    //acative streaming
    bool mActivateCam;

    //crop of image
    bool      mActivateCrop;
    cv::Rect  mRoi;
    glm::vec2 mCornerUp;
    glm::vec2 mCornerDown;
    glm::vec2 mDisp;
    cv::Mat   mCropMat;

    //id of the Image
    int mId;

    //img correction
    float mGamma;
};
