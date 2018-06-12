#pragma once
#include "ofMain.h"
#include "Common.h"


class Block{

public:

    Block(){
        mPos = glm::vec2(0, 0);
        mId = -1;
        mEnable = false;
    }


    glm::vec2 mPos;
    int mId;
    bool mEnable;
    MarkerType mMType;

private:

};
