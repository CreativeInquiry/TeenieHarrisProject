#pragma once


#define THUMB_W 256
#define THUMBSMESH_THUMB_W 0.8
#define THUMBSTEX_W 16384
#define THUMBSTEX_SHEETS 15
#define THUMBSTEX_ROWS 64
#define THUMBSTEX_CAP (THUMBSTEX_ROWS*THUMBSTEX_ROWS)
#define VIEWPORT_MINW 5
#define VIEWPORT_MAXW 300
#define VIEWPORT_HARD_MINW 0.5
#define VIEWPORT_HARD_MAXW 500
#define FACT1 277
#define FACT2 214
#define VIEWPORT0 (ofRectangle(-(float)FACT2 / 2, -(float)FACT1 / 2, FACT2, FACT1))
#define MAPWIN_W 2160
#define MAPWIN_H ((float)MAPWIN_W*(float)FACT1/(float)FACT2)
#define VIEWPORT_CHANGE_EPSILON 0
#define VIEWPORT_SPRINGINESS 0.2
#define THUMBS_3D 0

using namespace std;

float virtualWindowWidth() {
  return ofGetWindowWidth() * (HALF+1);
}
float virtualWindowHeight() {
  return ofGetWindowHeight() * (HALF + 1);
}
namespace THP {
  vector<ofTexture> thumbsTextures;
  vector<ofMesh> thumbsMeshes;
  ofEasyCam thumbsCam;

  vector<vector<ofVec3f>> embeddings;
  vector<ofRectangle> embeddingBBoxes;
  vector<string> embeddingNames;
  vector<string> embeddingDescriptions;
  int targetEmbeddingIdx = 0;
  int previousEmbeddingIdx = 0;
  int beenLerpingEmbedding = 0;

  vector<ofVec3f> pointcloud;
  ofRectangle pointcloudBBox;

  Viewport viewport;

  Viewport imageViewport;

  ofMesh depthMesh;
  ofEasyCam depthCam;
  ofTexture imageTexture;

  vector<tuple<string, string>> filenames;


  vector<tuple<string, string, string>> descriptions;

  void checkAllocationLimit() {
    char* thumbsBuffer;
    size_t maxSheets = THUMBSTEX_SHEETS;
    while (maxSheets > 0) {
      size_t needBytes = THUMBSTEX_CAP * THUMB_W*THUMB_W * maxSheets;
      std::cout << "see if i can malloc " << maxSheets << " sheets... (" << ((float)needBytes / 1048576.0) << "MB)" << endl;
      thumbsBuffer = (char*)malloc(needBytes);
      if (thumbsBuffer != NULL) {
        break;
      }
      std::cout << "oops, out of memory" << endl;
      maxSheets--;
    }
    std::cout << "turns out i can load " << maxSheets << " sheets." << endl;
    free(thumbsBuffer);
  }

  void loadThumbs(string path) {
    ////cout << path << endl;

    if (thumbsTextures.size() < THUMBSTEX_MAXSHEETS) {
      ofTexture thumbsTex;
      ofLoadImage(thumbsTex, path);// , IMAGE_LOAD_GRAYSCALE);
      thumbsTextures.push_back(thumbsTex);
    }
    ofMesh thumbsMesh;
    for (int i = 0; i < THUMBSTEX_ROWS; i++) {
      for (int j = 0; j < THUMBSTEX_ROWS; j++) {
        int idx = i * THUMBSTEX_ROWS + j;

        if (idx + thumbsMeshes.size()*THUMBSTEX_CAP >= FACT1 * FACT2) {
          break;
        }

        ofPoint ul(-1, -1, 0);
        ofPoint ur(1, -1, 0);
        ofPoint bl(-1, 1, 0);
        ofPoint br(1, 1, 0);

        thumbsMesh.addVertex(ul);
        thumbsMesh.addVertex(ur);
        thumbsMesh.addVertex(bl);
        thumbsMesh.addVertex(br);

        int x = THUMB_W * (idx % THUMBSTEX_ROWS);
        int y = THUMB_W * (idx / THUMBSTEX_ROWS);
        float fw = (float)THUMBSTEX_W;

        thumbsMesh.addTexCoord(ofVec2f((float)(x) / fw, (float)(y) / fw));
        thumbsMesh.addTexCoord(ofVec2f((float)(x + THUMB_W) / fw, (float)(y) / fw));
        thumbsMesh.addTexCoord(ofVec2f((float)(x) / fw, (float)(y + THUMB_W) / fw));
        thumbsMesh.addTexCoord(ofVec2f((float)(x + THUMB_W) / fw, (float)(y + THUMB_W) / fw));

        thumbsMesh.addIndex(idx * 4 + 0);
        thumbsMesh.addIndex(idx * 4 + 1);
        thumbsMesh.addIndex(idx * 4 + 2);

        thumbsMesh.addIndex(idx * 4 + 1);
        thumbsMesh.addIndex(idx * 4 + 3);
        thumbsMesh.addIndex(idx * 4 + 2);
      }
    }

    thumbsMeshes.push_back(thumbsMesh);
    //thumbsCam.disableMouseInput();
  }


  void loadEmbedding(string path) {
    float xmin = FLT_MAX;
    float xmax = -FLT_MAX;
    float ymin = FLT_MAX;
    float ymax = -FLT_MAX;

    vector<ofVec3f> embedding;

    ofBuffer buffer = ofBufferFromFile(path);
    if (buffer.size()) {
      for (ofBuffer::Line it = buffer.getLines().begin(), end = buffer.getLines().end(); it != end; ++it) {
        string line = *it;
        if (line.empty() == false) {
          int tab = line.find("\t");
          ofVec3f p(stof(line.substr(0, tab)), stof(line.substr(tab + 1)),
#if THUMBS_3D
            0.0001*embedding.size()
#else
            0
#endif
          );
          // p = ofVec3f(p.y, p.x, 0);
          // cout << p << endl;
          if (p.x < xmin) { xmin = p.x; }
          if (p.x > xmax) { xmax = p.x; }
          if (p.y < ymin) { ymin = p.y; }
          if (p.y > ymax) { ymax = p.y; }
          embedding.push_back(p);
        }
      }
    }
    float fx = VIEWPORT0.width / (xmax - xmin);
    float fy = VIEWPORT0.height / (ymax - ymin);
    float f = fmin(fx, fy);

    ofRectangle bbox(
      VIEWPORT0.x + (VIEWPORT0.width - (xmax - xmin)*f) / 2,
      VIEWPORT0.y + (VIEWPORT0.height - (ymax - ymin)*f) / 2,
      (xmax - xmin)*f,
      (ymax - ymin)*f
    );

    for (int i = 0; i < embedding.size(); i++) {
      embedding[i].x = bbox.x + (embedding[i].x - xmin) * f;
      embedding[i].y = bbox.y + (embedding[i].y - ymin) * f;
    }
    embeddings.push_back(embedding);
    embeddingBBoxes.push_back(bbox);
    ////cout << "embedding bbox: " << bbox << endl;

    if (!pointcloud.size()) { // first time
      for (int i = 0; i < embedding.size(); i++) {
        pointcloud.push_back(ofVec3f(embedding[i].x, embedding[i].y, embedding[i].z));
      }
      pointcloudBBox = ofRectangle(bbox.x, bbox.y, bbox.width, bbox.height);
      viewport.set(bbox.x, bbox.y, bbox.width, bbox.height);
    }
    embeddingNames.push_back(path);
  }

  void onGUISelectEmbedding(GUI::EventInfo& e) {
    if (e.elementId == "sel-embedding") {
      targetEmbeddingIdx = std::find(embeddingNames.begin(), embeddingNames.end(), GUI::getElementById<GUI::Radio>("sel-embedding")->currentValue) - embeddingNames.begin();
    }
  }

  void loadEmbeddingFolder(string path, bool gui = true) {
    ofDirectory dir(path);
    dir.allowExt("tsv");
    dir.listDir();

    if (gui) {
      GUI::Radio* sel = GUI::createElement<GUI::Radio>("sel-embedding");
      sel->setRect(ofRectangle(10, 200, 2000, 200));

      for (int i = 0; i < dir.size(); i++) {
        string fname = dir.getPath(i);
        sel->addItem(fname);
        loadEmbedding(fname);
      }
      ofAddListener(GUI::onEvent, onGUISelectEmbedding);

    } else {
      for (int i = 0; i < min((int)100, (int)dir.size()); i++) {
        string fname = dir.getPath(i);
        loadEmbedding(fname);
      }
    }
  }

  GUI::RadioTable* loadEmbeddingsToNamedButtons(vector<string> namePathPairs, int cols) {
    GUI::RadioTable* sel = GUI::createElement<GUI::RadioTable>("sel-embedding");
    sel->defaultSelect = 0;
    targetEmbeddingIdx = 0;
    sel->cols = cols;
    sel->rows = (int)ceil((float)(namePathPairs.size() / 2) / (float)cols);
    for (int i = 0; i < namePathPairs.size(); i += 2) {
      string name = namePathPairs[i];
      string path = namePathPairs[i + 1];
      sel->addItem(name);
      loadEmbedding(path);
      embeddingNames[embeddingNames.size() - 1] = name;
    }
    for (int i = 0; i < sel->items.size(); i++) {
      sel->items[i]->style.lineHeight = 0.4;
    }
    ofAddListener(GUI::onEvent, onGUISelectEmbedding);
    return sel;
  }


