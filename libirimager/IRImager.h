/******************************************************************************
 * Copyright (c) 2012-2015 All Rights Reserved, http://www.optris.de          *
 *  Optris GmbH                                                               *
 *  Ferdinand-Buisson-Str. 14                                                 *
 *  13127 Berlin                                                              *
 *  Germany                                                                   *
 *                                                                            *
 * Contributors:                                                              *
 * - Linux platform development in cooperation with Nuremberg Institute of    *
 *   Technology Georg Simon Ohm, http://www.th-nuernberg.de                   *
 * - Linux 64-Bit platform supported by Fraunhofer IPA,                       *
 *   http://www.ipa.fraunhofer.de                                             *
 *****************************************************************************/

#ifndef IRIMAGER_H
#define IRIMAGER_H

#include <stdlib.h>
#include "IRImagerClient.h"

class ImagerUVC;
class BaseControlDevice;
class NewFrameBuffer;
class ImageProcessing;

namespace optris
{

enum EnumControlInterface {HIDController=1, UVCController=2};
enum EnumOutputMode       {Energy=1, Temperature=2};
enum EnumTemperatureRange {TM20_100=0, T0_250=1, T150_900=2, T20_500=3};
enum EnumFlagState        {irFlagOpen, irFlagClose, irFlagOpening, irFlagClosing, irFlagError};

enum IRImager_Error
{
    IRIMAGER_SUCCESS      =  0,
    IRIMAGER_NODATA       = -1,
    IRIMAGER_DISCONNECTED = -2
};

/**
 * Callback type for thermal frames
 * @param data thermal data, convert to real temperatures with ((float)(data[i]-1000))/10.f
 * @param w width of image
 * @param h height of image
 * @param timestamp frame counter, multiply with value returned by IRImager::getAvgTimePerFrame() to get a timestamp
 * @param arg arbitrary user-defined argument (passed to process method)
 */
typedef void (*fptrOptrisFrame)(unsigned short* data, unsigned int w, unsigned int h, long long timestamp, void* arg);

/**
 * Callback type for visible frames
 * @param data RGB data
 * @param w width of image
 * @param h height of image
 * @param timestamp frame counter, multiply with value returned by IRImager::getAvgTimePerFrame() to get a timestamp
 * @param arg arbitrary user-defined argument (passed to process method)
 */
typedef void (*fptrOptrisVisibleFrame)(unsigned char* data, unsigned int w, unsigned int h, long long timestamp, void* arg);

/**
 * Callback type for flag state events. A registered function is called when the flag state changes.
 * @param fs flag state
 * @param arg arbitrary user-defined argument (passed to process method)
 */
typedef void (*fptrOptrisFlagState)(EnumFlagState fs, void* arg);

class Timer;

/**
 * @class IRImager
 * @brief Wrapper for optris driver and image processing library
 * @author Stefan May (Nuremberg Institute of Technology Georg Simon Ohm), Matthias Wiedemann (Optris GmbH)
 */
class IRImager
{
public:

  /**
   * Standard constructor
   * @param[in] path Path to XML-configuration file
   */
  IRImager(const char* xmlConfig, bool verbosity=0);

  /**
   * Standard constructor (Empty -> see comment of init routine)
   */
  IRImager(bool verbosity=0);

  /**
   * Destructor
   */
  ~IRImager();

  /**
   * Initializing routine, to be called if empty constructor was chosen to instantiate device
   * @param[in] v4lPath video4Linux device path
   * @param[in] serial Serial number (if set to 0, the device is tried to be automatically detected
   * @param[in] index of the video format (USB endpoint) to be use
   * @param[in] controller BaseControlInterface, i.e., HID or UVC
   * @param[in] fov Field of view of optics
   * @param[in] tMin Minimum temperature (cf. valid temperature ranges)
   * @param[in] tMax Maximum temperature (cf. valid temperature ranges)
   * @param[in] framerate Desired framerate (must be less or equal than the camera's framerate)
   * @param[in] mode Streaming output mode, i.e., energy data or temperature data
   * @param[in] bispectral 1, if bispectral technology is available (only PI200/PI230) and should be used, else 0
   * @param[in] averageIntermediate Average callback frames over intermediate frames (passed with process calls)
   */
  void init(const char* v4lPath, unsigned long serial, int videoformatindex, EnumControlInterface controller, int fov, EnumTemperatureRange tempRange, float framerate, EnumOutputMode mode, int bispectral, int averageIntermediate=0);

  /**
   * Check for opened device
   * @return device opened
   */
  bool isOpen();

  /**
   * Check existence of calibration file set
   * @param[in] serial Serial number to be checked
   * @return missing files as comma separated list
   */
  char* checkCalibration(unsigned long serial);

