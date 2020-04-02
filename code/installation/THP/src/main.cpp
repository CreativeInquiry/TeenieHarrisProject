int HALF = 0;
int THUMBSTEX_MAXSHEETS = 15;
#include "ofMain.h"
#include "ofApp.h"

//========================================================================
int main( ){
  if (ofFile(ofToDataPath("half")).exists()) {HALF = 1;}
  if (ofFile(ofToDataPath("subset")).exists()) {THUMBSTEX_MAXSHEETS= 1;}
  ofGLWindowSettings settings;
  // settings.setGLVersion(3,2);
  settings.windowMode = OF_WINDOW;
  settings.setSize(MAPWIN_W,MAPWIN_H);

  //glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);
  //glEnable(GL_MULTISAMPLE);
  //glHint(GL_MULTISAMPLE_FILTER_HINT_NV, GL_NICEST);
  //glEnable(GL_POLYGON_SMOOTH);

  ofCreateWindow(settings);     // this kicks off the running of my app
//  ofSetupOpenGL(1000,1000,OF_WINDOW);      // <-------- setup the GL context

  // this kicks off the running of my app
  // can be OF_WINDOW or OF_FULLSCREEN
  // pass in width and height too:
  ofRunApp(new ofApp());

}
