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

    cv::Mat detectMarkers(cv::Mat inputVideo);

    void resetMinMax();

    std::vector<int> getTagIds(){return mTagsIds;}
    int getNumMarkers(){return mNumFoundMarkers;}
    
    std::vector< Block > getBoard(){return mCentroid;}
    std::vector< std::vector< Block > >  getBoards(){return mCentroids;}

    void clearBlocks(){mCentroids.clear();}

private:

    //aruco
    cv::Ptr<cv::aruco::Board> board;
    cv::Ptr<cv::aruco::Dictionary> dictionary;
    cv::Ptr<cv::aruco::DetectorParameters> detectorParams;

    std::vector< std::vector< Block > > mCentroids;
    std::vector< Block >                mCentroid;

    std::vector< int > mTagsIds;

    int mMinFoundId;
    int mMaxFoundId;

    int mNumFoundMarkers;

};
