// Program to generate contour images from detectron JSON contour data.

boolean bDoSavePNGOutput = false; // main switch. Set true for PNG output
String pathToJsons;
float imageScale = 0.5;

ArrayList<File> arrayListOfFiles;
ArrayList<String> arrayListOfJsonFilenames;
String mySketchPath;
String imageFilenames[]; 
int whichImageIndex = 0;
int nJsonFiles;

int currentImageWidth; 
int currentImageHeight; 
int currentImageNumber;
String currentContourJsonFilename; 
JSONObject currentContourJsonObject; 

//--------------------------------------------------------
void setup() {
  size(800, 800);
  imageScale = width/1600.0;
  frameRate(3); 

  // String pathToJsons = "/Volumes/zariaTeenie/TEENIE/data_from_detectron/Teenie_detectron/Teenie_detectron_json_boxed/";
  String pathToJsons = sketchPath() + "/input_detectron_sample_jsons/"; 
  println(pathToJsons);

  arrayListOfJsonFilenames = new ArrayList<String>();
  arrayListOfFiles = listFilesRecursive(pathToJsons); 

  int nFiles = arrayListOfFiles.size();
  for (int i=0; i<nFiles; i++) {
    String ithFilename = (arrayListOfFiles.get(i)).toString(); 
    if (ithFilename.endsWith(".json")) {
      arrayListOfJsonFilenames.add( ithFilename);
    }
  } 
  nJsonFiles = arrayListOfJsonFilenames.size();
}


void keyPressed() {
}



//--------------------------------------------------------------
void draw() {
  background(127);

  currentContourJsonFilename = arrayListOfJsonFilenames.get(whichImageIndex); 
  currentContourJsonObject = loadJSONObject(currentContourJsonFilename);

  fetchCurrentImageDimensions(); 
  drawDetectronPersonContours();
  loadPixels();

  int dstWidth = (int)(imageScale * currentImageWidth);
  int dstHeight = (int)(imageScale * currentImageHeight);
  PImage outputImage = createImage(dstWidth, dstHeight, RGB);
  outputImage.loadPixels();
  for (int y=0; y<dstHeight; y++) {
    for (int x=0; x<dstWidth; x++) {
      int srcIndex = y*width + x;
      int dstIndex = y*dstWidth + x;
      outputImage.pixels[dstIndex] = pixels[srcIndex];
    }
  }
  outputImage.updatePixels();

  int indexOfLastSlash = 1+currentContourJsonFilename.lastIndexOf('/'); 
  int lastCharIndex = currentContourJsonFilename.length(); 
  String justTheJsonFilename = currentContourJsonFilename.substring(indexOfLastSlash, lastCharIndex); 
  int lastDotIndex = justTheJsonFilename.lastIndexOf('.'); 
  String justTheNumber = justTheJsonFilename.substring(0, lastDotIndex); 
  String outputImageFilename = "output_rendered_contours/" + justTheNumber + ".png";
  
  if (bDoSavePNGOutput) { // main switch
    outputImage.save(outputImageFilename); 
    println ("Saved: " + whichImageIndex + "\t" + outputImageFilename);
  } else {
    println ("Viewing: \t" + whichImageIndex + "\t" + justTheJsonFilename);
  }

  if (whichImageIndex < (nJsonFiles-1)) {
    whichImageIndex++;
  } else {
    noLoop();
    println("DONE at " + (millis()/1000) + " seconds");
  }
}
