/*
 - switch from single mesh draw to geometry shader draw to control size separately
 
 - use colors more effectively
 - cycle through different colors
 - cycle through different embeddings
 - cycle through different images
 - change blending modes
 - show the name of current image
 - show full images on full zoom
 */

#include "ofMain.h"
#include "Camera2d.h"
#include "cnpy.h"

class ZoomableRegion2D {
private:
    float speed = 5;
    float zoom = 1;
    ofVec2f offset, startOffset, startDrag;
    float startZoom;
    float width, height;
    bool hasZoomed = false;
public:
    ZoomableRegion2D() {
        ofAddListener(ofEvents().mouseDragged , this, &ZoomableRegion2D::mouseDragged);
        ofAddListener(ofEvents().mousePressed, this, &ZoomableRegion2D::mousePressed);
        ofAddListener(ofEvents().draw, this, &ZoomableRegion2D::draw);
    }
    ofVec2f unapply(ofVec2f x) {
        return (x / zoom) - offset;
    }
    void draw(ofEventArgs& args) {
        if (!hasZoomed) {
            ofDrawBitmapStringHighlight("shift+drag to zoom", ofGetWidth() / 2, ofGetHeight() / 2);
        }
    }
    void begin(float width, float height) {
        this->width = width, this->height = height;
        float x = ofGetMouseX();
        ofPushMatrix();
        ofScale(zoom, zoom);
        ofTranslate(offset);
    }
    void end() {
        ofPopMatrix();
    }
    void mouseDragged(ofMouseEventArgs& args) {
        ofVec2f diff = args - startDrag;
        offset = startOffset;
        if(ofGetKeyPressed(OF_KEY_SHIFT)) {
//        if(args.button == OF_MOUSE_BUTTON_RIGHT) {
            hasZoomed = true;
            zoom = startZoom * (1 + (exp((speed * diff.y) / height) - 1));
            zoom = MAX(zoom, 1);
            offset -= (startDrag) * (1 - startZoom) / startZoom;
            offset += (startDrag) * (1 - zoom) / zoom;
        } else {
            offset += diff / zoom;
        }
        offset.x = ofClamp(offset.x, width * (1 - zoom) / zoom, 0);
        offset.y = ofClamp(offset.y, height * (1 - zoom) / zoom, 0);
    }
    void mousePressed(ofMouseEventArgs& args) {
        startDrag = args;
        startZoom = zoom;
        startOffset = offset;
    }
};

template <class T>
inline const void at(T& x, const cnpy::NpyArray& arr, unsigned int i, unsigned int j) {
    const T* data = arr.data<T>();
    bool fortran = arr.fortran_order;
    if (fortran) {
        size_t n = arr.shape[0];
        x = data[i+(j*n)];
    } else {
        size_t n = arr.shape[1];
        x = data[(i*n)+j];
    }
}

template <class T>
inline const void at(T& x, const cnpy::NpyArray& arr, unsigned int i, unsigned int j, unsigned int k) {
    const T* data = arr.data<T>();
    x = data[i * arr.shape[2] * arr.shape[1] +
             j * arr.shape[2] +
             k];
}

class SpriteSheet {
private:
    int side, rows, cols;
public:
    ofImage img;
    ofMesh mesh;
    void load(string filename) {
        cnpy::NpyArray data = cnpy::npy_load(ofToDataPath(filename));
        int npn = data.shape[0];
        side = sqrt(npn);
        int w = side, h = side + 1;
        rows = data.shape[1];
        cols = data.shape[2];
        h *= rows;
        w *= cols;
        img.allocate(w, h, OF_IMAGE_GRAYSCALE);
        unsigned char* pix = img.getPixels().getData();
        for(int npi = 0; npi < npn; npi++) {
            int baserow = npi / side;
            int basecol = npi % side;
            baserow *= rows;
            basecol *= cols;
            for(int npj = 0; npj < rows; npj++) {
                for(int npk = 0; npk < cols; npk++) {
                    int row = baserow + npj;
                    int col = basecol + npk;
                    int ii = row * w + col;
                    at(pix[ii], data, npi, npj, npk);
                }
            }
        }
        img.update();
//        img.getTexture().setTextureMinMagFilter(GL_NEAREST, GL_NEAREST);
        
        mesh.setMode(OF_PRIMITIVE_TRIANGLES);
        mesh.addVertex(ofVec3f(0, 0));
        mesh.addVertex(ofVec3f(1, 0));
        mesh.addVertex(ofVec3f(1, 1));
        mesh.addVertex(ofVec3f(0, 1));
        mesh.addIndex(0);
        mesh.addIndex(1);
        mesh.addIndex(2);
        mesh.addIndex(0);
        mesh.addIndex(2);
        mesh.addIndex(3);
        mesh.addTexCoord(ofVec2f(0, 0));
        mesh.addTexCoord(ofVec2f(0, 0));
        mesh.addTexCoord(ofVec2f(0, 0));
        mesh.addTexCoord(ofVec2f(0, 0));
        
        cout << "Loaded sprites: " << ofToString(data.shape) << endl;
    }
    void addRect(ofMesh& m, int i, float x, float y, float w, float h) {
        int n = m.getNumVertices();
        
        m.addVertex(glm::vec3(x, y, 0));
        m.addVertex(glm::vec3(x + w, y, 0));
        m.addVertex(glm::vec3(x + w, y + h, 0));
        m.addVertex(glm::vec3(x, y + h, 0));
        
        glm::vec2 nw((i % side) * cols, (i / side) * rows);
        glm::vec2 se = nw + glm::vec2(cols, rows);
        nw += 0.5;
        se -= 0.5;
        m.addTexCoord(nw);
        m.addTexCoord(glm::vec2(se.x, nw.y));
        m.addTexCoord(se);
        m.addTexCoord(glm::vec2(nw.x, se.y));
        
        m.addIndex(n+0);
        m.addIndex(n+1);
        m.addIndex(n+2);
        m.addIndex(n+0);
        m.addIndex(n+2);
        m.addIndex(n+3);
    }
    void draw(int i, float x, float y, float w, float h) {
        vector<glm::vec3>& v = mesh.getVertices();
        v[0].x = x;
        v[0].y = y;
        v[1] = v[0];
        v[1].x += w;
        v[2] = v[0];
        v[2].x += w;
        v[2].y += h;
        v[3] = v[0];
        v[3].y += h;
        
        vector<glm::vec2>& t = mesh.getTexCoords();
        t[0].x = (i % side) * cols + 0.5;
        t[0].y = (i / side) * rows + 0.5;
        t[1] = t[0];
        t[1].x += cols - 1;
        t[2] = t[0];
        t[2].x += cols - 1;
        t[2].y += rows - 1;
        t[3] = t[0];
        t[3].y += rows - 1;
        
        mesh.draw();
    }
};