  void lerpToTargetEmbedding(float t, int pin = -1) {
    if (previousEmbeddingIdx != targetEmbeddingIdx) {
      beenLerpingEmbedding = 0;
      previousEmbeddingIdx = targetEmbeddingIdx;
    }
    beenLerpingEmbedding++;

    int idx = targetEmbeddingIdx;
    float w = THUMBSMESH_THUMB_W;
    for (int i = 0; i < pointcloud.size(); i++) {

      ofVec3f p = pointcloud[i] * (1 - t) + embeddings[idx][i] * t;
      if (pin == i) {
        //if (!ofRectangle(0, 0, viewport.width, viewport.height).inside(p)) {
        viewport.x += p.x - pointcloud[i].x;
        viewport.y += p.y - pointcloud[i].y;
        //}
      }
      pointcloud[i] = p;

      int meshIdx = (int)i / (int)THUMBSTEX_CAP;
      int ii = i % THUMBSTEX_CAP;

      thumbsMeshes[meshIdx].setVertex(ii * 4 + 0, ofPoint(pointcloud[i].x - w / 2, pointcloud[i].y - w / 2, pointcloud[i].z));
      thumbsMeshes[meshIdx].setVertex(ii * 4 + 1, ofPoint(pointcloud[i].x + w / 2, pointcloud[i].y - w / 2, pointcloud[i].z));
      thumbsMeshes[meshIdx].setVertex(ii * 4 + 2, ofPoint(pointcloud[i].x - w / 2, pointcloud[i].y + w / 2, pointcloud[i].z));
      thumbsMeshes[meshIdx].setVertex(ii * 4 + 3, ofPoint(pointcloud[i].x + w / 2, pointcloud[i].y + w / 2, pointcloud[i].z));

    }
    pointcloudBBox.x = ofLerp(pointcloudBBox.x, embeddingBBoxes[idx].x, t);
    pointcloudBBox.y = ofLerp(pointcloudBBox.y, embeddingBBoxes[idx].y, t);
    pointcloudBBox.width = ofLerp(pointcloudBBox.width, embeddingBBoxes[idx].width, t);
    pointcloudBBox.height = ofLerp(pointcloudBBox.height, embeddingBBoxes[idx].height, t);


  }



  void drawMap(bool doAA) {

    if (doAA) {
      glEnable(GL_POLYGON_SMOOTH);
    } else {
      glDisable(GL_POLYGON_SMOOTH);
    }

    ofPushMatrix();
    ofTranslate(ofVec2f((virtualWindowWidth() - MAPWIN_W) / 2, (virtualWindowHeight() - MAPWIN_H) / 2));

#if THUMBS_3D
    thumbsCam.setVFlip(true);
    thumbsCam.setNearClip(0.0001);
    ofVec2f center(viewport.x + viewport.width / 2, viewport.y + viewport.height / 2);
    float fov = thumbsCam.getFov()*thumbsCam.getAspectRatio() * PI / 180.0;
    float d = (viewport.width / 2) / tan(fov / 2);

    thumbsCam.setPosition(center.x, center.y, d);
    thumbsCam.lookAt(ofVec3f(center.x, center.y, 0));
    thumbsCam.begin();
#else
    ofPushMatrix();
    ofScale((float)MAPWIN_W / viewport.width, (float)MAPWIN_H / viewport.height);
    ofTranslate(-viewport.x, -viewport.y);
#endif

    for (int i = 0; i < THUMBSTEX_SHEETS; i++) {
      thumbsTextures[i%thumbsTextures.size()].bind();
      thumbsMeshes[i].draw();
      thumbsTextures[i%thumbsTextures.size()].unbind();
    }

#if THUMBS_3D
    thumbsCam.end();
#else
    ofPopMatrix();
#endif

    ofPopMatrix();

    glDisable(GL_POLYGON_SMOOTH);
  }

  void zoomMapAt(int x, int y, float t) {
    t = ofClamp(t, -0.99, 0.99);
    if (viewport.width < VIEWPORT_MINW && t > 0) {
      return;
    }
    if (viewport.width > VIEWPORT_MAXW && t < 0) {
      return;
    }
    float vx = viewport.x + ((float)(x - (virtualWindowWidth() - MAPWIN_W) / 2) / (float)MAPWIN_W) * viewport.width;
    float vy = viewport.y + ((float)(y - (virtualWindowHeight() - MAPWIN_H) / 2) / (float)MAPWIN_H) * viewport.height;

    float rx0 = ofLerp(viewport.x, vx, t);
    float ry0 = ofLerp(viewport.y, vy, t);
    float rx1 = ofLerp(viewport.x + viewport.width, vx, t);
    float ry1 = ofLerp(viewport.y + viewport.height, vy, t);
    viewport.set(rx0, ry0, rx1 - rx0, ry1 - ry0);
  }

  void panMap(int dx, int dy) {
    float vdx = ((float)dx / (float)MAPWIN_W) * viewport.width;
    float vdy = ((float)dy / (float)MAPWIN_H) * viewport.height;
    viewport.x -= vdx;
    viewport.y -= vdy;
  }

  float borderAverage(ofPixels& pix, int d = 5, bool mod = true) {
    int w = pix.getWidth();
    int h = pix.getHeight();
    float a = 0;
    float cnt = 0;
    for (int i = 0; i < h; i++) {
      for (int j = 0; j < w; j++) {
        if (j < d || j > w - d || i < d || i > h - d) {
          a += pix[i*w + j];
          cnt++;
        }
      }
    }
    a /= cnt;
    if (mod) {
      for (int i = 0; i < h; i++) {
        for (int j = 0; j < w; j++) {
          if (j < d || j > w - d || i < d || i > h - d) {
            pix[i*w + j] = a;
          }
        }
      }
    }
    return a;
  }

  class ImageLoadThread : public ofThread {
  public:
    string path;
    ofPixels pixels;
    int state = 0;
    int id = -1;

    // states
    // 0 = idle
    // 1 = loading the image
    // 2 = loading complete

    bool load(int _id, string _path) {
      lock();
      if (state == 1) {
        unlock();
        return false;
      }
      path = _path;
      state = 1;
      id = _id;
      unlock();
      return true;
    }
    void threadedFunction() {
      while (isThreadRunning()) {
        lock();
        if (state == 1) {
          ofLoadImage(pixels, path, IMAGE_LOAD_GRAYSCALE);
          state = 2;
        }
        unlock();
      }
    }
  };
  ofRectangle calculateViewportForImageBoundByScreenRect(ofRectangle r) {
    float w = imageTexture.getWidth();
    float h = imageTexture.getHeight();
    if (h > w) {
      float px = (r.width - r.height*w / h) / 2;
      r.x += px;
      r.width -= 2 * px;
    } else {
      float py = (r.height - r.width*h / w) / 2;
      r.y += py;
      r.height -= 2 * py;
    }
    float x0 = (r.x - (virtualWindowWidth() - (float)MAPWIN_W) / 2);
    float y0 = (r.y - (virtualWindowHeight() - (float)MAPWIN_H) / 2);
    float x1 = (r.x + r.width - (virtualWindowWidth() - (float)MAPWIN_W) / 2);
    float y1 = (r.y + r.height - (virtualWindowHeight() - (float)MAPWIN_H) / 2);

    //cout << imageViewport.zero << " " << MAPWIN_H << " " << imageTexture.getHeight() << endl;

    ofRectangle r1;
    if (w > h) {
      r1.width = imageViewport.zero.width * MAPWIN_W / (x1 - x0);
      r1.height = r1.width * imageViewport.zero.height / imageViewport.zero.width;
      r1.x = - x0 * imageViewport.zero.width / (x1 - x0);
      r1.y = -y0 * imageViewport.zero.width / (x1 - x0);
    }else{
      r1.height = imageTexture.getHeight() * virtualWindowHeight()/(r.height+ (virtualWindowHeight() - (float)MAPWIN_H) / 2);
      r1.width = r1.height * imageViewport.zero.width / imageViewport.zero.height;
      r1.x = -(r1.width - imageTexture.getWidth() )/2;
      r1.y = imageTexture.getHeight() * ((virtualWindowHeight() - (float)MAPWIN_H) / 2) / r.height;
    }
    imageViewport.constraintsOff = true;
    return r1;
  }


  void calculateImageViewportFromScreenRect(ofRectangle r) {


    float w = imageTexture.getWidth();
    float h = imageTexture.getHeight();
    //float vh = w * (float)FACT1 / (float)FACT2;

    //imageViewport.zero = ofRectangle(0, (h - vh) / 2, w, vh);

    if (h / w > (float)virtualWindowHeight() / (float)virtualWindowWidth()) {
      ////cout << "tall" << endl;
      float vw = h * (float)FACT2 / (float)FACT1;
      imageViewport.zero = ofRectangle((w - vw) / 2, 0, vw, h);
    } else {
      ////cout << "not tall" << endl;
      float vh = w * (float)FACT1 / (float)FACT2;

      imageViewport.zero = ofRectangle(0, (h - vh) / 2, w, vh);

    }



    imageViewport.inferConstraints();

    float x0 = ((r.x - (virtualWindowWidth() - (float)MAPWIN_W) / 2) / (float)MAPWIN_W)*viewport.width + viewport.x;
    float y0 = ((r.y - (virtualWindowHeight() - (float)MAPWIN_H) / 2) / (float)MAPWIN_H)*viewport.height + viewport.y;
    float x1 = ((r.x + r.width - (virtualWindowWidth() - (float)MAPWIN_W) / 2) / (float)MAPWIN_W)*viewport.width + viewport.x;
    float y1 = ((r.y + r.height - (virtualWindowHeight() - (float)MAPWIN_H) / 2) / (float)MAPWIN_H)*viewport.height + viewport.y;


    float fx = ((x0 + x1) / 2 - viewport.x) / viewport.width;
    float fy = ((y0 + y1) / 2 - viewport.y) / viewport.height;


    float crop = (40.0 / 640.0 * fmax(w, h)) / fmin(w, h);
    float tw = (x1 - x0) * (1 + crop);

    if (h > w) {
      float pcw = tw / viewport.width;
      imageViewport.width = w / pcw;
      imageViewport.height = imageViewport.width * (float)FACT1 / (float)FACT2;
    } else {
      float pch = tw / viewport.height;
      imageViewport.height = h / pch;
      imageViewport.width = imageViewport.height * (float)FACT2 / (float)FACT1;
    }
    imageViewport.x = w / 2 - imageViewport.width * fx;
    imageViewport.y = h / 2 - imageViewport.height *fy;

  }


