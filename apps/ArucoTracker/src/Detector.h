#pragma once

/*

Thomas Sanchez Lengeling
May, 2018

LLL
*/

#include "ofMain.h"

#include <opencv2/aruco.hpp>
#include <opencv2/calib3d.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

//addons
#include "ofxCv.h"
#include "ofxOpenCv.h"
#include "ofxDatGui.h"

#include "Block.h"

class Detector;
typedef std::shared_ptr<Detector> DetectorRef;


class Detector{
public:
    Detector();

    static DetectorRef create() {
        return std::make_shared<Detector>();
    }

    void setupCalibration(int markersX, int markersY);

    void detectMarkers(cv::Mat & inputVideo);

    void resetMinMax();

    std::vector<int> getTagIds(){return mTagsIds;}
    int getNumMarkers(){return mNumFoundMarkers;}

    std::vector< BlockRef > getBoard(){return mBlock;}

    cv::Mat getMatImg(){return mVidMat;}
    ofImage getOfImg(){return mVidImg;}

    int getMinId(){return mMinFoundId;}
    int getMaxId(){return mMaxFoundId;}


    //calibration
    bool readDetectorParameters(std::string filename, cv::Ptr< cv::aruco::DetectorParameters> & params);
    bool saveCameraParams(const std::string &filename, cv::Size imageSize,
                         float aspectRatio, int flags,
                         const cv::Mat & cameraMatrix, const cv::Mat & distCoeffs,
                         double totalAvgErr);

private:

    //aruco
    cv::Ptr<cv::aruco::Board> board;
    cv::Ptr<cv::aruco::Dictionary> dictionary;
    cv::Ptr<cv::aruco::DetectorParameters> detectorParams;

    std::vector< BlockRef >                mBlock;

    std::vector< int >                  mTagsIds;

    int mMinFoundId;
    int mMaxFoundId;

    int mNumFoundMarkers;

    //video output in two formats
    cv::Mat mVidMat;
    ofImage mVidImg;

};
