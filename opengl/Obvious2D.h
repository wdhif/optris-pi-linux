/*
 * This file is part of the Obviously library.
 *
 * Copyright(c) 2010-2012 Georg-Simon-Ohm University, Nuremberg.
 * http://www.ohm-university.eu
 *
 * This file may be licensed under the terms of of the
 * GNU General Public License Version 2 (the ``GPL'').
 *
 * Software distributed under the License is distributed
 * on an ``AS IS'' basis, WITHOUT WARRANTY OF ANY KIND, either
 * express or implied. See the GPL for the specific language
 * governing rights and limitations.
 *
 * You should have received a copy of the GPL along with this
 * program. If not, go to http://www.gnu.org/licenses/gpl.html
 * or write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 */

#ifndef OBVIOUS2D_H
#define OBVIOUS2D_H

#include "Obvious.h"
#include <pthread.h>
#include <vector>
#include <map>

#define GL_GLEXT_PROTOTYPES 1
#include <GL/freeglut.h>
#include <GL/gl.h>

#include "Obvious2DClient.h"
#include "FramerateCounter.h"

namespace optris
{

#define TEXTMAX 10
  struct TextStruct
  {
    std::string text;
    unsigned int col;
    unsigned int row;
    unsigned char rgba[4];
    unsigned char rgbaBG[4];
    double progress;
  };

  struct CrossHair
  {
    std::string text;
    int col;
    int row;
    float radius;  // range [0; 1] => 1.f states whole image edge length
    unsigned char rgba[4];
    unsigned char rgbaBG[4];
    float offset;
  };

  struct Rectangle
  {
    int col;
    int row;
    int width;
    int height;
    unsigned char rgba[4];
  };

  /**
   * GLUT-based 2D viewer
   * @author Stefan May
   */
  class Obvious2D
  {
  public:
    /**
     * Standard Constructor
     * @param[in] width Image width, i.e., number of columns
     * @param[in] height Image height, i.e., number of rows
     * @param[in] title Window title
     */
    Obvious2D(unsigned int width, unsigned int height, const char* title);

    /**
     * Destructor
     */
    ~Obvious2D();

    /**
     * Initial width of window
     * @return width
     */
    unsigned int getWidth();

    /**
     * Set window width
     * @param width window width
     */
    void setWidth(unsigned int width);

    /**
     * Height of window
     * @return height
     */
    unsigned int getHeight();

    /**
     * Set window height
     * @param height window height
     */
    void setHeight(unsigned int height);

    /**
     * Get width at initialization time
     * @return init width
     */
    unsigned int getInitWidth() const;

    /**
     * Get height at initialization time
     * @return init height
     */
    unsigned int getInitHeight() const;

    /**
     * Get width of screen
     * @return screen width
     */
    unsigned int getScreenWidth() const;

    /**
     * Get height of screen
     * @return screen height
     */
    unsigned int getScreenHeight() const;

    /**
     * Get fullscreen flag
     */
    bool getFullscreen() const;

    /**
     * Toggle fullscreen
     */
    void toggleFullscreen();

    /**
     * Set a black border horizontally and vertically
     * @param pixelsLeft number of pixels set as left border
     * @param pixelsRight number of pixels set as right border
     * @param pixelsTop number of pixels set as top border
     * @param pixelsBottom number of pixels set as bottom border
     */
    void setBorder(unsigned int pixelsLeft, unsigned int pixelsRight, unsigned int pixelsTop, unsigned int pixelsBottom);

    /**
     * Set GLUT font
     * @param font e.g., GLUT_BITMAP_HELVETICA_10, GLUT_BITMAP_HELVETICA_12, GLUT_BITMAP_HELVETICA_18. Default: GLUT_BITMAP_HELVETICA_12
     */
    void setFont(void* font);

    /**
     * Set displaying of help info
     * @param showHelp visibility of help info
     */
    void setShowHelp(bool showHelp);

    /**
     * Set displaying of fps info
     * @param showFPS visibility of frames per second
     */
    void setShowFPS(bool showFPS);

    /**
     * Flag indicating that viewer is still to be drawn
     */
    bool isAlive() const;

    /**
     * Terminate viewer
     */
    void terminate();