  /**
   * Set temperature range
   * @param[in] tRange temperature range
   */
  void setTempRange(EnumTemperatureRange tRange);

  /**
   * Shift temperature range to next standard value, i.e., TM20_100=0, T0_250=1 or T150_900=2
   */
  void shiftTempRange();

  /**
   * Get temperature range
   * @return temperature range
   */
  EnumTemperatureRange getTempRange();

  /**
   * Get serial number of device
   * @return serial number
   */
  unsigned long getSerial();

  /**
   * Get hardware revision
   * @return revision number
   */
  unsigned int getHWRevision();

  /**
   * Get firmware revision
   * @return revision number
   */
  unsigned int getFWRevision();

  /**
   * Get field of view of camera
   * @return field of view
   */
  int getFOV();

  /**
   * Start UVC data streaming
   * @return success flag (if not data is currently available, the driver might recover itself after some time)
   */
  IRImager_Error startStreaming();

  /**
   * Stop UVC data streaming
   */
  bool stopStreaming();

  /**
   * Get image width of thermal channel
   * @return Image width, i.e. number of columns
   */
  unsigned int getWidth();

  /**
   * Get image height of thermal channel
   * @return Image height, i.e. number of rows
   */
  unsigned int getHeight();

  /**
   * Get image width of visible channel (if available)
   * @return Image width, i.e. number of columns
   */
  unsigned int getVisibleWidth();

  /**
   * Get image height of visible channel (if available)
   * @return Image height, i.e. number of rows
   */
  unsigned int getVisibleHeight();

  /**
   * Get width of raw format (from UVC stream)
   * @return width
   */
  unsigned int getWidthIn();

  /**
   * Get height of raw format (from UVC stream)
   * @return height
   */
  unsigned int getHeightIn();

  /**
   * Get # of bits used for temperature coding
   * @return # of bits
   */
  unsigned short getBitCount();

  /**
   * Get average time per frame
   * @return average time
   */
  long long getAvgTimePerFrame();

  /**
   * Get configured frame rate
   * return frame rate (in frames/second)
   */
  float getFramerate();

  /**
   * Get maximum frame rate of device
   * return frame rate (in frames/second)
   */
  float getMaxFramerate();

  /**
   * Get time interval between hardware frames (camera timestamp)
   * @return time interval in [s]
   */
  float getHWInterval();

  /**
   * Get raw image size (includes meta data)
   * @return Number of bytes
   */
  unsigned int getRawBufferSize();

  /**
   * Get lower limit of temperature range
   * @return lower limit
   */
  int getTemperatureRangeMin();

  /**
   * Get upper limit of temperature range
   * @return upper limit
   */
  int getTemperatureRangeMax();

  /**
   * Check if bispectral technology is available
   * @return bispectral technology flag
   */
  bool hasBispectralTechnology();

  /**
   * Get raw image (needs to be processed to obtain thermal data)
   * @param[out] buffer Output buffer (needs to be allocated outside having the size of getRawBufferSize())
   * @return success flag
   */
  IRImager_Error getFrame(unsigned char* buffer);

  /**
   * Get thermal image (Temperature can be calculated with ((float)val-1000.f)/10.f)
   * @param[out] Output buffer (needs to be allocated outside having the size of getWidth()*getHeight())
   * @return success flag
   */
  IRImager_Error acquire(unsigned short* buffer);

  /**
   * Get energy buffer of previously acquired frame
   * @param[out] Output buffer (needs to be allocated outside having the size of getWidth()*getHeight())
   * @return success flag (==0)
   */
  int getEnergyBuffer(unsigned short* &buffer);

  /**
   * Get meta data container of previously acquired frame
   * @param[out] Output buffer
   * @param[in] size Size of buffer in bytes
   * @return number of copied bytes
   */
  int getMetaData(unsigned char* &buffer, int size);

  /**
   * Set callback function to be called for new frames
   * @param[in] callback Pointer to callback function for thermal channel
   */
  void setFrameCallback(fptrOptrisFrame callback);

  /**
   * Set callback function to be called for new frames
   * @param[in] callback Pointer to callback function for visible channel
   */
  void setVisibleFrameCallback(fptrOptrisVisibleFrame callback);

  /**
   * Set callback function to be called for changing flag states
   * @param[in] callback Pointer to callback function for flag state events
   */
  void setFlagStateCallback(fptrOptrisFlagState callback);

  /**
   * Set client as callback receiver
   * @param[in] client callback client
   */
  void setClient(IRImagerClient* client);

  /**
   * Deprecated: Release frame bound with getFrame of acquire method
   * Method is not needed to be called anymore
   */
  void releaseFrame() __attribute__ ((deprecated));