  void calculateImageViewport(int idx) {
    float w = imageTexture.getWidth();
    float h = imageTexture.getHeight();
    if (h / w > (float)virtualWindowHeight() / (float)virtualWindowWidth()) {
      ////cout << "tall" << endl;
      float vw = h * (float)FACT2 / (float)FACT1;
      imageViewport.zero = ofRectangle((w - vw) / 2, 0, vw, h);
    } else {
      ////cout << "not tall" << endl;
      float vh = w * (float)FACT1 / (float)FACT2;

      imageViewport.zero = ofRectangle(0, (h - vh) / 2, w, vh);
      
    }

    imageViewport.inferConstraints();

    ofVec2f p(pointcloud[idx]);
    float fx = (p.x - viewport.x) / viewport.width;
    float fy = (p.y - viewport.y) / viewport.height;


    float crop = (40.0 / 640.0 * fmax(w, h)) / fmin(w, h);
    float tw = THUMBSMESH_THUMB_W * (1 + crop);

    if (h > w) {
      float pcw = tw / viewport.width;
      imageViewport.width = w / pcw;
      imageViewport.height = imageViewport.width * (float)FACT1 / (float)FACT2;
    } else {
      float pch = tw / viewport.height;
      imageViewport.height = h / pch;
      imageViewport.width = imageViewport.height * (float)FACT2 / (float)FACT1;
    }
    imageViewport.x = w / 2 - imageViewport.width * fx;
    imageViewport.y = h / 2 - imageViewport.height *fy;

    float f = imageViewport.width / viewport.width;
    imageViewport.velocityTL = viewport.velocityTL * f;
    imageViewport.velocityBR = viewport.velocityBR * f;

    //cout << w << "x" << h << endl;

    //imageViewport.print();

    //imageViewport.zero = ofRectangle(imageViewport.x, imageViewport.y, imageViewport.width, imageViewport.height);//ofRectangle(0, (h - vh) / 2, w, vh);
    //imageViewport.inferConstraints();
  }

  void calculateViewportFromImageViewport(int idx) {
    float w = imageTexture.getWidth();
    float h = imageTexture.getHeight();

    ofVec2f p(pointcloud[idx]);

    float fx = (w / 2 - imageViewport.x) / imageViewport.width;
    float fy = (h / 2 - imageViewport.y) / imageViewport.height;

    float crop = (40.0 / 640.0 * fmax(w, h)) / fmin(w, h);
    float tw = THUMBSMESH_THUMB_W * (1 + crop);

    if (h > w) {
      float pcw = w / imageViewport.width;
      viewport.width = tw / pcw;
      viewport.height = viewport.width * (float)FACT1 / (float)FACT2;
    } else {
      float pch = h / imageViewport.height;
      viewport.height = tw / pch;
      viewport.width = viewport.height * (float)FACT2 / (float)FACT1;
    }
    viewport.x = p.x - viewport.width * fx;
    viewport.y = p.y - viewport.height *fy;

    float f = imageViewport.width / viewport.width;
    viewport.velocityTL = imageViewport.velocityTL / f;
    viewport.velocityBR = imageViewport.velocityBR / f;

    //cout << w << "x" << h << endl;

    //viewport.print();

  }

  vector<int> thumbNeighbors(int idx) {
    int argmins[4] = { -1,-1,-1,-1 };
    float mins[4] = { FLT_MAX,FLT_MAX,FLT_MAX,FLT_MAX };
    for (int i = 0; i < pointcloud.size(); i++) {
      if (i == idx) {
        continue;
      }
      float d = ofDistVec2f((ofVec2f)pointcloud[idx], (ofVec2f)pointcloud[i]);
      if (d > 1.4) {
        continue;
      }
      for (int j = 0; j < 4; j++) {

        if (d < mins[j]) {
          for (int k = 3; k > j; k--) {
            mins[k] = mins[k - 1];
            argmins[k] = argmins[k - 1];
          }
          mins[j] = d;
          argmins[j] = i;
          break;
        }
      }

    }
    int left = -1, right = -1, top = -1, bottom = -1;
    ofVec2f q = pointcloud[idx];
    for (int i = 0; i < 4; i++) {
      if (argmins[i] < 0) {
        continue;
      }
      ofVec2f p = pointcloud[argmins[i]];
      if (fabs(p.x - q.x) > fabs(p.y - q.y)) {
        if (p.x > q.x) {
          right = argmins[i];
        } else {
          left = argmins[i];
        }
      } else {
        if (p.y > q.y) {
          bottom = argmins[i];
        } else {
          top = argmins[i];
        }
      }
    }
    return vector<int>{ left, top, right, bottom };
  }

  void loadImage(int idx, string path) {
    ofLoadImage(imageTexture, path, IMAGE_LOAD_GRAYSCALE);
    calculateImageViewport(idx);
  }


  void loadDepth(string imPath, string depthPath, float df = 0.2) {
    ofLoadImage(imageTexture, imPath);
    imageViewport.zero = ofRectangle(0, 0, imageTexture.getWidth(), imageTexture.getWidth()*(float)FACT1 / (float)FACT2);
    imageViewport.set(imageViewport.zero);
    imageViewport.inferConstraints();

    depthMesh.clear();
    depthMesh.setMode(OF_PRIMITIVE_TRIANGLES);

    ofPixels depth; ofLoadImage(depth, depthPath, IMAGE_LOAD_GRAYSCALE);

    float d = df * fmax(imageTexture.getWidth(), imageTexture.getHeight());
    float d0 = 0; //borderAverage(depth)*d / 255.0;

    int w = depth.getWidth();
    int h = depth.getHeight();
    float sx = imageTexture.getWidth() / w;
    float sy = imageTexture.getHeight() / h;

    for (int y = 0; y < h; y++) {
      for (int x = 0; x < w; x++) {
        ofPoint p((x - w / 2)*sx, (h / 2 - y)*sy, d*(float)depth[y*w + x] / 255.0 - d0);
        depthMesh.addVertex(p);
        depthMesh.addTexCoord(ofVec2f((float)x / (float)w, (float)y / (float)h));
      }
    }
    for (int y = 0; y < h - 1; y++) {
      for (int x = 0; x < w - 1; x++) {
        depthMesh.addIndex(x + y * w);
        depthMesh.addIndex((x + 1) + y * w);
        depthMesh.addIndex(x + (y + 1)*w);

        depthMesh.addIndex((x + 1) + y * w);
        depthMesh.addIndex((x + 1) + (y + 1)*w);
        depthMesh.addIndex(x + (y + 1)*w);


      }
    }
    depthCam.disableMouseInput();
    depthCam.setPosition(0, 0, w*sx);
    depthCam.enableOrtho();
  }

  void drawDepth(float theta, float x = 0, float y = 0) {
    ofEnableDepthTest();
    depthCam.begin();
    ofPushMatrix();

    ofTranslate(x + imageTexture.getWidth() / 2 - virtualWindowWidth() / 2, y - imageTexture.getHeight() / 2 + virtualWindowHeight() / 2);
    ofRotateY(theta * 180 / PI);
    ofScale(1 / cos(theta), 1);

    imageTexture.bind();
    depthMesh.draw();
    imageTexture.unbind();
    ofPopMatrix();
    depthCam.end();
    ofDisableDepthTest();
  }

  void drawImage() {
    ofPushMatrix();
    ofTranslate(ofVec2f((virtualWindowWidth() - MAPWIN_W) / 2, (virtualWindowHeight() - MAPWIN_H) / 2));

    ofPushMatrix();
    ofScale((float)MAPWIN_W / imageViewport.width, (float)MAPWIN_H / imageViewport.height);
    ofTranslate(-imageViewport.x, -imageViewport.y);
    imageTexture.draw(0, 0);
    ofPopMatrix();

    ofPopMatrix();
  }

  void loadCanonicalFilenames(string path) {

    ofBuffer buffer = ofBufferFromFile(path);
    if (buffer.size()) {
      for (ofBuffer::Line it = buffer.getLines().begin(), end = buffer.getLines().end(); it != end; ++it) {
        string line = *it;
        if (line.empty() == false) {
          int slash = line.find("/");
          string boxname = line.substr(0, slash);
          string fname = line.substr(slash + 1);
          //          cout << fname << endl;
          filenames.push_back(make_tuple(boxname, fname));
        }
      }
    }
  }

