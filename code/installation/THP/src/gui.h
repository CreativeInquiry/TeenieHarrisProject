#pragma once

#define SMALLFONT_MAXSIZE (9/(HALF+1))
#define FONT_MAXSIZE (42/(HALF+1))
//#include <clocale>
//#include <locale>
namespace GUI {

  ofTrueTypeFont font;
  ofTrueTypeFont smallfont;

  bool initialized = false;



  void setup() {
    //std::setlocale(LC_ALL, "");
    //ofTrueTypeFontSettings settings("fonts/frabk.ttf", FONT_MAXSIZE);
    //settings.addRanges({
    //  ofUnicode::AdditionalEmoticons,
    //  ofUnicode::AdditionalTransportAndMap,
    //  ofUnicode::AlphabeticPresentationForms,
    //  ofUnicode::Arabic,
    //  ofUnicode::ArabicExtendedA,
    //  ofUnicode::ArabicMath,
    //  ofUnicode::ArabicPresFormsA,
    //  ofUnicode::ArabicPresFormsB,
    //  ofUnicode::ArabicSupplement,
    //  ofUnicode::Arrows,
    //  ofUnicode::BlockElement,
    //  ofUnicode::BoxDrawing,
    //  ofUnicode::CJKLettersAndMonths,
    //  ofUnicode::CJKUnified,
    //  ofUnicode::CurrencySymbols,
    //  ofUnicode::Cyrillic,
    //  ofUnicode::Devanagari,
    //  ofUnicode::DevanagariExtended,
    //  ofUnicode::Dingbats,
    //  ofUnicode::Emoticons,
    //  ofUnicode::EnclosedCharacters,
    //  ofUnicode::GeneralPunctuation,
    //  ofUnicode::GeometricShapes,
    //  ofUnicode::Greek,
    //  ofUnicode::GreekExtended,
    //  ofUnicode::HangulCompatJamo,
    //  ofUnicode::HangulExtendedA,
    //  ofUnicode::HangulExtendedB,
    //  ofUnicode::HangulJamo,
    //  ofUnicode::HangulSyllables,
    //  ofUnicode::Hiragana,
    //  ofUnicode::IdeographicSpace,
    //  ofUnicode::KanaSupplement,
    //  ofUnicode::Katakana,
    //  ofUnicode::KatakanaHalfAndFullwidthForms,
    //  ofUnicode::KatakanaPhoneticExtensions,
    //  ofUnicode::Latin,
    //  ofUnicode::Latin1Supplement,
    //  ofUnicode::LetterLikeSymbols,
    //  ofUnicode::MathOperators,
    //  ofUnicode::MiscSymbols,
    //  ofUnicode::MiscSymbolsAndPictographs,
    //  ofUnicode::MiscTechnical,
    //  ofUnicode::NumberForms,
    //  ofUnicode::OtherAdditionalSymbols,
    //  ofUnicode::RumiNumericalSymbols,
    //  ofUnicode::Space,
    //  ofUnicode::SuperAndSubScripts,
    //  ofUnicode::TransportAndMap,
    //  ofUnicode::Uncategorized,
    //  ofUnicode::VedicExtensions
    //});

    //font.load(settings);

    font.load("fonts/frabk-hacked.otf", FONT_MAXSIZE);
    smallfont.load("fonts/frabk.ttf", SMALLFONT_MAXSIZE);
    initialized = true;
  }

  struct Stylesheet {
    ofColor background = ofColor(0, 0, 0, 200);
    ofColor activeBackground = ofColor(255, 255, 255, 200);
    ofColor labelBackground = ofColor(0, 0, 0, 250);
    ofColor color = ofColor(255, 255, 255);
    ofColor activeColor = ofColor(0, 0, 0);
    ofColor borderColor = ofColor(255, 255, 255);
    float borderWidth = 2;
    float lineHeight = 0.5;
  };

  class Element {public:
    Stylesheet style;
    string text;
    ofRectangle rect;
    string id;
    float opacity = 1;
    int layer = 0;
    bool clickThrough = false;

    Element(){
      if (!initialized) {
        setup();
      }
    }
    void setRect(ofRectangle r) {
      rect = ofRectangle(r.x,r.y,r.width,r.height);
    }
    ofVec2f calculateTextDimension() {
      float h = rect.height * style.lineHeight;
      float w = font.stringWidth(text)*h / (float)FONT_MAXSIZE;
      return ofVec2f(w, h);
    }
    virtual bool check(ofVec2f p) { return false; }
    virtual void draw() { }
  };

  class Label : public Element {public:
    string _rawtext = "";
    float maxHeight = INT_MAX;

    void setText(string _text) {
      text = _text;
      _rawtext = _text;
    }

