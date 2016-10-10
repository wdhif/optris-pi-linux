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

#ifndef IRCALIBRATIONMANAGER_H
#define IRCALIBRATIONMANAGER_H

#include <fstream>
#include <string>

namespace optris
{

/**
 * @class IRCalibrationManager
 * @brief Class for checking and downloading calibration files for IRImager devices
 * @author Stefan May (Nuremberg Institute of Technology Georg Simon Ohm), Matthias Wiedemann (Optris GmbH)
 * @date 19.7.2015
 */
class IRCalibrationManager
{

public:

  /**
   * Determine serial number of attached device
   * @param serial Serial number (pass 0 to search for serial of an attached device, pass serial to search for a specific device).
   * @return success flag
   */
  static bool findSerial(unsigned long &serial);

  /**
   * Check for installed calibration files
   * @param serial Serial number of device
   * @param path Path of calibration files
   */
  static char* checkCalibration(unsigned long serial, const char* path);

  /**
   * Check for internet access to calibration files
   * @return availability flag
   */
  static bool isOnlineCalibrationRepoAccessible();

  /**
   * Download calibration files for a specific serial number
   * @param serial Serial number of device
   * @param targetDir Target directory for downloaded files
   */
  static bool downloadCalibration(unsigned long serial, std::string targetDir);

  /**
   * Copy calibration files from local device, e.g., USB stick
   * @param serial serial number of device for which calibration files are to be found
   * @param srcDir source directory to search for calibration files
   * @param targetDir target directory
   * @return success of search and copy operation
   */
  static bool copyCalibrationFromLocalRepo(unsigned long serial, std::string srcDir, std::string targetDir);

  /**
   * Generate XML configuration for a specific device
   * @param serial Serial number of device
   * @param calibDir Source directory with calibration files files
   */
  static std::string generateConfiguration(unsigned long serial, std::string calibDir);

  /**
   * Get (concat) path and name of configuration file
   * @param Path of configuration file
   * @param directory Directory of configuration file
   * @param maxLen Maxim character length of path parameter
   * @param fileName Name of configuration file
   * @param extension Extension of configuration file
   */
  static bool createConfigFilepath(char* path, const char* directory, short maxLen, const char* fileName, const char* extension);

private:

  static bool downloadTarget(char* listOfMissingFiles, std::string serial, std::string targetDir);

};

}

#endif // IRCALIBRATIONMANAGER_H
