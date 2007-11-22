/***************************************************************************
                          sensor_target_extensions.h - header for C2C specific
                                                extensions for the HAL
                                                for sensor input
                             -------------------
    begin                : Wed Mar 15 2000
    copyright            : (C) 2000 - 2004 Dipl.-Inform. Achim Spangler
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
 * Copyright (C) 1999 - 2004 Dipl.-Inform. Achim Spangler                 *
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

/* ********************************************************** */
/** \file IsoAgLib/hal/c2c/sensor/sensor_target_extensions.h
 * A module targetExtensions should be used
 * for all methods, which can't be simply 
 * mapped from ECU standard BIOS to the needs of
 * IsoAgLib by mostly renaming and reordering of functions, parameters
 * and types in <i>\<target\>/\<device\>/\<device\>.h</i> .
 * ********************************************************** */

#ifndef _HAL_C2C_SENSOR_TARGET_EXTENSIONS_H_
#define _HAL_C2C_SENSOR_TARGET_EXTENSIONS_H_

#include <IsoAgLib/hal/c2c/typedef.h>
#include <IsoAgLib/hal/c2c/errcodes.h>
namespace __HAL {

/* **************************************************** */
/** \name Sensor_c specific BIOS Extenders                */
/*@{*/
/**
  init counter for trigger events on digital input;
  rising edges are counted;
  @param aui16_timebase timebase to calculate periods, frequency
                     should be at least longer than longest
                     awaited signal period [msec.]
  @param ab_risingEdge true -> counter triggers on rising edge; else on falling edge
  @return C_NO_ERR if no error occured
*/
int16_t init_counter(uint16_t aui16_timebase, bool ab_risingEdge);
/**
  get counter value of an digital counter input
  @return counter events since init or last reset
*/
uint32_t getCounter();
/**
  reset the given counter
  @return C_NO_ERR ; C_RANGE if counter if it isn�t configured properly
*/
int16_t resetCounter();
/**
  get period of counter channel
  @return time between last two signals or 0xFFFF if time is longer than initially
           given timebase
*/
uint16_t getCounterPeriod();
/**
  get frequency of counter channel
  @return frequency calculated from time between last two signals
          or 0 if time is longer than initially given timebase
*/
uint16_t getCounterFrequency();
/**
 get time since last signal and reset according trigger timers
 if timebase is exceeded -> avoid overflow problems if timer floated around 0xFFFF
 @return time since last signal [msec.]
*/
uint16_t getCounterLastSignalAge();
/*@}*/
} // end of namespace __HAL
#endif