  void loadDescriptions(string path) {
    descriptions.clear();
    int maxlen = 0;
    int maxarg = 0;
    ofBuffer buffer = ofBufferFromFile(path);
    if (buffer.size()) {
      for (ofBuffer::Line it = buffer.getLines().begin(), end = buffer.getLines().end(); it != end; ++it) {
        string line = *it;
        if (line.empty() == false) {
          int tab = line.find("\t");
          string fname = line.substr(0, tab);
          string stuff = line.substr(tab + 1);
          tab = stuff.find("\t");
          string desc = stuff.substr(0, tab);
          string stuff2 = stuff.substr(tab + 1);
          tab = stuff2.find("\t");
          string date = stuff2.substr(0, tab);
          string stuff3 = stuff2.substr(tab + 1);
          tab = stuff3.find("\t");
          string acc = stuff3.substr(0, tab);
          descriptions.push_back(make_tuple(desc, date, acc));
          if (desc.size() > maxlen) {
            maxlen = desc.size();
            maxarg = descriptions.size() - 1;
          }
        }
      }
    }
    ////cout << "longest description: idx:" << maxarg << " n:" << maxlen << endl;

  }

  map<string, string> dataPaths;
  void loadPaths(string path) {
    dataPaths.clear();
    
    ofBuffer buffer = ofBufferFromFile(path);
    if (buffer.size()) {
      for (ofBuffer::Line it = buffer.getLines().begin(), end = buffer.getLines().end(); it != end; ++it) {
        string line = *it;
        if (line.empty() == false) {
          int tab = line.find("\t");
          string key = line.substr(0, tab);
          string v = line.substr(tab + 1);
          string nv = "";
          for (int i = 0; i < v.size(); i++) {
            if (v[i] != '\\') {
              nv += v[i];
            } else{
              nv += '/';
            }
          }
          string val = nv;
          if (key != "disk") {
            val = dataPaths["disk"] + ":/" + val + "/";
          }
          dataPaths[key] = val;
        }
      }
    }
  }

  bool fileExists(string path) {
    //https://forum.openframeworks.cc/t/error-loading-and-file-handling/792/2
    fstream fin;
    string fileNameInOF = ofToDataPath(path);
    fin.open(fileNameInOF.c_str(), ios::in);
    if (fin.is_open()) {
      fin.close();
      return true;
    }
    return false;
  }


  int getThumbIndexUnderCursor(float cursorX, float cursorY) {
    int midx = -1;
    float mdist = INFINITY;
    float x = ((cursorX - (virtualWindowWidth() - (float)MAPWIN_W) / 2) / (float)MAPWIN_W)*viewport.width + viewport.x;
    float y = ((cursorY - (virtualWindowHeight() - (float)MAPWIN_H) / 2) / (float)MAPWIN_H)*viewport.height + viewport.y;
    for (int i = 0; i < pointcloud.size(); i++) {
      float dx = pointcloud[i].x - x;
      float dy = pointcloud[i].y - y;
      float d = dx * dx + dy * dy;
      if (d < mdist) {
        mdist = d;
        midx = i;
      }
    }
    if (mdist > 2) {
      return -1;
    }
    return midx;
  }

  void enterThumbMatrix(int idx) {
    ofVec2f p = pointcloud[idx];
    ofTranslate((virtualWindowWidth() - (float)MAPWIN_W) / 2, (virtualWindowHeight() - (float)MAPWIN_H) / 2);
    ofScale((float)MAPWIN_W / viewport.width, (float)MAPWIN_H / viewport.height);
    ofTranslate(-viewport.x + p.x, -viewport.y + p.y);

  }

  ofVec2f thumbMatrixToScreenMatrix(int idx, ofVec2f p) {
    ofVec2f q = pointcloud[idx];
    return ofVec2f((q.x + p.x - viewport.x) / viewport.width*MAPWIN_W + (virtualWindowWidth() - (float)MAPWIN_W) / 2, (q.y + p.y - viewport.y) / viewport.height*MAPWIN_H + (virtualWindowHeight() - (float)MAPWIN_H) / 2);
  }

  ofVec2f imageMatrixToScreenMatrix(ofVec2f p) {
    return ofVec2f((p.x - imageViewport.x) / imageViewport.width*MAPWIN_W + (virtualWindowWidth() - (float)MAPWIN_W) / 2, (p.y - imageViewport.y) / imageViewport.height*MAPWIN_H + (virtualWindowHeight() - (float)MAPWIN_H) / 2);
  }


  namespace Autopilot {

    int target = -1;
    int nextTarget = -1;
    int state = 0;
    float stateProgress = 0;
    float focusWidth = 2;
    float overviewMargin = 5;
	int sequential = -1;

    int isNear = -1;

    ofRectangle viewportI;
    ofRectangle viewportF;



    float ease(float x) {
      return 0.5 - cos(PI*x) / 2.0;
    }

    float flatEndsEase(float x, float e=0.001) {
      if (x < e) {
        return 0;
      }
      if (x > 1-e) {
        return 1;
      }
      return ease((x - e) / (1 - e * 2));
    }

    float hillFun(float x) {
      return 0.5 - 0.5*cos(2 * PI*x);
    }

    void reset() {
      target = -1;
    }

	int randTarget() {
	  return  ((float)rand() / (float)RAND_MAX)*pointcloud.size();
	}

    void update(float t) {
      //cout << state << " " << target << " " << stateProgress << endl;
      if (target == -1) {
        state = 0;
        stateProgress = 0;
        viewportI = ofRectangle((ofRectangle)viewport);
        
		if (sequential < 0) {
			target = randTarget();
			while (true) {
				if (ofDistVec2f(viewport.getCenter(), pointcloud[target]) < 4) {
					break;
				}
				target = randTarget();
			}

			nextTarget = randTarget();
		}else {
			
			target = sequential;
			sequential = (sequential + 1) % pointcloud.size();
			nextTarget = sequential;
		}
		////cout << target << " " << nextTarget << endl;
        float w = focusWidth;
        float h = w * viewport.height / viewport.width;
        viewportF = ofRectangle(pointcloud[target].x - w / 2, pointcloud[target].y - h / 2, w, h);
      }

      float u = flatEndsEase(stateProgress);
      float hf = hillFun(stateProgress);
      ofVec2f nv(
        (ofNoise(ofGetFrameNum()*0.2*t,1)*2-1)*viewport.width*0.18*hf,
        (ofNoise(ofGetFrameNum()*0.2*t,2)*2-1)*viewport.width*0.18*hf
      );

      viewport.x = ofLerp(viewportI.x, viewportF.x, u) + nv.x;
      viewport.y = ofLerp(viewportI.y, viewportF.y, u) + nv.y;
      viewport.width = ofLerp(viewportI.width, viewportF.width, u);
      viewport.height = ofLerp(viewportI.height, viewportF.height, u);

      stateProgress += t*(0.1+0.9*viewport.width/VIEWPORT0.width);

      isNear = -1;

      if (fabs(stateProgress-1)<t) {
        stateProgress = 0;
        
        viewportI = ofRectangle(viewport);

        state = !state;
        if (state == 0) {

          target = nextTarget;
          int cand1 = randTarget();
          int cand2 = randTarget();
          ////cout << "dist" << ofDistVec2f(pointcloud[nextTarget], pointcloud[cand1]) << endl;
          nextTarget = ofDistVec2f(pointcloud[nextTarget],pointcloud[cand1]) < ofDistVec2f(pointcloud[nextTarget],pointcloud[cand2]) ? cand1 : cand2;
          float w = 2;
          float h = w * viewport.height / viewport.width;
          viewportF = ofRectangle(pointcloud[nextTarget].x - w / 2, pointcloud[nextTarget].y - h / 2, w, h);
          
        } else {

          ofVec2f p = pointcloud[target];
          ofVec2f q = pointcloud[nextTarget];
          float x = fmin(p.x, q.x);
          float y = fmin(p.y, q.y);
          float w = fabs(p.x - q.x);
          float h = fabs(p.y - q.y);
          float m = overviewMargin;
          ofRectangle minv(x - m, y - m, w + m * 2, h + m * 2);

          float h1 = minv.width * viewport.height / viewport.width;
          ofRectangle v1(minv.x, minv.y + minv.height / 2 - h1 / 2, minv.width, h1);
          float w2 = minv.height * viewport.width / viewport.height;
          ofRectangle v2(minv.x + minv.width / 2 - w2 / 2, minv.y, w2, minv.height);

          viewportF = (v1.getArea() > v2.getArea()) ? v1 : v2;

          isNear = target;
        }
      }
      
    }
  };

  vector<vector<ofRectangle>> _loadFaces(string path) {
    vector<vector<ofRectangle>> faces;

    ofBuffer buffer = ofBufferFromFile(path);
    if (buffer.size()) {
      for (ofBuffer::Line it = buffer.getLines().begin(), end = buffer.getLines().end(); it != end; ++it) {
        string line = *it;
        vector<ofRectangle> fs;
        ofRectangle* fptr;
        if (line.empty() == false) {
          int cnt = 0;
          int tab = 0;
          if (line[line.size() - 1] != '\t') {
            line += "\t";
          }
          while ((tab = line.find("\t")) != std::string::npos) {
            float val = stof(line.substr(0, tab));
            //cout << val << endl;
            line = line.substr(tab + 1);
            if (cnt % 4 == 0) {
              ofRectangle f;
              fs.push_back(f);
              fptr = &fs[fs.size() - 1];
              fptr->setX(val);
            } else if (cnt % 4 == 1) {
              fptr->setY(val);
            } else if (cnt % 4 == 2) {
              fptr->setWidth(val);
            } else if (cnt % 4 == 3) {
              fptr->setHeight(val);
            }
            cnt++;
          }
        }
        faces.push_back(fs);
      }
    }
    ////cout << "loaded face boxes for " << faces.size() << " images" << endl;
    return faces;
  }

