#pragma once
class Viewport : public ofRectangle {
public:
  float minWidth;
  float maxWidth;
  float hardMinWidth;
  float hardMaxWidth;
  float springiness = 0.2;
  float changeEpsilon = 0;
  float antifriction = 0.9;
  ofVec2f velocityTL;
  ofVec2f velocityBR;
  ofRectangle last;
  ofRectangle zero;
  bool constraintsOff;
  bool inertiaOff = false;
  ofVec4f constrainStrictness = ofVec4f(0.5,0.5,0.5,0.5);

  ofVec2f _zoomCenter;

  Viewport() {

  }

  void set(float l, float t, float w, float h) {
    x = l;
    y = t;
    width = w;
    height = h;
    last = ofRectangle(l, t, w, h);
  }

  void set(ofRectangle& rect) {
    set(rect.x, rect.y, rect.width, rect.height);
  }
  void copy(const Viewport& v) {
    x = v.x;
    y = v.y;
    width = v.width;
    height = v.height;
    minWidth = v.minWidth;
    maxWidth = v.maxWidth;
    hardMinWidth = v.hardMinWidth;
    hardMaxWidth = v.hardMaxWidth;
    springiness = v.springiness;
    changeEpsilon = v.changeEpsilon;
    zero = v.zero;
  }


  void inferConstraints() {
    hardMaxWidth = zero.width * 20;
    hardMinWidth = zero.width * 0.01;
    maxWidth = zero.width * 2;
    minWidth = zero.width * 0.1;
  }

  void updateVelocity() {
    ofVec2f v0(x - last.x, y - last.y);
    ofVec2f v1(x + width - (last.x + last.width), y + height - (last.y + last.height));

    velocityTL = ofLerpVec2f(velocityTL, v0*5, 0.01);
    velocityBR = ofLerpVec2f(velocityBR, v1*5, 0.01);

    last = ofRectangle(x, y, width, height);
  }

  float inertiaScalingFunction(float x, float r) {
    if (1 - r < x && x < 1 + r) {
      x = (x - (1 - r)) / (r * 2);
      float a = 0.5;
      float b = 0.5;
      float y = 4.0*pow(x - 0.5, 3.0) + 0.5;
      float t = 1.0 / 2 - cos(2 * PI*x) / 2;
      float w = y * t + x * (1 - t);
      return 1 - r + w * r * 2;
    }
    return x;
  }

  void zoomPan(ofVec2f p0, ofVec2f q0, ofVec2f p1, ofVec2f q1) {

    ofVec2f c0 = (p0 + q0) / 2;
    ofVec2f c1 = (p1 + q1) / 2;

    float d0 = p0.distance(q0);
    float d1 = p1.distance(q1);

    float scale = d0 / d1;

    scale = inertiaScalingFunction(scale, 0.05);
    scale = fmin(fmax(scale*width, hardMinWidth), hardMaxWidth) / width;

    x = c0.x + (x - c1.x)*scale;
    y = c0.y + (y - c1.y)*scale;

    width *= scale;
    height *= scale;

    _zoomCenter = ofVec2f(c1.x, c1.y);
    updateVelocity();
  }

  void pan(ofVec2f p0, ofVec2f p1) {
    ofVec2f p = p0 - p1;
    x = fmin(fmax(x + p.x, zero.x - width), zero.x + zero.width);
    y = fmin(fmax(y + p.y, zero.y - height), zero.y + zero.height);
    updateVelocity();
  }

  void updateInertia() {

    if (width <= hardMinWidth) {
      return;
    }
    while (width + velocityBR.x - velocityTL.x < 0) {
      velocityTL *= 0.9;
      velocityBR *= 0.9;
    }

    velocityTL *= antifriction;
    velocityBR *= antifriction;
  }
  void applyInertia() {
    if (!inertiaOff) {
      float rat = height / width;
      x += velocityTL.x;
      y += velocityTL.y;
      width += velocityBR.x - velocityTL.x;
      height = width * rat;
    }
  }
  void constrain() {
    if (constraintsOff) {
      return;
    }
    if (x < zero.x - width * constrainStrictness.x) {
      x = ofLerp(x, zero.x - width * constrainStrictness.x, springiness);
    }
    if (x > zero.x + zero.width - width * (1- constrainStrictness.z)) {
      x = ofLerp(x, zero.x + zero.width - width * (1- constrainStrictness.z), springiness);
    }
    if (y < zero.y - height * constrainStrictness.y) {
      y = ofLerp(y, zero.y - height * constrainStrictness.y, springiness);
    }
    if (y > zero.y + zero.height - height * (1- constrainStrictness.w)) {
      y = ofLerp(y, zero.y + zero.height - height * (1- constrainStrictness.w), springiness);
    }
    if (width > maxWidth) {
      float w = ofLerp(width, maxWidth, springiness);
      float s = w / width;
      x = (x - _zoomCenter.x) * s + _zoomCenter.x;
      y = (y - _zoomCenter.y) * s + _zoomCenter.y;
      width = w;
      height *= s;
    }
    if (width < minWidth) {
      float w = ofLerp(width, minWidth, springiness);
      float s = w / width;
      x = (x - _zoomCenter.x) * s + _zoomCenter.x;
      y = (y - _zoomCenter.y) * s + _zoomCenter.y;
      width = w;
      height *= s;
    }
  }

  void lerpTo(ofRectangle r, float t) {
    x = ofLerp(x, r.x, t);
    y = ofLerp(y, r.y, t);
    width = ofLerp(width, r.width, t);
    height = ofLerp(height, r.height, t);
  }

  void print() {
    cout << "x: " << x << ", ";
    cout << "y: " << y << ", ";
    cout << "w: " << width << ", ";
    cout << "h: " << height << ", ";
    cout << endl;
  }

};
