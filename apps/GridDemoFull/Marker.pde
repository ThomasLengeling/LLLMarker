class Marker {
  Vector<PVector> rect;
  boolean enable = true;
  int idAruco;

  PVector center;

  MarkerType type;
  color cId;

  Marker() {
    center = new PVector();
    rect = new Vector<PVector>();
    for (int i = 0; i < 4; i++) {
      PVector vr = new PVector(0, 0);
      rect.add(vr);
    }
    idAruco = -1;

    type = new MarkerType();
    cId = color(0, 0, 255);
  }

  void updateRect(PVector v0, PVector v1, PVector v2, PVector v3) {
    rect.set(0, v0);
    rect.set(1, v1);
    rect.set(2, v2);
    rect.set(3, v3);
  }


  void updateRect(PVector v0, PVector length) {
    rect.set(0, v0);
    rect.set(1, new PVector(v0.x + length.x, v0.y));
    rect.set(2, new PVector(v0.x + length.x, v0.y + length.y));
    rect.set(3, new PVector(v0.x, v0.y + length.x));

    float cx = 0;
    float cy = 0;
    for (PVector rct : rect) {
      cx += rct.x;
      cy += rct.y;
    }
    center = new PVector(cx/4.0, cy/4.0);
  }


  void drawMarker() {
    if (enable) {

      fill(type.getColorType());
      stroke(255);
      beginShape();
      for (PVector rct : rect) {
        vertex(rct.x, rct.y);
      }
      endShape(CLOSE);

      fill(0);
      textSize(5);
      text(idAruco, center.x - 17, center.y);
    } else {
      beginShape();

      fill(255, 255, 0);
      stroke(255);
      for (PVector rct : rect) {
        vertex(rct.x, rct.y);
      }
      endShape();

      fill(255);
      textSize(18);
      text(idAruco, center.x -17, center.y);
    }
  }
}
