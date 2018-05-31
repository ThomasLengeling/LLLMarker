#pragma once
#include "ofMain.h"

class RectAruco{
public:
    RectAruco();

    void setRectPos(glm::vec2 corner, glm::vec2 length);
    void drawRect();

private:
    std::vector<glm::vec2> mVertex;
};

class MarkerAruco : public RectAruco{
public:
    MarkerAruco() : RectAruco() {}

    void setId(int i){id = i;}
    void setPos(glm::vec2 mpos){pos = mpos;}

    glm::vec2 getPos(){return pos;}
    int getId(){return id;}

    void enableOn(){enable = true;}
    void enableOff(){enable = false;}

    bool isEnable(){return enable;}

private:
    int       id;
    glm::vec2 pos;
    bool      enable;

};
