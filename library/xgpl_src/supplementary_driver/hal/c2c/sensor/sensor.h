/***************************************************************************
                          sensor.h - definition of Hardware Abstraction
                                     Layer for sensor functions for C2C
                             -------------------
    begin                : Wed Mar 15 2000
    copyright            : (C) 2000 - 2009 Dipl.-Inform. Achim Spangler
						 : This file was based on the corresponding file in
						 : the ESX HAL and modified for the C2C HAL.
						 : These changes (C) 2004 - 2005 Michael D. Schmidt
    email                : a.spangler@osb-ag:de
						 : mike.schmidt@agcocorp:com
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
 * Copyright (C) 1999 - 2009 Dipl.-Inform. Achim Spangler                  *
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
/** \file
 * 
 * The header <i>\<target\>/\<device\>/\<device\>.h</i> performs a name
   mapping between platform specific BIOS / OS function names
   and the function names, the IsoAgLib uses for hardware access.
   In this header only the groups for error codes, global system
   functions and CAN must be adapted to a new platform.
   If a simple name mapping is not possible for a specific
   function, the module targetExtensions can be used to implement
   the needed activity. The implementation of the C2C version
   can be used to derive the meaning of the specific function.
*/
/* ************************************************************ */

#ifndef _HAL_C2C_SENSOR_H_
#define _HAL_C2C_SENSOR_H_

#include <IsoAgLib/hal/c2c/config.h>
#include <IsoAgLib/hal/c2c/typedef.h>
#include <IsoAgLib/hal/c2c/errcodes.h>
#include "sensor_target_extensions.h"

/* ******************************************************** */
/**
 * \name Basic BIOS limit constants
 */
/*@{*/
#define ANALOG_INPUT_MIN 0
// only uncomment following line if  ANALOG_INPUT_MIN > 0
//#define ANALOG_INPUT_MIN_GREATER_ZERO
#define ANALOG_INPUT_MAX 3
#define DIGITAL_INPUT_MIN 0
// only uncomment following line if  DIGITAL_INPUT_MIN > 0
//#define DIGITAL_INPUT_MIN_GREATER_ZERO
#define DIGITAL_INPUT_MAX 0

#define COUNTER_INPUT_MIN 0
// only uncomment following line if  COUNTER_INPUT_MIN > 0
//#define COUNTER_INPUT_MIN_GREATER_ZERO
#define COUNTER_INPUT_MAX 0

/// set maximal voltage input for analog input set to voltage in [mV]
#define MAX_ANALOG_MV 8500
///set max digital BIOS function return value corresponding to MAX_ANALOG_MV
#define MAX_ANALOG_MV_DIGIT 907
/// set maximal current input for analog input set [mA]
#define MAX_ANALOG_MA 20
///set max digital BIOS function return value corresponding to MAX_ANALOG_MA
#define MAX_ANALOG_MA_DIGIT 833

///set the max voltage that can be read off the ADC [V].  There are 2 variants of C2Cs
///Those with single CAN (that we have) are setup to be 0-30 V
///Those with dual CAN (that we have)  are setup to be 0-10 V
// Override this in your config_xxx.h file if you use single CAN C2Cs with 0-30 V range.
#ifndef MAX_ADC_V
#  define MAX_ADC_V 10
#endif

/*@}*/

namespace __HAL {
  extern "C" {
    /** include the BIOS specific header into __HAL */
    #include <commercial_BIOS/bios_c2c/c2c10osy.h>
  }
  /**
    deliver channel number for checking/requesting of analog input
    for call of __HAL:: functions (differences mostly caused
    by multiplexers)
    @param channel number from interval [0..maxNo] == [0..3]
    @return according channel number for __HAL call
  */
  inline uint8_t getAnaloginCheckNr(uint8_t ab_channel)
    {return (GET_A_IN_1 + ab_channel);};
}

/**
   namespace with layer of inline (cost NO overhead -> compiler replaces
   inline function with call to orig BIOS function)
   functions between all IsoAgLib calls for BIOS and the corresponding BIOS functions
   --> simply replace the call to the corresponding BIOS function in this header
       for adaptation to new platform
 */
namespace HAL
{
  /* ********************************* */
  /** \name Sensor_c Input BIOS functions */
/*@{*/