  namespace KenBurns {
    vector<vector<ofRectangle>> faces;
    int target = -1;
    int dir = 1;
    int arrive = 0;
    ofRectangle lastRect(-1,-1,-1,-1);
    float prog = 0;
    int previdx = -1;
    int laps = 0;

    void loadFaces(string path) {
      faces = _loadFaces(path);
    }

    float sigmoidx(float x, float a) {
      if (x <= 0.5) {
        return (powf(2.0*x, 1.0 / a)) / 2.0;
      } else {
        return 1.0 - (powf(2.0*(1.0 - x), 1.0 / a))/ 2.0;
      }
    }

    void update(int idx, float t) {
      //cout << "laps" << laps << "target" << target << "fs" << faces.size()  << "idx" << idx << "lr" << lastRect << endl;
      if (previdx != idx) {
        target = -1;
        previdx = idx;
        laps = 0;
      }
      //cout << "kbu" << idx << endl;
      if (idx < 0 || idx >= faces.size()) {
        return;
      }
      vector<ofRectangle> boxes = faces[idx];
      //cout << "bs" << boxes.size() << "t" << (target >= (int)boxes.size()) << endl;

      if ((!boxes.size())||(target>=(int)boxes.size())) {
        laps++;
        return;
      }

      //cout << "t" << endl;

      if (target == -1) {
        cout << "new kburns" << endl;
        target = 0;
        lastRect = ofRectangle(imageViewport.x, imageViewport.y, imageViewport.width, imageViewport.height);
        prog = 0;
      }
      float cx = imageTexture.getWidth() * (boxes[target].x + boxes[target].width / 2);
      float cy = imageTexture.getHeight() * (boxes[target].y + boxes[target].height / 2);

      //cout << target << " " << boxes.size() << " (" << boxes[target] << ") " << cx << " " << cy << endl;

      ofRectangle rect;
      rect.width = (imageTexture.getWidth() * boxes[target].width)*3;
      rect.height = rect.width * imageViewport.zero.height / imageViewport.zero.width;
      rect.x = cx - rect.width / 2;
      rect.y = cy - rect.height * 0.25;

  

      if (target != 0) {
        float u = sigmoidx(prog, 0.3);
        float v = sigmoidx(prog, 0.3);
        float ex = 1.08 - 0.08*cos(2 * PI*prog);
        //float ex = 1.5 - 0.5*pow(2 * prog - 1, 2);

        ofVec2f c = ofLerpVec2f(lastRect.getCenter(), rect.getCenter(), u);

        imageViewport.width = ofLerp(lastRect.width, rect.width, v)*ex;
        imageViewport.height = ofLerp(lastRect.height, rect.height, v)*ex;
        imageViewport.x = c.x - imageViewport.width / 2;
        imageViewport.y = c.y - imageViewport.height / 2;
        prog += t;
      } else {
        float u = prog;
        ofVec2f c = ofLerpVec2f(lastRect.getCenter(), rect.getCenter(), u);
        imageViewport.width = ofLerp(lastRect.width, rect.width, u);
        imageViewport.height = ofLerp(lastRect.height, rect.height, u);
        imageViewport.x = c.x - imageViewport.width / 2;
        imageViewport.y = c.y - imageViewport.height / 2;
        prog += t;
      }
      

      

      if (prog >= 1){
        if (target == boxes.size() - 1 && dir == 1) {
          if (ofDistVec2f(boxes[0].getCenter(), imageViewport.getCenter()) < imageViewport.zero.width * 0.5) {
            ////cout << "kenburns: close enough, do a loop" << endl;
            dir = 1;
            target = -1;
          } else {
            ////cout << "kenburns: too far, do palindrome" << endl;
            dir = -1;
          }
          laps++;
        } else if (target == 0 && dir == -1) {
          dir = 1;
        }
        target += dir;
        prog = 0;
        lastRect = ofRectangle(rect);
        
        
      }
    }
  };

  class Recents {public:
    int maxRecents = 36;
    int cols = 6;
    ofRectangle rect;
    vector<GUI::IconButton*> buttons;
    int indexer = 0;
    int selected = -1;
    float scroll = 0;
    float scrollVelocity = 0;

    ofVec2f calcButtonCenter(int i) {
      float W = rect.width / (float)cols;
      int ii = (i < indexer) ? (i + maxRecents - indexer) : (i - indexer);
      //float x = rect.x + (float)(ii % cols) * W + W / 2;
      //float y = rect.y + (float)((int)ii / (int)cols) * W + W / 2;
      float x = scroll + rect.x - (maxRecents-cols)*W + ii * W + W / 2;
      float y = rect.y + fmin(W,rect.height) / 2;
      return ofVec2f(x, y);
    }

    void update(ofRectangle r) {
      rect = r;
      float W = rect.width / (float)cols;
      float w = W * 0.7;
      for (int i = 0; i < buttons.size(); i++) {

        if (i == selected) {
          float cw = virtualWindowWidth() * 0.7;
          float s = 0.1;
          float xx = ofLerp(buttons[i]->rect.x, virtualWindowWidth() / 2 - cw/2, s);
          float yy = ofLerp(buttons[i]->rect.y, virtualWindowHeight() / 2 - cw/2, s);
          float ww = ofLerp(buttons[i]->rect.width, cw, s);
          float hh = ofLerp(buttons[i]->rect.height, cw, s);

          buttons[i]->setRect(ofRectangle(xx, yy, ww, hh));

          buttons[i]->opacity = fmax(0,buttons[i]->opacity - 0.02);

        } else {
          ofVec2f c = calcButtonCenter(i);

          float v = 0.3;
          if (ofDistVec2f(c,buttons[i]->rect.getCenter()) > virtualWindowWidth()) {
            v = 1;
          }

          float xx = ofLerp(buttons[i]->rect.x, c.x - w / 2, v);
          float yy = ofLerp(buttons[i]->rect.y, c.y - w / 2, v);
          float ww = ofLerp(buttons[i]->rect.width, w, v);
          float hh = ofLerp(buttons[i]->rect.height, fmin(w,rect.height), v);

          buttons[i]->setRect(ofRectangle(xx, yy, ww, hh));
          buttons[i]->opacity = 1;
        }
      }
      scroll += scrollVelocity;
      scrollVelocity*=0.92;
      float smin = 0;
      float smax = fmax(smin,(buttons.size()*W - rect.width));
      if (scroll < smin) {
        scroll = ofLerp(scroll, smin, 0.1);
      }
      if (scroll > smax) {
        scroll = ofLerp(scroll, smax, 0.1);
      }
      
    }

    bool updateScroll(ofVec2f startPosition, ofVec2f lastPosition, ofVec2f position) {
      float W = rect.width / (float)cols;
      startPosition.x *= virtualWindowWidth();
      startPosition.y *= virtualWindowHeight();
      position.x *= virtualWindowWidth();
      position.y *= virtualWindowHeight();
      lastPosition.x *= virtualWindowWidth();
      lastPosition.y *= virtualWindowHeight();
      if (rect.y < startPosition.y && startPosition.y < rect.y+rect.height) {
        float dx = (position.x - lastPosition.x) * 2;
        scrollVelocity = dx;
       
        return true;
      }
      return false;
    }


    void add(int idx) {
      for (int i = 0; i < buttons.size(); i++) {
        if (buttons[i]->text == ofToString(idx)) {
          ////cout << "already added." << endl;
          
          int front = (indexer - 1 + maxRecents) % maxRecents;
          GUI::IconButton* that = buttons[i];
          if (front < i) {
            front += maxRecents;
          }
          for (int j = i; j < front; j++) {
            buttons[j%maxRecents] = buttons[(j + 1) % maxRecents];
          }
          buttons[front] = that;
          //int front = (indexer - 1 + maxRecents) % maxRecents;
          //cout << "swapping with " << front << endl;
          //GUI::IconButton* tmp = buttons[front];
          //buttons[front] = buttons[i];
          //buttons[i] = tmp;

          return;
          
        }
      }


      string boxname; string fname;
      std::tie(boxname, fname) = THP::filenames[idx];

      ////cout << "adding " << fname << " to recents..." << endl;
      string path = dataPaths["512"] + fname;
      if (THP::fileExists(path)) {
        int li = indexer;
        indexer = (indexer + 1) % maxRecents;

        if (li >= buttons.size()) {
          ////cout << "appending..." << endl;
          GUI::IconButton* btn = GUI::createElement<GUI::IconButton>("recent-btn-"+ofToString(rand()));
          
          buttons.push_back(btn);
        }
        buttons[li]->load(path);
        buttons[li]->text = ofToString(idx);
        ofVec2f c = calcButtonCenter(li);
        buttons[li]->setRect(ofRectangle(c.x,c.y,0,0));
         
        
        ////cout << "ok..." << endl;
      }
      update(rect);
    }






  };


  namespace Message {

    GUI::Label* label;
    
    int state = -1;
    int timer = 0;
    string currentText = "";
    int lineCnt = 0;
    float targetOpacity = 1;
    float yshift = 0;
    int lateTimer = 0;

