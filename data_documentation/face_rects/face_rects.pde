PImage img; 

void setup() {
  size(449, 640);
  img = loadImage("15974.png"); 
  noLoop(); 
}

void draw() {
  background(0); 
  image(img, 0, 0); 
  float faceRects[] = {0.635,  0.29,  0.079,  0.056};
  
  noFill(); 
  strokeWeight(2);
  stroke(255,255,100); 
  
  int nFaces = faceRects.length/4; 
  for (int i=0; i<nFaces; i++){
    float fX = faceRects[(i*4)+0] * img.width;
    float fY = faceRects[(i*4)+1] * img.height; 
    float fW = faceRects[(i*4)+2] * img.width; 
    float fH = faceRects[(i*4)+3] * img.height; 
    rect(fX,fY,fW,fH); 
  }
}
