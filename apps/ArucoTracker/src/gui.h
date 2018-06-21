#pragma once

/*

Thomas Sanchez Lengeling
May, 2018

LLL
*/

#include "ofMain.h"
#include "ofxDatGui.h"


class ofxDatSlider;
typedef std::shared_ptr<ofxDatSlider> ofxDatSliderRef;

class ofxDatButton;
typedef std::shared_ptr<ofxDatButton> ofxDatButtonRef;

class ofxDatButton {
public:
    ofxDatButton() {mActive = false;}
    static ofxDatButtonRef create() {
        return std::make_shared<ofxDatButton>();
    }

    void setActivation(bool active){mActive = active;}

    void update(){button->update();}
    void draw(){button->draw();}
    bool isActive(){return mActive;}


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
    void update(){slider->update();}
    void draw(){slider->draw();}

	ofxDatGuiSlider *    slider;
	ofParameter<float>   ofParam;
	ofEventListener      sliderListener;

};
