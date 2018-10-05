#include "ofApp.h"
#include "ofMain.h"

//========================================================================
int main() {
  ofSetupOpenGL(1920, 1080, OF_WINDOW); // <-------- setup the GL context
  ofSetWindowPosition(0, 0);
  ofRunApp(new ofApp());
}
