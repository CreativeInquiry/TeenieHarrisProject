ofVec2f ofLerpVec2f(ofVec2f a, ofVec2f b, float t) {
  ofVec2f c;
  c.x = a.x*(1 - t) + b.x*t;
  c.y = a.y*(1 - t) + b.y*t;
  return c;
}
float ofDistVec2f(ofVec2f a, ofVec2f b) {
  return ofDist(a.x, a.y, b.x, b.y);
}
void ofDrawRectangleOutline(ofRectangle r, float w = 2, int ioset=0, bool alpha=false) {
  if (alpha) {
    if (ioset == 0) {
      ofDrawRectangle(r.x - w / 2, r.y - w / 2, w, r.height);
      ofDrawRectangle(r.x + w / 2, r.y - w / 2, r.width, w);
      ofDrawRectangle(r.x + r.width - w / 2, r.y + w / 2, w, r.height );
      ofDrawRectangle(r.x - w / 2, r.y + r.height - w / 2, r.width, w);
    } else if (ioset == -1) {
      ofDrawRectangle(r.x, r.y, w, r.height-w);
      ofDrawRectangle(r.x+w, r.y, r.width-w, w);
      ofDrawRectangle(r.x + r.width - w, r.y+w, w, r.height-w);
      ofDrawRectangle(r.x, r.y + r.height - w, r.width-w, w);
    } else if (ioset == 1) {
      ofDrawRectangle(r.x - w, r.y - w, w, r.height + w);
      ofDrawRectangle(r.x, r.y - w, r.width + w, w);
      ofDrawRectangle(r.x + r.width, r.y, w, r.height + w);
      ofDrawRectangle(r.x - w, r.y + r.height, r.width + w, w);
    }
  } else {
    if (ioset == 0) {
      ofDrawRectangle(r.x - w / 2, r.y - w / 2, w, r.height + w);
      ofDrawRectangle(r.x - w / 2, r.y - w / 2, r.width + w, w);
      ofDrawRectangle(r.x + r.width - w / 2, r.y - w / 2, w, r.height + w);
      ofDrawRectangle(r.x - w / 2, r.y + r.height - w / 2, r.width + w, w);
    } else if (ioset == -1) {
      ofDrawRectangle(r.x, r.y, w, r.height);
      ofDrawRectangle(r.x, r.y, r.width, w);
      ofDrawRectangle(r.x + r.width - w, r.y, w, r.height);
      ofDrawRectangle(r.x, r.y + r.height - w, r.width, w);
    } else if (ioset == 1) {
      ofDrawRectangle(r.x - w, r.y - w, w, r.height + w * 2);
      ofDrawRectangle(r.x - w, r.y - w, r.width + w * 2, w);
      ofDrawRectangle(r.x + r.width, r.y-w, w, r.height + w * 2);
      ofDrawRectangle(r.x - w, r.y + r.height, r.width + w * 2, w);
    }
  }
}

struct ofImageLoadSettingsGrayscale : public ofImageLoadSettings { bool grayscale = true; };
ofImageLoadSettingsGrayscale IMAGE_LOAD_GRAYSCALE;