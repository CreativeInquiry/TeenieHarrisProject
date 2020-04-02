#pragma once

/* ofApp.h
 * Contains main app logic
 * - Lingdong 2019-2020
 */

// enums for control modes
#define CONTROL_MOUSE 0
#define CONTROL_TRACKPAD 2
#define CONTROL_TOUCHSCREEN 1 // normal
#define CONTROL_AUTOPILOT 3   // screensaver

#define PARTICIPANT_DETECT_MODE 1 // 0 for face, 1 for pose

#define WTHRESH_GUIHIDE 0.025 // zoom threshold at which to hide top and bottom gui
#define WTHRESH_IMAGESELECT 0.25 // zoom threshold at which images can be selected (yellow box)
#define WTHRESH_DOUBLETAP 0.25 // zoom threshold at which double-tap-enter-image is enabled
#define MIN_FINGER_DIST 100 // finger distance threshold for some zoom gestures

#define HIDE_GUI_IN_AUTOPILOT (true)

// number of frames of inactivity before screensaver kicks in
#define TIMER_START_AUTOPILOT 60/*fps*/*60/*sec/min*/*3/*min*/

#include "ofFixes.h" // things OF should have but doesn't

#include "ofMain.h" // OF boilerplate stuff

#include "viewport.h" // viewport calculation and interaction
#include "gui.h" // graphical user interface
#include "thp.h" // main algorithms & components

#include "participant.h" // people tracking (face/pose/etc)

#include "ofxOsc.h" // open sound control (for receiving pose, trackpad, etc.)
#include "ofxMultitouch.h" // native touch screen support
#include "multitouch.h" // touch screen algorithms


class ofApp : public ofBaseApp {
public:
  ofFpsCounter fpsCounter; // frame-per-second tracking

#if PARTICIPANT_DETECT_MODE == 0
  Participant::Face::Thread faceThread;
  ofVideoGrabber webcam;
#endif

  int controlMode = CONTROL_TOUCHSCREEN; // MOUSE/TRACKPAD/TOUCHSCREEN/AUTOPILOT

  int currentImageIndex = -1; // index of image with the yellow frame
  int potentialImageIndex = -1; // index of image with semi-transparent yellow frame
  bool isViewingImage = false; // is viewing the high-res image? i.e. not in grid view
  bool isViewingAnalysis = false;
  bool willViewImage = false; // is about to view the high-res image, but is not because the high-res is still loading
  int beenViewingImage = 0; // # of frames the image is being viewed. Some interactions are disabled when you just entered the image
                            // this is also used to time some animations
  int imageViewBgAlpha = 0; // Fade out the grid view when viewing the image
  bool isSwitchingImageWoExiting = false; // when swiping, change image, but don't exit to grid view

  bool isViewingAnalysisNext = false; // is viewing analysis of similar faces

  THP::ImageLoadThread imageLoader; // image-loading thread

  int automatedExitImage = 0; // When set to x>0, will automatically zoom out gradually from image view to grid view for x frames 
  int automatedEnterImage = 0; // When set to 1, will automatically zoom into an image e.g. when someone double tapped on it

  int noTouch = 0; // disable some interactions when animation is playing

  ofTexture vignetteTex; // texture for the vignette
  ofTexture gradientTex; // bg texture for top and bottom gui

  float topGuiHeight = 0;       // current height of the top gui
  float topGuiHeightTarget = 0; // height of top gui will try to lerp to this number
  float bottomGuiHeight = 0;       // ditto
  float bottomGuiHeightTarget = 0; // ditto
  bool guiUpsideDown = false;   // are you in wheel chair?

  float topSwipeMaskHeight = 0; // mask out top thumbnails while swiping between images
  float bottomSwipeMaskHeight = 0; // ditto

  float analysisGuiTop = 10000; // top position of analysis

  THP::Recents recents; // component+gui for recent images
  THP::Recents fames;   // component+gui for famous images

  GUI::RadioTable* guiEmbeddingSelector; // gui for selecting which embedding to use
  GUI::Icon* guiSortIcon;  // a useless picture showing the sort icon 
  GUI::Button* guiBackButton; // the big back button in image viewing mode
  GUI::Button* guiUpsideDownButton; // the button for wheel chaired people
  GUI::Button* guiFameButton;  // gui to toggle between famous images and recent images
  GUI::Icon* guiButtonBg;  // a black shadow to put below fame button and upsideDown button
  GUI::Shield* guiShieldFU; // block touch events around Fame/Upsidedown buttons
  GUI::Shield* guiShieldBottom; // block touch events on bottom gui
  GUI::Shield* guiShieldTop;  // block touch events on top gui
  GUI::Label* guiImageInfo; // Image information in image viewing mode
  GUI::Label* guiTinyInfo;  // image information in grid mode

  bool showFPS = false;

  int noInteractionTimer = 0; // number of frames during which nobody touched the screen

  int idxUnderFing1 = -1; // index of image under first finger (used to determine which image to zoom into when using two finger gesture)
  int idxUnderFing2 = -1; // index of image under second finger

  float autoPlaySpeedFactor = 1; //100;  // speed of autoplaying
  float automatedExitImageSpeed = 1; // speed of automatically exiting the image
  float automatedEnterImageSpeed = 1; // speed of automatically entering the image

  int automatedNavigateToImageIndex = -1; // index of another image to automatically navigate to, e.g. when matching faces
  int automatedNavigateFromImageIndex = -1; // index of image from which the automated navigation starts
  int automatedNavigateToImage = -1; // state of automated navigation. -1 is off, otherwise a timer
  int automatedNavigateToImageTime = 800; // time it takes to navigate from image to image (variably calculated)

  bool isFamous = false; // is gui showing famous images instead of recent images?
  bool isPreviewingButton = false; // is finger hovering over a button to show the hint?

  // text data for hints, padded with font-specific number of spaces to simulate right-alignment
  string hintFame1 = "                                        Tap to show the curators' picks";
  string hintFame2 = "                            Tap to show the recent viewing history";
  string hintUpsideDown1 = "            Tap to swap the sort-buttons and curators' picks";
  string hintUpsideDown2 = "Tap to swap the sort-buttons and recent viewing history";
  bool messageFixBottomRight = false; // is the message right aligned (yes for famous & recent buttons)

  vector<string> thumbPaths;
  int thumbLoadIndex = 0;

