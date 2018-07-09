#include "GridDetector.h"

GridDetector::GridDetector(glm::vec2 dim) {
  mGridDim = dim;
  mDebugGrid = false;
  mRecordOnce = true;
  mCalibrateGrid = false;
  mCleanDone = false;

  mRadDetection = RAD_DETECTION;
  mMaxMarkers = mGridDim.x * mGridDim.y;
}
//-----------------------------------------------------------------------------
void GridDetector::setMaxMarkers(int max) {
  mMaxMarkers = max;
  ofLog(OF_LOG_NOTICE) << "Max Markers: " << mMaxMarkers;
}
//-----------------------------------------------------------------------------
void GridDetector::setId(int id) { mId = id; }

//-----------------------------------------------------------------------------
void GridDetector::setupBlocks() {
  for (int i = 0; i < mMaxMarkers; i++) {
    QRBlockRef block = QRBlock::create();
    block->setMarkerId(i);
    mBlocks.push_back(block);
  }

  ofLog(OF_LOG_NOTICE) << "setup blocks ";
}

void GridDetector::generateGridPos() {
  mMarkers.clear();
  float startGridX = 1920 * 0.1;
  float startGridY = 1080 * 0.1;

  float stepX = 50.0;
  float stepY = 50.0;

  float gapX = 13;
  float gapY = 13;

  int indeY = 0;
  int indeX = 0;
  ofLog(OF_LOG_NOTICE) << "Max Markers: " << mMaxMarkers;
  for (int i = 0; i < mMaxMarkers; i++) {
    MarkerArucoRef m = MarkerAruco::create();

    float x = indeX * stepX + indeX * gapX + startGridX;
    float y = indeY * stepY + indeY * gapY + startGridY;

    m->setPos(glm::vec2(x, y));
    m->setGridId(i);
    m->setMarkerId(-1);
    mMarkers.push_back(m);
    indeX++;
    if (indeX >= mGridDim.x) {
      indeY++;
      indeX = 0;
    }
  }
}

//-----------------------------------------------------------------------------
void GridDetector::setupCleaner() {

  // cleaner
  mWindowCounter = 0;
  mWindowIterMax = 3;

  for (int i = 0; i < mMaxMarkers; i++) {
    mIdsCounter.emplace(i, 0); // mFullIds.at(i), 0);
    mProCounter.emplace(i, 0);
  }

  // ids from 0 -1000, max number of counters
  for (int i = 0; i < MAX_MARKERS; i++) {
    mCenterCounter.emplace(i, 0);
  }

  ofLog(OF_LOG_NOTICE) << "setup clean";
}
//-----------------------------------------------------------------------------
void GridDetector::setupGridJsonPos(std::string filePos) {
  ofLog(OF_LOG_NOTICE) << "Loading gridpos json: " << filePos;
  ofFile file(filePos); //"gridpos.json");
  if (file.exists()) {
    ofJson js;
    file >> js;
    int i = 0;
    for (auto &gridPos : js) {
      if (i < mMaxMarkers) {
        MarkerArucoRef m = MarkerAruco::create();
        m->setMarkerId(-1);

        int type = gridPos[to_string(i)]["type"];

        if (type == BlockType::grid) {

          float posx = gridPos[to_string(i)]["posx"];
          float posy = gridPos[to_string(i)]["posy"];

          m->setRectPos(
              glm::vec2(posx - 20, posy - 20), glm::vec2(posx + 20, posy - 20),
              glm::vec2(posx + 20, posy + 20), glm::vec2(posx - 20, posy + 20));

          m->setPos(glm::vec2(posx, posy));
          m->setGridId(i);
          mMarkers.push_back(m);
        }
      }
      i++;
    }
  } else {
    ofLog(OF_LOG_NOTICE) << "cannot find file";
  }
}

