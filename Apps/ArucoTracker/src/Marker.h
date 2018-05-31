#pragma once
#include "ofMain.h"

class Rect{
public:
    Rect(){
        for(int i = 0; i < 4; i++){
            mVertex.push_back(glm::vec2(0, 0));
        }
    }

    void setRectPos(glm::vec2 corner, glm::vec2 length){
        mVertex.at(0) = corner;
        mVertex.at(1) = glm::vec2(corner.x + length.x, corner.y);
        mVertex.at(2) = corner + length;
        mVertex.at(3) = glm::vec2(corner.x, length.y + corner.y);;
    }

    void drawRect(){
        ofSetColor(255, 50);
        ofBeginShape();
        for(auto & v : mVertex){
            ofVertex(v);
        }
        ofEndShape();

        ofSetColor(255, 150);
        for(auto & v : mVertex){
            ofDrawCircle(v.x, v.y, 2, 2);
        }
    }

private:
    std::vector<glm::vec2> mVertex;
};

class Marker : public Rect{
public:
    Marker() : Rect() {}

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
