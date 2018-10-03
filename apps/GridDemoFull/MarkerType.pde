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
      case -1:
       col = color(255, 255, 0);
      break;
    case 0:
      name = "house-0";
      col = color(50, 100, 150);
      break;
    case 9:
      name = "house-1";
      col = color(50, 200, 90);
      break;
    case 19:
      name = "house-2";
      col = color(150, 0, 255);
      break;
    case 43:
      name = "house-3";
      col = color(255, 150, 50);
      break;
    case 63:
      name = "house-4";
      col = color(0, 255, 255);
      break;
    case 126:
      name = "house-5";
      col = color(255, 0, 150);
      break;
    case 138:
      name = "house-6";
      col = color(0, 255, 255);
      break;
      
    case 176:
    case 296:
    case 306:
    case 375:
    case 400:
    case 460:
    case 469:
    case 488:
    case 563:
    case 606:
    case 645:
    case 682:
    case 715:
    case 799:
    case 827:
    case 873:
    case 988:
    case 996:
      col = color(255, 155, 155);
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
