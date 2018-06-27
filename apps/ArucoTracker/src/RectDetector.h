/*

Thomas Sanchez Lengeling
May, 2018

LLL
*/
#pragma once

#include "ofMain.h"
#include "Common.h"

class RectDetector;

class RectDetector{
public:
    RectDetector();

    void setRectPos(glm::vec2 p0, glm::vec2 p1, glm::vec2 p2, glm::vec2 p3);
    void setRectPos(std::vector<glm::vec2> & vec);
    void drawRect();

    void setPos(int i, glm::vec2 pos);

    void subdivde();

private:
    std::vector<glm::vec2> mCornerVertex;
};