  void setup() {
	  cout << "                                                           " << endl;
	  cout << "                   The  Frank - Ratchye                    " << endl;
	  cout << "                   ,- --- . . .-  . .-.                    " << endl;
	  cout << "                   '-. !  ! ! ! ) ! ! !                    " << endl;
	  cout << "                   '-' '  `-' '-  ' '-'                    " << endl;
	  cout << "                   for Creative Inquiry                    " << endl;
	  cout << "                                                           " << endl;
	  cout << "                                                           " << endl;
	  cout << "                         Presents                          " << endl;
	  cout << "                                                           " << endl;
	  cout << ":                                                         :" << endl;
	  cout << ":   ___ ___ ___ _  _ ___ ___   _ _  ,  __   _  ___  __    :" << endl;
	  cout << ":    |  |_  |_  |\\ |  |  |_    |_| /_\\ |_) |_)  |  (_     :" << endl;
	  cout << ":    |  |__ |__ | \\| _|_ |__   | | | | | \\ | \\ _|_ __)    :" << endl;
	  cout << ":                                                         :" << endl;
	  cout << ":    T H E     I N T E R A C T I V E    D I S P L A Y     :" << endl;
	  cout << ":                                                         :" << endl;
	  cout << ":                                                         :" << endl;



    THP::loadPaths("paths.txt");
#if PARTICIPANT_DETECT_MODE == 0
    webcam.setup(320, 240);
#endif

    // use UV range 0.0-1.0 to be independent of texture size
    ofDisableArbTex();

    // set viewport constraints
    // constants are defined in thp.h
    THP::viewport.set(VIEWPORT0);
    THP::viewport.zero = VIEWPORT0;
    THP::viewport.maxWidth = VIEWPORT_MAXW;
    THP::viewport.minWidth = VIEWPORT_MINW;
    THP::viewport.hardMaxWidth = VIEWPORT_HARD_MAXW;
    THP::viewport.hardMinWidth = VIEWPORT_HARD_MINW;

    THP::loadCanonicalFilenames("canonical_filename_order.txt"); // load filenames and boxnames
    THP::loadDescriptions("descriptions.tsv"); // load descriptions, date, accession numbers
    THP::loadFamous(fames,"famous.txt"); // load "hero"/famous images into the famous bar

    THP::FaceMatch::load("bounding_boxes/face-nn7.tsv","bounding_boxes/4-serv-face-boxes.tsv");  // load face matches, first path is matched faces, second path is all boxes only

    THP::KenBurns::loadFaces("bounding_boxes/face-boxes.tsv"); // load faces and saliency boxes for Ken Burns effect during screensaver

    cout << "\n\nLoading..." << endl; // takes a while ...

    // thumbnail textures are separated into 16 spritesheets due to hardware limit
    thumbPaths.push_back(THP::dataPaths["thumbs"]+"thumbs_texture_00_256x256.png");
    thumbPaths.push_back(THP::dataPaths["thumbs"]+"thumbs_texture_01_256x256.png");
    thumbPaths.push_back(THP::dataPaths["thumbs"]+"thumbs_texture_02_256x256.png");
    thumbPaths.push_back(THP::dataPaths["thumbs"]+"thumbs_texture_03_256x256.png");
    thumbPaths.push_back(THP::dataPaths["thumbs"]+"thumbs_texture_04_256x256.png");
    thumbPaths.push_back(THP::dataPaths["thumbs"]+"thumbs_texture_05_256x256.png");
    thumbPaths.push_back(THP::dataPaths["thumbs"]+"thumbs_texture_06_256x256.png");
    thumbPaths.push_back(THP::dataPaths["thumbs"]+"thumbs_texture_07_256x256.png");
    thumbPaths.push_back(THP::dataPaths["thumbs"]+"thumbs_texture_08_256x256.png");
    thumbPaths.push_back(THP::dataPaths["thumbs"]+"thumbs_texture_09_256x256.png");
    thumbPaths.push_back(THP::dataPaths["thumbs"]+"thumbs_texture_10_256x256.png");
    thumbPaths.push_back(THP::dataPaths["thumbs"]+"thumbs_texture_11_256x256.png");
    thumbPaths.push_back(THP::dataPaths["thumbs"]+"thumbs_texture_12_256x256.png");
    thumbPaths.push_back(THP::dataPaths["thumbs"]+"thumbs_texture_13_256x256.png");
    thumbPaths.push_back(THP::dataPaths["thumbs"]+"thumbs_texture_14_256x256.png");
    thumbPaths.push_back(THP::dataPaths["thumbs"]+"thumbs_texture_15_256x256.png");

    // load embeddings (arrangement of thumbnails) into gui buttons
    guiEmbeddingSelector = THP::loadEmbeddingsToNamedButtons(vector<string>({
      "Visual Similarity", "embeddings/assignment/embedding_inception_features_0.001_05.tsv",
      "Keywords", "embeddings/new_combined_features_m4_0.001_10.tsv",
      "Year", "embeddings/sort/rect-sort-year.tsv",
      "Age", "embeddings/sort/rect-sort-age-median.tsv",
      "Headcount", "embeddings/sort/rect-sort-n-faces.tsv",
      "Luminosity", "embeddings/sort/rect-sort-color-avg-rev.tsv",
      "Accession #", "embeddings/rect-sort-acc.tsv",
      "Mosaic", "embeddings/rect-teenie-mosaic.tsv",

    }),4);
    guiEmbeddingSelector->pad = ofVec2f(20,15); // selector button padding

    // descriptions of embeddings shown when an embedding is first selected
    THP::embeddingDescriptions = vector<string>({
      "Photographs are clustered by their visual similarity.",
      "Photographs are clustered by the similarity of their textual descriptions.",
      "Photographs are sorted by the (estimated) year in which they were taken, \nfrom earliest (top) to most recent (bottom).Pink images have no year data.",
      "Photographs are sorted by the average (estimated) age of the people in each image, \nfrom youngest (top) to oldest (bottom).Pink images have no age data. ",
      "Photographs are sorted by the (estimated) number of faces in each image, \nfrom fewest (top) to most (bottom).In the pink images, faces were not detected.",
      "Photographs are sorted by their average brightness, \nfrom lightest (top) to darkest (bottom). ",
      "Photographs are sorted by their accession number: the unique \nidentifier assigned when they entered the Museum's collection.",
      "Zoom out to see a photomosaic of Teenie Harris!",
     
    });

    // load text labels to draw on top of the grid, corresponding to each embedding
    THP::LabelEmbedding::loadEmpty(); // load empty when no text needs to be drawn
    THP::LabelEmbedding::load("labels/label-embedding-desc.tsv", ofVec2f(-7, -2)); // keyword clusters
    THP::LabelEmbedding::load("labels/label-embedding-year2.tsv", ofVec2f(0, 2));  // 1945, 1960 etc.
    THP::LabelEmbedding::load("labels/label-embedding-age.tsv",ofVec2f(0,2));      // babies, teens, etc.
    THP::LabelEmbedding::load("labels/label-embedding-head.tsv",ofVec2f(0,2));     // singles, pairs, groups, etc.
    THP::LabelEmbedding::loadEmpty();
    THP::LabelEmbedding::loadEmpty();
    THP::LabelEmbedding::load("labels/label-embedding-mosaic.tsv",ofVec2f(-65,-1)); // Teenie Harris's name and dates

    guiSortIcon = GUI::createElement<GUI::Icon>("icons/sort2.png"); // logo/icon for embedding selector that doesn't do anything
    guiBackButton = GUI::createElement<GUI::Button>("Back"); // back button, shown in image viewing mode
    guiBackButton->iconPath = "icons/back.png"; // icon on the back button
    guiBackButton->style.lineHeight = 0.4;      // visual adjustments

    guiButtonBg = GUI::createElement<GUI::Icon>("fx/corner-gradient.png"); // background shadow behind fame/upsideDown buttons
    guiShieldFU = GUI::createElement<GUI::Shield>("shield-fu"); // touch event blocker under fame/upsideDown buttons to stop FAT fingers
    guiShieldTop = GUI::createElement<GUI::Shield>("shield-top"); // ditto for top gui
    guiShieldBottom = GUI::createElement<GUI::Shield>("shield-bottom"); // ditto

    // draw hideous blinking red debug boxes
    //guiShieldBottom->debug = true;
    //guiShieldTop->debug = true;

    // button for wheel-chaired people
    guiUpsideDownButton = GUI::createElement<GUI::Button>("upside-down-btn");
    guiUpsideDownButton->iconPath = "icons/swap_buttons_history.png";
    guiUpsideDownButton->altIconPath = "icons/swap_history_buttons.png";
    guiUpsideDownButton->altIconPath2 = "icons/swap_buttons_star.png";
    guiUpsideDownButton->altIconPath3 = "icons/swap_star_buttons.png";
    guiUpsideDownButton->style.lineHeight = 1.0;
    guiUpsideDownButton->text = "";

    // button for switching between famous and recent images
    guiFameButton = GUI::createElement<GUI::Button>("fame-btn");
    guiFameButton->iconPath = "icons/history.png";
    guiFameButton->altIconPath = "icons/star.png";
    guiFameButton->style.lineHeight = 1.0;
    guiFameButton->text = "";

    guiImageInfo = GUI::createElement<GUI::Label>("image-info"); // image desription/info in image viewing mode
    guiTinyInfo = GUI::createElement<GUI::Label>("tiny-image-info"); // image descript/info in grid mode

    #if PARTICIPANT_DETECT_MODE == 0
      faceThread.startThread(true);
    #endif

    // register touch event thandlers from multitouch.h
    ofAddListener(Multitouch::onAnyTouch, this, &ofApp::onAnyTouch);
    ofAddListener(Multitouch::offAnyTouch, this, &ofApp::offAnyTouch);
    ofAddListener(Multitouch::onFirstTouch, this, &ofApp::onFirstTouch);
    ofAddListener(Multitouch::onSecondTouch, this, &ofApp::onSecondTouch);
    ofAddListener(Multitouch::onTap, this, &ofApp::onTap);
    ofAddListener(Multitouch::onDoubleTap, this, &ofApp::onDoubleTap);
    ofAddListener(Multitouch::onSwipe, this, &ofApp::onSwipe);

    // register gui event handlers from gui.h
    ofAddListener(GUI::onEvent, this, &ofApp::onGUIEvent);
          
    ofLoadImage(vignetteTex, "fx/vignette.png", IMAGE_LOAD_GRAYSCALE); // vignete effect on all edges of screen
    ofLoadImage(gradientTex, "fx/gradient.png", IMAGE_LOAD_GRAYSCALE); // gradient effect on top and bottom edges

    ofHideCursor(); // make mouse cursor invisible

    setGuiRects(); // place gui elements at correct places
    imageLoader.startThread(true); // fire up threaded image loader, which will be constantly waiting for images you tell it to load 
    ofToggleFullscreen(); // full screen on start up, Shift-F to toggle. WARN: Fullscreen + VisualStudio + Segfault == Reboot, NO WAY OUT
  
    THP::imageViewport.inertiaOff = true; // turn off inertia for image viewing to avoid jank behavior
    THP::viewport.inertiaOff = false; // inertia for grid mode seems to be OK
    
    THP::imageViewport.constrainStrictness = ofVec4f(0.25,0,0.25,0);
    THP::viewport.constrainStrictness = ofVec4f(0.25, 0.2, 0.25, 0.2);

    // load previously saved logs (for user-tracking) if there are any
    THP::Logger::loadImageLog();
    THP::Logger::loadTouchLog();
  }

  // exit from image viewing mode, only resets the flags, does not handle animation, etc.
  void unsetIsViewingImage() {
    isViewingImage = false;
    isViewingAnalysis = false;
    if (controlMode != CONTROL_AUTOPILOT) {// in screensaver mode, don't log!
      THP::Logger::imageOut();
    }
    recents.add(imageLoader.id); // add to recent images bar
  }

