#pragma once

#include "ofMain.h"
#include "ofxNetwork.h"

#include <algorithm>
#include <ctime>
#include <iostream>
#include <map>
#include <vector>

// addons
#include "ofxCv.h"
#include "ofxOpenCv.h"

#include "Block.h"
#include "Common.h"
#include "Detector.h"
#include "KnobAruco.h"
#include "MarkerAruco.h"
#include "GridImage.h"

#define RAD_DETECTION 35
#define MAX_MARKERS 1000

class GridDetector;
typedef std::shared_ptr<GridDetector> GridDetectorRef;

class GridDetector{
public:

    GridDetector(glm::vec2 dim);

    static GridDetectorRef create(glm::vec2 dim) {
        return std::make_shared<GridDetector>(dim);
    }

    glm::vec2 getDim(){return mGridDim;}

    void setId(int id);
    int getId(){return mId;}

    int getNumMarkers(){return mNumMarkers;}

    void setMaxMarkers(int max);
    int getMaxMarkers(){return mMaxMarkers;}

    void setupGridJsonPos(std::string filePos);
    void generateGridPos();
    void setupBlocks();

    void generateMarkers(std::vector<int> & ids, std::vector<BlockRef> & blocks, bool sort = false );
    void clasification();
    void updateBlockTypes();

    void drawMarkers( );
    void toogleDebugGrid(){mDebugGrid =!mDebugGrid;}

    void calibrateGrid();
    void recordGrid();

    void setupCleaner();
    void cleanGrid();


    void setGridPos(glm::vec2 mousePos);

    //save json files
    void saveGridJson();

    //draw detected grid
    void drawDetectedGrid(float posx, float posy, float size = 20, float space = 5);
    void drawDetectedGridIn(float posx, float posy, float size = 20, float space = 5);
    void drawDetectedBlock(float posx, float posy, float size = 20, float space = 5);

    void drawBlock(float posx, float posy, float size = 20, float space = 5);

private:

    glm::vec2  mGridDim;

    //id;
    int mId;

    //max markers possible
    int mMaxMarkers;

    //number of markers detected
    int mNumMarkers;

    //debug grid
    bool mDebugGrid;
    bool mRecordOnce;

    //clearner
    int mWindowIterMax;
    int mWindowCounter;

    bool mCleanDone;

    //radius of detection
    float mRadDetection;

    //calibrate grid
    bool mCalibrateGrid;

    std::map<int, int> mIdsCounter;
    std::map<int, int> mProCounter;
    std::map<int, int> mCenterCounter;

    // check if found marker
    std::vector<MarkerArucoRef> mMarkers;

    std::vector<BlockRef> mCurrBlock;
    std::vector<std::vector<BlockRef>> mTmpBlocks;

    std::vector<BlockRef> mBlocks;
    std::vector<int> mTagsIds;
    std::vector<int> mFullIds;


};
