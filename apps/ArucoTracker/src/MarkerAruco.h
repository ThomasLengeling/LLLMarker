/*

Thomas Sanchez Lengeling
May, 2018

LLL
*/
#pragma once
#include "ofMain.h"

class MarkerAruco;
class RectAruco;

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

class MarkerAruco : public RectAruco{
public:
    MarkerAruco() : RectAruco() {}

    static MarkerArucoRef create(){
        return std::make_shared<MarkerAruco>();
    }

    void setId(int i){mId = i;}
    void setPos(glm::vec2 mpos){mPos = mpos;}

    glm::vec2 getPos(){return mPos;}
    int getId(){return mId;}

    void enableOn(){mEnable = true;}
    void enableOff(){mEnable = false;}

    bool isEnable(){return mEnable;}

private:
    int       mId;
    int       mOrientation;
    glm::vec2 mPos;
    bool      mEnable;

};
