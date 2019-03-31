


void fetchCurrentImageDimensions() {
  int imageSizeW = 1600; 
  int imageSizeH = 1600; 

  JSONArray maskRleArray = currentContourJsonObject.getJSONArray("mask_rle");
  if (maskRleArray != null) {
    int nObjects = maskRleArray.size();
    if (nObjects > 0) {
      JSONObject anObject = maskRleArray.getJSONObject(0); 
      JSONArray imageSizeArr = anObject.getJSONArray("size");
      imageSizeW = imageSizeArr.getInt(1); 
      imageSizeH = imageSizeArr.getInt(0);
    }
  }

  currentImageWidth = imageSizeW; 
  currentImageHeight = imageSizeH;
}


//--------------------------------------------------------------
void drawDetectronPersonContours() {

  // Detectron was calculated on images whose maximum dim was 1600...
  float maxImageDimension = max(currentImageWidth, currentImageHeight);
  float ciw = currentImageWidth;
  float cih = currentImageHeight;
  float imageScale = (float)width / maxImageDimension; 

  fill(0, 0, 0); 
  rect(0, 0, imageScale*ciw, imageScale*cih); 

  JSONArray objects = currentContourJsonObject.getJSONArray("objects");
  if (objects != null) {
    int nObjects = objects.size();
    for (int o=0; o<nObjects; o++) {

      JSONObject anObject = objects.getJSONObject(o); 
      if (anObject != null) {

        String objectType = anObject.getString("class"); 
        if (objectType.equals("person")) {

          JSONArray contours = anObject.getJSONArray("contours"); 
          if (contours != null) {

            int nContours = contours.size();
            for (int c=0; c<nContours; c++) {

              JSONArray aContour = contours.getJSONArray(c); 
              if (aContour != null) {

                int nPoints = aContour.size();

                noStroke(); 
                fill(255);
                beginShape();
                for (int p=0; p<nPoints; p++) {
                  JSONArray aPoint = aContour.getJSONArray(p); 
                  float px = imageScale * aPoint.getFloat(0);
                  float py = imageScale * aPoint.getFloat(1);
                  vertex(px, py);
                }
                endShape(CLOSE);
              }
            }
          }
        }
      }
    }
  }
}
