#pragma once

#include <memory>

#include "ofMain.h"
#include "CommonTypes.h"

/*
Class for each  Physical Block
*/

class QRBlock;
typedef std::shared_ptr<QRBlock> QRBlockRef;

class QRBlock {

public:
  QRBlock() {
    mPos = glm::vec2(0, 0);
    mRot = glm::vec2(0, 0);
    mEnable = false;
    mIdType = std::make_pair(-1, 0);
  }

  static QRBlockRef create() { return std::make_shared<QRBlock>(); }

  glm::vec2 getPos() { return mPos; }
  void setPos(glm::vec2 p) { mPos = p; }

  int getMarkerId() { return mIdType.first; }
  void setMarkerId(int id) { mIdType.first = id; }

  int getType() { return mIdType.second; }
  void setType(int type) { mIdType.second = type; }

  void setRot(glm::vec2 rot){mRot = rot;}
  glm::vec2 getRot(){return mRot;}

private:
  glm::vec2 mPos;
  glm::vec2 mRot;

  bool mEnable;
  MarkerType mMType;

  std::pair<int, int> mIdType;
};
