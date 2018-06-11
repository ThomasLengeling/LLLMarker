#include "KnobAruco.h"

KnobAruco::KnobAruco() {
  mStaticId = -1;
  mDynamicId = -1;

  mDynamicGridId = -1;
  mStaticGridId = -1;

  mStaticActive = false;
  mDynamicActive = false;

  mStaticPos = glm::vec2(50, 50);
  mDynamicPos = glm::vec2(100, 50);

  mDebugArcPos = true;

  mStartArc = 0;
  mFinalArc = 0;
  mMaxArcs = 8;
  mNumArcs = mMaxArcs - mStartArc - mFinalArc;

  fillArcs();

  mCurrentArc = 0;
}

void KnobAruco::fillArcs() {
  float dim = 116;
  mArcPos.clear();
  for (int i = mStartArc; i < mMaxArcs - mFinalArc; i++) {
    float a = i * (TWO_PI / (float(mMaxArcs)));
    float startAngle = PI + (TWO_PI / (float(mMaxArcs)));
    float cosx = cos(a + startAngle) * dim + mStaticPos.x;
    float siny = sin(a + startAngle) * dim + mStaticPos.y;
    mArcPos.push_back(glm::vec2(cosx, siny));
  }
}

void KnobAruco::drawArc() {

  fillArcs();
  bool gotArc = false;

  int i = 0;
  for (auto &arc : mArcPos) {
    float dis = ofDist(arc.x, arc.y, mDynamicPos.x, mDynamicPos.y);
    if (dis > 0.0 && dis < 38) {
      mCurrentArc = i;
      mMType.setType(mCurrentArc);
      gotArc = true;
      break;
    }
    i++;
  }

  if (!gotArc) {
    mCurrentArc = -1;
    mMType.setType(mCurrentArc);
  }

  if (mDebugArcPos) {
    int i = 0;
    for (auto &arcs : mArcPos) {

      if (mCurrentArc == i) {
        ofSetColor(255, 0, 0);
        ofDrawCircle(arcs.x, arcs.y, 8);
      } else {
        ofSetColor(0, 0, 50 + i * 10);
        ofDrawCircle(arcs.x, arcs.y, 5);
        ofDrawBitmapString(i, arcs.x - 10, arcs.y - 3);
      }
      i++;
    }
  }
}

void KnobAruco::draw() {
  if (mStaticActive) {
    ofSetColor(255, 255, 255);
    // ofDrawCircle(mStaticPos.x, mStaticPos.y, 7, 7);
  } else {
    ofSetColor(255, 255, 0);
    // ofDrawCircle(mStaticPos.x, mStaticPos.y, 4, 4);
  }

  ofDrawBitmapString("s", mStaticPos.x - 25, mStaticPos.y + 10);
  // ofDrawBitmapString(mStaticId, mStaticPos.x - 20, mStaticPos.y - 7);

  if (mDynamicActive) {
    ofSetColor(255, 255, 255);
    // ofDrawCircle(mDynamicPos.x, mDynamicPos.y, 7, 7);
  } else {
    ofSetColor(255, 255, 0);
    // ofDrawCircle(mDynamicPos.x, mDynamicPos.y, 4, 4);
  }

  ofDrawBitmapString("d", mDynamicPos.x - 25, mDynamicPos.y + 10);
  ofDrawBitmapString(mDynamicId, mDynamicPos.x - 25, mDynamicPos.y + 15);
}
