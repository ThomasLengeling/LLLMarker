/*

Thomas Sanchez Lengeling
May, 2018

LLL
*/
#pragma once
#include "ofMain.h"
#include "Common.h"


class MarkerAruco;
class RectAruco;
class ProbabiltyAruco;

typedef std::shared_ptr<MarkerAruco> MarkerArucoRef;

class RectAruco{
public:
    RectAruco();

    void setRectPos(glm::vec2 corner, glm::vec2 length);
    void setRectPos(glm::vec2 p0, glm::vec2 p1, glm::vec2 p2, glm::vec2 p3);
    void setRectPos(std::vector<glm::vec2> & vec);
    void drawRect();

private:
    std::vector<glm::vec2> mVertex;
};

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

class MarkerAruco : public RectAruco, public ProbabiltyAruco{
public:
    MarkerAruco() : RectAruco(), ProbabiltyAruco() {
        mGridId = -1;
        mId = -1;
        mBType = BlockType::grid;
    }

    static MarkerArucoRef create(){
        return std::make_shared<MarkerAruco>();
    }

    void setId(int i){mId = i;}
    int getId(){return mId;}

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


private:
    int        mId;
    int        mGridId;

    int        mOrientation;
    glm::vec2  mPos;
    bool       mEnable;

    BlockType  mBType;

    float      mProb;

};