    void computeAutoNewline() {
      text = _rawtext;
      float h = rect.height * style.lineHeight;

      int maxl = maxHeight / (font.getLineHeight()*h / (float)FONT_MAXSIZE);

      string out = "";
      string word = "";
      float lnw = 0;
      int nl = 1;
      for (int i = 0; i < text.size(); i++) {
        if (text[i] == '\n') {
          float w = font.stringWidth(word)*h / (float)FONT_MAXSIZE;
          if (lnw + w > rect.width) {
            nl++;
            if (nl > maxl) {
              out = out.substr(0, out.size()-3) + "...";
              word = "";
              break;
            }
            out += "\n" + word;
            nl++;
            if (nl > maxl) {
              out = out.substr(0, out.size() -3) + "...";
              word = "";
              break;
            }
            nl++;
            word = "";
            lnw = 0;
          } else {
            nl++;
            if (nl > maxl) {
              out = out.substr(0, out.size() -3) + "...";
              word = "";
              break;
            }
            out += word + "\n";
            word = "";
            lnw = 0;
          }
        } else if (text[i] == ' ') {
          float w = font.stringWidth(word+' ')*h / (float)FONT_MAXSIZE;
          if (lnw + w > rect.width) {
            nl++;
            if (nl > maxl) {
              out = out.substr(0, out.size() -3) + "...";
              word = "";
              break;
            }
            out += "\n" + word + " ";
            word = "";
            lnw = w;
            nl++;
          } else {
            out += word + ' ';
            word = "";
            lnw += w;
          }
        } else {
          word += text[i];
        }
      }
      if (word.size()) {
        float w = font.stringWidth(word)*h / (float)FONT_MAXSIZE;
        if (lnw + w > rect.width) {
          out += "\n"+word;
        } else {
          out += word;
        }
      }
      text = out;
    }
    void draw(){
      ofVec2f wh = calculateTextDimension();
      ofPushStyle();
      ofSetColor(style.color.r,style.color.g,style.color.b,opacity*255);
      ofPushMatrix();
      ofTranslate(rect.x, rect.y + rect.height / 2 + wh.y / 2);
      if (wh.y <= SMALLFONT_MAXSIZE) {
        ofScale(wh.y / (float)SMALLFONT_MAXSIZE);
        smallfont.drawString(text,0,0);
      } else {
        ofScale(wh.y / (float)FONT_MAXSIZE);
        font.drawString(text, 0, 0);
      }
      ofPopMatrix();
      ofPopStyle();
    }
  
  
  };

  class Icon : public Element{public:
    ofTexture texture;

    void load(string path="") {
      if (!path.size()) {
        path = id;
      }
      ofLoadImage(texture, path, IMAGE_LOAD_GRAYSCALE);
    }
    void draw() {
      if (!texture.isAllocated()) {
        load();
      }
      // cout << id << rect << endl;
      ofPushStyle();
      ofSetColor(255, 255, 255, 255 * opacity);
      texture.draw(rect.x, rect.y, rect.width, rect.height);
      ofPopStyle();
    }
    bool check(ofVec2f p) {
      float w = fmin(rect.width, rect.height)*0.1;
      ofRectangle extendRect(rect.x - w, rect.y - w, rect.width + w * 2, rect.height + w * 2);
      if (extendRect.inside(p.x, p.y)) {
        return true;
      }
      return false;
    }
  };

  class Shield : public Element {public:
    bool debug = false;
    bool check(ofVec2f p) {
      if (rect.inside(p.x, p.y)) {
        return true;
      }
      return false;
    }
    void draw() {
      if (debug) {
        ofPushStyle();
        ofSetColor(ofRandom(255),0,0);
        ofDrawRectangle(rect);
        ofPopStyle();
      }
    }
  };

  class InteractiveElement : public Element {public:
    int active = 0;

    bool check(ofVec2f p) {
      float w = fmin(rect.width,rect.height)*0.1;
      ofRectangle extendRect(rect.x-w, rect.y-w, rect.width+w*2, rect.height+w*2);
      if (extendRect.inside(p.x,p.y)) {
        return true;
      }
      return false;
    }
    void draw(){
      Element::draw();
      ofPushStyle();
      ofColor bg = active ? style.activeBackground : style.background;
      ofSetColor( bg, bg.a*opacity);
      ofDrawRectangle(rect);
      //ofNoFill();
      //ofSetLineWidth(style.borderWidth);
      ofSetColor(style.borderColor, opacity*255);
      //ofDrawRectangle(rect);
      ofDrawRectangleOutline(rect,style.borderWidth,0,true);
      ofPopStyle();


    }
  };
  class Button : public InteractiveElement{ public:
    int activeTimeout = 20;
    string iconPath = "";
    string altIconPath = "";
    string altIconPath2 = "";
    string altIconPath3 = "";
    ofTexture iconTexture;
    ofTexture altIconTexture;
    ofTexture altIconTexture2;
    ofTexture altIconTexture3;
    int useAltTexture = 0;
    Button(){
      InteractiveElement();
      text = "_auto_from_id";
    }
    bool check(ofVec2f p) {
      bool b = InteractiveElement::check(p);
      if (b) {
        active = activeTimeout;
      }
      return b;
    }

