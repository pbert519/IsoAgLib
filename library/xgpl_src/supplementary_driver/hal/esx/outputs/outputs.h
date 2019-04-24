/*
  outputs.h: definition of HAL namesapce layer for ESX

  (C) Copyright 2009 - 2019 by OSB AG

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Usage under Commercial License:
  Licensees with a valid commercial license may use this file
  according to their commercial license agreement. (To obtain a
  commercial license contact OSB AG via <http://isoaglib.com/en/contact>)

  Usage under GNU General Public License with exceptions for ISOAgLib:
  Alternatively (if not holding a valid commercial license)
  use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/

#ifndef _HAL_ESX_OUTPUTS_H_
#define _HAL_ESX_OUTPUTS_H_

#include <IsoAgLib/isoaglib_config.h>
#include <IsoAgLib/hal/esx/typedef.h>
#include <IsoAgLib/hal/esx/errcodes.h>

/* ******************************************************** */
/**
 * \name Basic BIOS limit constants
 */
/*@{*/
#define DIGITAL_OUTPUT_MIN 0
#ifdef _INIT_BABYBOARD_
#define DIGITAL_OUTPUT_MAX 40 // @todo Set to the right value for Babyboard Outputs...
#else
#define DIGITAL_OUTPUT_MAX 11
#endif
/*@}*/

namespace __HAL {
  extern "C" {
    /** include the BIOS specific header into __HAL */
    #include <commercial_BIOS/bios_esx/Xos20esx.h>

    #ifdef _INIT_BABYBOARD_
    #include <commercial_BIOS/bios_esx/Module/xma20.h>
    #endif
  }

  /**
    deliver channel number for checking/requesting of
    ADC value at pwm output
    for call of __HAL:: functions (differences mostly caused
    by multiplexers)
    @param channel number from interval [0..maxNo]
    @return according channel number for __HAL call
  */
  inline uint8_t getPwmoutAdcCheckNr(uint8_t channel)
    {
    if (channel < 4) return (43 - (channel * 2));
    else if (channel == 4) return 44;
    else if (channel < 8) return ((47+5) - channel);
    else return ((93+8) - channel);
    };

  /**
    deliver channel number for checking/requesting of
    current output through given PWM output
    @param channel number from interval [0..maxNo]
    @return according channel number for __HAL call
  */
  inline uint8_t getPwmCurrentCheckNr(uint8_t channel)
    {return (42 - (2 * channel));};
}

namespace HAL
{
  /* **************************** */
  /** \name Output BIOS functions */
/*@{*/

  static unsigned isCachedMaxPwm[ DIGITAL_OUTPUT_MAX-DIGITAL_OUTPUT_MIN+1 ] = {}; // all fields will be init'd to 0, meaning "false". Use this instead of bool for this reason
  static uint16_t cachedMaxPwm[ DIGITAL_OUTPUT_MAX-DIGITAL_OUTPUT_MIN+1 ];

  inline void setMainRelais(bool bitState)
  { __HAL::set_relais(bitState); }
  
  // ESX BIOS lets PWM channels 0 to 3 configure individual PWM FREQ
  // rest of PWM channels [4..11] use common PWM freq
  inline int16_t setPwmFreq(uint8_t bOutput, uint32_t dwFrequency)
  {
    isCachedMaxPwm[ bOutput ] = 0; // false

    if (bOutput < 4)
      return __HAL::set_pwm_freq(bOutput, dwFrequency);
    #ifndef _INIT_BABYBOARD_
    else
      return __HAL::set_pwm_freq(4, dwFrequency);
    #else
    else if (bOutput < 12)
      return __HAL::set_pwm_freq(4, dwFrequency);
    // the BA_set_pwm_freq function counts the babyboard PWM channels beginning with 0
    // --> 12 channels on core ESX --> use offset 12
    // the babyboard PWM Freq is given directly in Hz (and not mHz as for standard core PWMs)
    else return __HAL::BA_set_pwm_freq(POSITION_1, (bOutput-12), dwFrequency/100);
    #endif
  }


  inline uint16_t getMaxPwmDigout(uint8_t aui8_channel)
  { 
    if( isCachedMaxPwm[ aui8_channel ] == 0 ) // false
	{
      #ifndef _INIT_BABYBOARD_
      __HAL::tOutput tOutputstatus;
      __HAL::get_digout_status(aui8_channel,&tOutputstatus);

      cachedMaxPwm[ aui8_channel ] = tOutputstatus.wMaxOutput;
      #else
      if ( aui8_channel < 12 )
      { __HAL::tOutput tOutputstatus;
        __HAL::get_digout_status(aui8_channel,&tOutputstatus);

        cachedMaxPwm[ aui8_channel ] = tOutputstatus.wMaxOutput;
      }
      else
      { __HAL::tBAOutput tOutputstatus;
        __HAL::BA_get_digout_status(POSITION_1, (aui8_channel-12),&tOutputstatus);

        cachedMaxPwm[ aui8_channel ] = tOutputstatus.wMaxOutput;
      }
      #endif

      isCachedMaxPwm[ aui8_channel ] = 1; // true
	}

    return cachedMaxPwm[ aui8_channel ];
  }

  void setDigout(uint8_t aui8_channel, uint16_t wPWMValue);
  uint16_t getDigout( uint8_t bOutputNo );

  inline int16_t getDigoutCurrent( uint8_t aui8_channel )
  {
    if ( aui8_channel < 5 )
    {
      int16_t i16_currentTemp = __HAL::get_adc( __HAL::getPwmCurrentCheckNr( aui8_channel ));
      int16_t i16_current = ((i16_currentTemp * 3) + ((i16_currentTemp * 5)/100));
      return i16_current;
    }
    return -1;
  }

  int16_t getDigoutDiagnose(uint8_t aui8_channel, uint16_t aui16_minCurrent, uint16_t aui16_maxCurrent);
  int16_t getDigoutAdc( uint8_t aui8_channel );

  /*@}*/

}

#endif
