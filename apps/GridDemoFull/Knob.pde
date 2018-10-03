class Knob {
  float posx;
  float posy;

  float centerX;
  float centerY;

  int div;

  int currentArc;

  Vector<PVector> arcPos; 

  int maxArcs;
  int numArcs;

  int startArc = 2;
  int finalArc = 1;

  MarkerType  type;

  boolean debugArcs = false;

  Knob() {
    currentArc  = -1;
    centerX = 957;
    centerY = 445;

    type = new MarkerType();

    maxArcs = 10;
    numArcs = 10 - startArc - finalArc;
    arcPos = new Vector<PVector>();
    fillArcPos();
  }

  void setCenterPos(float centerX, float centerY) {
    this.centerX = centerX;
    this.centerY = centerY;

    arcPos.clear();
    fillArcPos();
  }

  void setPos(float px, float py) {
    this.posx = px;
    this.posy = py;
  }

  void fillArcPos() {
    float dim = 64;
    int ang =10;
    for (int i = startArc; i < ang - finalArc; i++) {
      float a =  i*(TWO_PI/ang);
      float xcos = cos( a ) * dim + centerX;
      float ysin = sin( a ) * dim + centerY;
      arcPos.add(new PVector(xcos, ysin));
    }
  }


  void drawArc() {
    boolean gotArc =false;
    for (int i = 0; i < numArcs; i++) {
      float dis = dist(arcPos.get(i).x, arcPos.get(i).y, posx, posy);
      if (dis > 0.0 && dis < 36) {
        currentArc = i;
        gotArc = true;
        type.setType(currentArc);
        break;
      } else {
      }
    }

    if (gotArc == false) {
      currentArc = -1;
      type.setType(currentArc);
    }

    if (debugArcs) {

      for (int i = 0; i < numArcs; i++) {
        float x = arcPos.get(i).x;
        float y = arcPos.get(i).y;

        if (currentArc == i) {
          noStroke();
          fill(255, 0, 0);
          ellipse(x, y, 35, 35);
        } else {
          noStroke();
          fill(100 + 15*i);
          ellipse(x, y, 35, 35);
        }

        stroke(255);
        line(x, y, centerX, centerY);
      }
    }
  }



  void draw() {
    rectMode(CENTER);
    noStroke();
    fill( type.getColorType());
    rect(centerX, centerY, tamX, tamY);

    // fill(0, 255, 255);
    //ellipse(posx, posy, 1, 1);

    //stroke(255, 255, 0);
    // line(centerX, centerY, posx, posy);
  }
}