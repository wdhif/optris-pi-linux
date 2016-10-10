#include "Obvious2D.h"

#include <iostream>
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <sstream>
#include <iostream>
#include <iomanip>

using namespace std;

namespace optris
{

Obvious2D* Obvious2D::_this[16];
bool Obvious2D::_is_glut_initialized = false;
bool Obvious2D::_instance_map[16] = {0};

typedef void (*fpOnCloseEvent)(void);
typedef void (*fpOnToggleScreen)(void);
typedef void (*fpOnKeyboard)(unsigned char key, int x, int y);
typedef void (*fpOnChangeSize)(GLsizei w, GLsizei h);
typedef void (*fpSetupGlut)(void);

template <unsigned int instance>
void onCloseEvent();
static fpOnCloseEvent _onCloseEventSlot[] =
{
    &onCloseEvent<0>,
    &onCloseEvent<1>,
    &onCloseEvent<2>,
    &onCloseEvent<3>,
    &onCloseEvent<4>,
    &onCloseEvent<5>,
    &onCloseEvent<6>,
    &onCloseEvent<7>,
    &onCloseEvent<8>,
    &onCloseEvent<9>,
    &onCloseEvent<10>,
    &onCloseEvent<11>,
    &onCloseEvent<12>,
    &onCloseEvent<13>,
    &onCloseEvent<14>,
    &onCloseEvent<15>
};

template <unsigned int instance>
void onToggleScreen();
static fpOnToggleScreen _onToggleScreenSlot[] =
{
    &onToggleScreen<0>,
    &onToggleScreen<1>,
    &onToggleScreen<2>,
    &onToggleScreen<3>,
    &onToggleScreen<4>,
    &onToggleScreen<5>,
    &onToggleScreen<6>,
    &onToggleScreen<7>,
    &onToggleScreen<8>,
    &onToggleScreen<9>,
    &onToggleScreen<10>,
    &onToggleScreen<11>,
    &onToggleScreen<12>,
    &onToggleScreen<13>,
    &onToggleScreen<14>,
    &onToggleScreen<15>
};

template <unsigned int instance>
void onKeyboard(unsigned char key, int x, int y);
static fpOnKeyboard _onKeyboardSlot[] =
{
    &onKeyboard<0>,
    &onKeyboard<1>,
    &onKeyboard<2>,
    &onKeyboard<3>,
    &onKeyboard<4>,
    &onKeyboard<5>,
    &onKeyboard<6>,
    &onKeyboard<7>,
    &onKeyboard<8>,
    &onKeyboard<9>,
    &onKeyboard<10>,
    &onKeyboard<11>,
    &onKeyboard<12>,
    &onKeyboard<13>,
    &onKeyboard<14>,
    &onKeyboard<15>
};

template <unsigned int instance>
void onChangeSize(GLsizei w, GLsizei h);
static fpOnChangeSize _onChangeSizeSlot[] =
{
    &onChangeSize<0>,
    &onChangeSize<1>,
    &onChangeSize<2>,
    &onChangeSize<3>,
    &onChangeSize<4>,
    &onChangeSize<5>,
    &onChangeSize<6>,
    &onChangeSize<7>,
    &onChangeSize<8>,
    &onChangeSize<9>,
    &onChangeSize<10>,
    &onChangeSize<11>,
    &onChangeSize<12>,
    &onChangeSize<13>,
    &onChangeSize<14>,
    &onChangeSize<15>
};

template <unsigned int instance>
void setupGlut();
static fpSetupGlut _setupGlutSlot[] =
{
    &setupGlut<0>,
    &setupGlut<1>,
    &setupGlut<2>,
    &setupGlut<3>,
    &setupGlut<4>,
    &setupGlut<5>,
    &setupGlut<6>,
    &setupGlut<7>,
    &setupGlut<8>,
    &setupGlut<9>,
    &setupGlut<10>,
    &setupGlut<11>,
    &setupGlut<12>,
    &setupGlut<13>,
    &setupGlut<14>,
    &setupGlut<15>
};

int getFreeSlot()
{
  int id = -1;
  for(int i=0; i<16; i++)
  {
    if(Obvious2D::_instance_map[i]==false)
    {
      id = i;
      break;
    }
  }
  return id;
}

template <unsigned int instance>
void onCloseEvent()
{
  Obvious2D* viewer = Obvious2D::_this[instance];
  viewer->terminate();
  if(glutGameModeGet(GLUT_GAME_MODE_ACTIVE)) glutLeaveGameMode();
}

template <unsigned int instance>
void onToggleScreen()
{
  Obvious2D* viewer = Obvious2D::_this[instance];
  viewer->toggleFullscreen();
}

// Custom keyboard handler for registered callback functions
template <unsigned int instance>
void onKeyboard(unsigned char key, int x, int y)
{
  Obvious2D* viewer = Obvious2D::_this[instance];
  viewer->processCallback(key);
}

// Resize handler
template <unsigned int instance>
void onChangeSize(GLsizei w, GLsizei h)
{
  Obvious2D* viewer = Obvious2D::_this[instance];
  viewer->setWidth(w);
  viewer->setHeight(h);
  glViewport(0, 0, w, h);
  _setupGlutSlot[instance]();
  glutPostRedisplay();
}

template <unsigned int instance>
void setupGlut()
{
  glDisable(GL_ALPHA_TEST);
  glDisable(GL_DEPTH_TEST);
  glDisable(GL_BLEND);
  glDisable(GL_DITHER);
  glDisable(GL_FOG);
  glDisable(GL_LIGHTING);
  glDisable(GL_LOGIC_OP);
  glDisable(GL_STENCIL_TEST);
  glDisable(GL_TEXTURE_1D);
  glDisable(GL_CULL_FACE);

  glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_CONTINUE_EXECUTION);
  //glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_EXIT);