    void draw(){
      if (text == "_auto_from_id") {
        text = id;
      }
      if (iconPath.size() && !iconTexture.isAllocated()) {
        ofLoadImage(iconTexture, iconPath, IMAGE_LOAD_GRAYSCALE);
      }
      if (altIconPath.size() && !altIconTexture.isAllocated()) {
        ofLoadImage(altIconTexture, altIconPath, IMAGE_LOAD_GRAYSCALE);
      }
      if (altIconPath2.size() && !altIconTexture2.isAllocated()) {
        ofLoadImage(altIconTexture2, altIconPath2, IMAGE_LOAD_GRAYSCALE);
      }
      if (altIconPath3.size() && !altIconTexture3.isAllocated()) {
        ofLoadImage(altIconTexture3, altIconPath3, IMAGE_LOAD_GRAYSCALE);
      }
      InteractiveElement::draw();

      ofVec2f wh = calculateTextDimension();

      ofPushStyle();
      if (active) {
        ofSetColor(style.activeColor,style.activeColor.a*opacity);
      } else {
        ofSetColor(style.color,style.color.a*opacity);
      }

      float iw = 0;
      float ip = 0;
      
      if (iconPath.size()) {
        ip = (1 - style.lineHeight)*rect.height / 2;
        if (useAltTexture == 1) {
          altIconTexture.draw(rect.x + ip, rect.y + rect.height / 2 - wh.y / 2, wh.y, wh.y);
        } else if (useAltTexture == 2) {
          altIconTexture2.draw(rect.x + ip, rect.y + rect.height / 2 - wh.y / 2, wh.y, wh.y);
        } else if (useAltTexture == 3) {
          altIconTexture3.draw(rect.x + ip, rect.y + rect.height / 2 - wh.y / 2, wh.y, wh.y);
        } else {
          iconTexture.draw(rect.x + ip, rect.y + rect.height / 2 - wh.y / 2, wh.y, wh.y);
        }
        iw = ip * 2 + wh.y;
      }
      ofPushMatrix();
      ofTranslate(rect.x+iw + (rect.width-iw) / 2 - wh.x / 2 - ip, rect.y + rect.height / 2 + wh.y / 2);
      ofScale(wh.y / (float)FONT_MAXSIZE);
      font.drawString(text, 0, 0);
      ofPopMatrix();

      ofPopStyle();

      if (active > 0) {
        active--;
      }
    }
  };

  class IconButton : public Button {public:
    

    void load(string path) {
      iconPath = path;
      ofLoadImage(iconTexture, iconPath, IMAGE_LOAD_GRAYSCALE);
    }

    void draw() {
      //InteractiveElement::draw();
      //cout << rect.height << endl;
      if (rect.height < 1) {
        return;
      }

      if (iconTexture.isAllocated()) {
        ofPushStyle();
        ofSetColor(active ? 200 : 255, opacity*255);
        iconTexture.draw(rect.x,rect.y,rect.width,rect.height);
        ofPopStyle();
      }

      ofPushStyle();
      //ofNoFill();
      //ofSetLineWidth(style.borderWidth * (active ? 2 : 1));
      ofSetColor(style.borderColor.r, style.borderColor.g, style.borderColor.b, style.borderColor.a * opacity);
      //ofDrawRectangle(rect);
      ofDrawRectangleOutline(rect, style.borderWidth * (active ? 2 : 1), 0, true);
      ofPopStyle();

      if (active > 0) {
        active--;
      }
    }
  };


  class Toggle : public InteractiveElement { public:

    virtual void draw() {
      
      ofVec2f wh = calculateTextDimension();
      float pad = wh.y;

      ofPushStyle();
      ofSetColor(style.labelBackground);
      ofDrawRectangle(rect.x+rect.width, rect.y, wh.x+pad*2, rect.height);
      ofPopStyle();

      InteractiveElement::draw();

      ofPushStyle();
      ofSetColor(style.color);
      ofPushMatrix();
      ofTranslate(rect.x + rect.width + pad, rect.y + rect.height / 2 + wh.y / 2);
      ofScale(wh.y / (float)FONT_MAXSIZE);
      font.drawString(text, 0, 0);
      ofPopMatrix();
      ofPopStyle();

    }

