
import java.util.Vector;
import controlP5.*;
import codeanticode.syphon.*;
import hypermedia.net.*;

//font
PFont myFont;

//spyphon
SyphonServer server;

int gridWidht = 38;
int gridHeight = 26;

//gui
ControlP5 cp5;
boolean  guiDraw = false;

//UDP
UDP udp;

float startX = 10.0;
float startY = 19.0;

float tamX = 18.0;
float tamY = 18.0;

float spaceX = 8;
float spaceY = 8;

//grid mananager
MarkerManager markerManager;

Knob mKnob;

void settings() {
  size(1024, 768, P3D);
  PJOGL.profile=1;
}


void setup() {

  myFont = createFont("Georgia", 18);
  textFont(myFont);

  //setup UDP
  setupUDP();

  //markers
  markerManager = new MarkerManager();

  mKnob = new Knob();

  smooth();
  // Create syhpon server to send frames out.
  server = new SyphonServer(this, "Processing Syphon");

  cp5 = new ControlP5(this);
  cp5.setAutoDraw (guiDraw);


  cp5.addSlider("startX")
    .setPosition(50, 50)
    .setSize(200, 20)
    .setRange(0, 100)
    .setValue(startX)
    ;

  cp5.addSlider("startY")
    .setPosition(50, 80)
    .setSize(200, 20)
    .setRange(0, 100)
    .setValue(startY)
    ;


  cp5.addSlider("tamX")
    .setPosition(50, 110)
    .setSize(200, 20)
    .setRange(0, 200)
    .setValue(tamX)
    ;

  cp5.addSlider("tamY")
    .setPosition(50, 140)
    .setSize(200, 20)
    .setRange(0, 200)
    .setValue(tamY)
    ;

  cp5.addSlider("spaceX")
    .setPosition(50, 170)
    .setSize(200, 20)
    .setRange(0.0, 100.0)
    .setValue(spaceX)
    ;

  cp5.addSlider("spaceY")
    .setPosition(50, 200)
    .setSize(200, 20)
    .setRange(0.0, 100.0)
    .setValue(spaceY)
    ;
}

void draw() {

  background(0);


  //markerManager = new MarkerManager();
  markerManager.drawGrid();

  server.sendScreen();

  mKnob.draw();
  //mKnob.drawArc();
}


void keyPressed() {
  if (key == 'g') {
    guiDraw = !guiDraw;
    cp5.setAutoDraw (guiDraw);
  }
}

void mouseDragged() {

  {
    float dis = dist(mouseX, mouseY, mKnob.centerX, mKnob.centerY);
    if ( dis > 0 && dis < 20) {
      mKnob.setCenterPos(mouseX, mouseY);
      println(mouseX, mouseY);
    }
  }
}