    void updateText() {
      label->setText(currentText);
      label->computeAutoNewline();
      lineCnt = 1;
      for (int i = 0; i < label->text.size(); i++) {
        if (label->text[i] == '\n') {
          lineCnt++;
        }
      }
    }

    void trigger(string _text="") {
      state = 1;
      timer = 0;
      if (_text.size()) {
        currentText = _text;
        updateText();
      }
      ////cout << currentText << endl;
      ////cout << label->text << endl;
      label->style.color = ofColor(255, 255, 0);
      targetOpacity = 1;
      
    }

    void update(ofRectangle rect) {
      //cout << rect << endl;
      if (state == -1) {
        label = GUI::createElement<GUI::Label>("msg");
        state = 0;
      }
      //if (embeddingDescriptions.size() > targetEmbeddingIdx && embeddingDescriptions[targetEmbeddingIdx] != currentText) {
      //  currentText = embeddingDescriptions[targetEmbeddingIdx];
      //  updateText();
      //  state = 1;
      //  timer = 0;
      //  label->style.color = ofColor(255, 255, 0);
      //}
      if (state == 1) {
        if (timer > lateTimer) {
          label->opacity = ofLerp(label->opacity, targetOpacity, 0.1);
          yshift = ofLerp(yshift, 1, 0.1);
          label->style.color.lerp(ofColor(255), 0.05);
        }
        if (timer > 400) {
          state = 0;
        }
      } else {
        label->opacity = ofLerp(label->opacity, 0, 0.1);
        yshift = ofLerp(yshift, 0, 0.1);
      }
      float o = 0;
      if (label->text.find("\n") == string::npos && rect.y > virtualWindowHeight()/2) {
        o += 51;
      }

      label->setRect(ofRectangle(rect.x, rect.y - 50 + yshift*50 + o, rect.width, rect.height));

      //cout << label->text << " " << label->rect << endl;
      timer++;
    }
  };


  void loadFamous(Recents& r, string path) {
    vector<int> famous;
    ofBuffer buffer = ofBufferFromFile(path);
    if (buffer.size()) {
      for (ofBuffer::Line it = buffer.getLines().begin(), end = buffer.getLines().end(); it != end; ++it) {
        string line = *it;
        if (line.empty() == false) {
          ////cout << "you wanted famous image " << line << endl;
          for (int i = 0; i < filenames.size(); i++) {
            string boxname;
            string fname;
            std::tie(boxname, fname) = THP::filenames[i];
            if (fname == line) {
              ////cout << "you got it. it's index #" << i << endl;
              famous.push_back(i);
              break;
            }
          }
        }
      }
    }
    for (int i = 0; i < famous.size(); i++) {
      r.add(famous[i]);
    }
  }

  namespace Logger {
    string logdir = "data/log/";
    string dateStr() {
      time_t now = time(0);
      tm *ltm = localtime(&now);
      // history before 1900 doesn't exist, January is 0th month of a year
      return ofToString(1900 + ltm->tm_year) + "-" + ofToString(1 + ltm->tm_mon) + "-" + ofToString(ltm->tm_mday);
    }
    void logRaw(string s) {
      string fname = logdir + dateStr() + "-raw.txt";

      fstream f;
      ////cout << "trying to log " << s << " to " << fname << endl;
      f.open(fname.c_str(), std::fstream::in | std::fstream::out | std::fstream::app);
      if (!f) {
        f.open(fname.c_str(), fstream::in | fstream::out | fstream::trunc);
        f << "\n";
        f.close();
      } else {
        f << s + " ";
        f.close();
      }
    }

    vector<int> imageTime;
    int entryImage = -1;
    unsigned long entryTime;


    inline bool exists_test(const std::string& name) {
      ifstream f(name.c_str());
      bool b = f.good();
      f.close();
      return b;
    }

    void loadImageLog() {
      string fname = logdir + dateStr() + "-imview.txt";

      imageTime.clear();
      for (int i = 0; i < FACT1*FACT2; i++) {
        imageTime.push_back(0);
      }

      if (exists_test(fname)) {

        ofBuffer buffer = ofBufferFromFile("../"+fname);
        bool header = true;
        if (buffer.size()) {
          for (ofBuffer::Line it = buffer.getLines().begin(), end = buffer.getLines().end(); it != end; ++it) {
            string line = *it;
            if (line.empty() == false) {
              if (header) {
                header = false;
                continue;
              }
              int tab = line.find("\t");
              string bname = line.substr(0, tab);
              string rest = line.substr(tab + 1);
              tab = rest.find("\t");
              string fname = rest.substr(0, tab);

              int sec = stoi(rest.substr(tab + 1));
              for (int i = 0; i < THP::filenames.size(); i++) {
                string bboxname; string ffname;
                std::tie(bboxname, ffname) = THP::filenames[i];
                if (fname == ffname) {
                  imageTime[i] = sec;
                  break;
                }
              }
            }
          }
        }
      } 
      ////cout << "imagetimesize" << imageTime.size() << endl;
    }
    
    void saveImageLog() {
      string fname = logdir + dateStr() + "-imview.txt";
      fstream f;
      f.open(fname.c_str(), std::fstream::in | std::fstream::out | ::fstream::trunc);
      f << "boxname\tfilename\tsecs\n";
      for (int i = 0; i < imageTime.size(); i++) {
        if (imageTime[i] != 0) {
          string boxname; string fname;
          std::tie(boxname, fname) = THP::filenames[i];
          f << boxname << "\t" << fname << "\t" << imageTime[i] << "\n";
        }
      }
      f.close();
    }

    void imageIn(int idx) {
      entryImage = idx;
      entryTime = (unsigned long)time(NULL);
      ////cout << "L " << entryImage << ":" << entryTime << endl;
    }

    void imageOut() {
      if (entryImage != -1) {
        unsigned long dwell = ((unsigned long)time(NULL)) - entryTime;
        imageTime[entryImage] += dwell;
        ////cout << "L " << entryImage << ":" << dwell << endl;
      } else {
        ////cout << "useless call to imageOut " << endl;
      }
      entryImage = -1;
      saveImageLog();
    }

    vector<int> touchHist;

    void loadTouchLog() {
      string fname = logdir + dateStr() + "-touchhist.txt";

      touchHist.clear();
      for (int i = 0; i < 144; i++) {
        touchHist.push_back(0);
      }

      if (exists_test(fname)) {

        ofBuffer buffer = ofBufferFromFile("../" + fname);
        bool header = true;
        int i = 0;
        if (buffer.size()) {
          for (ofBuffer::Line it = buffer.getLines().begin(), end = buffer.getLines().end(); it != end; ++it) {
            string line = *it;
            if (line.empty() == false) {
              if (header) {
                header = false;
                continue;
              }
              int tab = line.find("\t");
              int n = stoi(line.substr(tab + 1));
              touchHist[i] = n;
              i++;
            }
          }
        }
      }
      ////cout << "touchhistsize" << touchHist.size() << endl;
    }

    void saveTouchLog() {
      string fname = logdir + dateStr() + "-touchhist.txt";
      fstream f;
      f.open(fname.c_str(), std::fstream::in | std::fstream::out | ::fstream::trunc);
      f << "time\t#touches\n";
      for (int i = 0; i < touchHist.size(); i++) {
        string hour = ofToString( (int)(i / 6) );
        string minu = ofToString((i % 6)*10);
        if (hour.size() < 2) {
          hour = "0" + hour;
        }
        if (minu.size() < 2) {
          minu = "0" + minu;
        }
        f << hour << ":" << minu << "\t" << touchHist[i] << "\n";
      }
      f.close();
    }

    void logTouch() {
      time_t now = time(0);
      tm *ltm = localtime(&now);
      int hour = ltm->tm_hour;
      int minu = ltm->tm_min;
      int i = hour * 6 + (int)(minu / 10);
      touchHist[i]++;
      saveTouchLog();
    }
  }

  namespace FaceMatch {
    typedef struct _match_t {
      int idx;
      int id0;
      int id1;
      ofRectangle roi0;
      ofRectangle roi1;
      float dist;
      //string rawdist;
    } match_t;
    vector<vector<match_t>> matches;
    vector<vector<ofRectangle>> allFaces;

    void load(string nn_path, string all_path) {
      allFaces = _loadFaces(all_path);

      ofBuffer buffer = ofBufferFromFile(nn_path);
      if (buffer.size()) {
        for (ofBuffer::Line it = buffer.getLines().begin(), end = buffer.getLines().end(); it != end; ++it) {
          string line = *it;
          vector <match_t> im;
          if (line.empty() == false) {
            line = line + "\t";
            string cash = "";
            int jj = 0;
            for (int i = 0; i < line.size(); i++) {
              if (line[i] == '\t' || line[i] == ',') {
                if (jj == 0) {
                  match_t match;
                  match.idx = stoi(cash);
                  cash = "";
                  im.push_back(match);
                } else if (jj == 1) {
                  im[im.size() - 1].roi0.x = stoi(cash);
                  im[im.size() - 1].id0 = stoi(cash);
                  cash = "";
                } else if (jj == 2) {
                  im[im.size() - 1].roi0.y = stoi(cash);
                  cash = "";
                } else if (jj == 3) {
                  im[im.size() - 1].roi0.width = stoi(cash);
                  cash = "";
                } else if (jj == 4) {
                  im[im.size() - 1].roi0.height = stoi(cash);
                  cash = "";
                } else if (jj == 5) {
                  im[im.size() - 1].roi1.x = stoi(cash);
                  im[im.size() - 1].id1 = stoi(cash);
                  cash = "";
                } else if (jj == 6) {
                  im[im.size() - 1].roi1.y = stoi(cash);
                  cash = "";
                } else if (jj == 7) {
                  im[im.size() - 1].roi1.width = stoi(cash);
                  cash = "";
                } else if (jj == 8) {
                  im[im.size() - 1].roi1.height = stoi(cash);
                  cash = "";
                } else if (jj == 9) {
                  //im[im.size() - 1].rawdist = cash;
                  im[im.size() - 1].dist = stof(cash);
                  cash = "";
                }
                jj++;
                if (jj == 10) {
                  jj = 0;
                }
              } else {
                cash += line[i];
              }
            }

          }
          matches.push_back(im);
        }

      }
    }
    int each = 6;
    int show = 3;
    int n_view = 0;

