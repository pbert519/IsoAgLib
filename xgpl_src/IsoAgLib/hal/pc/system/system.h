/***************************************************************************
                          system.h - definition of Hardware Abstraction
                                     Layer for central system functions
                                     for debug/simulation
                                     system on PC
                             -------------------
    begin                : Wed Mar 15 2000
    copyright            : (C) 2000 - 2004 Dipl.-Inform. Achim Spangler
    email                : a.spangler@osb-ag:de
    type                 : Header
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 * This file is part of the "IsoAgLib", an object oriented program library *
 * to serve as a software layer between application specific program and   *
 * communication protocol details. By providing simple function calls for  *
 * jobs like starting a measuring program for a process data value on a    *
 * remote ECU, the main program has not to deal with single CAN telegram   *
 * formatting. This way communication problems between ECU's which use     *
 * this library should be prevented.                                       *
 * Everybody and every company is invited to use this library to make a    *
 * working plug and play standard out of the printed protocol standard.    *
 *                                                                         *
 * Copyright (C) 1999 - 2004 Dipl.-Inform. Achim Spangler                  *
 *                                                                         *
 * The IsoAgLib is free software; you can redistribute it and/or modify it *
 * under the terms of the GNU General Public License as published          *
 * by the Free Software Foundation; either version 2 of the License, or    *
 * (at your option) any later version.                                     *
 *                                                                         *
 * This library is distributed in the hope that it will be useful, but     *
 * WITHOUT ANY WARRANTY; without even the implied warranty of              *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU       *
 * General Public License for more details.                                *
 *                                                                         *
 * You should have received a copy of the GNU General Public License       *
 * along with IsoAgLib; if not, write to the Free Software Foundation,     *
 * Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA           *
 *                                                                         *
 * As a special exception, if other files instantiate templates or use     *
 * macros or inline functions from this file, or you compile this file and *
 * link it with other works to produce a work based on this file, this file*
 * does not by itself cause the resulting work to be covered by the GNU    *
 * General Public License. However the source code for this file must still*
 * be made available in accordance with section (3) of the                 *
 * GNU General Public License.                                             *
 *                                                                         *
 * This exception does not invalidate any other reasons why a work based on*
 * this file might be covered by the GNU General Public License.           *
 *                                                                         *
 * Alternative licenses for IsoAgLib may be arranged by contacting         *
 * the main author Achim Spangler by a.spangler@osb-ag:de                  *
 ***************************************************************************/

/* ************************************************************ */
/** \file IsoAgLib/hal/pc/system/system.h
 * The header <i>\<target\>/\<device\>/\<device\>.h</i> performs a name
   mapping between platform specific BIOS / OS function names
   and the function names, the IsoAgLib uses for hardware access.
   In this header the groups for error codes, global system
   functions and CAN must be adopted to a new platform.
   If a simple name mapping is not possible for a specific
   function, the module targetExtensions can be used to implement
   the needed activity. The implementation of the PC version
   can be used to derive the meaning of the specific function.
*/
/* ************************************************************ */
#ifndef _HAL_PC_SYSTEM_H_
#define _HAL_PC_SYSTEM_H_

#include "../config.h"
#include "../typedef.h"
#include "../errcodes.h"
#include "system_target_extensions.h"


/** decide if HAL for PC shall use simulated sensor input for U_BAT and
    reference voltage monitoring - but in this case the sensor package
		of IsoAgLib must be installed */
#define USE_SENSOR_FOR_SYSTEM_MONITORING 0

#if USE_SENSOR_FOR_SYSTEM_MONITORING
	#include <supplementary_driver/hal/pc/sensor/sensor_target_extensions.h>
#endif

#include <cstdio>



/**
   namespace with layer of inline (cost NO overhead -> compiler replaces
   inline function with call to orig BIOS function)
   functions between all IsoAgLib calls for BIOS and the corresponding BIOS functions
   --> simply replace the call to the corresponding BIOS function in this header
       for adaption to new platform
 */
namespace HAL
{
  /* *********************************** */
  /** \name Global System BIOS functions
    */
  /*@{*/
#ifndef SYSTEM_PC_VC
  using std::printf;
  using std::scanf;
  using std::sscanf;
  using std::sprintf;
#endif
  /**
    open the system with system specific function call
    @return error state (HAL_NO_ERR == o.k.)
  */
  inline int16_t  open_system()
  {
      return __HAL::open_system();
  };
  /**
    close the system with system specific function call
    @return error state (HAL_NO_ERR == o.k.)
  */
  inline int16_t  closeSystem()
  {
      return __HAL::closeSystem();
  };
  /** check if open_System() has already been called */
  inline bool isSystemOpened( void ) { return __HAL::isSystemOpened();};

  /**
    configure the watchdog of the system with the
    settings of configEsx
    @return error state (HAL_NO_ERR == o.k.)
      or DATA_CHANGED on new values -> need call of wdReset to use new settings
    @see wdReset
  */
  inline int16_t configWatchdog()
    {return __HAL::configWatchdog();};

  /**
    reset the watchdog to use new configured watchdog settings
    @see configWd
  */
  inline int16_t  wdReset(void)
    {return __HAL::wdReset();};

  /** trigger the watchdog */
  inline void wdTriggern(void)
    {__HAL::wdTriggern();};

  /**
    get the system time in [ms]
    @return [ms] since start of the system
  */
  inline int32_t getTime(void)
    {return __HAL::getTime();};

  inline int16_t getSnr(uint8_t *snrDat)
    {return __HAL::getSnr(snrDat);};

  /**
    start the Task Timer -> time between calls of Task Manager
  */
  inline void startTaskTimer ( void )
    {__HAL::startTaskTimer ();};

  /**
    check if D+/CAN_EN is active
    (if NO_CAN_EN_CHECK is defined this function return always return ON)
    @return ON(1) or OFF(0)
  */
  inline int16_t  getOn_offSwitch(void)
  {
    #if defined(NO_CAN_EN_CHECK)
      return ON;
    #else
      return __HAL::getOn_offSwitch();
    #endif
  };

  /**
    get the main power voltage
    @return voltage of power [mV]
  */
  inline int16_t  getAdcUbat( void )
	#if USE_SENSOR_FOR_SYSTEM_MONITORING
    {return (33 * __HAL::getAdc(GET_U_C));};
	#else
		{return 13500;};
	#endif
  /**
    get the voltage of the external reference 8.5Volt for work of external sensors
    @return voltage at external reference [mV]
  */
  inline int16_t  getAdc_u85( void )
	#if USE_SENSOR_FOR_SYSTEM_MONITORING
    {int16_t i16_temp = __HAL::getAdc(GET_U_EXT_8_5_V);
     return ((i16_temp * 14) + ((i16_temp * 67)/100));};
	#else
	{return 8500;};
	#endif

  /**
    activate the power selfholding to perform system
    stop (f.e. store values) actions after loss of CAN_EN
  */
  inline void stayingAlive(void)
    {__HAL::stayingAlive();};

  /**
    shut off the whole system (set power down)
  */
  inline void powerDown(void)
    {__HAL::powerDown();};

  /**
    switch relais on or off
    @param bitState true -> Relais ON
  */
  inline void setRelais(bool bitState)
  {__HAL::setRelais(bitState);};

/*@}*/


}
#endif
