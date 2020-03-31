PImage img;

void setup() {
  size(640, 640); 
  img = loadImage("jpg640/15974_demo.jpg"); 
  noLoop(); 
}

void draw() {
  background(127); 
  image(img, 0, 0, img.width, img.height); 
  
  noFill(); 
  strokeWeight(2); 
  stroke(255,255,100);
  rectMode(CORNER);
  
  
  float largestDimension = max(img.width, img.height); 
  float centerCropX = 0; 
  float centerCropY = 0; 
  float centerCropW = img.width;
  float centerCropH = img.height;
  if (img.height == largestDimension){
    centerCropX = 0; 
    centerCropY = (img.height - img.width)/2;
    centerCropW = img.width;
    centerCropH = centerCropW;
  } else {
    centerCropX = (img.width - img.height)/2;
    centerCropY = 0;
    centerCropH = img.height;
    centerCropW = centerCropH;
  }
  rect(centerCropX, centerCropY, centerCropW, centerCropH); 

  // Saliency rects for image #15974 obtained 
  // from row #8229 of saliency_nms_boxes.tsv
  // (using canonical_filename_order.txt)
  float saliencyData[] = {
    0.252, 0.078, 0.089, 0.062, 
    0.409, 0.172, 0.089, 0.062, 
    0.659, 0.234, 0.089, 0.062
  };
  
  // WHY IS THIS NECESSARY FOR NMS RECTS TO LINE UP?
  float WEIRD_OFFSET_X = 0; //-9; 
  float WEIRD_OFFSET_Y = 0; //36;
  
  int nSaliencyBoxes = saliencyData.length/4;
  for (int i=0; i<nSaliencyBoxes; i++){
    float bX = saliencyData[(i*4)+0] * img.width;
    float bY = saliencyData[(i*4)+1] * img.height;
    float bW = saliencyData[(i*4)+2] * img.width;
    float bH = saliencyData[(i*4)+3] * img.height;
    rect(WEIRD_OFFSET_X+bX,WEIRD_OFFSET_Y+bY, bW,bH); 
  }
}