    int slots[18] = {
                    -1,-1,-1,-1,-1,-1,
                     -1,-1,-1,-1,-1,-1,
                     -1,-1,-1,-1,-1,-1 };
    ofRectangle rects[18];
    ofVec2f drags[18];

    THP::ImageLoadThread thread;
    bool threadStarted = false;
    std::deque<int> loadQueue;

    vector<ofTexture> textures;
    ofTexture srcTex;
    int currIdx = -1;

    int strw = 8;


    string jpgPathFromIndex(int idx) {

      string boxname; string fname;
      std::tie(boxname, fname) = THP::filenames[idx];
      string jpgname = "" + fname + "";
      ofStringReplace(jpgname, ".png", ".jpeg");
      ////cout << jpgname << endl;
      string path = "images/jpg640/" + jpgname;
      return path;
    }
    string fullPathFromIndex(int idx) {

      string boxname; string fname;
      std::tie(boxname, fname) = THP::filenames[idx];
      string path = dataPaths["1600"] + boxname + "/" + fname;
      return path;
    }

    void loadMatchImages(int idx) {

      if (!threadStarted) {
        thread.startThread();
        threadStarted = true;
      }
      if (currIdx != idx) {
        ofLoadImage(srcTex, fullPathFromIndex(idx), IMAGE_LOAD_GRAYSCALE);
        currIdx = idx;
      }
      vector<match_t> im = matches[idx];
      n_view = (int)fmin(each*show, im.size());

      for (int i = 0; i < n_view; i++) {
        if (i >= textures.size()) {
          ofTexture tex;
          textures.push_back(tex);
        }
        if (slots[i] == im[i].idx) {
          continue;
        }
        if (count(loadQueue.begin(), loadQueue.end(), im[i].idx)) {
          continue;
        }
        loadQueue.push_back(im[i].idx);
        slots[i] = im[i].idx;
        drags[i] = ofVec2f(0, 0);
      }
      if (thread.state == 0) {
        if (loadQueue.size()) {
          thread.load(loadQueue[0], fullPathFromIndex(loadQueue[0]));
        }
      } else if (thread.state == 1) {
        //wait;
      } else if (thread.state == 2) {
        ofPixels pix(thread.pixels);
        for (int i = 0; i < n_view; i++) {
          if (i >= textures.size()) {
            ofTexture tex;
            textures.push_back(tex);
          }
          if (slots[i] == thread.id) {
            textures[i].allocate(pix);
          }
        }

        if (loadQueue.size()) {
          loadQueue.pop_front();
          thread.load(loadQueue[0], fullPathFromIndex(loadQueue[0]));
        }

      }
    }
    ofRectangle calcRoi(float lw, ofRectangle roi1) {
      //ofSetColor(255, 0, 0);
             //ofDrawRectangle(x, y, w, w);
      ofRectangle roi2(
        roi1.x * lw / 800,
        roi1.y * lw / 800,
        roi1.width * lw / 800,
        roi1.height * lw / 800
      );

      ofRectangle roi3(
        roi2.x - roi2.width,
        roi2.y + roi2.height / 2 - roi2.width / 2 - roi2.width,
        roi2.width * 3,
        roi2.height * 3
      );

      return roi3;
    }

    ofColor colors[4] = { ofColor(100,143,255), ofColor(254,97,0), ofColor(255,202,0)};

    void drawSrcRoi(ofRectangle roi0, ofColor color, int sw = strw) {
      float s = max(imageTexture.getWidth(), imageTexture.getHeight()) / 800.0;
      float x = roi0.x * s;
      float y = roi0.y * s;
      float w = roi0.width * s;
      float h = roi0.height * s;
      ofPushStyle();
      ofPushMatrix();
      ofTranslate((virtualWindowWidth() - (float)MAPWIN_W) / 2, (virtualWindowHeight() - (float)MAPWIN_H) / 2);
      ofScale((float)MAPWIN_W / imageViewport.width, (float)MAPWIN_H / imageViewport.height);
      //ofTranslate((virtualWindowWidth() - (float)MAPWIN_W) / 2, (virtualWindowHeight() - (float)MAPWIN_H) / 2);
      ofTranslate(-imageViewport.x, -imageViewport.y);

      if (sw != 1) {
        ofSetColor(0, 0, 0);
        ofDrawRectangleOutline(ofRectangle(x - w * 0.1, y - h * 0.1, w*1.2, h*1.2), (float)(strw + 2+2*HALF) / ((float)MAPWIN_W / imageViewport.width));
        ofSetColor(color);
        ofDrawRectangleOutline(ofRectangle(x - w * 0.1, y - h * 0.1, w*1.2, h*1.2), (float)(strw) / ((float)MAPWIN_W / imageViewport.width));
      } else {
        ofSetColor(color);
        ofNoFill();
        ofSetLineWidth(sw);
        ofDrawRectangle(x, y, w, h);
      }

      ofPopMatrix();
      ofPopStyle();
    }
    ofVec2f touch0;
    ofVec2f touch;
    void sendTouch(ofVec2f p) {
      touch = p;
    }
    void noTouch() {
      touch0.x = -1;
      touch0.y = -1;
      touch.x = -1;
      touch.y = -1;
    }

    int detail = -1;
    int wasDetail = -1;
    int activate = 0;
    float detailFadeAlpha = 0;
    GUI::Label* disclaim;
    bool disclaimCreated = false;
    ofTrueTypeFont font;
    ofTrueTypeFont smallfont;

