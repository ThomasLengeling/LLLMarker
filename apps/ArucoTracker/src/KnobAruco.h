#pragma once
#include "ofMain.h"

class KnobAruco;

typedef std::shared_ptr<KnobAruco> KnobArucoRef;

class KnobAruco{
public:

    static KnobArucoRef create(){
        return std::make_shared<KnobAruco>();
    }

    void setStaticId(int id){mStaticId =id;}
    void setDynamicId(int id){mDynamicId =id;}

private:

    //piece that does not move
    int mStaticId;

    //moving piece
    int mDynamicId;

    bool mActive;


};