  glutCloseFunc (_onCloseEventSlot[instance]);
  glutKeyboardFunc(_onKeyboardSlot[instance]);
  glutReshapeFunc(_onChangeSizeSlot[instance]);
}

Obvious2D::Obvious2D(unsigned int width, unsigned int height, const char* title)
{
  char fake[]      = "dummy";
  char *fakeargv[] = { fake, NULL };
  int argc         = 1;
  _font = GLUT_BITMAP_8_BY_13;

  if(width==0 || height==0)
  {
    cout << "Obvious2D:: Wrong width and height passed" << endl;
    abort();
  }
  else if(Obvious2D::_is_glut_initialized==false)
  {
    glutInit(&argc, fakeargv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
    Obvious2D::_is_glut_initialized = true;
  }

  int id = getFreeSlot();

  if(id==-1)
  {
    cout << "Obvious2D:: Maximum number of instances reached" << endl;
    abort();
  }

  glutInitWindowSize(width, height);
  glutInitWindowPosition(0, 0);
  _handle = glutCreateWindow(title);

  _instanceID = id;
  Obvious2D::_instance_map[_instanceID] = true;

  pthread_mutex_init(&_mutex, NULL);

  _fpsDisplay      = new FramerateCounter(50);
  _fullscreen      = false;
  _screen_width    = glutGet(GLUT_SCREEN_WIDTH);
  _screen_height   = glutGet(GLUT_SCREEN_HEIGHT);
  _width           = width;
  _height          = height;
  _init_width      = _width;
  _init_height     = _height;
  _borderLeft      = 0;
  _borderRight     = 0;
  _borderTop       = 0;
  _borderBottom    = 0;
  _showHelp        = false;
  _showFPS         = true;

  TextStruct ts;
  ts.text = std::string("q: Quit");
  unsigned char green[4] = {0,   255,   0, 255};
  unsigned char black[4] = {0,   0,   0, 255};
  ts.rgba[0] = green[0];
  ts.rgba[1] = green[1];
  ts.rgba[2] = green[2];
  ts.rgba[3] = green[3];
  ts.rgbaBG[0] = black[0];
  ts.rgbaBG[1] = black[1];
  ts.rgbaBG[2] = black[2];
  ts.rgbaBG[3] = black[3];

  // Add standard quit key 'q' to help text
   _mCallback['q'] = _onCloseEventSlot[_instanceID];
   _helpText.push_back(ts);
   // Register fullscreen toggle method
  _mCallback['f']  = _onToggleScreenSlot[_instanceID];
  ts.text = std::string("f: Toggle Fullscreen");
  _helpText.push_back(ts);

  Obvious2D::_this[_instanceID] = this;

  _setupGlutSlot[_instanceID]();
  glPixelZoom(1.0, -1.0);

  _isAlive = true;


  glClearColor (0.0, 0.0, 0.0, 0.0);
  glShadeModel(GL_FLAT);
  glEnable(GL_DEPTH_TEST);

  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

  glGenTextures(1, &_texture);
  glBindTexture(GL_TEXTURE_2D, _texture);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
}

Obvious2D::~Obvious2D()
{
  if (glutGameModeGet(GLUT_GAME_MODE_ACTIVE)) glutLeaveGameMode();
  glutDestroyWindow(_handle);
  pthread_mutex_destroy(&_mutex);
  Obvious2D::_instance_map[_instanceID] = false;
  delete _fpsDisplay;
  cout << "Destroy window" << endl;
}

unsigned int Obvious2D::getWidth()
{
  return _width;
}

unsigned int Obvious2D::getHeight()
{
  return _height;
}

void Obvious2D::setWidth(unsigned int width)
{
  _width = width;
}

void Obvious2D::setHeight(unsigned int height)
{
  _height = height;
}

unsigned int Obvious2D::getInitWidth() const
{
  return _init_width;
}

unsigned int Obvious2D::getInitHeight() const
{
  return _init_height;
}

unsigned int Obvious2D::getScreenWidth() const
{
  return _screen_width;
}

unsigned int Obvious2D::getScreenHeight() const
{
  return _screen_height;
}

bool Obvious2D::getFullscreen() const
{
  return _fullscreen;
}

void Obvious2D::toggleFullscreen()
{
  if(!getFullscreen())
  {
    setWidth(getScreenWidth());
    setHeight(getScreenHeight());
    glutReshapeWindow(getWidth(), getHeight());
    glutFullScreen();
    _fullscreen = true;
  }
  else
  {
    setWidth(getInitWidth());
    setHeight(getInitHeight());
    glutReshapeWindow(getWidth(), getHeight());
    _fullscreen = false;
  }

  _setupGlutSlot[_instanceID]();
}

void Obvious2D::setBorder(unsigned int pixelsLeft, unsigned int pixelsRight, unsigned int pixelsTop, unsigned int pixelsBottom)
{
  _borderLeft   = pixelsLeft;
  _borderRight  = pixelsRight;
  _borderTop    = pixelsTop;
  _borderBottom = pixelsBottom;
}

void Obvious2D::setFont(void* font)
{
  _font = font;
}

void Obvious2D::setShowHelp(bool showHelp)
{
  _showHelp = showHelp;
}

void Obvious2D::setShowFPS(bool showFPS)
{
  _showFPS = showFPS;
}

void Obvious2D::drawHelp()  const
{
  float borderScaleHor  = (float)(_width - _borderLeft - _borderRight) / (float)_width;
  float borderScaleVert = (float)(_height - _borderTop - _borderBottom) / (float)_height;
  float widthScaled     = (float)_width * borderScaleHor;
  float heightScaled    = (float)_height * borderScaleVert;

  int maxTextLength = 0;
  std::ostringstream strs;
  strs << std::fixed << std::setprecision(1) << _fpsDisplay->getFps();
  std::string sText("Display: ");
  sText.append(strs.str());
  sText.append(" fps");
  const char* textFPS = (const char*)sText.c_str();
  int lines = 0;

  if(_showFPS)
  {
    lines++;
    maxTextLength = getBitmapLength(textFPS);
  }

  if(_showHelp)
  {

    for(int i=0; i<_helpText.size(); i++)
    {
      int textLength = getBitmapLength(_helpText[i].text.c_str());
      if(textLength>maxTextLength) maxTextLength = textLength;
    }

    lines+=_helpText.size();
  }

  int fontHeight = glutBitmapHeight(_font);
  int fontBorder = fontHeight/4;

  int borderTotal = 2*fontBorder;
  int lineHeight = fontHeight + borderTotal;

  unsigned int textHeight = lineHeight;

  int xpos = _width - maxTextLength;
  if(xpos<0) xpos = 0;

  int posX = (float)xpos * borderScaleHor + _borderLeft - borderTotal;
  int posY = (float)(_height) * borderScaleVert - lineHeight + fontBorder + _borderBottom;

  int minX = posX - fontBorder;
  int maxX = posX + maxTextLength * borderScaleHor + fontBorder;
  int maxY = posY + lineHeight - borderTotal;
  int minY = maxY - lines * lineHeight;

  unsigned char blueTransparent[4] = {0, 0, 255, 160};
  drawTextBackground(minX, maxX, minY, maxY, blueTransparent);
  // -----------------------------------------


  unsigned char green[4]            = {0,   255,   0, 255};
  unsigned char black[4]            = {0,   0,   0, 255};

  if(_showFPS)
  {
    int wText = glutBitmapLength(_font, (const unsigned char*)textFPS);
    glColor4ub(green[0], green[1], green[2], green[3]);
    glWindowPos2i(posX, posY);
    glutBitmapString(_font, (const unsigned char*)textFPS);

    glColor4ub(black[0], black[1], black[2], black[3]);
    glWindowPos2i(posX-1, posY);
    glutBitmapString(_font, (const unsigned char*)textFPS);
    glWindowPos2i(posX+1, posY);
    glutBitmapString(_font, (const unsigned char*)textFPS);
    glWindowPos2i(posX, posY-1);
    glutBitmapString(_font, (const unsigned char*)textFPS);
    glWindowPos2i(posX, posY+1);
    glutBitmapString(_font, (const unsigned char*)textFPS);

    glColor4ub(green[0], green[1], green[2], green[3]);
    glWindowPos2i(posX, posY);
    glutBitmapString(_font, (const unsigned char*)textFPS);
    posY -= lineHeight;
  }

  // Draw help text
  if(_showHelp)
  {
    for(int i=0; i<_helpText.size(); i++)
    {
      if(_helpText[i].text.length()>0)
      {

        const unsigned char* text = (const unsigned char*)_helpText[i].text.c_str();

        // Shift text: 1 pixel in each direction to draw background
        glColor4ub(_helpText[i].rgbaBG[0], _helpText[i].rgbaBG[1], _helpText[i].rgbaBG[2], _helpText[i].rgbaBG[3]);
        glWindowPos2i(posX-1, posY);
        glutBitmapString(_font, text);
        glWindowPos2i(posX+1, posY);
        glutBitmapString(_font, text);
        glWindowPos2i(posX, posY-1);
        glutBitmapString(_font, text);
        glWindowPos2i(posX, posY+1);
        glutBitmapString(_font, text);

        glColor4ub(_helpText[i].rgba[0], _helpText[i].rgba[1], _helpText[i].rgba[2], _helpText[i].rgba[3]);
        glWindowPos2i(posX, posY);
        glutBitmapString(_font, text);

        posY -= lineHeight;
      }
    }
  }
  // -----------------------------------------
}

void Obvious2D::drawInfo() const
{
  int lines = _infoText.size();

  if(lines==0) return;

  float borderScaleHor  = (float)(_width - _borderLeft - _borderRight) / (float)_width;
  float borderScaleVert = (float)(_height - _borderTop - _borderBottom) / (float)_height;
  float widthScaled     = (float)_width * borderScaleHor;
  float heightScaled    = (float)_height * borderScaleVert;

  int maxTextLength = 0;
  for(int i=0; i<_infoText.size(); i++)
  {
    int textLength = getBitmapLength(_infoText[i].text.c_str());
    if(textLength>maxTextLength) maxTextLength = textLength;
  }

  int fontHeight = glutBitmapHeight(_font);
  int fontBorder = fontHeight/4;

  int borderTotal = 2*fontBorder;
  int lineHeight = fontHeight + borderTotal;

  int posX = _borderLeft + borderTotal;
  int posY = (lines - 1) * lineHeight + _borderBottom + borderTotal;

  int minX = posX - fontBorder;
  int maxX = posX + maxTextLength * borderScaleHor + fontBorder;
  int maxY = posY + fontHeight - fontBorder;// + hBoxBorder;
  int minY = posY - (lines-1) * lineHeight - fontBorder;

  unsigned char blueTransparent[4] = {0, 0, 255, 160};
  drawTextBackground(minX, maxX, minY, maxY, blueTransparent);
  // -----------------------------------------

  unsigned char green[4]            = {0,   255,   0, 255};
  unsigned char black[4]            = {0, 0, 0, 255};

  int textHeight = 0;

  // Draw info text
  for(int i=0; i<_infoText.size(); i++)
  {
    if(_infoText[i].text.length()>0)
    {

      glColor4ub(_infoText[i].rgbaBG[0], _infoText[i].rgbaBG[1], _infoText[i].rgbaBG[2], _infoText[i].rgbaBG[3]);

      // Draw progress bar
      float fMinX =  2.f * ((float)minX /(float)_width) - 1.f;
      float fMaxX =  2.f * ((float)(minX+(maxX-minX)*_infoText[i].progress) /(float)_width) - 1.f;
      float fMinY =  2.f * ((float)(posY- fontBorder) /(float)_height) - 1.f;
      float fMaxY =  2.f * ((float)(posY+(lineHeight+fontBorder)/2.0) /(float)_height) - 1.f;
      glBegin(GL_POLYGON);
          glVertex2f(fMinX, fMinY);
          glVertex2f(fMaxX, fMinY);
          glVertex2f(fMaxX, fMaxY);
          glVertex2f(fMinX, fMaxY);
      glEnd();


      const unsigned char* text = (const unsigned char*)_infoText[i].text.c_str();
      // Shift text: 1 pixel in each direction to draw background
      glWindowPos2i(posX-1, posY);
      glutBitmapString(_font, text);
      glWindowPos2i(posX+1, posY);
      glutBitmapString(_font, text);
      glWindowPos2i(posX, posY-1);
      glutBitmapString(_font, text);
      glWindowPos2i(posX, posY+1);
      glutBitmapString(_font, text);

      glColor4ub(_infoText[i].rgba[0], _infoText[i].rgba[1], _infoText[i].rgba[2], _infoText[i].rgba[3]);
      glWindowPos2i(posX, posY);
      glutBitmapString(_font, text);

      posY -= lineHeight;
    }
  }
  // -----------------------------------------
}

void Obvious2D::drawPopup() const
{
  int lines = _popupText.size();

  if(lines==0) return;

  float borderScaleHor  = (float)(_width - _borderLeft - _borderRight) / (float)_width;
  float borderScaleVert = (float)(_height - _borderTop - _borderBottom) / (float)_height;
  float widthScaled     = (float)_width * borderScaleHor;
  float heightScaled    = (float)_height * borderScaleVert;

  int maxTextLength = getBitmapLength(_popupText[0].text.c_str());;

  // glutBitmapHeight: font height is inter-line spacing and does not relate to the real font height, thus we set it to a fixed value
  float fontHeight = getBitmapHeight();
  float fontBorder = fontHeight/4;

  float borderTotal = 2*fontBorder;
  float lineHeight = fontHeight + borderTotal;

  float posX = _borderLeft + borderTotal;
  float posY = borderTotal + _borderBottom + 1 + 3;
  float minX = posX - fontBorder;
  float maxX = widthScaled - fontBorder + _borderLeft;
  float maxY = posY + fontHeight + fontBorder + 1;
  float minY = posY - fontBorder - 1 - 2;

  unsigned char white[4] = {255, 255, 255, 255};
  drawTextBackground(minX-3, maxX+3, minY-2, maxY+4, white);
  // -----------------------------------------

  unsigned char green[4]            = {0,   255,   0, 255};
  unsigned char black[4]            = {0, 0, 0, 255};

  int textHeight = 0;

  // Draw info text
  if(_popupText[0].text.length()>0)
  {

    glColor4ub(_popupText[0].rgbaBG[0], _popupText[0].rgbaBG[1], _popupText[0].rgbaBG[2], _popupText[0].rgbaBG[3]);

    // Draw progress bar
    float fMinX =  2.f * ((float)minX /(float)_width) - 1.f;
    float fMaxX =  2.f * ((float)(minX+(maxX-minX)*_popupText[0].progress) /(float)_width) - 1.f;
    float fMinY =  2.f * ((float)(minY) /(float)_height) - 1.f;
    float fMaxY =  2.f * ((float)(maxY) /(float)_height) - 1.f;
    glBegin(GL_POLYGON);
        glVertex2f(fMinX, fMinY);
        glVertex2f(fMaxX, fMinY);
        glVertex2f(fMaxX, fMaxY);
        glVertex2f(fMinX, fMaxY);
    glEnd();


    const unsigned char* text = (const unsigned char*)_popupText[0].text.c_str();
    // Shift text: 1 pixel in each direction to draw background
    glWindowPos2i(posX-1, posY);
    glutBitmapString(_font, text);
    glWindowPos2i(posX+1, posY);
    glutBitmapString(_font, text);
    glWindowPos2i(posX, posY-1);
    glutBitmapString(_font, text);
    glWindowPos2i(posX, posY+1);
    glutBitmapString(_font, text);

    glColor4ub(_popupText[0].rgba[0], _popupText[0].rgba[1], _popupText[0].rgba[2], _popupText[0].rgba[3]);
    glWindowPos2i(posX, posY);
    glutBitmapString(_font, text);
  }
  // -----------------------------------------
}

void Obvious2D::drawTextBackground(int minX, int maxX, int minY, int maxY, unsigned char color[4]) const
{
  float fMinX =  2.f * ((float)minX /(float)_width) - 1.f;
  float fMaxX =  2.f * ((float)maxX /(float)_width) - 1.f;
  float fMinY =  2.f * ((float)minY /(float)_height) - 1.f;
  float fMaxY =  2.f * ((float)maxY /(float)_height) - 1.f;

  // Draw background of text
  glColor4ub(color[0], color[1], color[2], color[3]);
  glBegin(GL_POLYGON);
    glVertex2f(fMinX, fMinY);
    glVertex2f(fMaxX, fMinY);
    glVertex2f(fMaxX, fMaxY);
    glVertex2f(fMinX, fMaxY);
  glEnd();
}

bool Obvious2D::isAlive() const
{
  return _isAlive;
}

void Obvious2D::terminate()
{
  _isAlive = false;
}

int Obvious2D::getBitmapLength(const char* text) const
{
  float borderScaleHor = (float)(_width - _borderLeft - _borderRight) / (float)_width;
  float l = glutBitmapLength(_font, (const unsigned char*)text);
  return (int)(l/borderScaleHor + .5f);
}

int Obvious2D::getBitmapHeight() const
{
  float borderScaleVert = (float)(_height - _borderTop - _borderBottom) / (float)_height;
  float h = glutBitmapHeight(_font);
  return (int)(h / borderScaleVert + .5f);
}

void Obvious2D::draw(unsigned char* image, unsigned int width, unsigned int height, unsigned int channels)
{
  if(!_isAlive) return;

  float pixelWidth      = 2.f/(float)_width;
  float pixelHeight     = 2.f/(float)_height;
  float borderLeft      = (float)_borderLeft * pixelWidth;
  float borderRight     = (float)_borderRight * pixelWidth;
  float borderTop       = (float)_borderTop * pixelHeight;
  float borderBottom    = (float)_borderBottom * pixelHeight;
  float borderScaleHor  = (float)(_width - _borderLeft - _borderRight) / (float)_width;
  float borderScaleVert = (float)(_height - _borderTop - _borderBottom) / (float)_height;

  pthread_mutex_lock(&_mutex);
  glutSetWindow(_handle);

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  if(channels==1)
  {
    //glDrawPixels(width, height, GL_LUMINANCE,GL_UNSIGNED_BYTE, image);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, width, height, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, image);
  }
  else if(channels==3)
  {
    //glDrawPixels(width, height, GL_RGB,GL_UNSIGNED_BYTE, image);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
  }
  else
  {
    cout << "WARNING: draw method not implemented for channels=" << channels << endl;
    return;
  }

  _fpsDisplay->trigger();

  glEnable(GL_TEXTURE_2D);
  glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
  glBindTexture(GL_TEXTURE_2D, _texture);
  glBegin(GL_QUADS);
  glTexCoord2f(0.0, 0.0); glVertex3f(-1.0+borderLeft, 1.0-borderTop, 0.0);
  glTexCoord2f(0.0, 1.0); glVertex3f(-1.0+borderLeft, -1.0+borderBottom, 0.0);
  glTexCoord2f(1.0, 1.0); glVertex3f(1.0-borderRight, -1.0+borderBottom, 0.0);
  glTexCoord2f(1.0, 0.0); glVertex3f(1.0-borderRight, 1.0-borderTop, 0.0);
  glEnd();
  glDisable(GL_TEXTURE_2D);

  // Flip vertically for correct text display
  float ratioW = 1.001*(float)_width/width;
  float ratioH = 1.001*(float)_height/height;
  glPixelZoom(ratioW, ratioH);

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glEnable( GL_LINE_SMOOTH );
  glPolygonMode(GL_FRONT_AND_BACK, GL_QUADS);

  drawHelp();
  drawPopup();

  if(_popupText.size()==0)
  {
    drawInfo();
  }

  float lineWidth = 1.5f;
  if(_width>320) lineWidth = 2.0f;
  if(_width>640) lineWidth = 2.5f;

  float crossScale = (float)1920 / (float)_width;
  float ratio = (float)_width/(float)_height;
  for(int j=0; j<_crosshairs.size(); j++)
  {
    CrossHair c = _crosshairs[j];
    float radiusW = c.radius*pixelWidth;
    float radiusH = c.radius*pixelHeight;
    float centerX = (((float)c.col)*pixelWidth) * borderScaleHor - 1.f + borderLeft;
    float centerY = (((float)c.row)*pixelHeight) * borderScaleVert  - 1.f + borderBottom;

    glColor4ub(c.rgbaBG[0], c.rgbaBG[1], c.rgbaBG[2], c.rgbaBG[3]);
    float lwFactor = 2.f;
    glLineWidth(lwFactor*lineWidth);
    glBegin(GL_LINES);
    float xLeft   = centerX-radiusW;
    float xRight  = centerX+radiusW;
    float yTop    = centerY+radiusH;
    float yBottom = centerY-radiusH;
    float dXSpot  = 3.f*pixelWidth;
    float dYSpot  = 3.f*pixelHeight;
    // left line
    glVertex2f(xLeft, centerY);
    glVertex2f(centerX-dXSpot, centerY);
    // right line
    glVertex2f(centerX+dXSpot, centerY);
    glVertex2f(xRight, centerY);
    // bottom line
    glVertex2f(centerX, yBottom);
    glVertex2f(centerX, centerY-dYSpot);
    // top line
    glVertex2f(centerX, centerY+dYSpot);
    glVertex2f(centerX, yTop);
    glEnd();

    const unsigned char* text = (const unsigned char*) c.text.c_str();
    int wText = glutBitmapLength(_font, text);
    int hText = glutBitmapHeight(_font);
    float offsetW = c.offset;
    float offsetH = c.offset;
    if(offsetW > 0.f)
      if(c.col>(_width-offsetW-wText)) offsetW = (-offsetW-wText);
    if(offsetH > 0.f)
      if(c.row>(_height-offsetH-hText)) offsetH = (-offsetH-hText);

    int posX = (float)(c.col+offsetW) * borderScaleHor + _borderLeft;
    int posY = (float)(c.row+offsetH) * borderScaleVert + _borderBottom;

    glWindowPos2i(posX-1, posY);
    glutBitmapString(_font, text);
    glWindowPos2i(posX+1, posY);
    glutBitmapString(_font, text);
    glWindowPos2i(posX, posY-1);
    glutBitmapString(_font, text);
    glWindowPos2i(posX, posY+1);
    glutBitmapString(_font, text);

    glColor4ub(c.rgba[0], c.rgba[1], c.rgba[2], c.rgba[3]);
    glLineWidth(lineWidth);
    glBegin(GL_LINES);
    // left line
    glVertex2f(xLeft, centerY);
    glVertex2f(centerX-dXSpot, centerY);
    // right line
    glVertex2f(centerX+dXSpot, centerY);
    glVertex2f(xRight, centerY);
    // bottom line
    glVertex2f(centerX, yBottom);
    glVertex2f(centerX, centerY-dYSpot);
    // top line
    glVertex2f(centerX, centerY+dYSpot);
    glVertex2f(centerX, yTop);
    glEnd();

    glWindowPos2i(posX, posY);
    glutBitmapString(_font, text);
  }

  glDisable(GL_BLEND);

  glutSwapBuffers();
  glutMainLoopEvent();

  _crosshairs.clear();
  _infoText.clear();
  _popupText.clear();

  pthread_mutex_unlock(&_mutex);
}

void Obvious2D::registerKeyboardCallback(const char key, fptrKeyboardCallback callback, std::string helpText, unsigned char rgba[4], unsigned char rgbaBG[4])
{
  _mCallback[key] = callback;

  TextStruct ts;
  ts.text = key;
  ts.text.append(": ");
  ts.text.append(helpText);

  ts.rgba[0] = rgba[0];
  ts.rgba[1] = rgba[1];
  ts.rgba[2] = rgba[2];
  ts.rgba[3] = rgba[3];

  ts.rgbaBG[0] = rgbaBG[0];
  ts.rgbaBG[1] = rgbaBG[1];
  ts.rgbaBG[2] = rgbaBG[2];
  ts.rgbaBG[3] = rgbaBG[3];

  _helpText.push_back(ts);
}

void Obvious2D::registerKeyboardClient(const char key, Obvious2DClient* client, std::string helpText, unsigned char rgba[4], unsigned char rgbaBG[4])
{
  _mClient[key] = client;

  TextStruct ts;
  ts.text = key;
  ts.text.append(": ");
  ts.text.append(helpText);

  ts.rgba[0] = rgba[0];
  ts.rgba[1] = rgba[1];
  ts.rgba[2] = rgba[2];
  ts.rgba[3] = rgba[3];

  ts.rgbaBG[0] = rgbaBG[0];
  ts.rgbaBG[1] = rgbaBG[1];
  ts.rgbaBG[2] = rgbaBG[2];
  ts.rgbaBG[3] = rgbaBG[3];

  _helpText.push_back(ts);
}

void Obvious2D::addCrosshair(unsigned int x, unsigned int y, const char* text, unsigned char rgba[4], unsigned char rgbaBG[4], float radius, float offset)
{
  CrossHair c;
  c.text = string(text);
  c.radius = radius;
  c.col = x;
  c.row = y;
  c.rgba[0] = rgba[0];
  c.rgba[1] = rgba[1];
  c.rgba[2] = rgba[2];
  c.rgba[3] = rgba[3];
  c.rgbaBG[0] = rgbaBG[0];
  c.rgbaBG[1] = rgbaBG[1];
  c.rgbaBG[2] = rgbaBG[2];
  c.rgbaBG[3] = rgbaBG[3];
  c.offset = offset;
  _crosshairs.push_back(c);
}

void Obvious2D::addVolatileInfoText(const char* text, unsigned char rgba[4], unsigned char rgbaBG[4], double progress, bool popup)
{
  TextStruct ts;
  ts.text = string(text);

  ts.rgba[0] = rgba[0];
  ts.rgba[1] = rgba[1];
  ts.rgba[2] = rgba[2];
  ts.rgba[3] = rgba[3];

  if(rgbaBG)
  {
    ts.rgbaBG[0] = rgbaBG[0];
    ts.rgbaBG[1] = rgbaBG[1];
    ts.rgbaBG[2] = rgbaBG[2];
    ts.rgbaBG[3] = rgbaBG[3];
  }
  else
  {
    ts.rgbaBG[0] = 0;
    ts.rgbaBG[1] = 0;
    ts.rgbaBG[2] = 0;
    ts.rgbaBG[3] = 0;
  }

  if(progress<0.0) progress=0.0;
  if(progress>1.0) progress=1.0;

  ts.progress = progress;

  if(popup)
    _popupText.push_back(ts);
  else
    _infoText.push_back(ts);
}

void Obvious2D::processCallback(char key)
{
  fptrKeyboardCallback fptr = _mCallback[key];
    if(fptr!=NULL)(*fptr)();
  Obvious2DClient* client = _mClient[key];
    if(client!=NULL) client->keyboardCallback(key);
}

}