  // position all the gui's at the right places
  void setGuiRects() {
    
    // variables used for complex calculation of top and bottom gui heights
    float guiHeight1 = 350;
    float guiHeight2 = 370;

    if (isViewingImage) { // if is viewing image, top and bottom gui should be hidden
      topGuiHeightTarget = 0;
      bottomGuiHeightTarget = 0;
    } else { // otherwise it is more complicated...
      bottomGuiHeightTarget = THP::viewport.width > VIEWPORT0.width * WTHRESH_GUIHIDE ? (guiHeight2) : 0;
      topGuiHeightTarget = (THP::viewport.width > VIEWPORT0.width * WTHRESH_GUIHIDE && !automatedEnterImage) ? (guiHeight1) : 0;
      if (isSwitchingImageWoExiting) {
        topGuiHeightTarget = 0;
      }
    }
    if (automatedNavigateToImage >= 0 || isViewingAnalysisNext) {
      // when automatically navigating to images (face hyperlink), hide top and bottom GUI
      bottomGuiHeightTarget = 0;
      topGuiHeightTarget = 0;
    }
    // lerp gui height toward target gui height, so it doesn't look abrupt.
    // in screensaver, do the same thing, but slower
    topGuiHeight = ofLerp(topGuiHeight, (HIDE_GUI_IN_AUTOPILOT && controlMode==CONTROL_AUTOPILOT) ? 0: topGuiHeightTarget, (controlMode == CONTROL_AUTOPILOT) ? 0.02 : 0.3);
    bottomGuiHeight = ofLerp(bottomGuiHeight, (HIDE_GUI_IN_AUTOPILOT && controlMode == CONTROL_AUTOPILOT) ? 0 : bottomGuiHeightTarget, (controlMode == CONTROL_AUTOPILOT) ? 0.02 : 0.3);

    // calculate where to put recent/famous bars
    float rw = virtualWindowWidth() * (float)(recents.cols) / (float)(recents.cols + 1);
    float rx = virtualWindowWidth() * (float)(1) / (float)(recents.cols + 1);
    float rh = virtualWindowWidth() * (float)(1) / (float)(recents.cols + 1);
    float ry = guiUpsideDown ? (topGuiHeight - guiHeight1) : (virtualWindowHeight() - bottomGuiHeight);

    rx += 200; // shifting patch
    guiFameButton->useAltTexture = isFamous; // display correct icon for recent/famous button
    guiUpsideDownButton->useAltTexture = (isFamous)?(guiUpsideDown?0:1):(guiUpsideDown ? 2 : 3);
    
    // squash/expand the recent/famous bars depending on which is desired
    if (isFamous) {
      recents.update(ofRectangle(100, ry, rw, rh));
      fames.update(ofRectangle(100, ry + 100, rw, 0));
    } else {
      fames.update(ofRectangle(100, ry, rw, rh));
      recents.update(ofRectangle(100, ry + rh + 100, rw, 0));
    }


    // draw embedding/sort method description
    if (messageFixBottomRight) {
      THP::Message::update(ofRectangle(virtualWindowWidth()-1160, virtualWindowHeight()-500, virtualWindowWidth() - 300, 72));
    } else {
      THP::Message::update(ofRectangle(guiUpsideDown ? 70 : 215, guiUpsideDown ? (virtualWindowHeight() - bottomGuiHeight - 150) : (topGuiHeight - 25), virtualWindowWidth() - 300, 72));
    }
    THP::Message::label->layer = 2;
    // more calculations to draw the embedding selectors
    float h1, h2, h3;
    bool b = (isViewingImage || isSwitchingImageWoExiting);
    if (!guiUpsideDown) {
      h1 = (b ? (0) : (topGuiHeight)) - 310;
      h2 = (b ? (topGuiHeight) : (0)) - 270;
    } else {
      h1 = virtualWindowHeight() - (b ? (0) : (bottomGuiHeight));
      h2 = virtualWindowHeight() - (b ? (bottomGuiHeight) : (0));
    }
    h3 = h2;
    if (isViewingImage) {
      h3 = virtualWindowHeight() - bottomGuiHeight;
      float bt = (THP::imageTexture.getHeight() - THP::imageViewport.y) / THP::imageViewport.height*MAPWIN_H + (virtualWindowHeight() - (float)MAPWIN_H) / 2;
      h3 = fmin(virtualWindowHeight() - 250, fmin(bt + 20, h3));
      if (isViewingAnalysis) {

      }
    } else {
      h3 = -10000000;
    }

    if (h1 < virtualWindowHeight() / 2){ // not upsidedown
      guiEmbeddingSelector->setRect(ofRectangle(200, h1, virtualWindowWidth() - 240, 280));
      guiSortIcon->setRect(ofRectangle(40, h1-7, 150, 150));
      // block touch events behind gui
      guiShieldTop->setRect(ofRectangle(0, h1 - 40, virtualWindowWidth(), 335));
      guiShieldBottom->setRect(ofRectangle(0, ry+22, virtualWindowWidth(), rh + 50));
      
    } else { // upside down
      h1 += 12;
      guiEmbeddingSelector->setRect(ofRectangle(50, h1, virtualWindowWidth() - 280, 280));
      guiSortIcon->setRect(ofRectangle(-200, h1, 120, 120));
      // block touch events behind gui
      guiShieldTop->setRect(ofRectangle(0, h1 - 10, virtualWindowWidth(), 380));
      guiShieldBottom->setRect(ofRectangle(0, ry, virtualWindowWidth(), rh - 18));
    }

    guiShieldTop->layer = -1; // layer -1 : above the graphics but below other GUI elements
    guiShieldBottom->layer = -1; // layer -1 : above the graphics but below other GUI elements

    // calculate position for back button and image description text
    guiBackButton->setRect(ofRectangle(50, h3, 420, 180));
    guiImageInfo->setRect(ofRectangle(515, h3-12, virtualWindowWidth()-775, 50));

    // background shadow for fame and recent buttons
    guiButtonBg->setRect(ofRectangle(virtualWindowWidth() - 350, virtualWindowHeight() - 750 * !isViewingImage + 150 - bottomGuiHeight+guiHeight2, 350, 600));
    
    // draw famous/recents 
    guiFameButton->setRect(ofRectangle(virtualWindowWidth() - 160, virtualWindowHeight()-330*!isViewingImage+5 - bottomGuiHeight + guiHeight2-18, 110, 110));
    guiUpsideDownButton->setRect(ofRectangle(virtualWindowWidth() - 160, virtualWindowHeight() - 193 * !isViewingImage + 5 - bottomGuiHeight + guiHeight2 - 15, 110, 110));

    guiButtonBg->layer = 1;
    guiButtonBg->clickThrough = true;

    // block touch events under famous and reccent buttons
	if (!guiUpsideDown) {
		guiShieldFU->setRect(ofRectangle(virtualWindowWidth() - 220, virtualWindowHeight() - 750 * !isViewingImage + 350 - bottomGuiHeight + guiHeight2, 220, 400));
	}else {
		guiShieldFU->setRect(ofRectangle(-1, -1, 1, 1));
	}
	guiShieldFU->layer = 1;

    // place button on top layer
    guiUpsideDownButton->layer = 2;
    guiFameButton->layer = 2;
    
    // draw small preview description in grid mode
    if (!isViewingImage && THP::viewport.width < VIEWPORT0.width * 0.05) {
      ofVec2f p0 = THP::thumbMatrixToScreenMatrix(currentImageIndex, ofVec2f(-THUMBSMESH_THUMB_W / 2, THUMBSMESH_THUMB_W / 2));
      ofVec2f p1 = THP::thumbMatrixToScreenMatrix(currentImageIndex, ofVec2f(THUMBSMESH_THUMB_W / 2, THUMBSMESH_THUMB_W / 2));
      ofVec2f p2 = THP::thumbMatrixToScreenMatrix(currentImageIndex, ofVec2f(0, 0.5));
      guiTinyInfo->maxHeight = 2 * (p2.y - p0.y);
      guiTinyInfo->computeAutoNewline();
      guiTinyInfo->setRect(ofRectangle(p0.x - 2, 2 + p0.y, (p1.x - p0.x)*0.91, 18));
    } else {
      guiTinyInfo->setRect(ofRectangle(0, -1000, 1000, 18));
    }
  }

