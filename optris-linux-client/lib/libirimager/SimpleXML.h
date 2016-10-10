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

#ifndef SIMPLEXML_H
#define SIMPLEXML_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fstream>

using namespace std;

namespace optris
{

#define NODES 256

/**
 * @class SimpleXML
 * @brief Simple XML parser
 * @author Matthias Wiedemann (Optris GmbH), Stefan May (Nuremberg Institute of Technology Georg Simon Ohm)
 */
class SimpleXML
{
public:

  /**
   * Constructor
   */
  SimpleXML(void);

  /**
   * Destructor
   */
  ~SimpleXML(void);

  /**
   * Open file
   * @param Dir directory
   * @param XMLfile file name
   * @param Root xml root
   * @return success
   */
  bool Open(const char* Dir, char* XMLfile, const char* Root);

  /**
   * Open file
   * @param Filename full file path
   * @param Root xml root
   * @return success
   */
  bool Open(const char* Filename, const char* Root);

  /**
   * Set node to root of tree
   * @return
   */
  bool SetNode(void);

  /**
   * Set specific node by name
   * @param Node node name
   * @return success
   */
  bool SetNode(const char* Node);

  /**
   * Set specific node by name and index
   * @param Node node name
   * @param Index n'th node with same name
   * @return success
   */
  bool SetNode(const char* Node, int Index);

  /**
   * Move one node back
   * @return success (false, if first node with same name is already reached)
   */
  bool ResetNode(void);

  /**
   * Get number of nodes with same name
   * @param Node node name
   * @return number of nodes
   */
  int GetNodeCount(const char* Node);

  /**
   * Get integer value
   * @param Node node tag
   * @param Value value
   * @return success
   */
  bool GetInt(const char* Node, int* Value);

  /**
   * Get long integer
   * @param Node node tag
   * @param Value value
   * @return success
   */
  bool GetLong(const char* Node, long* Value);

  /**
   * Get float value
   * @param Node node tag
   * @param Value value
   * @return success
   */
  bool GetFloat(const char* Node, float* Value);

  /**
   * Get string
   * @param Node node tag
   * @param Value value
   * @return success
   */
  bool GetString(const char *Node, char **Value);

private:

  int FileSize;

  int Begin[NODES], End[NODES];

  unsigned short NodeIndex;

  char* Data;

  int GetKey(const char* key, int begin, int end, bool CloseTag = false);
};

} // namespace

#endif // SIMPLEXML_H