    void draw(int idx, float top, bool on) {
      if (!disclaimCreated) {
        disclaim = GUI::createElement<GUI::Label>("disclaim");
        disclaimCreated = true;
        font.load("fonts/frabk.ttf", 54);
        smallfont.load("fonts/frabk.ttf", 13);
      }
      ofPushStyle();
      ofSetColor(30, 30, 30, fmin(255, (virtualWindowHeight() - top) / 5));
      ofDrawRectangle(0, top, virtualWindowWidth(), virtualWindowHeight());
      ofPopStyle();

      if (n_view != 0) {
        font.drawString("Best-Matching Faces", 50, top + 100);
        disclaim->setText("Below are the best-matching faces (within the Teenie Harris Archive) for some people in the image above. Tap on any face surrounded by a thin white square to show its matches below. You can also double-tap the faces below to jump to that image. Face recognition is an experimental feature, and these results may vary widely in accuracy.");
      } else {
        font.drawString("No Matchable Faces", 50, top + 100);
        disclaim->setText("Our system can't find any faces in this image. If there are faces, they may be too small, too blurry, too poorly lit, too occluded, or too poorly oriented to be detected. ");
      }
      
      disclaim->computeAutoNewline();
      disclaim->setRect(ofRectangle(50, top + 135, virtualWindowWidth() - 350, 50));

      top += 300;

      loadMatchImages(idx);

      vector<match_t> im = matches[idx];
      int left = 30;
      int h = 370;
      int w = (virtualWindowWidth() - left * 2 - h) / 6;
      int pd = 20;
      ofPushStyle();
      ofSetColor(255, 255, 255);
      int row = 0;
      int col = 0;
      int rowid = 0;
      bool first;

      if (on) {
        for (int i = 0; i < allFaces[idx].size(); i++) {
          ofPushStyle();
          ofPushMatrix();
          ofTranslate((virtualWindowWidth() - (float)MAPWIN_W) / 2, (virtualWindowHeight() - (float)MAPWIN_H) / 2);
          ofScale((float)MAPWIN_W / imageViewport.width, (float)MAPWIN_H / imageViewport.height);
          ofTranslate(-imageViewport.x, -imageViewport.y);
          ofSetColor(255, 255, 255);
          ofRectangle r(allFaces[idx][i]);
          r.x *= imageTexture.getWidth();
          r.y *= imageTexture.getHeight();
          r.width *= imageTexture.getWidth();
          r.height *= imageTexture.getHeight();

          bool gotit = false;
          for (int j = 0; j < im.size(); j++) {
            ofRectangle r0 = im[j].roi0;
            float s = max(imageTexture.getWidth(), imageTexture.getHeight()) / 800.0;
            float x = r0.x * s;
            float y = r0.y * s;
            float w = r0.width * s;
            float h = r0.height * s;
            if (ofDistVec2f(ofVec2f(x,y),r.getTopLeft())<20 && ofDistVec2f(ofVec2f(x+w, y+h), r.getBottomRight())<20) {
              gotit = true;
              break;
            }
          }
          if (!gotit) {


            //ofDrawRectangle(r);
            glPushAttrib(GL_ENABLE_BIT);
            glLineStipple(2, 0xAAAA);
            glEnable(GL_LINE_STIPPLE);
            glBegin(GL_LINES);

            glVertex3f(r.x, r.y, 0);
            glVertex3f(r.x + r.width, r.y, 0);
            glVertex3f(r.x + r.width, r.y, 0);
            glVertex3f(r.x + r.width, r.y + r.height, 0);
            glVertex3f(r.x + r.width, r.y + r.height, 0);
            glVertex3f(r.x, r.y + r.height, 0);
            glVertex3f(r.x, r.y + r.height, 0);
            glVertex3f(r.x, r.y, 0);

            glEnd();
            glPopAttrib();
          }
          ofPopStyle();
          ofPopMatrix();
        }
      }

      float slw = fmax(srcTex.getWidth(), srcTex.getHeight());
      for (int i = n_view; i < im.size(); i++) {
        if (on) {
          drawSrcRoi(im[i].roi0, ofColor(250, 250, 250), 1);
        }
      }
      for (int i = 0; i < n_view; i++) {
        if (i >= textures.size() || !textures[i].isAllocated()) {
          continue; // still loading
        }
        first = false;
        if (rowid == 0) {
          rowid = im[i].id0;
          first = true;

        } else if (rowid != im[i].id0) {
          row++;
          rowid = im[i].id0;
          col = 0;
          first = true;

        } else {
          col++;
          if (col >= 6) {
            col = 100;
          }
        }
        if (first) {
          ofRectangle r(left + pd, h*row + pd + top, h - pd * 2, h - pd * 2);
          srcTex.drawSubsection(r, calcRoi(slw, im[i].roi0));
          ofPushStyle();
          ofSetColor(0, 0, 0);
          ofDrawRectangleOutline(r, strw + 2);
          ofSetColor(colors[row]);
          //ofNoFill();
          //ofSetLineWidth(5);
          ofDrawRectangleOutline(r, strw);
          ofPopStyle();

          if (on) {
            drawSrcRoi(im[i].roi0, colors[row]);
          }
        }
        int x = col * w + h;
        int y = row * h;
        ofRectangle roi1 = im[i].roi1;
        float lw = fmax(textures[i].getWidth(), textures[i].getHeight());

        //cout << roi1 << endl;
        rects[i] = ofRectangle(left + x + pd, y + pd + top, (w - pd * 2), (w - pd * 2));

        ofRectangle sr = calcRoi(lw, im[i].roi1);
        sr.x += drags[i].x;
        sr.y += drags[i].y;
        if (sr.x < 0) {
          sr.x = 0;
        }
        if (sr.y < 0) {
          sr.y = 0;
        }
        if (sr.x + sr.width > textures[i].getWidth()) {
          sr.x = textures[i].getWidth() - sr.width;
        }
        if (sr.y + sr.height > textures[i].getHeight()) {
          sr.y = textures[i].getHeight() - sr.height;
        }
        if (detail == i && fabs(touch0.x - touch.x) < 5000 && touch0.x > 0 && touch.x > 0) {
          drags[i] = ofLerpVec2f(drags[i], touch0 - touch, 0.2);
        } else {
          drags[i] = ofLerpVec2f(drags[i], ofVec2f(0, 0), 0.2);
        }

        textures[i].drawSubsection(rects[i], sr);

        //ofPushStyle();
        //ofSetColor(50);
        //ofDrawRectangle(rects[i].x, rects[i].y + rects[i].height + 15, rects[i].width, 15);
        //ofSetColor(128);
        //ofDrawRectangle(rects[i].x, rects[i].y + rects[i].height + 15, rects[i].width*(1 - im[i].dist), 15);
        //ofPopStyle();
        string desc, date, acc;
        std::tie(desc,date,acc) = descriptions[im[i].idx];

        ofPushStyle();
        ofSetColor(192);
        smallfont.drawString("ID#: "+ofToString(acc), rects[i].x, rects[i].y + rects[i].height + 22);
        smallfont.drawString("Match: "+ofToString((float)(int)(pow(1-im[i].dist,3)*1000)/10.0)+"%",rects[i].x, rects[i].y + rects[i].height + 41);
        //textures[i].draw(x,y,w,ofLoadImage(textures[i], jpgPathFromIndex(im[i].idx), IMAGE_LOAD_GRAYSCALE);w);
        ofPopStyle();
      }

      ofPushStyle();
      ofSetColor(0, 0, 0, detailFadeAlpha*0.8);
      //ofDrawRectangle(0, top, virtualWindowWidth(), virtualWindowHeight());
      if (detail == -1) {
        detailFadeAlpha = ofLerp(detailFadeAlpha, 0, 0.1);
      }

      ofPopStyle();

      ofPopStyle();

      wasDetail = detail;
    }


    int envictRow = 0;
    int checkTap(ofVec2f p) {

      touch0 = ofVec2f(p);
      touch = ofVec2f(p);
      vector<match_t> im = matches[currIdx];
      for (int i = 0; i < im.size(); i += each) {
        float s = max(imageTexture.getWidth(), imageTexture.getHeight()) / 800.0;
        float x0 = im[i].roi0.x * s;
        float y0 = im[i].roi0.y * s;
        float x1 = x0 + im[i].roi0.width * s;
        float y1 = y0 + im[i].roi0.height * s;
        ofVec2f p0 = THP::imageMatrixToScreenMatrix(ofVec2f(x0, y0));
        ofVec2f p1 = THP::imageMatrixToScreenMatrix(ofVec2f(x1, y1));
        ofRectangle r(p0.x, p0.y, p1.x - p0.x, p1.y - p0.y);

 
        if (r.inside(p)) {
          if (i >= n_view) {
            ////cout << "sel face " << i << " rep row " << envictRow << endl;
            vector<match_t> tmp;
            for (int j = 0; j < im.size(); j++) {
              tmp.push_back(im[j]);
            }
            for (int j = envictRow * each; j < envictRow*each + each; j++) {
              matches[currIdx][j] = tmp[i + j - envictRow * each];
            }
            for (int j = i; j < i + each; j++) {
              matches[currIdx][j] = tmp[j - i + envictRow * each];
            }
            envictRow = (envictRow + 1) % show;
            
          }
          return -2;
        }
      }
      for (int i = 0; i < n_view; i++) {
        if (rects[i].inside(p.x, p.y)) {
          if (detail == i) {
            detail = -1;
            return i;
          }
          detail = i;
          return i;
        }
      }
      detail = -1;
      return -1;
    }

  }


  namespace LabelEmbedding {
    vector<vector<tuple<ofVec2f,string>>> labels;

    ofTrueTypeFont font;
    bool fontLoaded = false;

    void loadEmpty() {
      vector<tuple<ofVec2f, string>> row;
      row.clear();
      labels.push_back(row);
    }

    void load(string path, ofVec2f offset) {
      vector<tuple<ofVec2f, string>> row;

      ofBuffer buffer = ofBufferFromFile(path);
      if (buffer.size()) {
        for (ofBuffer::Line it = buffer.getLines().begin(), end = buffer.getLines().end(); it != end; ++it) {
          string line = *it;
          if (line.empty() == false) {
            int tab = line.find("\t");
            float x = stof(line.substr(0, tab));
            string stuff = line.substr(tab + 1);
            tab = stuff.find("\t");
            float y = stof(stuff.substr(0, tab));
            string s = stuff.substr(tab + 1);
            tuple<ofVec2f, string> t = make_tuple(ofVec2f(offset.x+x,offset.y+y),s);
            row.push_back(t);
          }
        }
      }
      labels.push_back(row);

    }

    void draw() {
      int idx = targetEmbeddingIdx;
      if (!fontLoaded) {
        font.load("fonts/frabk-hacked.otf", 30);
        fontLoaded = true;
      }
      float alp = ofMap(viewport.width / viewport.zero.width, 0.8, 1, 0, 1, true);
      alp = fmin(alp, ofMap(beenLerpingEmbedding, 0, 300, 0, 1, true));

      if (alp < 0.05) {
        return;
      }

      ofPushMatrix();
      
      ofTranslate((virtualWindowWidth() - (float)MAPWIN_W) / 2, (virtualWindowHeight() - (float)MAPWIN_H) / 2);
      ofScale((float)MAPWIN_W / viewport.width, (float)MAPWIN_H / viewport.height);
      ofTranslate(-viewport.x, -viewport.y);
      
      //ofScale(0.5);
      //ofTranslate(-FACT2 / 2, -FACT1 / 2);
      //cout << viewport.zero << endl;
      ofPushStyle();


      for (int i = 0; i < labels[idx].size(); i++) {
        ofVec2f p; string txt;
        tie(p, txt) = labels[idx][i];
        vector<string> lines = ofSplitString(txt, "$");
        ofPushMatrix();
        ofTranslate(p.x, p.y);
        ofScale(0.1);
        
        for (int j = 0; j < lines.size(); j++) {
          ofSetColor(0,alp*255*0.8);
          float s = 5;
          font.drawString(lines[j], -s, j * 40-s);
          font.drawString(lines[j], -s, j * 40);
          font.drawString(lines[j], -s, j * 40+s);

          //font.drawString(lines[j], 0, j * 40-s);
          //font.drawString(lines[j], 0, j * 40+s);

          font.drawString(lines[j], s, j * 40-s);
          font.drawString(lines[j], s, j * 40);
          font.drawString(lines[j], s, j * 40+s);
          ofSetColor(255,alp*255);
          font.drawString(lines[j], 0, j * 40);
        }
        ofPopMatrix();
      }
      ofPopStyle();
      ofPopMatrix();
    }


  };



}




