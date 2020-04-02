#include "ofxOpenCv.h"
#include "ofxCv.h"

#include "ofxOsc.h"

#define OSC_PORT_POSE 9527

namespace Participant{
  namespace Face{
    class Thread : public ofThread {public:
      ofImage input;
      ofxCvHaarFinder haar;
      bool isNew = false;
      ofVec2f center;
      float faceShift;
      float estimateDist;
      
      void threadedFunction() {
        haar.setup("haarcascade_frontalface_default.xml");
        while(isThreadRunning()) {
          lock();
          if (input.isAllocated() && isNew){
            haar.findHaarObjects(input);
            isNew = false;
          
            float maxA = 0;
            float maxI = -1;
            for(int i = 0; i < haar.blobs.size(); i++) {
              float a = haar.blobs[i].boundingRect.getArea();
              if (a > maxA){
                maxA = a;
                maxI = i;
              }
            }
            if (maxI != -1){
              ofVec2f c = haar.blobs[maxI].boundingRect.getCenter();
              center = ofLerpVec2f(center, c, 0.3);
            }
            faceShift = (center.x / (float)input.getWidth()-0.5)*2;
            
          }
          unlock();
        }
      }
    };

    void drawDebug(ofVideoGrabber& webcam, Thread& faceThread){
      faceThread.lock();

      ofPushMatrix();
      ofTranslate(ofGetWindowWidth()-faceThread.input.getWidth(),0);
      ofScale(0.5,0.5);
      webcam.draw(0,0);
      ofPushStyle();
      ofNoFill();
      ofSetLineWidth(2);
      for(int i = 0; i < faceThread.haar.blobs.size(); i++) {
        ofDrawRectangle( faceThread.haar.blobs[i].boundingRect );
      }
      ofPopStyle();
      ofPushStyle();
      ofSetColor(255,0,0);
      ofDrawRectangle( faceThread.center.x-5, faceThread.center.y-5, 10, 10 );
      ofDrawRectangle( (faceThread.faceShift*0.5+0.5)* faceThread.input.getWidth(), 0, 1, faceThread.input.getHeight() );
      ofPopStyle();
      ofPopMatrix();

      faceThread.unlock();
    }
  };

  struct Keypoint{
    ofVec2f position;
    float score;
  };
  struct Pose{
    vector<Keypoint> keypoints;
    float score;
  };

  namespace PoseOSC {
    ofxOscReceiver receiver;
    bool initialized = false;

    vector<Pose> poses;
    int videoWidth;
    int videoHeight;

    int bones[16][2] = {
        {0,1  },{0,  2},{1, 3 },{2,4  },//face
        {5,6  },{11,12},{5, 11},{6,12 },//body
        {5,7  },{7,  9},{6, 8 },{8,10 },//arms
        {11,13},{13,15},{12,14},{14,16},//legs
    };

    ofVec2f center;
    float faceShift = 0;
    float estimateDist = 0;
    float currentTrackedIndex = 0;

    bool receiveOSC(){
      bool gotNews = false;

      while(receiver.hasWaitingMessages()){
        gotNews = true;

        ofxOscMessage m;
        receiver.getNextMessage(m);
        if(m.getAddress() == "/poses/arr"){
            
          poses.clear();

          videoWidth = m.getArgAsInt(0);
          videoHeight = m.getArgAsInt(1);
          int nPoses = m.getArgAsInt(2);

          for (int i = 0; i < nPoses; i++){
            Pose pose;

            pose.score = m.getArgAsFloat(3+i*52);


            for (int j = 0; j < 17; j++){
              Keypoint kpt;

              kpt.position.x = m.getArgAsFloat(3+i*52+1+j*3);
              kpt.position.y = m.getArgAsFloat(3+i*52+1+j*3+1);
              kpt.score = m.getArgAsFloat(3+i*52+1+j*3+2);

              pose.keypoints.push_back(kpt);
            }

            poses.push_back(pose);
          }

        }else{
          cout << "unrecognized OSC message received @ " <<m.getAddress()<< endl;
        }
      }
      return gotNews;
    }

    void update(){
      if (!initialized){
        receiver.setup(OSC_PORT_POSE);
        initialized = true;
      }
      if (receiveOSC()){

        float maxA = 0;
        float maxI = -1;
        for(int i = 0; i < poses.size(); i++) {
          float a = ofDistVec2f(poses[i].keypoints[5].position, poses[i].keypoints[6].position);
          if (a > maxA){
            maxA = a;
            maxI = i;
          }
        }
        if (maxI >= 0) {
          center = ofLerpVec2f(center, poses[maxI].keypoints[0].position, 0.3);
          faceShift = (center.x / (float)videoWidth - 0.5) * 2;

          ofVec2f waist = (poses[maxI].keypoints[11].position + poses[maxI].keypoints[12].position) / 2;
          float d = (float)(videoHeight - waist.y)/(float)videoHeight;
          estimateDist = ofLerp(estimateDist, d, 0.3);
        }
        currentTrackedIndex = maxI;
      }
    }
    
    void drawDebug(){

      ofPushMatrix();

      ofTranslate(ofGetWindowWidth()-videoWidth/2,0);
      ofScale(0.5,0.5);

      ofPushStyle();
      ofSetLineWidth(2);

      ofSetColor(0, 255, 255);
      ofNoFill();
      ofDrawRectangle(0,0,videoWidth,videoHeight);
      for (int i = 0; i < poses.size(); i++){
        if (i == currentTrackedIndex) {
          ofSetColor(255, 0, 255);
        } else {
          ofSetColor(0, 255, 255);
        }
        for (int j = 0; j < 16; j++){
          ofVec2f p0 = poses[i].keypoints[bones[j][0]].position;
          ofVec2f p1 = poses[i].keypoints[bones[j][1]].position;
          ofDrawLine(p0,p1);
        }
      }
      ofPopStyle();


      ofPopMatrix();
    }

  };





};