  // overlay red mask on top of thumbnails whre data is not available
  float redOverlayOpacity = 0;
  void redOverlayHack() {
    string ename = THP::embeddingNames[THP::targetEmbeddingIdx];

    // these are the embeddings that contain missing info
    if (ename == "Age" || ename == "Headcount" || ename == "Year" || ename == "Accession #") {
      redOverlayOpacity = ofLerp(redOverlayOpacity, 1, 0.02);
    } else {// otherwise be transparent
      redOverlayOpacity = ofLerp(redOverlayOpacity, 0, 0.02);
    }
    ofPushStyle();

    // transform matrix to grid space
    ofPushMatrix();
    ofTranslate(ofVec2f((virtualWindowWidth() - MAPWIN_W) / 2, (virtualWindowHeight() - MAPWIN_H) / 2));
    ofScale((float)MAPWIN_W / THP::viewport.width, (float)MAPWIN_H / THP::viewport.height);
    ofTranslate(-THP::viewport.x, -THP::viewport.y);

    ofEnableBlendMode(OF_BLENDMODE_MULTIPLY);
    float t = 255 - 55 * redOverlayOpacity * ofMap(THP::viewport.width/2, 0.01, VIEWPORT0.width*0.05, 0, 1, true);
    ofSetColor(255, (int)t, (int)t);

    ofTranslate(0.1, -0.1);

    // specific box sizes for different embeddings
    if (ename == "Year" ) {
      ofDrawRectangle(-(float)FACT2 / 2.0 - 0.5, -(float)FACT1 / 2.0 - 0.5, FACT2 + 1, 1.2);
      ofDrawRectangle(-(float)FACT2 / 2.0 - 0.5, -(float)FACT1 / 2 - 0.5 + 1.2, 167.5, 1);
    } else if (ename == "Accession #") {
      ofDrawRectangle(-(float)FACT2 / 2.0 - 0.5, -(float)FACT1 / 2.0 - 0.5, FACT2 + 1, 1.2);
      ofDrawRectangle(-(float)FACT2 / 2.0 - 0.5, -(float)FACT1 / 2 - 0.5 + 1.2, 140.5, 1);
    } else {
      ofDrawRectangle(-(float)FACT2 / 2.0 - 0.5, -(float)FACT1 / 2.0 - 0.5, FACT2 + 1, 40.2);
      ofDrawRectangle(-(float)FACT2 / 2.0 - 0.5, -(float)FACT1 / 2 - 0.5 + 40.2, 131.5, 1);
    }
    ofDisableBlendMode();
    ofPopMatrix();
    ofPopStyle();
  }

  // determine whether one can swipe to the prev/next image at given instant
  bool isSwipable(){
    //cout << (isViewingAnalysis && THP::FaceMatch::wasDetail >= 0) << endl;
    bool full = false;
    
    //if (THP::imageTexture.getWidth() > THP::imageTexture.getHeight()) {
      full = (THP::imageViewport.width > THP::imageTexture.getWidth()*0.95);

    //} else {
      //full = (THP::imageViewport.height > THP::imageTexture.getHeight()*0.95);
    //}
    return isViewingImage && beenViewingImage > 30 && !automatedExitImage &&
      full && (! (isViewingAnalysis && THP::FaceMatch::wasDetail >= 0));
  }

