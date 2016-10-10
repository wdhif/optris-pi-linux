#include <stdio.h>
#include <string.h>
#include <iostream>
#include <iomanip>
#include <pthread.h>
#include <sys/time.h>
#include <signal.h>

#include <GL/freeglut.h>

#include <ctime>
#include <sstream>
#include <cpr/cpr.h>

/**
 * Optris device interface
 */
#include "IRImager.h"

/**
 * Optris image converter
 */
#include "ImageBuilder.h"

/**
 * Visualization
 */
#include "Obvious2D.h"

#define API_PATH "http://10.31.16.130:3000/api/points"

using namespace std;
using namespace optris;
using namespace cpr;

IRImager *_imager = NULL;
ImageBuilder _iBuilder;

bool _showVisibleChannel = false;
EnumOptrisColoringPalette _palette = eIron;
bool _biSpectral = false;
bool _showHelp = true;
bool _showFPS = true;

struct thread_context {
    pthread_mutex_t mutex;
    pthread_cond_t available;
};

bool _shutdown = false;

// Threaded working function to display images
void *displayWorker(void *arg);

unsigned int _w;
unsigned int _h;
unsigned short *_thermal = NULL;
unsigned char *_yuyv = NULL;

void onThermalFrame(unsigned short *thermal, unsigned int w, unsigned int h, long long counterHW, void *arg) {
    thread_context *context = (thread_context *) arg;
    pthread_mutex_lock(&(context->mutex));
    _w = w;
    _h = h;
    if (_thermal == NULL) _thermal = new unsigned short[w * h];
    if (_imager->isFlagOpen()) {
        memcpy(_thermal, thermal, w * h * sizeof(*thermal));
        pthread_cond_signal(&(context->available));
    }
    pthread_mutex_unlock(&(context->mutex));

    //cout << counterHW << endl;
    //cout << "Hardware timestamp: " << (float)counterHW * _imager->getHWInterval() << " s" << endl;
}

void onVisibleFrame(unsigned char *yuyv, unsigned int w, unsigned int h, long long counterHW, void *arg) {
    thread_context *context = (thread_context *) arg;
    if (_showVisibleChannel) {
        pthread_mutex_lock(&(context->mutex));
        _w = w;
        _h = h;
        if (_yuyv == NULL) _yuyv = new unsigned char[w * h * 2];
        memcpy(_yuyv, yuyv, 2 * w * h * sizeof(*yuyv));
        pthread_cond_signal(&(context->available));
        pthread_mutex_unlock(&(context->mutex));
    }
}

void onFlageStateChange(EnumFlagState fs, void *arg) {
    // cout << "Flag state: " << fs << endl;
}

void drawMeasurementInfo(Obvious2D *viewer, ImageBuilder *iBuilder, unsigned char *dst, unsigned int w, unsigned int h,
                         unsigned int x, unsigned int y, float value, unsigned char rgba[4], unsigned char rgbaBG[4]) {
    unsigned int width = viewer->getWidth();
    unsigned int height = viewer->getHeight();

    if (viewer->getFullscreen()) {
        width = viewer->getScreenWidth();
        height = viewer->getScreenHeight();
        float fw = (float) width;
        float fh = (float) height;
        // check aspect ratio, there might be a dual monitor configuration
        if (fw / fh > (16.f / 9.f + 1e-3))
            width /= 2;
    } else {
        width = viewer->getWidth();
        height = viewer->getHeight();
    }

    char text[20];
    sprintf(text, "%2.1f", value);

    float radius = 20.f;
    float offset = radius / 2.f;
    viewer->addCrosshair(width * x / w, height - height * y / h, text, rgba, rgbaBG, radius, offset);
}

void cbShowHelp() {
    _showHelp = !_showHelp;
}

void cbShowFPS() {
    _showFPS = !_showFPS;
}

void cbPalette() {
    unsigned int val = (unsigned int) _palette;
    if ((val++) > eAlarmRed) val = 1;
    _palette = (EnumOptrisColoringPalette) val;
}

