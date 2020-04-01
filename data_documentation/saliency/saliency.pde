// Simple test program to preview saliency maxima. 
// Written for Processing v.3.5.3
PImage img;

void setup() {
  size(449, 640); 
  img = loadImage("jpg640/15974_demo.jpg"); 
  noLoop();
}

void draw() {
  background(127); 
  image(img, 0, 0, img.width, img.height); 
  stroke(255, 255, 100);
  noFill(); 

  // Saliency rects for image #15974 obtained from
  // row #8228 (numbering from 0) of saliency_nms.tsv
  // (id' using canonical_filename_order.txt)
  float saliencyData[] = {
    0.015, 0.160, 1, 
    0.296, 0.225, 41, 
    0.452, 0.291, 46, 
    0.702, 0.335, 135, 
    0.202, 0.554, 16, 
    0.702, 0.686, 24
  };

  
  int nSaliencyBoxes = saliencyData.length/3;
  for (int i=0; i<nSaliencyBoxes; i++) {
    float bX = saliencyData[(i*3)+0] * img.width;
    float bY = saliencyData[(i*3)+1] * img.height;
    float bR = saliencyData[(i*3)+2];

    strokeWeight(1);
    ellipse(bX, bY, bR, bR);
    strokeWeight(2);
    ellipse(bX, bY, 1, 1);
  }
}
*/
