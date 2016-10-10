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

#ifndef IRIMAGERSNAPSHOT_H
#define IRIMAGERSNAPSHOT_H

#include <stdlib.h>

using namespace std;

namespace optris
{

typedef void (*fptrOptrisFrame)(unsigned short* data, unsigned int w, unsigned int h, long long counterHW, void* arg);

/**
 * @class IRImagerSnapshot
 * @brief Wrapper for optris driver and image processing library considering snapshot images
 * @author Stefan May (Nuremberg Institute of Technology Georg Simon Ohm), Matthias Wiedemann (Optris GmbH)
 */
class IRImagerSnapshot
{
public:

  /**
   * Constructor
   * @param[in] imgData energy image data
   * @param[in] size of imgData, i.e., width x height
   * @param[in] metaData meta data container
   */
  IRImagerSnapshot(const char* imgData, unsigned int size, const char* metaData);

  /**
   * Destructor
   */
  ~IRImagerSnapshot();

  /**
   * Get image width
   * @return image width
   */
  unsigned int getWidth();

  /**
   * Get image height
   * @return image height
   */
  unsigned int getHeight();

  int getTemperatureRangeMin();

  int getTemperatureRangeMax();

  /**
   * Repeat frame in order to have callback routines frequently called
   */
  void repeatFrame(void* arg=NULL);

  /**
   * Get thermal image (Temperature can be calculated with ((float)val-1000.f)/10.f)
   * @param[out] Output buffer (needs to be allocated outside having the size of getWidth()*getHeight())
   * @return success flag (==0)
   */
  int acquire(unsigned short* buffer);

  /**
   * Internal init callback
   * @param[in] width image width
   * @param[in] height image height
   */
  void onThermalFrameInit(unsigned int width, unsigned int height);

  /**
   * Internate frame callback
   * @param[in] temperature buffer
   */
  void onThermalFrame(unsigned short* buffer, float timestamp);

  /**
   * Set callback function to be called for new frames
   * @param[in] callback Pointer to callback function for thermal channel
   */
  void setFrameCallback(fptrOptrisFrame callback);

  static void readSnapshot(const char* filename, char* energyData, int* sizeEnergy, char* metaData, int* sizeMeta);

  static void writeSnapshot(const char* filename, const char* png, int sizePng, const char* energyData, int sizeEnergy, const char* metaData, int sizeMeta);

private:

  unsigned int _width;

  unsigned int _height;

  unsigned short* _buffer;

  fptrOptrisFrame _cbFrame;

  bool _firstFrame;

  void* _fb;

  int _temperatureRangeMin;

  int _temperatureRangeMax;

  void* _arg;
};

}

#endif
