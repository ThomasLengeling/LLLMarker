#include "MarkerAruco.h"

//--------------------------------------------------------------
ProbabiltyAruco::ProbabiltyAruco() {
  mProba = 0.0;
  mInc = 0;
}

void ProbabiltyAruco::incProba() { mInc++; }
float ProbabiltyAruco::getProba(int maxNum) {
  return ((float)mInc / (float)maxNum);
}

void ProbabiltyAruco::resetProba() {
  mProba = 0.0;
  mInc = 0;
}
//--------------------------------------------------------------
RectAruco::RectAruco() {
  for (int i = 0; i < 4; i++) {
    mVertex.push_back(glm::vec2(0, 0));
  }
}

void RectAruco::setRectPos(glm::vec2 corner, glm::vec2 length) {
  mVertex.at(0) = corner;
  mVertex.at(1) = glm::vec2(corner.x + length.x, corner.y);
  mVertex.at(2) = corner + length;
  mVertex.at(3) = glm::vec2(corner.x, length.y + corner.y);
}

void RectAruco::setRectPos(glm::vec2 p0, glm::vec2 p1, glm::vec2 p2,
                           glm::vec2 p3) {
  mVertex.at(0) = p0;
  mVertex.at(1) = p1;
  mVertex.at(2) = p2;
  mVertex.at(3) = p3;
}

void RectAruco::setRectPos(std::vector<glm::vec2> &vec) { mVertex = vec; }

void RectAruco::drawRect() {
  ofSetColor(255, 50);
  ofBeginShape();
  for (auto &v : mVertex) {
    ofVertex(v);
  }
  ofEndShape();

  ofSetColor(255, 150);
  for (auto &v : mVertex) {
    ofDrawCircle(v.x, v.y, 2, 2);
  }
}
