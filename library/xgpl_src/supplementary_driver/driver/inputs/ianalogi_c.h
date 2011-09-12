/*
  ianalogi_c.h:
    interface header file for iAnalogI_c, an object for analog input

  (C) Copyright 2009 - 2011 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/
#ifndef IANALOGI_C_H
#define IANALOGI_C_H

#include "impl/analogi_c.h"


// Begin Namespace IsoAgLib
namespace IsoAgLib {

/**Interface class for Analog input objects
  *@author Dipl.-Inform. Achim Spangler
  */
class iAnalogI_c : private __IsoAgLib::AnalogI_c
{
public:
  /**
    Promote baseclass private enums to public
  */
    typedef InputBase_c::analogType_t analogType_t;
    typedef InputBase_c::state_t state_t;
    typedef InputBase_c::onoff_t onoff_t;
    typedef InputBase_c::inputType_t inputType_t;

  /**
    internal called constructor which creates a new input channel,initialize the hardware and configures conversion calculation

    possible errors:
        * Err_c::range wrong input number
        * Err_c::precondition wrong input type
    @see iInputs_c::createAnalog
    @see t_analogType
    @param ab_channel default-argument for the hardware channel of the input
    @param ren_analogType default-argument for choosing voltage(default) or current as input type
    @param ab_useMean default-argument for setting the calculation of mean value on true (false as default)
    @param ab_fastAdc default-argument for setting fast ADC method (false as default)
  */
  iAnalogI_c(uint8_t ab_channel = 0xFF, analogType_t ren_analogType = voltage, bool ab_useMean = false, bool ab_fastAdc = false )
    : AnalogI_c(ab_channel, ren_analogType, ab_useMean, ab_fastAdc ) {}

  /**
    internal called constructor which creates a new input channel,initialize the hardware and configures conversion calculation

    possible errors:
        * Err_c::range wrong input number
        * Err_c::precondition wrong input type
    @see iInputs_c::createAnalog
    @see t_analogType
    @param ab_channel default-argument for the hardware channel of the input
    @param ren_analogType default-argument for choosing voltage(default) or current as input type
    @param ab_useMean default-argument for setting the calculation of mean value on true (false as default)
    @param ab_fastAdc default-argument for setting fast ADC method (false as default)
  */
  void init(uint8_t ab_channel, analogType_t ren_analogType = voltage, bool ab_useMean = false, bool ab_fastAdc = false )
    { AnalogI_c::init(ab_channel, ren_analogType, ab_useMean, ab_fastAdc );}

  /** destructor which can close the hardware input channel */
  virtual ~iAnalogI_c() {}

  /**
    get the actual input value with the configured linear conversion (use the configured ADC method)
    (uses BIOS function)

    @return input value: A) Volt [mV], or B) Ampere [mA]
  */
  uint16_t val() const {return AnalogI_c::val();}

  /**
    check if value is greater than 0

    possible errors:
        * Err_c::range wrong input number
    @return true if input value is different from 0, otherwise 0
  */
  bool active() const {return AnalogI_c::active();}

  /**
    configure fast ADC gathering
    @param ab_useFast default-argument for setting fast ADC (true as default)
  */
  void setFastAdc(bool ab_useFast=true){AnalogI_c::setFastAdc(ab_useFast);}

  /**
    deliver the channel number of the output object
    @return number to use for BIOS access to this channel
  */
  uint8_t channelNr() const { return AnalogI_c::channelNr();}

private:
  /**
    HIDDEN! copy constructor for AnalogI_c
    NEVER copy a AnalogI_c around!!!!
    ONLY copy pointers to the wanted instance!!!
    ==> the copy constructor is defined as private, so that compiler
        detects this fault, and shows you this WARNING!!
    <!--@param acrc_src source-->
  */
  iAnalogI_c(const iAnalogI_c &);

  /**
    HIDDEN! assignment for AnalogI_c
    NEVER assign a AnalogI_c to another instance!!!!
    ==> the asignment is defined as private, so that compiler
        detects this fault, and shows you this WARNING!!
    <!--@param acrc_src source-->
  */
  iAnalogI_c& operator=(const iAnalogI_c &);
};

} // IsoAgLib
#endif
