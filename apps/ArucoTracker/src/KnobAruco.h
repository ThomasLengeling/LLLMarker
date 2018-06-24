#pragma once
#include "ofMain.h"
#include "Common.h"

class KnobAruco;

typedef std::shared_ptr<KnobAruco> KnobArucoRef;

class KnobAruco{
public:

    KnobAruco();

    static KnobArucoRef create(){
        return std::make_shared<KnobAruco>();
    }

    int getBlockType(){return mBType;}


    void setStaticGridId(int & id){mStaticGridId =id;}
    void setDynamicGridId(int & id){mDynamicGridId =id;}

    void setStaticId(int & id){mStaticId =id;}
    void setDynamicId(int & id){mDynamicId =id;}

    int getStaticId(){return mStaticId;}
    int getDynamicId(){return mDynamicId;}

    int getStaticGridId(){return mStaticGridId;}
    int getDynamicGridId(){return mDynamicGridId;}

    void setStaticPos(glm::vec2 pos){mStaticPos = pos;}
    void setDynamicPos(glm::vec2 pos){mDynamicPos = pos;}

    glm::vec2 getStaticPos(){return mStaticPos;}
    glm::vec2 getDynamicPos(){return mDynamicPos;}

    MarkerType getType(){return mMType;}

    void fillArcs();
    void drawArc();
    void draw();
private:

    //piece that does not move
    int mStaticId;
    int mStaticGridId;

    //moving piece
    int mDynamicId;
    int mDynamicGridId;

    bool mStaticActive;
    bool mDynamicActive;

    glm::vec2 mStaticPos;
    glm::vec2 mDynamicPos;

    BlockType mBType;

    MarkerType mMType;
    std::vector< glm::vec2> mArcPos;

    bool mDebugArcPos;

    int mCurrentArc;

    int mNumArcs;
    int mMaxArcs;

    int mStartArc;
    int mFinalArc;

};