  void draw() {
	  if (thumbLoadIndex < thumbPaths.size()) {
		  ofBackground(0);
		  string txt = "Loading... " + ofToString(thumbLoadIndex) + "/" + ofToString(thumbPaths.size());
		  //ofDrawBitmapString(txt, 50, 50);
		  GUI::font.drawString(txt, 100, 100);
		  THP::loadThumbs(thumbPaths[thumbLoadIndex]);
		  thumbLoadIndex++;
		  return;
	  }


    ofPushMatrix();
    ofScale(1.0/((float)HALF+1));
    ofBackground(0);

    THP::lerpToTargetEmbedding(0.02, currentImageIndex);
    
    // draw the grid
    // first argument is antialiasing
    THP::drawMap(controlMode == CONTROL_AUTOPILOT || THP::viewport.width > 20 || Multitouch::nTouches() != 0);
    
    // draw red shade over thumbnails with unavailable data
    redOverlayHack();

    // draw text labels on top of thumbnails (year, keyword, etc.)
    THP::LabelEmbedding::draw();

    // calculate & draw swipe masks, which puts a shade over top and bottom thumbnails when swiping horizontally
    ofPushStyle();
    ofSetColor(0, 0, 0, ofMap(THP::viewport.width / 2, 0.01, VIEWPORT0.width*0.05, 255, 0, true));
    ofDrawRectangle(0, 0, virtualWindowWidth(), topSwipeMaskHeight-20);
    ofDrawRectangle(0, virtualWindowHeight()-bottomSwipeMaskHeight+20, virtualWindowWidth(), bottomSwipeMaskHeight);
    ofPopStyle();
    if (!isSwitchingImageWoExiting) {
      topSwipeMaskHeight = 0;
      bottomSwipeMaskHeight = 0;
    } else {
      if (imageLoader.id >= 0) {
        float nby = THP::thumbMatrixToScreenMatrix(imageLoader.id, ofVec2f(0, THUMBSMESH_THUMB_W / 2)).y;
        bottomSwipeMaskHeight = ofLerp(bottomSwipeMaskHeight, virtualWindowHeight() - nby, 0.5);
      }
    }

    // draw yellow frame over selected image
    if (currentImageIndex != -1 && !automatedExitImage) {
      ofPushMatrix();
      THP::enterThumbMatrix(currentImageIndex);
      ofPushStyle();
      ofSetColor(255,255,0);
      // ofNoFill();
      // ofSetLineWidth(3);
      // ofDrawRectangle(-THUMBSMESH_THUMB_W/2, -THUMBSMESH_THUMB_W/2, THUMBSMESH_THUMB_W, THUMBSMESH_THUMB_W);
      
      ofDrawRectangleOutline(ofRectangle(-THUMBSMESH_THUMB_W / 2, -THUMBSMESH_THUMB_W / 2, THUMBSMESH_THUMB_W, THUMBSMESH_THUMB_W), 3* THP::viewport.width/(float)MAPWIN_W, 1, false);

      ofPopStyle();
      ofPopMatrix();
    }
    // draw semi-translucent yellow frame over image that's about to be selected
    // i.e. received touch down but not touch up. So the gesture might be a swipe/pinch, in which case this image shouldn't be selected
    // hence drawing it in semi-translucency to show responsiveness.
    if (potentialImageIndex != -1) {
      ofPushMatrix();
      THP::enterThumbMatrix(potentialImageIndex);
      ofPushStyle();
      ofSetColor(80,80,0);
      ofDrawRectangleOutline(ofRectangle(-THUMBSMESH_THUMB_W / 2, -THUMBSMESH_THUMB_W / 2, THUMBSMESH_THUMB_W, THUMBSMESH_THUMB_W), 3 * THP::viewport.width / (float)MAPWIN_W, 1, false);

      ofPopStyle();
      ofPopMatrix();
    }

    // for estimating position of real human observer
    #if PARTICIPANT_DETECT_MODE == 0
      float faceShift = faceThread.faceShift;
      float estimateDist = faceThread.estimateDist;
    #else
      float faceShift = Participant::PoseOSC::faceShift;
      float estimateDist = Participant::PoseOSC::estimateDist;
    #endif
     
    //faceShift = ofClamp(faceShift, -1, 1);
    //estimateDist = ofClamp(estimateDist, 0, 0.4);
    //float shiftFun = 0.5 + 0.5*cos(PI*faceShift);
    //THP::drawDepth(-faceShift * estimateDist*PI*0.5);

    // get two touches; can be null
    Multitouch::Touch* touch1;
    Multitouch::Touch* touch2;
    std::tie(touch1, touch2) = Multitouch::getTwoTouches();


    // fade out background when entering image from recnts/famous bars
    bool didTheAlpha = false; // make sure background shade is drawn only once
    if (recents.selected != -1 || fames.selected != -1) {
      imageViewBgAlpha = ofLerp(imageViewBgAlpha, 255, 0.1);
      ofPushStyle();
      ofSetColor(0, 0, 0, imageViewBgAlpha);
      ofDrawRectangle(0, 0, virtualWindowWidth(), virtualWindowHeight());    
      ofPopStyle();
      didTheAlpha = true;
    }

    // main logic during image viewing mode
    if (isViewingImage) {
      if (beenViewingImage > 10) {
        isSwitchingImageWoExiting = false; // disable swipe animations after viewing image for a while
        currentImageIndex = -1; // unselect selected thumbnail, so when exiting the image, user is not suprised by a pre-selected thumbnail
      }

      // turn on the analysis mode if previous image was in analysis mode
      if (beenViewingImage > 5) { 
        if (isViewingAnalysisNext && !isViewingAnalysis) {
          if (automatedNavigateToImage == -1 && imageLoader.id == automatedNavigateToImageIndex) {
            isViewingAnalysisNext = false;
            isViewingAnalysis = true;
          }
        }
      }

      // update grid viewport from image viewport, but not when autonavigating
      if (automatedNavigateToImage <= 0) {
        THP::calculateViewportFromImageViewport(imageLoader.id);
      }

      // draw the shade over grid when viewing image
      // using complex logic
      ofPushStyle();
      if (!didTheAlpha) {
        if (isSwipable() && Multitouch::nTouches() && !isViewingAnalysis) {
          imageViewBgAlpha = ofLerp(imageViewBgAlpha, 0, 0.01);

        } else {
          if (THP::imageTexture.getHeight() / THP::imageTexture.getWidth() > virtualWindowHeight() / virtualWindowWidth()) {
            imageViewBgAlpha = ofLerp(imageViewBgAlpha,
              fmin(255, fmin(beenViewingImage * 20, 2000 * THP::imageTexture.getWidth() / THP::imageViewport.width)),
              0.1);
          } else {
            imageViewBgAlpha = ofLerp(imageViewBgAlpha,
              fmin(255, fmin(beenViewingImage * 20, 500 * THP::imageTexture.getWidth() / THP::imageViewport.width)),
              0.1);
          }
        }
        ofSetColor(0, 0, 0, imageViewBgAlpha);
        if (!isSwitchingImageWoExiting) {
          ofDrawRectangle(0, 0, virtualWindowWidth(), virtualWindowHeight());
        }
      }

      // calculate swipe masks to be drawn on top and bottom thumbnails when swiping horiziontally
      if (controlMode != CONTROL_AUTOPILOT) {
        topSwipeMaskHeight = (0 - THP::imageViewport.y) / THP::imageViewport.height*MAPWIN_H + (virtualWindowHeight() - (float)MAPWIN_H) / 2;
        bottomSwipeMaskHeight = virtualWindowHeight() - ((THP::imageTexture.getHeight() - THP::imageViewport.y) / THP::imageViewport.height*MAPWIN_H + (virtualWindowHeight() - (float)MAPWIN_H) / 2);
      } else {
        topSwipeMaskHeight = 0;
        bottomSwipeMaskHeight = 0;
      }

      // calculate transparency for drawing the big image in image drawing mode
      ofPopStyle();
      ofPushStyle();
      //if (!isSwitchingImageWoExiting) {
        float s0 = ofMap(THP::imageViewport.width / THP::imageViewport.zero.width,12,5,0,255,true);

        if (THP::imageTexture.getHeight() > THP::imageTexture.getWidth()) {
          s0 = ofMap(THP::imageViewport.width / THP::imageViewport.zero.width, 20, 8, 0, 255, true);
        }

        float s1 = 255;
        //if (isSwipable()) {
        //  s1 = ofMap(fabs(THP::imageViewport.getCenter().x - THP::imageViewport.zero.getCenter().x) / THP::imageViewport.zero.width,0.2,0.6,255,50,true);
        //}
        ofSetColor(s1, s1, s1, fmin(fmin(beenViewingImage * 10, 255),s0));
      //} else {
        //ofSetColor(255, 255, 255, 255);
      //}
      THP::drawImage(); // draw the big main image
      ofPopStyle();
      
      // calculate & set transparency of Back Button and description text
      //float iif = fmin((float)THP::imageTexture.getWidth() / (float)THP::imageViewport.getWidth(), (float)THP::imageTexture.getHeight() / (float)THP::imageViewport.getHeight());
      float iif = (float)THP::imageTexture.getWidth() / (float)THP::imageViewport.getWidth();

      float iio = fmin(iif,
        fmin(pow(imageViewBgAlpha / 255.0,4), fmin(beenViewingImage*0.01, 1)));
      if (controlMode == CONTROL_AUTOPILOT) {
        iio *= pow((float)THP::imageTexture.getWidth() / (float)THP::imageViewport.width,10);
      }
      if ((float)THP::imageTexture.getHeight() / (float)THP::imageTexture.getWidth() > (float)virtualWindowHeight() / (float)virtualWindowWidth()) {
        iio *= 8;
      }
      iio = fmax(0, fmin(1, iio));
      guiImageInfo->opacity = iio;
      guiBackButton->opacity = iio;

      // control gesture inertia
      if (THP::imageTexture.getWidth() > THP::imageViewport.width*1.5) {
        THP::imageViewport.inertiaOff = true;
      } else {
        THP::imageViewport.inertiaOff = false;
      }

      if (automatedExitImage > 0) {// automatically zoom out the image for the viewer, e.g. when double tapping
        ofRectangle r(THP::imageViewport.zero);
        r.scaleFromCenter(100); // amount of out-zooming
        THP::imageViewport.lerpTo(r, 0.002*automatedExitImageSpeed);
        THP::imageViewport.constraintsOff = true;
        //THP::imageViewport.print();
        automatedExitImage--;
      } else if (automatedNavigateToImage > 0){// controlled by some other code
        //ok...
      }else if ((Multitouch::nTouches() == 0 && THP::imageTexture.getWidth() < THP::imageViewport.width*1.2) || beenViewingImage < 60) {
        if (controlMode != CONTROL_AUTOPILOT && !isViewingAnalysis) { // snapping to default view when image is only slightly off/zoomed
          THP::imageViewport.lerpTo(THP::imageViewport.zero, 0.05);
        }
       }
      
      
      if (automatedExitImage <= 0 && automatedNavigateToImage <= 0 && beenViewingImage > 120) {
        if (THP::imageViewport.width > THP::imageTexture.getWidth() * 4 && Multitouch::nTouches() < 2) {
          unsetIsViewingImage(); // exit image when zoomed out too much
          automatedExitImage = 0;
        }
        THP::imageViewport.constraintsOff = false;
        THP::viewport.constraintsOff = false;
      }
      
      // unselect recent/famous buttons when viewing image
      if (beenViewingImage > 5 && recents.selected != -1) {
        recents.buttons[recents.selected]->setRect(ofRectangle(virtualWindowWidth()/2, virtualWindowHeight()+2, 1, 1));
        recents.selected = -1;
      }
      if (beenViewingImage > 5 && fames.selected != -1) {
        fames.buttons[fames.selected]->setRect(ofRectangle(virtualWindowWidth() / 2, virtualWindowHeight() + 2, 1, 1));
        fames.selected = -1;
      }

      // extra swipe behavior: trigger swipe when image is way off the center. disabled for being weird
      //if (beenViewingImage > 240 && !automatedExitImage && Multitouch::nTouches() == 1) {
      //  vector<int> neighbors = THP::thumbNeighbors(imageLoader.id);
      //  float d0 = ofDistVec2f(THP::viewport.getCenter(), THP::pointcloud[imageLoader.id]);
      //  for (int i = 0; i < neighbors.size(); i++) {
      //    if (ofDistVec2f(THP::viewport.getCenter(), THP::pointcloud[neighbors[i]]) < d0) {
      //      cout << "Loading neighbor..." << endl;
      //      isViewingImage = false;
      //      activateAutomatedEnterImage(neighbors[i]);
      //    }
      //  }
      //}

      // control entry and exit of analysis mode:
      // - swipe up to enter, swipe down to exit
      // - zoom out to exit
      if (isViewingAnalysis && beenViewingImage > 10){// && !(isSwipable() && Multitouch::nTouches() == 1) && beenViewingImage > 10) {
        float bt = (THP::imageTexture.getHeight() - THP::imageViewport.y) / THP::imageViewport.height*MAPWIN_H + (virtualWindowHeight() - (float)MAPWIN_H) / 2 + 300;
        analysisGuiTop = ofLerp(analysisGuiTop, bt, 0.1);
        //THP::calculateImageViewportFromScreenRect(ofRectangle(500,0,virtualWindowWidth()-1000, virtualWindowWidth() - 1000));
        THP::imageViewport.lerpTo(THP::calculateViewportForImageBoundByScreenRect(ofRectangle(50,50,virtualWindowWidth()-100,virtualWindowWidth()-100)),0.05);
        //THP::imageViewport.zero = THP::calculateViewportForImageBoundByScreenRect(ofRectangle(0, 0, virtualWindowWidth(), virtualWindowWidth()));
        if (touch1 != NULL && touch2 != NULL) {
          ofVec2f q1(touch1->startPosition.x * virtualWindowWidth(), touch1->startPosition.y * virtualWindowHeight());
          ofVec2f q2(touch2->startPosition.x * virtualWindowWidth(), touch2->startPosition.y * virtualWindowHeight());

          ofVec2f p1(touch1->position.x * virtualWindowWidth(), touch1->position.y * virtualWindowHeight());
          ofVec2f p2(touch2->position.x * virtualWindowWidth(), touch2->position.y * virtualWindowHeight());

          float dp = ofDistVec2f(p1, p2);
          float dq = ofDistVec2f(q1, q2);
          if ((dq <= (float)MIN_FINGER_DIST && dp > (float)MIN_FINGER_DIST) || ((dp - dq) > 250)) {
            isViewingAnalysis = false;
          }
        }
      } else {
        analysisGuiTop = ofLerp(analysisGuiTop, virtualWindowHeight()+1500, 0.1);
      }
      
      // counter for checking how long image viewing mode is on
      beenViewingImage++;

    } else { // not viewing image
      beenViewingImage = 0;
      analysisGuiTop = ofLerp(analysisGuiTop, virtualWindowHeight() + 1500, 0.1); // hide face matches
    }
   
    // draw the face matches regardless of mode, to make transition seamless
    // but can be optimized to not draw under certain cases, but performance doesn't seem to be a problem currently
    THP::FaceMatch::draw(imageLoader.id, analysisGuiTop, isViewingAnalysis);

    if (!isViewingAnalysis) {
      THP::FaceMatch::detail = -1; // unset current preview (scrub) image in analysis panel
    } else {
      if (touch1 && !touch2) { // single finger
        ofVec2f p(touch1->position.x * virtualWindowWidth(), touch1->position.y * virtualWindowHeight());
        THP::FaceMatch::sendTouch(p);
      } else { // tell face match panel that nobody is touching it
        THP::FaceMatch::noTouch();
      }
     
    }

    // when image is loading..., do some zoom animation
    if (willViewImage) {
      if (automatedEnterImage) {
        if (recents.selected == -1 && fames.selected == -1) {
          ofVec2f p = THP::pointcloud[imageLoader.id];
          float rat = THP::viewport.height / THP::viewport.width;
          ofRectangle r(p.x - 0.5, p.y - 0.5*rat, 1, rat);
          THP::viewport.lerpTo(r, 0.05*automatedEnterImageSpeed);
        } else {

        }
      }
    }


    // two-finger-pinch to enter and exit image
    if (touch1 && touch2) {
      // normalize touches to screen coordinates (from normalized coordinates)
      ofVec2f q1(touch1->startPosition.x * virtualWindowWidth(), touch1->startPosition.y * virtualWindowHeight());
      ofVec2f q2(touch2->startPosition.x * virtualWindowWidth(), touch2->startPosition.y * virtualWindowHeight());

      ofVec2f p1(touch1->position.x * virtualWindowWidth(), touch1->position.y * virtualWindowHeight());
      ofVec2f p2(touch2->position.x * virtualWindowWidth(), touch2->position.y * virtualWindowHeight());

      float dp = ofDistVec2f(p1, p2);
      float dq = ofDistVec2f(q1, q2);

      //cout << dp << " " << dq << endl;
      
      if (isViewingImage) { // check way out
        // cout << (dp - dq) << " " << touch1->age << " " << touch2->age << endl;
        if ((dp <= (float)MIN_FINGER_DIST && dq > (float)MIN_FINGER_DIST) || ((dq-dp)>250 && touch1->age < 8 && touch2->age < 8)) {
          unsetIsViewingImage();
          THP::imageViewport.constraintsOff = false;
          THP::viewport.constraintsOff = false;
        }
      } else if (!willViewImage && THP::viewport.width < VIEWPORT0.width * WTHRESH_IMAGESELECT){ // check way in
        if (!noTouch) {
          if (dp >= (float)MIN_FINGER_DIST && dp >= dq * 2) {
            
            if (idxUnderFing1 == idxUnderFing2 && idxUnderFing1 != -1) {
              THP::viewport.constraintsOff = true;
              attemptLoadImage(idxUnderFing1);
            }
          }
        }
      }
    }

    #if PARTICIPANT_DETECT_MODE == 0
      webcam.update();
      if (webcam.isFrameNew()){
        faceThread.lock();
        faceThread.isNew = true;
        faceThread.input.setFromPixels(webcam.getPixels());
        faceThread.unlock();
      }
      
      Participant::Face::drawDebug(webcam,faceThread);
      
    #elif PARTICIPANT_DETECT_MODE == 1
      Participant::PoseOSC::update();
      Participant::PoseOSC::drawDebug();
    #endif
    if (controlMode) { // not mouse
      
      if (controlMode == CONTROL_TRACKPAD) { // macOS trackpad control
        Multitouch::OSC::update(isViewingImage ? THP::imageViewport : THP::viewport);
      }
      else if (controlMode == CONTROL_TOUCHSCREEN) { // windows touchscreen control
        if (isViewingImage) {
          if (!(THP::FaceMatch::detail >= 0)) {
            Multitouch::TouchScreen::update(THP::imageViewport);
          } else {
            Multitouch::TouchScreen::dryUpdate(); // viewport is not interactive when scrubbing preview image in analysis panel

          }
        } else {
          // don't manipulate the viewport just yet, we want to do some checks to make sure the viewport should be currently interactive
          Multitouch::TouchScreen::dryUpdate();

          Multitouch::Touch* touch1;
          Multitouch::Touch* touch2;
          std::tie(touch1, touch2) = Multitouch::getTwoTouches(false);

          
          // check if touch event should be absorbed by overlaying GUI etc.
          if (touch1 != NULL && touch2 == NULL && recents.updateScroll(touch1->startPosition, touch1->lastPosition, touch1->position)) {

          } else {
            if (touch1 != NULL && touch2 == NULL && fames.updateScroll(touch1->startPosition, touch1->lastPosition, touch1->position)) {
              
            } else if (!isViewingAnalysis) {
              if (touch1 == NULL || !GUI::check(ofVec2f(touch1->position.x * virtualWindowWidth(), touch1->position.y * virtualWindowHeight()),false)) {
                if (touch2 == NULL || !GUI::check(ofVec2f(touch2->position.x * virtualWindowWidth(), touch2->position.y * virtualWindowHeight()),false)) {
                  Multitouch::updateViewport(THP::viewport); // alright, viewport is interactive
                }
              }
            }
          }
          // align famous and recent bars
          // they don't need to be totally in sync, just need the gaps to align
          // hence mod
          float w = recents.rect.width / (float)recents.cols;
          float offs = fmod((recents.scroll - fames.scroll), w);
          if (!isFamous) {
            recents.scroll -= offs;
          } else {
            fames.scroll += offs;
          }
        }
        
      }
      // do the screen saver
      else if (controlMode == CONTROL_AUTOPILOT) {
        // disabled code for old screensaver: no KenBurns effect
        //if (isViewingImage) {
        //  if (!automatedExitImage) {
        //    activateAutomatedExitImage();
        //  }
        //} else {
        //  isViewingImage = false;
        //  currentImageIndex = -1;
        //  THP::Autopilot::update(0.0005);
        //}

        // screensaver with ken burns effect
        isViewingAnalysis = false;
        if (!automatedExitImage && !automatedEnterImage) {
          if (isViewingImage) {// if viewing image, do ken burns

            THP::KenBurns::update(imageLoader.id, 0.000575*autoPlaySpeedFactor);
            //cout << "o" << endl;
            if (THP::KenBurns::laps >= 1) {
              activateAutomatedExitImage();
			  if (autoPlaySpeedFactor < 5) {
				  automatedExitImage = 1200;
				  automatedExitImageSpeed = 0.03;
			  }
			  else {
				  automatedExitImage = 100;
				  automatedExitImageSpeed = 1;
			  }
              THP::Autopilot::reset();
            }
          } else { // if not viewing image, just pan around the grid and pick images to zoom into
            currentImageIndex = -1;
            THP::Autopilot::update(0.005*autoPlaySpeedFactor);
            if (THP::Autopilot::isNear != -1) {
              //cout << "isnear" << THP::Autopilot::isNear << endl;
              activateAutomatedEnterImage(THP::Autopilot::isNear);
              THP::KenBurns::previdx = -1;
              THP::KenBurns::laps = 0;
              THP::KenBurns::target = -1;
            }
          }
        }
      }
      
    }

    // image just loaded in thread, allocate it into the texture, and enter image viewing mode
    if (imageLoader.state == 2 && willViewImage) {

      THP::imageTexture.allocate(imageLoader.pixels);
      if (recents.selected != -1) {
        THP::calculateImageViewportFromScreenRect(recents.buttons[recents.selected]->rect);
      } else if (fames.selected != -1) {
        THP::calculateImageViewportFromScreenRect(fames.buttons[fames.selected]->rect);
      } else {
        THP::calculateImageViewport(imageLoader.id);
      }
        
      isViewingImage = true;
      willViewImage = false;
      automatedEnterImage = 0;
      if (controlMode != CONTROL_AUTOPILOT) {
        THP::Logger::imageIn(imageLoader.id); // trackig users to see images they're interested in
      }
    }

   
    if (automatedNavigateToImage > 0) {
      THP::imageViewport.constraintsOff = true;
      THP::viewport.constraintsOff = true;
      float t = 1-(float)automatedNavigateToImage / (float)automatedNavigateToImageTime;
      ofVec2f p = THP::pointcloud[automatedNavigateFromImageIndex];
      ofVec2f q = THP::pointcloud[automatedNavigateToImageIndex];
      
      float rat = THP::viewport.height / THP::viewport.width;
      float v = THP::KenBurns::sigmoidx(t, 0.5);
      ofVec2f m = ofLerpVec2f(p, q, v);
      //float s = 100* (0.5-0.5*cos(t*PI*2)) + 1;
      float ss = ofDistVec2f(p, q)*1;
      float s = ss * sin(t*PI) + 1;
      
      //cout << t << " " << s << endl;
      ofRectangle r(m.x - 0.5*s, m.y - 0.5*rat*s, s, rat*s);

      THP::viewport.lerpTo(r, 0.1);
      if (t < 0.5) {
        THP::calculateImageViewport(automatedNavigateFromImageIndex);
      } else {
        isViewingImage = false;
      }
      ////if (automatedNavigateToImage > 700) {
      //  ofRectangle r(THP::imageViewport.zero);
      //  r.scaleFromCenter(100);
      //  THP::imageViewport.lerpTo(r, 0.01);
      //  cout << "x1" << endl;
      ///*} else if (automatedNavigateToImage > 300) {
      //  isViewingImage = false;
      //  ofVec2f p = THP::pointcloud[automatedNavigateToImageIndex];
      //  float rat = THP::viewport.height / THP::viewport.width;
      //  float s = 100;
      //  ofRectangle r(p.x - 0.5*s, p.y - 0.5*rat*s, s, rat*s);
      //  THP::viewport.lerpTo(r, 0.01);
      //  cout << "x2" << endl;
      //} else {
      //  ofVec2f p = THP::pointcloud[automatedNavigateToImageIndex];
      //  float rat = THP::viewport.height / THP::viewport.width;
      //  float s = 1;
      //  ofRectangle r(p.x - 0.5*s, p.y - 0.5*rat*s, s, rat*s);
      //  THP::viewport.lerpTo(r, 0.01);
      //  cout << "x3" << endl;
      //}*/
      //if (automatedNavigateToImage == 100) {
        //attemptLoadImage(automatedNavigateToImageIndex);
      //}
      automatedNavigateToImage-=1;
      ofPushStyle();
      ofSetColor(0,0,0,pow(sin(t*PI),0.6)*150);
      ofDrawRectangle(0, 0, virtualWindowWidth(), virtualWindowHeight());
      ofPopStyle();

    } else if (automatedNavigateToImage == 0) {
      //THP::calculateImageViewport(automatedNavigateFromImageIndex);
      //attemptLoadImage(automatedNavigateToImageIndex);
      activateAutomatedEnterImage(automatedNavigateToImageIndex);
      //THP::calculateImageViewport(automatedNavigateFromImageIndex);
      automatedNavigateToImage = -1;
    }

    vignetteTex.draw(0, 0, virtualWindowWidth(), virtualWindowHeight());


    ofPushStyle();
    ofSetColor(255,255,255, 230);
    gradientTex.draw(0, virtualWindowHeight()-300-bottomGuiHeight, virtualWindowWidth(), 300);
    gradientTex.draw(0, topGuiHeight+300, virtualWindowWidth(), -300);

    ofSetColor(0,0,0,230);
    ofDrawRectangle(0, virtualWindowHeight() - bottomGuiHeight, virtualWindowWidth(), bottomGuiHeight);
    ofDrawRectangle(0, 0, virtualWindowWidth(), topGuiHeight);
    ofPopStyle();


    

    GUI::draw();

 

    //ofPushStyle();
    //ofSetColor(255, 0, 0);
    //ofDrawRectangle(0, 0, THP::Autopilot::stateProgress*virtualWindowWidth(), virtualWindowHeight());
    //ofPopStyle();

    if (noTouch > 0) {
      noTouch--;
    }

    if (Multitouch::nTouches() == 0) {
      noInteractionTimer++;
    } else {
      noInteractionTimer = 0;
    }
    if (noInteractionTimer > TIMER_START_AUTOPILOT && controlMode != CONTROL_AUTOPILOT) {
      THP::Autopilot::reset();
      controlMode = CONTROL_AUTOPILOT;
      THP::Logger::imageOut();
    }

    if (showFPS) {
      ofDrawBitmapStringHighlight("FPS:" + ofToString(fpsCounter.getFps()), 10, 50);
    }
    //ofDrawBitmapStringHighlight(THP::embeddingNames[THP::targetEmbeddingIdx], 10, 25);
    
    fpsCounter.newFrame();

    //cout << currentImageIndex << endl;
    ofPopMatrix();
    Multitouch::drawDebug();// visualie fingers
  }