    /**
     * Determine length of string in pixels
     */
    int getBitmapLength(const char* text) const;

    /**
     * Determine height of font in pixels
     */
    int getBitmapHeight() const;

    /**
     * Draw image to screen
     * @param[in] image Pointer to image data (size = width*height*channels)
     * @param[in] width Image width
     * @param[in] height Image height
     * @param[in] channels Number of image channels
     */
    void draw(unsigned char* image, unsigned int width, unsigned int height, unsigned int channels);

    /**
     * Register keyboard events
     * @param callback pointer to callback function
     */
    void registerKeyboardCallback(char key, fptrKeyboardCallback callback, std::string helpText, unsigned char rgba[4], unsigned char rgbaBG[4]);

    /**
     * Register keyboard events
     * @param callback pointer to callback function
     */
    void registerKeyboardClient(const char key, Obvious2DClient* client, std::string helpText, unsigned char rgba[4], unsigned char rgbaBG[4]);

    /**
     * Add crosshair to be displayed
     * @param[in] x Column of display position
     * @param[in] y Row of display position
     * @param[in] rgba Drawing color
     * @param[in] rgba Drawing background color
     * @param[in] radius Radius of crosshair
     */
    void addCrosshair(unsigned int x, unsigned int y, const char* text, unsigned char rgba[4], unsigned char rgbaBG[4], float radius=0.04f, float offset=0.0f);

    /**
     * Add a info text to be displayed once at next drawing
     * @param[in] text text buffer
     * @param[in] rgba drawing color
     * @param[in] rgbaBG drawing background color
     * @param[in] progress progress status [0 ... 1.0]
     * @param[in] popup true=draw centered progress bar with text info, false=draw text in lower left corner (only one popup is allowed)
     */
    void addVolatileInfoText(const char* text, unsigned char rgba[4]=NULL, unsigned char rgbaBG[4]=NULL, double progress=0.0, bool popup=false);

    /**
     * Process registered callback
     * @param key registered key
     */
    void processCallback(char key);

    static Obvious2D* _this[16];
    static bool _is_glut_initialized;
    static bool _instance_map[16];

  private:

    /**
     * Draw help text
     */
    void drawHelp() const;

    /**
     * Draw user-defined information
     */
    void drawInfo() const;

    /**
     * Draw user-defined information
     */
    void drawPopup() const;

    /**
     * Draw transparent background for help and info boxes
     */
    void drawTextBackground(int minX, int maxX, int minY, int maxY, unsigned char color[4]) const;

    std::vector<CrossHair> _crosshairs;

    // GLUT window handle
    int _handle;

    // Mutex
    pthread_mutex_t _mutex;

    // Is alive flag
    bool _isAlive;

    // Width of GLUT window
    unsigned int _width;

    // Height of GLUT window
    unsigned int _height;

    // Initial width passed via constructor
    unsigned int _init_width;

    // Initial height passed via constructor
    unsigned int _init_height;

    // Width of screen
    unsigned int _screen_width;

    // Height of screen
    unsigned int _screen_height;

    // Fullscreen flag
    bool _fullscreen;

    // Show help flag
    bool _showHelp;

    // Instance ID
    unsigned int _instanceID;

    // Callback map
    std::map<char, fptrKeyboardCallback> _mCallback;

    // Client map
    std::map<char, Obvious2DClient*> _mClient;

    // helping description for shortcuts
    std::vector<TextStruct> _helpText;

    // User-defined information
    std::vector<TextStruct> _infoText;

    // User-defined information
    std::vector<TextStruct> _popupText;

    // GLUT fonts, e.g., GLUT_BITMAP_HELVETICA_10, GLUT_BITMAP_HELVETICA_12, GLUT_BITMAP_HELVETICA_18
    void* _font;

    // 2D texture for drawing pixels
    GLuint _texture;

    // Black Border
    unsigned int _borderLeft;
    unsigned int _borderRight;
    unsigned int _borderTop;
    unsigned int _borderBottom;

    // fps calculator of displaying process
    FramerateCounter* _fpsDisplay;

    bool _showFPS;

  };

}

#endif //OBVIOUS2D_H
