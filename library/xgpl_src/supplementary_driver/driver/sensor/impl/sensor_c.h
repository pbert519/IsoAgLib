/*
  sensor_c.h:
    header file for SensorI_c, a central object for enum types used in
    all sensor objects

  (C) Copyright 2009 - 2010 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/
#ifndef SENSOR_H
#define SENSOR_H

#include <IsoAgLib/isoaglib_config.h>


// Begin Namespace __IsoAgLib
namespace __IsoAgLib {

/**class for all enum types in relation to Sensor_I
  @see Sensor_I
  @see Analog_I
  @see Digital_I
  @see SensorBase
  *@author Dipl.-Inform. Achim Spangler
  */
class Sensor_c {
public:
  /** enum type input type of analog: voltage or current (values equivalent  to BIOS) */
  enum analogType_t {voltage = 0, current = 1};

  /** enum type for state of an sensor input */
  enum state_t {good = 0, error = 1};

  /** enum type for mapping input HI/LO to on/off */
  enum onoff_t {OnHigh = 1, OnLow = 0};

  /** enum type for the different input type object: at the moment digital and analog */
  enum inputType_t {undef_input = 0, digital = 1, analog = 2, counter = 3};

  /** constructor with no function, because this object is only needed for its enum types */
  Sensor_c();
  /** destructor with no function */
  ~Sensor_c();
};
}

#endif
