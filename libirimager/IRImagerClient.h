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

#ifndef IRIMAGERCLIENT_H
#define IRIMAGERCLIENT_H
#include <iostream>

/**
 * @class IRImagerClient
 * @brief Interface for specifying object-oriented frame callback methods
 * @author Stefan May (Nuremberg Institute of Technology Georg Simon Ohm)
 */
class IRImagerClient
{

public:

  /**
   * Constructor
   */
  IRImagerClient(){};

  /**
   * Destructor
   */
  virtual ~IRImagerClient(){};

  /**
   * Callback method for thermal frames
   * @param data thermal image
   * @param w width of thermal image
   * @param h height of thermal image
   * @param timestamp frame counter, multiply with value returned by IRImager::getAvgTimePerFrame() to get a timestamp
   * @param arg user arguments (passed to process method of IRImager class)
   */
  virtual void onThermalFrame(unsigned short* data, unsigned int w, unsigned int h, long long timestamp, void* arg) = 0;

  /**
   * Callback method for visible frames
   * @param data visible image
   * @param w width of visible image
   * @param h height of visible image
   * @param timestamp point of time at capture event
   * @param arg user arguments (passed to process method of IRImager class)
   */
  virtual void onVisibleFrame(unsigned char* data, unsigned int w, unsigned int h, float timestamp, void* arg) { };

  /**
   * Callback method for flag state events. The method is called when the flag state changes.
   * @param flagstate current flag state
   * @param arg user arguments (passed to process method of IRImager class)
   */
  virtual void onFlagStateChange(unsigned int flagstate, void* arg) { };
};

#endif