  /**
    initialize one of the [0..3] analog input channels to VOLTAGE input
    @param bNumber number of the analog input channel
    @param bitPullDownResistor set to TRUE to add an additional pull down resistor
    @return error state (C_NO_ERR == o.k.)
  */
  inline int16_t  init_analoginVolt(uint8_t bNumber, bool bitPullDownResistor = FALSE)
    {return __HAL::init_analogin(bNumber, bitPullDownResistor);};

  /**
    initialize one of the [0..7] analog input channels to CURRENT input
    @param bNumber number of the analog input channel
    @return error state (C_NO_ERR == o.k.)
  */
  inline int16_t  init_analoginCurrent(uint8_t bNumber)
    {return HAL_CONFIG_ERR;};

  /**
    initialize one of the [0..0] digital input channels
    @param ab_channel number of the digital input channel
    @param bMode input mode {DIGIN, RISING_EDGE, FALLING_EDGE, BOTH_EDGE}
    @param bAktivhighlow {HIGH_ACTIV, LOW_ACTIV}
    @param pfFunctionName adress of function which is called on input events (NULL -> none)
    @return error state (C_NO_ERR == o.k.)
  */
  /* The C2C has only 1 channel (DIN1), so ab_channel must be set to DIN1 otherwise C_RANGE is returned */
  /* The C2C also does not support bAktivhighlow and so bAktivhighlow is ignored */
  inline int16_t init_digin(uint8_t ab_channel,uint8_t bMode,uint8_t bAktivhighlow,void (*pfFunctionName)())
    {
	int16_t ret;
	if( ab_channel == DIN1 )
	    ret = __HAL::init_digin(bMode, pfFunctionName);
	else
		ret = C_RANGE;
//{
//    byte pbString[128];
//    sprintf( (char*)pbString, "HAL::init_digin(%u,%u) returned %u\r", (unsigned int)bMode, (unsigned int)(pfFunctionName!=NULL), (unsigned int)ret );
//    __HAL::put_rs232_string(RS232_1, pbString);
//}
	return ret;
	}

  /**
    init counter for trigger events on digital inoput;
    rising edges are counted;
    @param ab_channel input channel to use [0..15]
    @param aui16_timebase timebase to calculate periods, frequency
                       should be at least longer than longest
                       awaited signal period [msec.]
    @param ab_activHigh true -> counter input is configured fo ACTIV_HIGH; else ACTIV_LOW
    @param ab_risingEdge true -> counter triggers on rising edge; else on falling edge
    @return C_NO_ERR if no error occured
  */
  inline int16_t init_counter(uint8_t ab_channel, uint16_t aui16_timebase, bool ab_activHigh, bool ab_risingEdge)
  {return __HAL::init_counter(aui16_timebase, ab_risingEdge);};
  /**
    get counter value of an digital counter input
   @param ab_channel channel of counter [0..15]
    @return counter events since init or last reset
  */
  inline uint32_t getCounter(uint8_t ab_channel)
  {  return (ab_channel == DIN1) ? __HAL::getCounter() : 0; };
  /**
    reset the given counter
   @param ab_channel channel of counter [0..15]
    @return C_NO_ERR ; C_RANGE if counter for ab_channel isn�t configured properly
  */
  inline int16_t resetCounter(uint8_t ab_channel)
  {  return (ab_channel == DIN1) ? __HAL::resetCounter() : C_RANGE; };
  /**
    get period of counter channel
    @param ab_channel channel of counter [0..15]
    @return time between last two signals or 0xFFFF if time is longer than initially
             given timebase
  */
  inline uint16_t getCounterPeriod(uint8_t ab_channel)
  {  return (ab_channel == DIN1) ? __HAL::getCounterPeriod() : 0xFFFF; };
  /**
    get frequency of counter channel
   @param ab_channel channel of counter [0..15]
    @return frequency calculated from time between last two signals
            or 0 if time is longer than initially given timebase
  */
  inline uint16_t getCounterFrequency(uint8_t ab_channel)
  {  return (ab_channel == DIN1) ? __HAL::getCounterFrequency() : 0; };
  /**
   get time since last signal
   @param ab_channel channel of counter
   @return time since last signal [msec.]
  */
  inline uint32_t getCounterLastSignalAge(uint8_t ab_channel)
  {  return (ab_channel == DIN1) ? __HAL::getCounterLastSignalAge() : 0xFFFF; };

