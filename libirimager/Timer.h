/******************************************************************************
 * Copyright (c) 2012-2015 All Rights Reserved, http://www.optris.de          *                                                                          *
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

#ifndef TIMER_H__
#define TIMER_H__

/**
 * @namespace optris
 */
namespace optris
{

/**
 * @class Timer
 * @brief Time measurement class
 * @author Stefan May (Nuremberg Institute of Technology)
 */
class Timer
{

public:

	/**
	 * Constructor
	 * @brief Reference time is taken at instantiation
	 */
	Timer();

	/**
	 * Destructor
	 */
	~Timer();

	/**
	 * Reset time measurement, i.e., set new reference
	 * @return elapsed time since last reset in [ms.µs]
	 */
	long double reset();

	/**
	 * Retrieve elapsed time in [ms.µs] without timer reset
	 * @return elapsed time since last reset in [ms.µs]
	 */
	long double getTime();

private:

  long double getTimeNow();

	long double _timeRef;

};

} /*namespace*/

#endif /*TIMER_H__*/
