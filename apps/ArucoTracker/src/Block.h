#pragma once
#include "ofMain.h"
#include "Common.h"

/*
Class for each  Physical Block
*/

class Block{

public:

    Block(){
        mPos = glm::vec2(0, 0);
        mId = -1;
        mEnable = false;
        mIdType = std::make_pair(mId, 0);
    }


    glm::vec2 mPos;
    int mId;
    bool mEnable;
    MarkerType mMType;

    std::pair<int, int> mIdType;

private:

};
