#pragma once
#include "Common.h"
#include "ofMain.h"

/*
Class for each  Physical Block
*/

class Block;
typedef std::shared_ptr<Block> BlockRef;

class Block {

public:
  Block() {
    mPos = glm::vec2(0, 0);
    mRot = glm::vec2(0, 0);
    mEnable = false;
    mIdType = std::make_pair(-1, 0);
  }

  static BlockRef create() { return std::make_shared<Block>(); }

  glm::vec2 getPos() { return mPos; }
  void setPos(glm::vec2 p) { mPos = p; }

  int getMarkerId() { return mIdType.first; }
  void setMarkerId(int id) { mIdType.first = id; }

  int getType() { return mIdType.second; }
  void setType(int type) { mIdType.second = type; }

private:
  glm::vec2 mPos;
  glm::vec2 mRot;
  bool mEnable;
  MarkerType mMType;

  std::pair<int, int> mIdType;
};