    bool check(ofVec2f p) {
      bool b = InteractiveElement::check(p);
      if (b) {
        active = !active;
      }
      return b;
    }
 
  };

  class ToggleButton : public Toggle {public:
    void draw() {

      InteractiveElement::draw();

      ofVec2f wh = calculateTextDimension();

      ofPushStyle();
      if (active) {
        ofSetColor(style.activeColor);
      } else {
        ofSetColor(style.color);
      }
      ofPushMatrix();
      ofTranslate(rect.x + rect.width / 2 - wh.x / 2, rect.y + rect.height / 2 + wh.y / 2);
      ofScale(wh.y / (float)FONT_MAXSIZE);
      font.drawString(text, 0, 0);
      ofPopMatrix();
      ofPopStyle();
    }
  };

  class Radio : public Element {public:
    vector<Toggle*> items;
    int currentIndex;
    string currentValue;
    float spacing = 5;

    virtual void setItemRect(int i) {
      items[i]->rect = ofRectangle(rect.x, rect.y + rect.height * i, rect.width - spacing, rect.height - spacing);
    }

    virtual void addItem(string t) {
      
      Toggle* e = new Toggle();
      e->text = t;
      if (items.size() == 0) {
        currentIndex = 0;
        currentValue = e->text;
        e->active = true;
      }
      items.push_back(e);
      setItemRect(items.size()-1);
    }
    void setRect(ofRectangle r) {
      Element::setRect(r);
      for (int i = 0; i < items.size(); i++) {
        setItemRect(i);
      }
    }
    int dryCheck(ofVec2f p) {
      for (int i = 0; i < items.size(); i++) {
        cout << items[i]->rect << " " << p << endl;
        if (items[i]->rect.inside(p.x, p.y)) {
          return i;
        }
      }
      return -1;
    }
    bool check(ofVec2f p) {
      bool b = false;
      for (int i = 0; i < items.size(); i++) {
        if (items[i]->check(p)) {
          currentIndex = i;
          currentValue = items[i]->text;
          b = true;
          break;
        }
      }
      for (int i = 0; i < items.size(); i++) {
        if (i == currentIndex) {
          items[i]->active = true;
        } else {
          items[i]->active = false;
        }
      }
      return b;
    }
    void draw() {
      for (int i = 0; i < items.size(); i++) {
        items[i]->draw();
      }
    }

  };
  
  class RadioTable : public Radio {public:
    int rows = 2;
    int cols = 4;
    ofVec2f pad = ofVec2f(5,5);
    int defaultSelect = 0;

    void setItemRect(int i) {
      float w = rect.width / cols;
      float h = rect.height / rows;
      items[i]->rect = ofRectangle(rect.x + (i % cols)*w + pad.x, rect.y + ((int)(i / cols))*h + pad.y, w - pad.x * 2, h - pad.y * 2);
    }

    void addItem(string t) {
      ToggleButton* e = new ToggleButton();

      e->text = t;
      if (items.size() == defaultSelect) {
        currentIndex = defaultSelect;
        currentValue = e->text;
        e->active = true;
      }
      items.push_back(e);
      setItemRect(items.size() - 1);
    }
  
  };

  
  struct EventInfo {
    string elementId;
    Element* element;
  };

  ofEvent<EventInfo> onEvent;
  vector<Element*> elements;

  template <typename T>
  inline T* createElement(string id) {
    T* el = new T();
    el->id = id;
    elements.push_back(el);
    return el;
  }

  template <typename T>
  inline T* getElementById(string id) {
    for (int i = 0; i < elements.size(); i++) {
      if (elements[i]->id == id) {
        return (T*)elements[i];
      }
    }
    throw;
  }

  bool check(ofVec2f p, bool fire = true) {
    for (int l = 2; l >= -2; l--) {
      for (int i = elements.size() - 1; i >= 0; i--) {
        if (elements[i]->clickThrough) {
          continue;
        }
        if (fire) {
          if (elements[i]->layer == l && elements[i]->check(p)) {
          
            EventInfo e;
            e.elementId = elements[i]->id;
            e.element = elements[i];
            ofNotifyEvent(onEvent, e);
            cout << e.elementId << endl;
            return true;
          }
          
        } else {
          if (elements[i]->rect.inside(p.x,p.y)) {
            return true;
          }
        }
      }
    }
    return false;
  }

  void draw() {
    for (int l = -2; l <= 2; l++) {
      for (int i = 0; i < elements.size(); i++) {
        //cout << elements[i]->id << endl;
        if (elements[i]->layer == l) {
          elements[i]->draw();
        }
      }
    }
  }

};