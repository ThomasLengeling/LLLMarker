

void setupUDP() {

  // create a new datagram connection on port 6000
  // and wait for incomming message
  udp = new UDP( this, 15800 );
  //udp.log( true );     // <-- printout the connection activity
  udp.listen( true );
}


void receive( byte[] data, String ip, int port ) {  // <-- extended handler


  // get the "real" message =
  // forget the ";\n" at the end <-- !!! only for a communication with Pd !!!
  data = subset(data, 0, data.length-1);
  String message = new String( data );

  // read the header
  {
    String[] splitH = splitTokens(message, " \n");
    if (splitH[0].equals("header")) {
      println("got header");
      int i =0;
     
      for ( String header : splitH) {
        if (header.equals("ncols")) {
          println("ncosls "+splitH[i+1]);
        }
        if (header.equals("nrows")) {
          println("nrows "+splitH[i+1]);
        }
        if(header.equals("cellsize")){
            println("cell "+splitH[i+1]);
        }
        i++;
      }
    }
  }
  
   // read the types
  {
    String[] splitT = message.split(" ");
    int i =0;
    if (splitT[0].equals("type")) {
      // println("got types");
      for ( String type : splitT) {
        if ((type).equals("type")) {
          continue;
        } else {
          if ( i < gridHeight * gridWidht) {
            MarkerType mt = new MarkerType();
            mt.setType(int(type));
            markerManager.updateTypeMarker(i, mt);
          }
          i++;
        }
      }
    }
  }


  //enables
  {
    String[] splitE = message.split(" ");
    int i =0;
    if (splitE[0].equals("e")) {
      //println("got enables");
      for ( String enables : splitE) {
        if (enables.equals("e")) {
          continue;
        } else {
          if ( i < gridHeight * gridWidht) {
            markerManager.updateActivation(i, (enables.equals("1") ? true : false));
          }
          i++;
        }
      }
    }
  }

  //ids
  {
    String[] splitI = message.split(" ");
    int i =0;
    if (splitI[0].equals("i")) {
      //println("got ids");
      for ( String ids : splitI) {
        if (ids.equals("i")) {
          continue;
        } else {
          if ( i < gridHeight * gridWidht) {
            markerManager.updateIdMarker(i, int(ids));
          }
          i++;
        }
      }
    }
  }




  {
    String[] splitT = message.split(" ");
    if (splitT[0].equals("dkn")) {
      mKnob.setPos(map(float(splitT[1]), 0, 1920, width, 0) + 30, map(float(splitT[2]), 0, 1080, 0, height));
    }
  }

  {
    String[] splitT = message.split(" ");
    if (splitT[0].equals("skn")) {
      if (splitT.length >= 4) {
        int idStatic = int(splitT[1]);
        mKnob.setCenterPos(map(float(splitT[3]), 0, 1920, width, 0) +30, map(float(splitT[4]), 0, 1080, 0, height));
        mKnob.type.setType(int(splitT[2]));
        //println("id :"+ idStatic +" "+int(splitT[2]));
      }
    }
  }


  // print the result
  // println( "receive: \""+message+"\" from "+ip+" on port "+port );
}