/***************************************************************************
                          eeprom.h - definition of HAL
                                     namesapce layer for
                                     debug/simulation
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
/** \file hal/pc/eeprom/eeprom.h
 * The header <i>\<target\>/\<device\>/\<device\>.h</i> performs a name
   mapping between platform specific BIOS / OS function names
   and the function names, the IsoAgLib uses for hardware access.
   In this header only the groups for error codes, global system
   functions and CAN must be adapted to a new platform.
   If a simple name mapping is not possible for a specific
   function, the module targetExtensions can be used to implement
   the needed activity. The implementation of the PC version
   can be used to derive the meaning of the specific function.
*/
/* ************************************************************ */

#ifndef _HAL_PC_EEPROM_H_
#define _HAL_PC_EEPROM_H_

#include "../config.h"
#include "../typedef.h"
#include "../errcodes.h"
#include "eeprom_target_extensions.h"

#include <cstdio>

/**
   namespace with layer of inline (cost NO overhead -> compiler replaces
   inline function with call to orig BIOS function)
   functions between all IsoAgLib calls for BIOS and the corresponding BIOS functions
   --> simply replace the call to the corresponding BIOS function in this header
       for adaptation to new platform
 */
namespace HAL
{
  /* *************************** */
  /** \name EEPROM BIOS functions */
  /*@{*/

  /**
   deliver the EEPROM size in kbyte
   @return EEPROM size in kbyte
  */
  inline int16_t getEepromSize(void)
    {return __HAL::getEepromSize();};

  /**
   deliver the EEPROM segment size in kbyte
   @return EEPROM segment size in kbyte
  */
  inline int16_t getEepromSegmentSize(void)
    {return __HAL::getEepromSegmentSize();};

  /**
    read amount of uint8_t in uint8_t string variable
    @param wAddress start adress to read
    @param wNumber amount of bytes to read
    @param pbByte pointer to uint8_t string, where data is inserted
    @return error state (C_NO_ERR == o.k.)
  */
  inline int16_t eepromRead(uint16_t wAddress,uint16_t wNumber,uint8_t *pbByte)
    {return __HAL::eepromRead(wAddress, wNumber, pbByte);};

  /**
    write amount of uint8_t from string into EEPROM from given start adress
    @param wAddress adress of first written uint8_t in EEPROM
    @param wNumber amount of written data
    @param pbData pointer to uint8_t string which should be written into EEPROM
    @return error state (C_NO_ERR == o.k.)
  */
  inline int16_t eepromWrite(uint16_t wAddress,uint16_t wNumber,uint8_t *pbData)
    {return __HAL::eepromWrite(wAddress, wNumber, pbData);};

  /**
    set or unset set write protection of EEPROM, should be called before
    every write access
    @param bitMode sets write protection OFF or ON
    @return error state (C_NO_ERR == o.k.)
  */
  inline int16_t eepromWp(bool bitMode)
    {return __HAL::eepromWp(bitMode);};

  /**
    check if EEPROM is ready for actions
    @return EE_READY -> ready
  */
  inline int16_t eepromReady(void)
    {return __HAL::eepromReady();};

 /*@}*/

#ifdef USE_CAN_EEPROM_EDITOR
  /* ********************************* */
  /** \name CAN EEPROM Editor functions */
  /*@{*/

  /**
    initialize the CAN EEPROM editor module
    @param bBus CAN BUS number to use for EEEditor
    @param iObjNrReceiveCan CAN Msg Obj number for receive of Editor msg
    @param iObjNrTransmitCan CAN Msg Obj number for send of Editor msg
    @param dwReceiveCanId CAN Ident_c to use for receive of Editor msg
    @param bUseExtendedCAN set to 0 for standard 11bit, to 1 for extended 29bit Ident
    @param iNumberMsgsReceive size of CAN receive buffer size
    @param iNumberMsgsTransmit size of CAN send buffer size
    @return HAL_NO_ERR if no error occured
  */
  inline int16_t InitEEEditor(  uint8_t bBus,
                       int16_t iObjNrReceiveCan, int16_t iObjNrTransmitCan,
                       uint32_t dwReceiveCanId, uint8_t bUseExtendedCAN,
                       int16_t iNumberMsgsReceive, int16_t iNumberMsgsTransmit)
  {return __HAL::iInitEEEditor(bBus, iObjNrReceiveCan, iObjNrTransmitCan,
          dwReceiveCanId, bUseExtendedCAN, iNumberMsgsReceive, iNumberMsgsTransmit);
  };

  /**
    periodic call to the CAN EEEditor, to process editor msg
    @return true -> EEPROM write msg recieved, and EEPROM values changed
  */
  inline int16_t ProcessCANEEEditorMsg()
  {return __HAL::iCallCanEEMonitor();};

 /*@}*/
#endif

#ifdef USE_RS232_EEPROM_EDITOR
  /* *********************************** */
  /** \name RS232 EEPROM Editor functions */
  /*@{*/

  /**
    periodic call to the RS232 EEEditor, to process editor msg
    @return true -> EEPROM write msg recieved, and EEPROM values changed
  */
  inline int16_t ProcessRS232EEEditorMsg(void)
  {return __HAL::iCallRs232EEMonitor();};
  /*@}*/
#endif

}
#endif
