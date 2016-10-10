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

#ifndef FRAMERATECOUNTER_H_
#define FRAMERATECOUNTER_H_

#include <iostream>
#include <list>

namespace optris
{

struct LinkedElement
{
  unsigned int id;
  long ticks;
  float dt;
  struct LinkedElement* next;
};

class Timer;

/**
 * @class FramerateCounter
 * @brief Framerate calculation helper
 * @author Stefan May (Nuremberg Institute of Technology Georg Simon Ohm)
 */
class FramerateCounter
{
public:
  /**
   * Standard constructor
   * @param[in] smoothSize size of history, of which mean value is calculated.
   */
  FramerateCounter(unsigned int smoothSize=30);

  /**
   * Standard destructor
   */
  virtual ~FramerateCounter();

  /**
   * Trigger, i.e., integrate new measurement
   * @return current frames per second rate
   */
  double trigger();

  /**
   * Accessor to current frames per second rate (no triggering is perfomed)
   * @return fps
   */
  double getFps();

  /**
   * Print frame rate at reduced time interval
   * @param[in] interval time interval in ms
   * @param[in] output stream
   */
  void printMean(double interval, std::ostream& stream);

private:

  Timer* _tElapsed;

  Timer* _tPrint;

  double _weight;

  double _maxWeight;

  double _fps;

  bool _firstTrigger;

  struct LinkedElement* _oldest;
  struct LinkedElement* _newest;
  unsigned int _ticksSum;
  unsigned int _smoothSize;
  double _dtSum;

};

}

#endif /* FRAMERATECOUNTER_H_ */
