/*
  isystem.h: central object for encapsulation of platform dependent
    elements

  (C) Copyright 2009 - 2012 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/
#ifndef ISYSTEM_H
#define ISYSTEM_H

#include "impl/system_c.h"

// Begin Namespace IsoAgLib
namespace IsoAgLib {

/**
  Layer class to encapsulate the hardware specific details.
  All system interaction must be done via this class (and member
  classes). To avoid runtime overhead, most functions are declared inline.
  @short Central layer object to encapsulate hardware specific details which uses
    other object to implement specific I/O operations.
  @author Dipl.-Inform. Achim Spangler
*/
class iSystem_c : private __IsoAgLib::System_c {
public:
  /**
    Initialize the system hardware.
    (uses BIOS function)

    possible errors:
        * Err_c::SystemOpen problem during start of system with BIOS call
        * Err_c::SystemWatchdog the System_c::init_wd call caused an error
        * Err_c::unspecified Bios calls for TaskTimer or StayAlive caused an error
    @return true -> everything without errors initialised
  */
  bool init() { return System_c::init();}

  /** Stimulate final shutdown of ECU
    */
  bool close( void ) { return System_c::close();}

  /**
    init the hardware watchdog
    (uses BIOS function)

    possible errors:
        * Err_c::SystemWatchdog BIOS watchdog configuration without success
    @return true -> watchdog successful configured
  */
  static bool initWd ( void ) { return System_c::initWd();}

  /** trigger the watchdog */
  static inline void triggerWd( void ) { System_c::triggerWd();}

  /**
    deliver lasted time from start of system in msec.
    @return running time in [msec.]
  */
  static int32_t getTime(){return System_c::getTime();};

  /**
    @return true -> ECU's On/Off-Switch reports system being "On".
  */
  static bool switchedOn() { return System_c::switchedOn(); }

  /**
    get the main power voltage
    @return voltage of power [mV]
  */
  static int16_t getBatteryVoltage( void ) {return System_c::getBatteryVoltage();};
  /**
    get the voltage of the external reference 8.5Volt for work of external sensors
    @return voltage at external reference [mV]
  */
  static int16_t getExternalSensorPowerVoltage( void )
    { return System_c::getExternalSensorPowerVoltage();};

  /**
    deliver the serial nr of the device into uint8_t[6] array
    - f.e. to calculated individual wait berfore address claim
    @param snrDat pointer to 6 uint8_t array, where the serial no of the device is stored
  */
  static void serialNo(uint8_t *const snrDat){System_c::serialNo(snrDat);};
//  static uint32_t serialNo( void ){return System_c::serialNo();};

private:
  /** allow getIsystemInstance() access to shielded base class.
      otherwise __IsoAgLib::getSystemInstance() wouldn't be accepted by compiler
    */
  friend iSystem_c& getIsystemInstance( void );
  /** private constructor which prevents direct instantiation in user application
    * NEVER define instance of iSystem_c within application
    */
  iSystem_c( void ) : System_c() {};
};

/** C-style function, to get access to the unique System_c singleton instance */
inline iSystem_c& getIsystemInstance( void ) { return static_cast<iSystem_c&>(__IsoAgLib::getSystemInstance());};

}

#endif