void cbSnapshot() {
    _iBuilder.setData(_w, _h, _thermal);
    unsigned char *bufferThermal = new unsigned char[_iBuilder.getStride() * _h * 3];;
    _iBuilder.convertTemperatureToPaletteImage(bufferThermal);

    cout << "Minimum temperature: " << _iBuilder.getIsothermalMin() << ", Maximum temperature: "
         << _iBuilder.getIsothermalMax() << endl;

    unsigned char *ppm;
    unsigned int size;
    string file("/tmp/snapshot.ppm");
    _iBuilder.convert2PPM(ppm, &size, bufferThermal, _iBuilder.getStride(), _h);
    _imager->serialize(file.c_str(), (char *) ppm, size);
    delete[] ppm;
    delete[] bufferThermal;
    cout << "Serialized file to " << file << endl;

    unsigned char *bar;
    unsigned int wBar = 20;
    unsigned int hBar = 382;
    string fileBar("/tmp/bar.ppm");
    _iBuilder.getPaletteBar(wBar, hBar, bar);
    _iBuilder.serializePPM(fileBar.c_str(), bar, wBar, hBar);
    delete[] bar;
    cout << "Serialized palette bar to " << fileBar << endl;
}

void cbChannel() {
    _showVisibleChannel = !_showVisibleChannel;
}

void cbTempRange() {
    EnumTemperatureRange tRange = _imager->getTempRange();
    switch (tRange) {
        case TM20_100:
            cout << "Switching temperature range to [0-250]" << endl;
            _imager->setTempRange(T0_250);
            break;
        case T0_250:
            cout << "Switching temperature range to [150-900]" << endl;
            _imager->setTempRange(T150_900);
            break;
        case T150_900:
            cout << "Switching temperature range to [-20-100]" << endl;
            _imager->setTempRange(TM20_100);
            break;
    }
    _imager->forceFlagEvent(500.f);
}

void cbManualFlag() {
    _imager->forceFlagEvent();
}

void sendData(string type, float value) {
    auto t = time(nullptr);
    auto tm = *localtime(&t);

    ostringstream oss;
    oss << put_time(&tm, "%d/%m/%Y %H:%M:%S");
    auto date = oss.str();

    GetAsync(
        Url{API_PATH},
        Parameters{
            {"type", type},
            {"value", to_string(value)},
            {"date", date},
        }
    );
}

void *displayWorker(void *arg) {
    thread_context *context = (thread_context *) arg;
    int w = _w;
    int h = _h;
    if (w < 320 && h < 240) {
        w *= 2;
        h *= 2;
    }
    Obvious2D viewer(w, h, "Optris Imager Client - Smalt Creation");
    unsigned char green[4] = {0, 255, 0, 255};
    unsigned char black[4] = {0, 0, 0, 255};
    viewer.registerKeyboardCallback('h', cbShowHelp, "Show help", green, black);
    viewer.registerKeyboardCallback('d', cbShowFPS, "Display FPS", green, black);
    viewer.registerKeyboardCallback('p', cbPalette, "Switch palette", green, black);
    viewer.registerKeyboardCallback('t', cbTempRange, "Switch temperature range", green, black);
    viewer.registerKeyboardCallback('m', cbManualFlag, "Manual flag event", green, black);
    viewer.registerKeyboardCallback('s', cbSnapshot, "Serialize snapshot", green, black);
    if (_biSpectral) viewer.registerKeyboardCallback('c', cbChannel, "Toggle thermal/visible channel", green, black);

    _iBuilder.setPaletteScalingMethod(eMinMax);
    //_iBuilder.setPaletteScalingMethod(eSigma1);
    //_iBuilder.setPaletteScalingMethod(eManual);
    //_iBuilder.setManualTemperatureRange(15.0f, 40.0f);
    _palette = _iBuilder.getPalette();

    unsigned char *bufferThermal = NULL;
    unsigned char *bufferVisible = NULL;

    while (viewer.isAlive() && !_shutdown) {
        pthread_mutex_lock(&(context->mutex));
        pthread_cond_wait(&(context->available), &(context->mutex));

        w = _w;
        h = _h;

        _iBuilder.setPalette(_palette);
        viewer.setShowHelp(_showHelp);
        viewer.setShowFPS(_showFPS);

        if (_showVisibleChannel) {
            if (bufferVisible == NULL)
                bufferVisible = new unsigned char[w * h * 3];
            _iBuilder.yuv422torgb24(_yuyv, bufferVisible, w, h);
            pthread_mutex_unlock(&(context->mutex));
            viewer.draw(bufferVisible, w, h, 3);
        } else {
            _iBuilder.setData(w, h, _thermal);
            if (bufferThermal == NULL)
                bufferThermal = new unsigned char[_iBuilder.getStride() * h * 3];

            _iBuilder.convertTemperatureToPaletteImage(bufferThermal);
            pthread_mutex_unlock(&(context->mutex));

            int radius = 3;
            ExtremalRegion minRegion;
            ExtremalRegion maxRegion;
            _iBuilder.getMinMaxRegion(radius, &minRegion, &maxRegion);
            unsigned char rgba[4] = {0, 0, 255, 255};
            unsigned char white[4] = {255, 255, 255, 128};
            unsigned char whiteBright[4] = {255, 255, 255, 255};
            unsigned char gray[4] = {32, 32, 32, 128};
            unsigned char black[4] = {0, 0, 0, 255};
            drawMeasurementInfo(&viewer, &_iBuilder, bufferThermal, w, h, (minRegion.u1 + minRegion.u2) / 2,
                                (minRegion.v1 + minRegion.v2) / 2, minRegion.t, rgba, white);
            rgba[0] = 255;
            rgba[2] = 0;
            drawMeasurementInfo(&viewer, &_iBuilder, bufferThermal, w, h, (maxRegion.u1 + maxRegion.u2) / 2,
                                (maxRegion.v1 + maxRegion.v2) / 2, maxRegion.t, rgba, white);
            float mean = _iBuilder.getMeanTemperature(w / 2 - radius, h / 2 - radius, w / 2 + radius, h / 2 + radius);
            rgba[1] = 255;
            rgba[2] = 255;
            drawMeasurementInfo(&viewer, &_iBuilder, bufferThermal, w, h, w / 2 - 1, h / 2 - 1, mean, rgba, gray);

            viewer.draw(bufferThermal, _iBuilder.getStride(), _h, 3);

//            cout << "Temp: min = " << minRegion.t << " ; avg = " << mean << " ; max = " << maxRegion.t << endl;

            sendData("min", minRegion.t);
            sendData("avg", mean);
            sendData("max", maxRegion.t);
        }
    }

    if (bufferThermal) delete[] bufferThermal;
    if (bufferVisible) delete[] bufferVisible;
    _shutdown = true;

    cout << "Shutdown display worker" << endl;
    pthread_exit(NULL);

    return NULL;
}

