class MarkerType {
  int type;
  color col;
  String name;

  MarkerType() {
    name = "";
    //default type 
    setType(0);
  }


  void setType(int t) {
    type = t; 

    switch(type) {
    case 0:
      name = "house-0";
      col = color(50, 100, 150);
      break;
    case 1:
      name = "house-1";
      col = color(50, 200, 90);
      break;
    case 2:
      name = "house-2";
      col = color(150, 0, 255);
      break;
    case 3:
      name = "house-3";
      col = color(255, 150, 50);
      break;
    case 4:
      name = "house-4";
      col = color(0, 255, 255);
      break;
    case 5:
      name = "house-5";
      col = color(255, 0, 150);
      break;
    case 6:
      name = "house-6";
      col = color(0, 255, 255);
      break;
    }
  }

  color getColorType() {
    return col;
  }

  String getName() {
    return name;
  }
}