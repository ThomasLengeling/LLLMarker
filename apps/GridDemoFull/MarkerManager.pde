class MarkerManager {
  Vector<Marker> mMarkers;

  MarkerManager() {
    mMarkers = new Vector<Marker>();

    for (int i = 0; i < gridHeight; i++) {
      for (int j = gridWidht-1; j >= 0; --j) {
        Marker mark = new Marker();

        float x = j * tamX + j * spaceX + startX;
        float y = i * tamY + i * spaceY + startY;

        mark.updateRect(new PVector(x, y), new PVector(tamX, tamY));
        mMarkers.add(mark);
      }
    }
    {
      mMarkers.add(new Marker());
      mMarkers.add(new Marker());
    }
  }


  void drawGrid() {
    rectMode(CORNER);
    for (Marker mark : mMarkers) {
      mark.drawMarker();
    }
  }

  void updateActivation(int index, boolean enable) {
    mMarkers.get(index).enable = enable;
  }

  void updateIdMarker(int index, int id) {
    if(id == -1){
        mMarkers.get(index).enable = false;
    }else{
      mMarkers.get(index).enable = true;
    }
    
    MarkerType mt = new MarkerType();
    mt.setType(int(id));
    
    mMarkers.get(index).type = mt;
    mMarkers.get(index).idAruco = id;
  }

  void updateTypeMarker(int index, MarkerType mt) {
    mMarkers.get(index).type = mt;
  }
}
