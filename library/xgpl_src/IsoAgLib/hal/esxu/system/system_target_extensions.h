/*
  system_target_extensions.h: header for ESXu specific extensions for
    the HAL for central system

  (C) Copyright 2009 - 2010 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/

/** \file IsoAgLib/hal/esxu/system/system_target_extensions.h
 * A module targetExtensions should be used
 * for all methods, which can't be simply 
 * mapped from ECU standard BIOS to the needs of
 * IsoAgLib by mostly renaming and reordering of functions, parameters
 * and types in <i>\<target\>/\<device\>/\<device\>.h</i> .
 * ********************************************************** */

#ifndef _HAL_ESXu_SYSTEM_TARGET_EXTENSIONS_H_
#define _HAL_ESXu_SYSTEM_TARGET_EXTENSIONS_H_

namespace __HAL {
  extern "C" {
    /** include the BIOS specific header into __HAL */
    #include <commercial_BIOS/bios_esxu/mos10osy.h>
  }
}

//#include <cstdio>
#include <cstdio>
#include <IsoAgLib/hal/esxu/config.h>
#include <IsoAgLib/hal/esxu/typedef.h>
#include <IsoAgLib/hal/esxu/errcodes.h>

#if defined( DEBUG_HAL )
#  include <supplementary_driver/driver/rs232/irs232io_c.h>
#endif

namespace __HAL {

/* ******************************************** */
/** \name General BIOS Extenders                */
/*@{*/
/**
  open the system with system specific function call
  @return error state (C_NO_ERR == o.k.)
*/
int16_t open_system( void );
/**
  close the system with system specific function call
  @return error state (C_NO_ERR == o.k.)
*/
int16_t closeSystem( void );
/** check if open_System() has already been called */
bool isSystemOpened( void );

/**
  configure the watchdog of the system with the
  settings of configESXu
  @return error state (C_NO_ERR == o.k.)
    or DATA_CHANGED on new values -> need call of wdReset to use new settings
  @see wdReset
*/
int16_t configWatchdog( void );
/*@}*/
}

#endif
