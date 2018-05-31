#pragma once

#include "ofMain.h"
#include "ofxDatGui.h"


class ofxDatSlider;
typedef std::shared_ptr<ofxDatSlider> ofxDatSliderRef;

class ofxDatButton;
typedef std::shared_ptr<ofxDatButton> ofxDatButtonRef;

class ofxDatButton {
public:
    ofxDatButton() {}
    static ofxDatButtonRef create() {
        return std::make_shared<ofxDatButton>();
    }

    ofxDatGuiButton     * button;
    ofParameter<bool>     ofParamBoolean;
    ofEventListener       buttonListener;
    bool                  mActive;

};


class ofxDatSlider {
public:
	ofxDatSlider() {}
	static ofxDatSliderRef create() {
		return std::make_shared<ofxDatSlider>();
	}

	ofxDatGuiSlider *  slider;
	ofParameter<int>   ofParamInt;
	ofEventListener    sliderListener;

};
