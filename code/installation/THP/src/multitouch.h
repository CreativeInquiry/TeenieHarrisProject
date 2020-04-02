//
//  multitouch.h
//  THP
//
//  Created by admin on 2019-09-11.
//

#pragma once

#define OSC_PORT_TRACKPAD 9000
#define EXPIRE_TOUCH_AFTER_INVISIBLE_FOR_FRAMES 1

namespace Multitouch{
  struct Touch{
    ofVec2f startPosition;
    ofVec2f lastPosition;
    ofVec2f position;
    int age;
    int id;
    int cold;
    bool expired;
    bool overlap;
  };
  vector<Touch> touches;
  ofVec2f viewportScaleCenter;
  ofEvent<ofVec2f> onAnyTouch;
  ofEvent<ofVec2f> offAnyTouch;
  ofEvent<ofVec2f> onFirstTouch;
  ofEvent<ofVec4f> onSecondTouch;
  ofEvent<ofVec2f> onDoubleTap;

  ofEvent<ofVec2f> onTap;

  ofEvent<ofVec4f> onSwipe;

  float _doubleTapTimer = 0;
  ofVec2f _doubleTapLocation;

  int nTouches() {
    int n = 0;
    for (int i = 0; i < touches.size(); i++) {
      if (!touches[i].expired) {
        n++;
      }
    }
    return n;
  }

  void addTouch(int id, ofVec2f position){
    bool added = false;

    // attempt update
    for (int i = 0; i < touches.size(); i++){
      if (touches[i].expired){
        continue;
      }
      if (touches[i].id == id){
        touches[i].lastPosition = ofVec2f(touches[i].position.x,touches[i].position.y);
        touches[i].position = ofVec2f(position.x,position.y);
        touches[i].cold = 0;
        added = true;
        break;
      }
    }

    int n = nTouches();

    if (n == 0) {
      if (ofGetFrameNum()-_doubleTapTimer > 30 || ofDistVec2f(_doubleTapLocation,position)>0.02) {
        ofNotifyEvent(onFirstTouch, position);
        _doubleTapTimer = ofGetFrameNum();
        _doubleTapLocation = ofVec2f(position.x, position.y);
      } else {
        ofNotifyEvent(onDoubleTap, position);
        _doubleTapTimer = 0;
      }
      
    }

    if (!added) {
      ofVec2f pos = ofVec2f(position);
      ofNotifyEvent(onAnyTouch, pos);
    }
    if (!added && n == 1) {
      for (int i = 0; i < touches.size(); i++) {
        if (!touches[i].expired) {
          ofVec4f p(touches[i].position.x, touches[i].position.y, position.x, position.y);
          ofNotifyEvent(onSecondTouch, p);
          break;
        }
      }
    }
    if (!added && n > 0) {
      for (int i = 0; i < touches.size(); i++) {
        if (!touches[i].expired) {
          touches[i].overlap = true;
        }
      }
    }

    // attempt envict
    if (!added){
      for (int i = 0; i < touches.size(); i++){
        if (touches[i].expired){
          touches[i].expired = false;
          touches[i].id = id;
          touches[i].position = position;
          touches[i].lastPosition = position;
          touches[i].startPosition = position;
          touches[i].cold = 0;
          touches[i].age = 0;
          touches[i].overlap = n > 0;
          added = true;
          break;
        }
      }
    }

    // append
    if (!added){
      Touch touch;
      touch.id = id;
      touch.position = position;
      touch.lastPosition = position;
      touch.startPosition = position;
      touch.expired = false;
      touch.cold = 0;
      touch.age = 0;
      touch.overlap = n > 0;
      touches.push_back(touch);
    }
  }

  void updateTouches() {
    int n = nTouches();
    for (int i = 0; i < touches.size(); i++) {
      if (touches[i].expired) {
        continue;
      }
      if (touches[i].cold >= EXPIRE_TOUCH_AFTER_INVISIBLE_FOR_FRAMES) {
        touches[i].expired = true;
        ofNotifyEvent(offAnyTouch, touches[i].position);
        if (n == 1 && touches[i].overlap == false) {
          float d = ofDistVec2f(touches[i].startPosition, touches[i].position);
          if (d < 0.02) {
            ofVec2f p(touches[i].position.x, touches[i].position.y);
            ofNotifyEvent(onTap, p);
          } else {
            float dx = touches[i].position.x - touches[i].startPosition.x;
            float dy = touches[i].position.y - touches[i].startPosition.y;
            ofVec4f v(touches[i].startPosition.x, touches[i].startPosition.y, d, 0);
            if (fabs(dx) / fabs(dy) > 2) {
              if (dx < 0) {
                v.w = 0;
              } else {
                v.w = 2;
              }
              ofNotifyEvent(onSwipe, v);
            }else if (fabs(dy) / fabs(dx) > 2) {
              if (dy < 0) {
                v.w = 1;
              } else {
                v.w = 3;
              }
              ofNotifyEvent(onSwipe, v);
            }
          }
          
        }
      }
    }
    for (int i = 0; i < touches.size(); i++) {
      if (!touches[i].expired) {
        touches[i].cold++;
        touches[i].age++;
      }
    }
  }