  void exit() {
    cout << "exiting..." << endl;
    imageLoader.stopThread();

    #if PARTICIPANT_DETECT_MODE == 0
      faceThread.stopThread();
    #endif
  }


  void mouseScrolled(int x, int y, float scrollX, float scrollY) {
    if (controlMode == CONTROL_MOUSE) {
      THP::zoomMapAt(x, y, ofClamp(scrollY*0.1, -0.1, 0.1));
    }
  }

  void mouseDragged(int x, int y, int button) {
    if (controlMode == CONTROL_MOUSE) {
      int dx = x - ofGetPreviousMouseX();
      int dy = y - ofGetPreviousMouseY();
      THP::panMap(dx, dy);
    }
  }

  void update() {
    
    setGuiRects();
  }

  void onGUIEvent(GUI::EventInfo& e) {
    if (e.elementId == "Back") {
      if (isViewingImage) {
        activateAutomatedExitImage();
        isViewingAnalysis = false;
      } else {
        ////cout << "how did you manage to click that?" << endl;
      }
    } else if (e.elementId == "Face"){
      if (isViewingImage) {
        isViewingAnalysis = !isViewingAnalysis;
      } else {
        ////cout << "how did you manage to click that?" << endl;
      }
    } else if (e.elementId == "upside-down-btn") {
      guiUpsideDown = !guiUpsideDown;

      isPreviewingButton = false;
    } else if (e.elementId == "fame-btn"){
      isFamous = !isFamous;
      guiUpsideDown = false;

      isPreviewingButton = false;
    } else if (ofIsStringInString(e.elementId,"recent-btn")) {
      if (!isViewingImage && !willViewImage) {
        int idx = stoi(e.element->text);
        activateAutomatedEnterImage(idx);
        //recents.selected = std::find(recents.buttons.begin(), recents.buttons.end(), e.element) - recents.buttons.begin();
        auto it = std::find(recents.buttons.begin(), recents.buttons.end(), e.element);
        if (it == recents.buttons.end()) {
          it = std::find(fames.buttons.begin(), fames.buttons.end(), e.element);
          fames.selected = it - fames.buttons.begin();
        } else {
          recents.selected = it - recents.buttons.begin();
        }
      }
    } else if (e.elementId == "sel-embedding") {
      THP::Message::trigger();
      isPreviewingButton = false;
      messageFixBottomRight = false;
    }
  }