  /**
   * Process raw data
   * @param[in] buffer Raw data acquired with getFrame()
   */
  void process(unsigned char* buffer, void* arg=NULL);

  /**
   * Set automatic flag activation state. Disabling will prevent camera from getting freezed frequently for several frames.
   * But temperature data might deviate too much.
   * @param[in] flag Automatic flag activation state
   */
  void setAutoFlag(bool flag);

  /**
   * Access automatic flag activation state
   * @return automatic flag
   */
  bool getAutoFlag();

  /**
   * Force shutter flag event manually (close/open cycle)
   * @param[in] time point of time in future in [ms], when flag should be closed
   */
  void forceFlagEvent(float time=0.f);

  /**
   * Check if shutter flag is open
   * @return flag open
   */
  bool isFlagOpen();

  /**
   * Get temperature of shutter flag
   * @return temperature
   */
  float getTempFlag();

  /**
   * Get temperature of housing
   * @return temperature
   */
  float getTempBox();

  /**
   * Get temperature of chip
   * @return temperature
   */
  float getTempChip();

  /**
   * Enable heating of bolometers
   * @param enable enable flag
   */
  void enableChipHeating(bool enable);

  /**
   * Get state of bolometers heating
   * @return enable flag
   */
  bool isChipHeatingEnabled();

  /**
   * Set reference input of bolometer heating (limited to +20° - 55°). The chip temperature can be monitored with getTempChip().
   * @param t temperature in °C
   */
  void setTempChipReference(float t);

  /**
   * Get reference input of bolometer heating
   * @return temperature of bolometers in °C
   */
  float getTempChipReference();

  /**
   * Set radiation properties, i.e. emissivity and transmissivity parameters
   * @param emissivity emissivity of observed object [0;1]
   * @param transmissivity transmissivity of observed object [0;1]
   */
  void setRadiationParameters(float emissivity, float transmissivity);

  /**
   * Internal method not to be used by any application
   */
  void onFlagState(unsigned int flagstate);

  /**
   * Internal method not to be used by any application!
   */
  void onThermalFrameInit(unsigned int width, unsigned int height, unsigned short bitCount, long long avgTimePerFrame);

  /**
   * Internal method not to be used by any application!
   */
  void onThermalFrame(unsigned short* buffer, long long counterHW);

  /**
   * Internal method not to be used by any application!
   */
  void onVisibleFrameInit(unsigned int width, unsigned int height);

  /**
   * Internal method not to be used by any application!
   */
  void onVisibleFrame(unsigned char* buffer, long long counterHW);

  /**
   * Internal method to communicate with uvc device
   */
  int readControl(unsigned int id, int* value);

  /**
   * Internal method to communicate with uvc device
   */
  int writeControl(unsigned int id, int value);

  /**
   * Serialize image
   */
  int serialize(const char* filename, const char* preview, int sizePreview);

  void printTrace(const unsigned int pixelID);

private:

  void startFlag();

  void tRangeToTemp(EnumTemperatureRange tRange, int* tMin, int* tMax);

  void tempToTRange(int tMin, int tMax, EnumTemperatureRange* tRange);

  bool _init;

  unsigned int _widthIn;

  unsigned int _heightIn;

  unsigned int _widthOut;

  unsigned int _heightOut;

  long long    _avgTimePerFrame;

  unsigned short _bitCount;

  unsigned short* _buffer;

  unsigned int _widthOutVisible;

  unsigned int _heightOutVisible;

  unsigned char* _bufferVisible;

  fptrOptrisFrame _cbFrame;

  fptrOptrisVisibleFrame _cbVisibleFrame;

  fptrOptrisFlagState _cbFlag;

  IRImagerClient* _client;

  int _fov;

  char* _optics_text;

  char* _cali_path;

  EnumTemperatureRange _tRange;

  int _tMin;

  int _tMax;

  float _framerate;

  float _maxFramerate;

  int _outputmode;

  unsigned long _serial;

  const char* _v4lPath;

  EnumControlInterface _controller;

  int _videoformatindex;

  bool _autoFlag;

  bool _firstFlag;

  float _tBox;

  float _tChip;

  float _tFlag;

  float _emissivity;

  float _transmissivity;

  int _bispectral;

  Timer* _t;

  Timer* _tManual;

  float _tManualEvent;

  ImagerUVC* _uvc;

  BaseControlDevice* _udev;

  NewFrameBuffer* _SGBuffer;

  ImageProcessing* _ip;

  unsigned int _instanceID;

  float _interval;

  EnumFlagState _eFlagstatePrev;
  EnumFlagState _eFlagstate;
  bool _supressDefreezing;
  bool _startFlag;
};

}

#endif