  std::tuple<Touch*,Touch*> getTwoTouches(bool pickFurthest=true) {
    Touch* first = NULL;
    Touch* second = NULL;
    if (!pickFurthest) {
      for (int i = 0; i < touches.size(); i++) {
        if (touches[i].expired) {
          continue;
        };
        if (!first) {
          first = &touches[i];

        }else if (!second) {
          second = &touches[i];
          break;
        }
      }
    } else {
      vector<Touch*> validTouches;
      for (int i = 0; i < touches.size(); i++) {
        if (!touches[i].expired) {
          validTouches.push_back(&touches[i]);
        }
      }
      int n = validTouches.size();
      if (n == 1) {
        first = validTouches[0];
      } else if (n > 1) {
        float max_d = 0;
        for (int i = 0; i < validTouches.size(); i++) {
          for (int j = i+1; j < validTouches.size(); j++) {
            float d = ofDistVec2f(validTouches[i]->position, validTouches[j]->position);
            if (d > max_d) {
              max_d = d;
              first = validTouches[i];
              second = validTouches[j];
            }
          }
        }
      }
    }
    return std::make_tuple(first, second);
  }

  ofVec2f normalizedToViewport(Viewport& viewport, ofVec2f& p){
    float screenX = p.x*(float)ofGetWindowWidth();
    float screenY = p.y*(float)ofGetWindowHeight();
    float mapX = screenX - (ofGetWindowWidth() - (float)MAPWIN_W) / 2;
    float mapY = screenY - (ofGetWindowHeight() - (float)MAPWIN_H) / 2;
    return ofVec2f(
      mapX  / (float)MAPWIN_W*viewport.width +viewport.x,
      mapY / (float)MAPWIN_H*viewport.height+viewport.y
    );
  }

  void updateViewport(Viewport& viewport){

    Touch* first; Touch* second;
    std::tie(first,second) = getTwoTouches();

    if (second){
      ofVec2f p0 = normalizedToViewport(viewport,first->lastPosition);
      ofVec2f p1 = normalizedToViewport(viewport,first->position);

      ofVec2f q0 = normalizedToViewport(viewport,second->lastPosition);
      ofVec2f q1 = normalizedToViewport(viewport,second->position);

      float d0 = ofDistVec2f(first->position, second->position);
      //cout << d0 << endl;
      if (d0 < 0.05) {
        ofVec2f c = (p1 + q1)*0.5;
        float d = ofDistVec2f(p0, q0)/2;
        ofVec2f np = (p1 - c).normalized();
        ofVec2f nq = (q1 - c).normalized();
        ofVec2f p2 = c + np*d;
        ofVec2f q2 = c + nq * d;
        p1 = ofLerpVec2f(p1, p2, 0.5);
        q1 = ofLerpVec2f(q1, q2, 0.5);
      }

      viewport.zoomPan(p0, q0, p1, q1);
      //viewport.updateInertia();
      viewport.velocityBR *= 0;
      viewport.velocityTL *= 0;

    }else if (first){
      ofVec2f p0 = normalizedToViewport(viewport,first->lastPosition);
      ofVec2f p1 = normalizedToViewport(viewport,first->position);

      viewport.pan(p0,p1);

      if (first->age < 3) {
        viewport.velocityBR *= 0;
        viewport.velocityTL *= 0;
      }

      if (!first->overlap) {
        viewport.updateInertia();
      }
    } else {
      viewport.updateInertia();
      viewport.applyInertia();
      viewport.constrain();
    }

  }

  namespace TouchScreen {
    bool initialized = false;

    void touchEventHandler(ofTouchEventArgs& touch) {
      addTouch(touch.id, ofVec2f((float)touch.x/(float)ofGetWindowWidth(), (float)touch.y/(float)ofGetWindowHeight()));
    }
    void touchUp(ofTouchEventArgs& touch) {
    }
    void dryUpdate() {
      if (!initialized) {
        ofxMultitouch::EnableTouch();
        ofAddListener(ofxMultitouch::touchDown, touchEventHandler);
        ofAddListener(ofxMultitouch::touchMoved, touchEventHandler);
        initialized = true;
      }
      updateTouches();
    }
    void update(Viewport& viewport) {
      dryUpdate();
      updateViewport(viewport);
    }
  }


  namespace OSC {
    ofxOscReceiver receiver;
    ofXml xml;
    bool initialized = false;

    ofVec2f str2vec2f(string s) {
      int comma = s.find(",");
      return ofVec2f(stof(s.substr(0, comma)), stof(s.substr(comma + 1)));
    }

    void update(Viewport& viewport) {
      if (!initialized) {
        receiver.setup(OSC_PORT_TRACKPAD);
        initialized = true;
      }


      while (receiver.hasWaitingMessages()) {

        ofxOscMessage m;
        receiver.getNextMessage(m);
        if (m.getAddress() == "/trackpad") {
          string s = m.getArgAsString(0);
          xml.load(s);
          auto touchxml = xml.find("//multitouch/touch");

          for (auto & touch : touchxml) {
            ofVec2f p = str2vec2f(touch.getAttribute("position").getValue());
            p.y = 1 - p.y; // trackpad: bottom=0
            addTouch(stoi(touch.getAttribute("id").getValue()), p);

          }

        }
        else {
          cout << "unrecognized OSC message received" << endl;
        }

        updateViewport(viewport);
      }

      updateTouches();

    }
  };
  ofColor id2color(int id){
    return ofColor(29,174,236,200);
    int r = (id>>2)&1;
    int g = (id>>1)&1;
    int b = id&1;
    return ofColor(r*255,g*255,b*255,200);

  }
  void drawDebug(){
    for (int i = 0; i < touches.size(); i++){
      if (touches[i].expired){
        continue;
      }
      ofVec2f p(touches[i].position.x * ofGetWindowWidth(),
                touches[i].position.y * ofGetWindowHeight());
      ofPushStyle();
      ofSetColor(id2color(touches[i].id));
      ofDrawCircle(p,20);
      ofPopStyle();
      //ofDrawBitmapStringHighlight(ofToString(touches[i].id),p);
    } 
  }
};
