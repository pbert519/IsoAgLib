/***************************************************************************
                           ivtobjectworkingset_c.h
                             -------------------
    begin                : Mon Jun 30 14:41:54 2003
    copyright            : (C) 2003 by Martin Wodok
    email                : m.wodok@osb-ag.de
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
 * Copyright (C) 2000 - 2004 Dipl.-Inform. Achim Spangler                  *
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
 * the main author Achim Spangler by a.spangler@osb-ag.de                  * 
 ***************************************************************************/ 

#ifndef IVTOBJECTWORKINGSET_C_H
#define IVTOBJECTWORKINGSET_C_H


// +X2C includes
#include "impl/vtobjectworkingset_c.h"
#include "ivtobjectmask_c.h"
// ~X2C

// Begin Namespace IsoAgLib
namespace IsoAgLib {


//  +X2C Class 66 : iVtObjectWorkingSet_c
//!  Stereotype: Klasse
class iVtObjectWorkingSet_c : public __IsoAgLib::vtObjectWorkingSet_c
{

public:

  //  Operation: init
  //! Parameter:
  //! @param vtObjectWorkingSetSROM:
  //! @param b_initPointer:
  void init(const iVtObjectWorkingSet_s* vtObjectWorkingSetSROM) {
    vtObjectWorkingSet_c::init (vtObjectWorkingSetSROM);
  };

  //  Operation: get_vtObjectWorkingSet_a
  iVtObjectWorkingSet_s* get_vtObjectWorkingSet_a() { return vtObjectWorkingSet_c::get_vtObjectWorkingSet_a(); };

  //  Operation: changeActiveMask
  //! Parameter:
  //! @param rpc_vtObjectMask:
  //! @param b_updateObject:
  void changeActiveMask(iVtObjectMask_c* rpc_iVtObjectMask, bool b_updateObject= false) {
    vtObjectWorkingSet_c::changeActiveMask (rpc_iVtObjectMask, b_updateObject);
  };

  //  Operation: changeBackgroundColour
  //! Parameter:
  //! @param newValue:
  //! @param b_updateObject:
  void changeBackgroundColour(uint8_t newValue, bool b_updateObject= false) {
    vtObjectWorkingSet_c::changeBackgroundColour (newValue, b_updateObject);
  };

  //  Operation: moveChildLocation
  //! Parameter:
  //! @param rpc_childObject:
  //! @param dx:
  //! @param dy:
  //! @param b_updateObject:
  bool moveChildLocation(IsoAgLib::iVtObject_c* rpc_childObject, int8_t dx, int8_t dy, bool b_updateObject) {
    return vtObjectWorkingSet_c::moveChildLocation(rpc_childObject, dx, dy, b_updateObject);
  }

}; // ~X2C

} // end of namespace IsoAgLib

#endif // -X2C