//-----------------------------------------------------------------------------
void GridDetector::generateMarkers(std::vector<int> &ids,
                                   std::vector<QRBlockRef> &blocks, bool sort) {
  mTagsIds = ids;
  mCurrBlock = blocks;

  // clasification of ids and blocks
  if (sort) {
    std::sort(mCurrBlock.begin(), mCurrBlock.end(),
              [](const QRBlockRef &lhs, const QRBlockRef &rhs) -> bool {
                return lhs->getPos().x < rhs->getPos().x;
              });
    ofLog(OF_LOG_NOTICE) << "sorted";
  }
  mNumMarkers = mCurrBlock.size();
  mTmpBlocks.push_back(mCurrBlock);
}
//-----------------------------------------------------------------------------
void GridDetector::drawBlock(float posx, float posy, float size, float space) {
  int i = 0;
  int j = 0;
  float squareSize = size;
  float squareSpace = space;
  for (auto &block : mBlocks) {

    ofSetColor(0, 255, 255);

    float x = i * squareSize + i * squareSpace + posx;
    float y = j * squareSize + j * squareSpace + posy;
    ofDrawRectangle(glm::vec2(x, y), squareSize, squareSize);

    ofSetColor(255);
    ofDrawBitmapString(block->getMarkerId(), x + squareSize / 3.0,
                       y + squareSize * (1.0 / 3.0));
    ofDrawBitmapString(block->getType(), x + squareSize / 3.0,
                       y + squareSize * (2.0 / 3.0));
    i++;
    if (i >= mGridDim.x) {
      i = 0;
      j++;
    }
  }
}
void GridDetector::drawDetectedGridIn(float posx, float posy, float size,
                                      float space) {
  int i = 0;
  int j = mGridDim.y - 1;
  float squareSize = size;
  float squareSpace = space;
  for (auto &mk : mMarkers) {
    if (mk->isEnable()) {
      ofSetColor(0, 200, 255);
    } else {
      ofSetColor(255, 255, 0);
    }
    float x = i * squareSize + i * squareSpace + posx;
    float y = j * squareSize + j * squareSpace + posy;
    ofDrawRectangle(glm::vec2(x, y), squareSize, squareSize);

    ofSetColor(255);
    ofDrawBitmapString(mk->getGridId(), x + squareSize / 3.0,
                       y + squareSize * (1.0 / 3.0));
    ofDrawBitmapString(mk->getMarkerId(), x + squareSize / 3.0,
                       y + squareSize * (2.0 / 3.0));
    i++;
    if (i >= mGridDim.x) {
      i = 0;
      j--;
    }
  }
}
//-----------------------------------------------------------------------------
void GridDetector::drawDetectedGrid(float posx, float posy, float size,
                                    float space) {
  int i = 0;
  int j = 0;
  float squareSize = size;
  float squareSpace = space;
  for (auto &block : mCurrBlock) {
    if (block->getType() == -1) {
      ofSetColor(255, 255, 0);
    } else {
      ofSetColor(0, 200, 255);
    }
    float x = i * squareSize + i * squareSpace + posx;
    float y = j * squareSize + j * squareSpace + posy;
    ofDrawRectangle(glm::vec2(x, y), squareSize, squareSize);

    ofSetColor(255);
    float strx = x + squareSize / 3.0;
    float stry1 = y + squareSize * (1.0 / 3.0);
    float stry2 = y + squareSize * (2.0 / 3.0);
    ofDrawBitmapString(block->getType(), strx, stry1);
    ofDrawBitmapString(block->getMarkerId(), strx, stry2);
    i++;
    if (i >= mGridDim.x) {
      i = 0;
      j++;
    }
  }
}
//-----------------------------------------------------------------------------
void GridDetector::updateBlockTypes() {
  // update blocks and types
  for (auto &block : mBlocks) {
    int id = block->getMarkerId();
    for (auto &mk : mMarkers) {
      if (mk->getIdTypePair().first == id) {
        mk->updateTypePair(block->getType());
        break;
      }
    }
  }
}

//-----------------------------------------------------------------------------
void GridDetector::setGridPos(glm::vec2 mousePos) {
  if (mDebugGrid) {
    for (auto &mk : mMarkers) {
      glm::vec2 pos = mk->getPos();
      float dist = glm::fastDistance(pos, mousePos);
      if (dist >= 0.0 && dist <= 15) {
        mk->setPos(mousePos);
      }
    }
  }
}
//-----------------------------------------------------------------------------
void GridDetector::drawMarkers() {
  for (auto &mk : mMarkers) {
    glm::vec2 pos = mk->getPos();

    if (mk->isEnable()) {
      ofSetColor(255);
      ofDrawCircle(pos.x, pos.y, 7, 7);
      ofSetColor(200, 80);
      ofDrawCircle(pos.x, pos.y, mRadDetection / 2.0, mRadDetection / 2.0);
    } else {
      ofSetColor(255, 255, 0);
      ofDrawCircle(pos.x, pos.y, 4, 4);
      ofSetColor(255, 355, 100);
      ofDrawCircle(pos.x, pos.y, mRadDetection / 2.0, mRadDetection / 2.0);
    }

    if (mDebugGrid) {
      ofSetColor(0, 100, 200, 100);
      ofDrawCircle(pos.x, pos.y, mRadDetection, mRadDetection);
    }

    ofSetColor(255);
    ofDrawBitmapString(mk->getMarkerId(), pos.x - 20, pos.y - 7);
    ofDrawBitmapString(mk->getGridId(), pos.x - 25, pos.y - 17);
  }
}
//-----------------------------------------------------------------------------
void GridDetector::saveGridJson() {
  // save json
  ofJson writer;
  int i = 0;
  for (auto &mk : mMarkers) {
    ofJson pt;
    pt[to_string(i)]["posx"] = mk->getPos().x;
    pt[to_string(i)]["posy"] = mk->getPos().y;
    pt[to_string(i)]["type"] = mk->getBlockType();
    writer.push_back(pt);
    i++;
  }
  ofLog(OF_LOG_NOTICE) << "json write: 0" + to_string(mId + 1) + " - " +
                              to_string(mId);
  ofSaveJson("gridpos_0" + to_string(mId + 1) + ".json", writer);
}