class ofApp : public ofBaseApp {
public:
//    Camera2d region;
    ZoomableRegion2D region;
    ofMesh mesh, texMesh;
    SpriteSheet sprites;
    
    void setup() {
        ofBackground(0);
        ofSetFrameRate(60);
        ofSetColor(255);
        
        //        region.setup();
        
//        sprites.load("photos/openface-ellipses/images.npy");
//        cnpy::NpyArray embedding = cnpy::npy_load(ofToDataPath("photos/openface-ellipses/umap/0.100_05.npy"));
//        cnpy::NpyArray colors = cnpy::npy_load(ofToDataPath("photos/openface-ellipses/colors.npy"));
        
//        sprites.load("photos/images.npy");
//        cnpy::NpyArray embedding = cnpy::npy_load(ofToDataPath("photos/assignment.npy"));
//        cnpy::NpyArray colors = cnpy::npy_load(ofToDataPath("photos/depth/colors.npy"));
        
        sprites.load("faces/images.npy");
//        cnpy::NpyArray embedding = cnpy::npy_load(ofToDataPath("faces/openface-descriptors/umap/0.100_03-assignment.npy"));
        cnpy::NpyArray embedding = cnpy::npy_load(ofToDataPath("faces/openface-descriptors/umap/0.100_05.npy"));
        cnpy::NpyArray colors = cnpy::npy_load(ofToDataPath("faces/openface-descriptors/colors.npy"));
        
        cout << "Loaded embedding: " << ofToString(embedding.shape) << endl;
        cout << "Loaded colors: " << ofToString(colors.shape) << endl;
        

        int n = embedding.shape[0];
        for(int i = 0; i < n; i++) {
            float x, y; // if this type does not match the npy type, there will be an issue
            at(x, embedding, i, 0);
            at(y, embedding, i, 1);
            mesh.addVertex(glm::vec3(x, y, 0));
            
            unsigned char r, g, b;
            at(r, colors, i, 0);
            at(g, colors, i, 1);
            at(b, colors, i, 2);
            mesh.addColor(ofColor(r, g, b));
        }
        mesh.setMode(OF_PRIMITIVE_POINTS);
        
        texMesh.setMode(OF_PRIMITIVE_TRIANGLES);
        auto& v = mesh.getVertices();
//        float wh = 0.004; // good for photo grid
        float wh = 0.002; // good for photo grid
        for(int i = 0; i < v.size(); i++) {
            sprites.addRect(texMesh, i, v[i].x, v[i].y, wh, wh);
        }
    }
    void update() {
    }
    void draw() {
        //        region.begin();
        region.begin(ofGetWidth(), ofGetHeight());
        
        ofTranslate(ofGetWidth() / 2, ofGetHeight() / 2);
        
        ofScale(50, 50); // good for point cloud
//        ofScale(500, 500); // good for assignment
        
//        ofEnableBlendMode(OF_BLENDMODE_ADD);
        
        sprites.img.bind();
        texMesh.draw();
        sprites.img.unbind();
        
        ofEnableBlendMode(OF_BLENDMODE_ALPHA);
        mesh.draw();
        
        region.end();
        
        ofDrawBitmapString(ofToString(int(round(ofGetFrameRate()))), 10, ofGetHeight() - 40);
    }
};

int main() {
    ofSetupOpenGL(1024, 1024, OF_FULLSCREEN);
    ofRunApp(new ofApp());
}