  void keyPressed(int key) {
    cout <<"key"<< key << endl;
    if (key == 9) {
      THP::Autopilot::reset();
      if (controlMode == CONTROL_TOUCHSCREEN) {
        controlMode = CONTROL_AUTOPILOT;
      } else if (controlMode == CONTROL_AUTOPILOT) {
        controlMode = CONTROL_TOUCHSCREEN;
      }
    }
    if (key == 32) {
      THP::viewport.set(VIEWPORT0);
    }
    if (key == 8) {
      unsetIsViewingImage();
    }
    if (key == 70) {
      cout << "TOGGLE FULLSCREEEN!" << endl;
      ofToggleFullscreen();
    }

    if (0 <= key - 48 && key - 48 <= 9) {
      int n = (key - 39) % 10;
      THP::targetEmbeddingIdx = n % THP::embeddings.size();
    }
    if (key == 57356) {
      currentImageIndex = -1;
      THP::targetEmbeddingIdx = (THP::targetEmbeddingIdx - 1 + THP::embeddings.size()) % THP::embeddings.size();
    }
    if (key == 57358) {
      currentImageIndex = -1;
      THP::targetEmbeddingIdx = (THP::targetEmbeddingIdx + 1) % THP::embeddings.size();
    }
    if (key == 61) {
      cout << "GUI UPSIDE DOWN!" << endl;
      guiUpsideDown = !guiUpsideDown;
    }
    if (key == 80) {
      showFPS = !showFPS;
    }
    if (key == 57360) {
      autoPlaySpeedFactor *= 1.2;
      cout << "autoplay speed factor " << autoPlaySpeedFactor << endl;
    } else if (key == 57361) {
      autoPlaySpeedFactor /= 1.2;
      cout << "autoplay speed factor " << autoPlaySpeedFactor << endl;
    }
    
    if (key == 65) {
      isViewingAnalysis = !isViewingAnalysis;
    }
  }

  

  void mousePressed(int x, int y, int button) {
    if (controlMode == CONTROL_MOUSE) {
      ofVec2f v((float)x / virtualWindowWidth(), (float)y / virtualWindowHeight());
      if (button == 2) {
        onDoubleTap(v);
      } else {
        onTap(v);
      }
    }
  }

 
  void onAnyTouch(ofVec2f& p) {
    float x = p.x * virtualWindowWidth();
    float y = p.y * virtualWindowHeight();

    bool msgTriggered = false;
    int i = guiEmbeddingSelector->dryCheck(ofVec2f(x,y));
    ////cout << i << endl;
    if (i != -1) {
      THP::Message::trigger(THP::embeddingDescriptions[i]);
      msgTriggered = true;
      messageFixBottomRight = false;
      THP::Message::lateTimer = 0;
    } else if (guiUpsideDownButton->rect.inside(ofVec2f(x,y))) {
      THP::Message::trigger(isFamous ? hintUpsideDown1 : hintUpsideDown2);
      msgTriggered = true;
      messageFixBottomRight = true;
      THP::Message::lateTimer = 5;
    } else if (guiFameButton->rect.inside(ofVec2f(x, y))) {
      THP::Message::trigger(isFamous ? hintFame1 : hintFame2);
      msgTriggered = true;
      messageFixBottomRight = true;
      THP::Message::lateTimer = 5;
    }

    if (msgTriggered) {
      THP::Message::label->style.color = ofColor(255, 255, 255);
      THP::Message::label->opacity = 0;
      THP::Message::targetOpacity = 0.4;
      THP::Message::yshift = 0;
      isPreviewingButton = true;
    }

    if (isViewingImage) {

      int i = THP::FaceMatch::checkTap(ofVec2f(x, y));
      ////cout << "face touch " << i << endl;
    }

    THP::Logger::logTouch();
  }
  void offAnyTouch(ofVec2f& p) {
    if (isPreviewingButton) {
      THP::Message::state = 0;
    }
    if (isViewingAnalysis  && isViewingImage) {
      THP::FaceMatch::detail = -1;
    }
    potentialImageIndex = -1;
  }