//-----------------------------------------------------------------------------
void GridDetector::calibrateGrid() {
  // draw grid
}
//-----------------------------------------------------------------------------
void GridDetector::recordGrid() {
  if (mRecordOnce) {
    // ofLog(OF_LOG_NOTICE) << mCurrBlock.size() << " " << mMarkers.size();
    if (mCurrBlock.size() == mMaxMarkers) {
      ofLog(OF_LOG_NOTICE) << mCurrBlock.size() << " markes = " << mMaxMarkers;

      ofLog(OF_LOG_NOTICE) << "Detect";
      // set ids
      mFullIds.clear();
      for (auto &mk : mMarkers) {
        glm::vec2 pos = mk->getPos();
        int k = 0;
        for (auto &cblock : mCurrBlock) {
          glm::vec2 cenPos = cblock->getPos();
          float dis = glm::fastDistance(cenPos, pos);
          if (dis >= 0.0 && dis <= mRadDetection) {
            mk->setMarkerId(mTagsIds.at(k));

            // got ids/
            mFullIds.push_back(mTagsIds.at(k));
            break;
          }
          k++;
        }
      }

      ofLog(OF_LOG_NOTICE) << "Done update fullids";

      {
        /*
      int i = 0;
      for (auto &block : mBlocks) {
        for (auto &marker : mMarkers) {
          if (block->getMarkerId() == marker->getIdTypePair().first) {
            int mkId = marker->getIdTypePair().first;
            block->setMarkerId(mkId);
            // std::cout << block->getMarkerId() << " " << mkId <<
            // std::endl;
            break;
          }
          i++;
        }
      }
      */
      }

      ofLog(OF_LOG_NOTICE) << "Num Uniques: " << mFullIds.size();
      ofLog(OF_LOG_NOTICE) << "Done Recording";

      mRecordOnce = false;
    }
  }
}

//-----------------------------------------------------------------------------
void GridDetector::cleanGrid() {
  if (mWindowCounter >= mWindowIterMax) {

    mWindowCounter = 0;

    // clasical probabilty of ocurance

    // ofLog(OF_LOG_NOTICE) << "reset proba";
    for (auto &mk : mMarkers) {
      mk->resetProba();
    }

    // ofLog(OF_LOG_NOTICE) << "calculate freq";
    // calculate the frequency of ocurance
    for (auto &blocks : mTmpBlocks) {
      for (auto &block : blocks) {

        glm::vec2 blockPos = block->getPos();

        int k = 0;
        for (auto &mk : mMarkers) {
          glm::vec2 boardPos = mk->getPos();
          float dis = glm::fastDistance(blockPos, boardPos);
          if (dis >= 0 && dis <= mRadDetection) {
            mIdsCounter[k] = block->getMarkerId(); // block.mId
            mk->incProba();
            // not sure i need it break;
            break;
          }
          k++;
        }
      }
    }

    // ofLog(OF_LOG_NOTICE) << "Update";

    // send upd data and activations;
    int i = 0;
    for (auto &mk : mMarkers) {
      float proba = mk->getProba(mWindowIterMax);
      if (proba >= 1.0 / (float)mWindowIterMax) {
        mk->enableOn();
        mk->setMarkerId(mIdsCounter[i]);
        mk->updateIdPair(mIdsCounter[i]);

        // find id and update it;
        /*
        for (auto &blocks : mBlocks) {
          int makerId = mk->getIdTypePair().first;
          if (blocks->getMarkerId() == makerId) {
            mk->updateType(blocks->getType());
            // need it?
            break;
          }
        }
        */
      } else {
        mk->enableOff();
        mk->setMarkerId(-1);
      }
      i++;
    }

    // done activation and disactivation
    mTmpBlocks.clear();
    // ofLog(OF_LOG_NOTICE) << "Clear";

    // update blocks and types
    // updateBlockTypes();
  }
  mWindowCounter++;
}