int main(int argc, char *argv[]) {
    cout << "Optris Imager Client - Smalt Création <contact@smaltcreation.com>" << endl;

    cout << std::fixed;
    cout << std::setprecision(2);

    if (argc != 2) {
        cout << "usage: " << argv[0] << " <xml configuration file>" << endl;
        return -1;
    }

    /**
     * Initialize Optris image processing chain
     */
    bool verbose = false;
    _imager = new IRImager(argv[1], verbose);
    _w = _imager->getWidth();
    _h = _imager->getHeight();
    if (_w == 0 || _h == 0) {
        cout << "Error: Image streams not available or wrongly configured. Check connection of camera and config file."
             << endl;
        return -1;
    }

    cout << "Connected camera, serial: " << _imager->getSerial() << ", HW(Rev.): " << _imager->getHWRevision()
         << ", FW(Rev.): " << _imager->getFWRevision() << endl;

    cout << "Thermal channel: " << _imager->getWidth() << "x" << _imager->getHeight() << "@" << _imager->getFramerate()
         << "Hz" << endl;

    unsigned char *bufferRaw = new unsigned char[_imager->getRawBufferSize()];

    _biSpectral = _imager->hasBispectralTechnology();
    if (_biSpectral)
        cout << "Visible channel: " << _imager->getVisibleWidth() << "x" << _imager->getVisibleHeight() << "@"
             << _imager->getFramerate() << "Hz" << endl;

    _imager->setFrameCallback(onThermalFrame);
    _imager->setVisibleFrameCallback(onVisibleFrame);
    _imager->setFlagStateCallback(onFlageStateChange);

    if (_imager->startStreaming() == IRIMAGER_DISCONNECTED) {
        cout << "Error occurred in starting stream ... aborting. You may need to reconnect the camera." << endl;
        exit(-1);
    }

    pthread_t th;
    thread_context context = {PTHREAD_MUTEX_INITIALIZER, PTHREAD_COND_INITIALIZER};
    pthread_create(&th, NULL, displayWorker, &context);

    /**
     * Enter endless loop in order to pass raw data to Optris image processing library.
     * Processed data are supported by the frame callback function.
     */
    FramerateCounter fpsStream(50);

    while (!_shutdown) {
        if (_imager->getFrame(bufferRaw) == IRIMAGER_SUCCESS) {
            _imager->process(bufferRaw, &context);

            fpsStream.trigger();

            // Update rate of grabbing raw images (might differ from update rate of display)
            // fpsStream.printMean(1000.0, cout);
        }
    }

    pthread_mutex_lock(&(context.mutex));
    pthread_cond_signal(&(context.available));
    pthread_mutex_unlock(&(context.mutex));
    pthread_join(th, NULL);

    delete[] bufferRaw;

    _imager->stopStreaming();
    delete _imager;

    cout << "Exiting application" << endl;

    raise(SIGTERM);

    return 1;
}