  void onFirstTouch(ofVec2f& p) {
    THP::viewport.velocityTL *= 0;
    THP::viewport.velocityBR *= 0;
    THP::imageViewport.velocityTL *= 0;
    THP::imageViewport.velocityBR *= 0;

    if (controlMode == CONTROL_AUTOPILOT) {
      controlMode = CONTROL_TOUCHSCREEN;
      automatedEnterImage = 0;
      automatedExitImage = 0;
      THP::Autopilot::reset();
    }

    if (!isViewingImage) {
      potentialImageIndex = THP::getThumbIndexUnderCursor(p.x * virtualWindowWidth(), p.y * virtualWindowHeight());
      
    }

  }

  void onTap(ofVec2f& p) {
    
    potentialImageIndex = -1;
    if (noTouch) {
      //THP::Logger::logRaw("tap(nop)!");
      return;
    }

    float x = p.x * virtualWindowWidth();
    float y = p.y * virtualWindowHeight();


    if (GUI::check(ofVec2f(x, y))) {
      //THP::Logger::logRaw("tap(gui)!");
      return;
    }

    //if (isViewingAnalysis && isViewingImage) {
    //  if (THP::FaceMatch::checkTap(ofVec2f(x, y))) {
    //    return;
    //  }
    //}
    if (isViewingImage) {
      float x = p.x * virtualWindowWidth();
      float y = p.y * virtualWindowHeight();
      int i = THP::FaceMatch::checkTap(ofVec2f(x, y));
      ////cout << "face tap " << i << endl;
      if (i == -2) {
        isViewingAnalysis = true;
        return;
      }
    }
    if (THP::viewport.width < VIEWPORT0.width * WTHRESH_IMAGESELECT) {

      int midx = THP::getThumbIndexUnderCursor(x, y);
      ////cout << midx << endl;
      if (midx != currentImageIndex) {
        //THP::Logger::logRaw("tap(select-image,"+ofToString(midx)+")!");
        currentImageIndex = midx;
        string desc, date, acc;
        
        std::tie(desc, date, acc) = THP::descriptions[currentImageIndex];
        guiTinyInfo->setText(date + " " + desc+".");
        guiTinyInfo->computeAutoNewline();
      }
    } else {
      //THP::Logger::logRaw("tap(nop)!");
      ////cout << "zoom in more before selecting!" << endl;
    }
  }

  void onSwipe(ofVec4f& p) {
    ////cout << "swipe!" << p.w << " " << p.z << endl;
    if (p.z > 0.07) {
      ////cout << (THP::imageViewport.width > THP::imageTexture.getWidth()*0.95) << endl;
      if (isSwipable()){
        if (((int)round(p.w)) % 2 == 0) {
          vector<int> neighbors = THP::thumbNeighbors(imageLoader.id);
          int nb = neighbors[(((int)round(p.w)) + 2) % 4];
          ////cout << nb << endl;
          if (nb == -1) {
            //isViewingImage = false;
            activateAutomatedExitImage();
            return;
          }
          activateAutomatedEnterImage(nb);
          unsetIsViewingImage();
          isSwitchingImageWoExiting = true;
        } else {
          if (p.w < 2) {
            isViewingAnalysis = true;
          } else {
            isViewingAnalysis = false;
          }
        }
      }
    }
  }
  
  void attemptLoadImage(int idx) {
    string boxname; string fname;
    std::tie(boxname, fname) = THP::filenames[idx];
    if (THP::fileExists(THP::dataPaths["full"] + boxname + "/" + fname)) {
      //THP::loadImage(idx, "images/full/" + boxname + "/" + fname);
      imageLoader.load(idx, THP::dataPaths["full"] + boxname + "/" + fname);

      string date;
      string desc;
      string acc;
      std::tie(desc,date,acc) = THP::descriptions[idx];

      //guiImageInfo->text = "Image " + fname + " from " + boxname + "\n";
      guiImageInfo->setText(desc+" \n"+date+" = ID#: "+acc);
      guiImageInfo->computeAutoNewline();

      willViewImage = true;
    }
    isViewingAnalysis = false;

  }



  void onSecondTouch(ofVec4f& p) {
    potentialImageIndex = -1;
    if (noTouch) {
      return;
    }

    float x1 = p.x * virtualWindowWidth();
    float y1 = p.y * virtualWindowHeight();
    float x2 = p.z * virtualWindowWidth();
    float y2 = p.w * virtualWindowHeight();
    
    idxUnderFing1 = THP::getThumbIndexUnderCursor(x1, y1);
    idxUnderFing2 = THP::getThumbIndexUnderCursor(x2, y2);

    //cout << "# " << isViewingImage << " " << willViewImage << endl;

    //if (!isViewingImage && !willViewImage) {
    //  if (ofDist(x1, y1, x2, y2) > 20) {
    //    int idx1 = THP::getThumbIndexUnderCursor(x1, y1);
    //    int idx2 = THP::getThumbIndexUnderCursor(x2, y2);
    //    if (idx1 == idx2) {
    //      attemptLoadImage(idx1);
    //    }
    //  }
    //}
  }

  void activateAutomatedEnterImage(int idx) {
    attemptLoadImage(idx);
    THP::imageViewport.constraintsOff = true;
    THP::viewport.constraintsOff = true;
    automatedEnterImage = 1;
    automatedEnterImageSpeed = 1;
    noTouch = 50;
  }

  void activateAutomatedExitImage() {
    isViewingAnalysis = false;
    automatedExitImageSpeed = 1;
    if (automatedExitImage <= 0) {
      automatedExitImage = 100;// ofMap(THP::imageViewport.width, 0, THP::imageTexture.getWidth(), 60, 40, true);
      noTouch = 50;
    }
  }

  void activateAutomatedNavigateToImage(int idx) {
    //attemptLoadImage(idx);
    THP::imageViewport.constraintsOff = true;
    THP::viewport.constraintsOff = true;
    automatedNavigateToImageIndex = idx;
    automatedNavigateToImageTime = (int)ofDistVec2f(THP::pointcloud[idx], THP::pointcloud[imageLoader.id])*1+20;
    automatedNavigateToImage = automatedNavigateToImageTime;
    automatedNavigateFromImageIndex = imageLoader.id;
    noTouch = automatedNavigateToImageTime;
    //isViewingImage = false;
    isViewingAnalysis = false;
    if (controlMode != CONTROL_AUTOPILOT) {
      THP::Logger::imageOut();
    }
    recents.add(imageLoader.id);
  }

  void onDoubleTap(ofVec2f& p) {
    if (noTouch) {
      return;
    }
    ////cout << "DOUBLE TAP!!!!" << endl;

    float x = p.x * virtualWindowWidth();
    float y = p.y * virtualWindowHeight();

    if (GUI::check(ofVec2f(x, y))) {
      return;
    }

    if (isViewingAnalysis && isViewingAnalysis) {
      int i = THP::FaceMatch::checkTap(ofVec2f(x, y));
      if (i >= 0) {
        int idx = THP::FaceMatch::slots[i];
        if (idx >= 0) {
          //isViewingImage = false;
          //activateAutomatedExitImage();
          //automatedExitImage = 50;
          //automatedExitImageSpeed = 10;
          ////THP::FaceMatch::activate = 1;
          ////activateAutomatedEnterImage(idx);
          //automatedEnterImageTimer = 50;
          //automatedEnterImageTimerIndex = idx;
          //automatedEnterImageSpeed = 0.5;
          activateAutomatedNavigateToImage(idx);
          isViewingAnalysisNext = true;
          return;
        }
      }
    }

    if (!isViewingImage && !willViewImage) {

      if (THP::viewport.width > VIEWPORT0.width * WTHRESH_DOUBLETAP) {
        ////cout << "zoom in more first!" << endl;
        return;
      }

      int idx = THP::getThumbIndexUnderCursor(x, y);
      ////cout << "double tap loading " << idx << endl;
      activateAutomatedEnterImage(idx);
    } else {
      activateAutomatedExitImage();
    }
  }
  
  void keyReleased(int key) {}
  void mouseMoved(int x, int y) {}
  void mouseReleased(int x, int y, int button) {}
  void mouseEntered(int x, int y) {}
  void mouseExited(int x, int y) {}
  void windowResized(int w, int h) {
   
  }
  void dragEvent(ofDragInfo dragInfo) {}
  void gotMessage(ofMessage msg) {}

};
