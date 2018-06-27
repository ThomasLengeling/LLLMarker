/*

Thomas Sanchez Lengeling
May, 2018

LLL
*/
#pragma once
#include "ofMain.h"
#include "Common.h"
#include "RectDetector.h"


class MarkerAruco;
class ProbabiltyAruco;

typedef std::shared_ptr<MarkerAruco> MarkerArucoRef;


class ProbabiltyAruco{
public:
    ProbabiltyAruco();

    void incProba();
    float getProba(int num);
    void resetProba();
private:
    float mProba;
    int   mInc;
};

class MarkerAruco : public RectDetector, public ProbabiltyAruco{
public:
    MarkerAruco() : RectDetector(), ProbabiltyAruco() {
        mGridId = -1;
        mMakerId = -1;
        mBType = BlockType::grid;
        mIdType = std::make_pair(-1, 0);
    }

    static MarkerArucoRef create(){
        return std::make_shared<MarkerAruco>();
    }

    void setMarkerId(int i){mMakerId = i;}
    int getMarkerId(){return mMakerId;}

    void setGridId(int i){mGridId = i;}
    int getGridId(){return mGridId;}

    void setPos(glm::vec2 mpos){mPos = mpos;}
    glm::vec2 getPos(){return mPos;}

    void enableOn(){mEnable = true;}
    void enableOff(){mEnable = false;}

    bool isEnable(){return mEnable;}

    float getProb(){return mProb;}
    void setProb(float p){mProb = p;}

    void setBlockType(BlockType bt){mBType = bt;}
    int getBlockType(){return mBType;}

    void makePairIdType(int id, int type);
    void updateType(int type);
    void updateId(int id);
    std::pair<int, int> getIdTypePair();


private:
    int        mMakerId;
    int        mGridId;

    int        mOrientation;
    glm::vec2  mPos;
    bool       mEnable;

    BlockType  mBType;

    MarkerType mCurrentType;

    float      mProb;

    //id correponds the type
    std::pair<int, int> mIdType;

};
