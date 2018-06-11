#pragma once
#include "ofMain.h"


//block types
enum BlockType{
    grid = 0,
    knobStatic = 1,
    knobDynamic = 2,
    button= 3
};

enum MarkerName{
    house_0 = 0,
    house_1 = 1,
    house_2 = 2,
    office_0 = 3,
    office_1 = 4,
    office_2 = 5
};


class MarkerType{
public:
    MarkerType(){
        mName = "";
        setType(0);
    }

    void setType(int t){
        mType  = static_cast<MarkerName>(t);
        switch(mType){
        case 0:
            mType = MarkerName::house_0;
            break;
        case 1:
            mType = MarkerName::house_1;
            break;
        case 2:
            mType = MarkerName::house_2;
            break;
        case 3:
            mType = MarkerName::office_0;
            break;
        case 4:
            mType = MarkerName::office_1;
            break;
        case 5:
            mType = MarkerName::office_2;
            break;
        }
    }

private:
    ofColor mColor;
    std::string mName;
    MarkerName  mType;

};