  /**
    set fast ADC mode ON or OFF
    !!! the C2C doesn't provide setFastAnalogin !!
    @param bMode set fast ADC to ON or OFF
  */
  inline void setFastAnalogin(bool bMode)
    {return;};

  /**
    get the measured voltage value of a channel in [mV]
    @param ab_channel measured channel
    @return voltage [0..8500] [mV] or C_RANGE on wrong input channel number
  */
  inline int16_t  getAdcVoltage(uint8_t ab_channel)
    {int16_t i16_temp = __HAL::get_adc(__HAL::getAnaloginCheckNr(ab_channel));
     if ( i16_temp == C_RANGE ) return C_RANGE;
     return (i16_temp * MAX_ADC_V); }
  /**
    get the MEDIUM of measured voltage value of a channel in [mV]
    @param ab_channel measured channel
    @return voltage [0..8500] [mV] or C_RANGE on wrong input channel number
  */
  inline int16_t  getAdcMeanVoltage(uint8_t ab_channel)
    {int16_t i16_temp = __HAL::get_adc_mean(__HAL::getAnaloginCheckNr(ab_channel));
     if ( i16_temp == C_RANGE ) return C_RANGE;
     return (i16_temp * MAX_ADC_V); }
  /**
    get the measured current value of a channel in [uA]
    @param ab_channel measured channel
    @return current [4000..20000] [uA] or C_RANGE on wrong input channel number
  */
  inline int16_t  getAdcCurrent(uint8_t ab_channel)
    { return C_RANGE; }
  /**
    get the MEDIUM of measured current value of a channel in [uA]
    @param ab_channel measured channel
    @return current [4000..20000] [uA] or C_RANGE on wrong input channel number
  */
  inline int16_t  getAdcMeanCurrent(uint8_t ab_channel)
    { return C_RANGE; }
  /**
    get the diagnose ADC value from specified DIGITAL INPUT channel
    @param ab_channel channel number [0..15]
    @return ADC diagnose voltage [ or C_RANGE on wrong input channel number
  */
  inline int16_t  getDiginDiagnoseAdc(uint8_t ab_channel)
    { return C_RANGE; }
  /**
    get the temperature
    @return temperature in degree [-40..85]
  */
  inline int16_t  getAdcTemp( void )
    { return __HAL::get_boardtemperature(); }
   #if !defined(GET_U_THRESHOLD) && defined(GET_U_2_3_V)
     #define GET_U_THRESHOLD GET_U_2_3_V
   #endif

  /**
    deliver state of digital input based on Activ-High/Low setting
    (evalutation of sensor signals independent from switching type)
    @param ab_channelNumber input channel number [DIN1..DIN16]
    @return ON, OFF or C_RANGE
  */
  inline int16_t  getDiginOnoff(uint8_t ab_channelNumber)
    {return __HAL::get_digin_hilo();};

  /**
    deliver debounced state of digital input based on Activ-High/Low setting
    (evalutation of sensor signals independent from switching type)
    @param ab_channelNumber input channel number [DIN1..DIN16]
    @return ON, OFF or C_RANGE
  */
  inline int16_t  getDiginOnoffStatic(uint8_t ab_channelNumber)
    {return __HAL::get_digin_hilo_static();};

  /**
    deliver frequency of digital interrupt channel
    (this function uses default value for timebase for
     calculating frequency == 1,67sec. for 20MHz CPU)
    @param ab_channelNumber input channel number [DIN1..DIN16]
    @param b_useVirtual (default false) use virtual calculated
           value, if last trigger is lasted longer than last period
           -> virt_val uses time from last trigger to actual time
    @return frequency of triggered events [mHz] or BIOS_WARN on too less impulses
  */
  inline uint16_t getDiginFreq(uint8_t ab_channelNumber, bool b_useVirtual = false)
    {uint16_t ui16_result;
     return ((__HAL::get_digin_freq(&ui16_result) == C_NO_ERR) || (b_useVirtual))?ui16_result:0;};

  /*@}*/
}
#endif
