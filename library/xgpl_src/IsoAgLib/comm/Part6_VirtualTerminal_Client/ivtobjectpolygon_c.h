/***************************************************************************
                            ivtobjectpolygon_c.h
                             -------------------
    begin                : Don Sep 4 2003
    copyright            : (C) 2003-2004 by Martin Wodok / Bradford Cox
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


#ifndef IVTOBJECTPOLYGON_C_H
#define IVTOBJECTPOLYGON_C_H


// +X2C includes
#include "impl/vtobjectpolygon_c.h"
// ~X2C

// Begin Namespace IsoAgLib
namespace IsoAgLib {



//  +X2C Class 119 : iVtObjectPolygon_c
//!  Stereotype: Klasse
class iVtObjectPolygon_c : public __IsoAgLib::vtObjectPolygon_c
{

public:

  //  Operation: init
  //! Parameter:
  //! @param vtObjectpolygonSROM:
  //! @param b_initPointer:
  void init(const iVtObjectPolygon_s* vtObjectPolygonSROM SINGLETON_VEC_KEY_PARAMETER_DEF_WITH_COMMA) {
    vtObjectPolygon_c::init (vtObjectPolygonSROM SINGLETON_VEC_KEY_PARAMETER_USE_WITH_COMMA);
  }

  //  Operation: get_vtObjectPolygon_a
  const iVtObjectPolygon_s& get_vtObjectPolygon_a() { return *vtObjectPolygon_c::get_vtObjectPolygon_a(); }

  //  Operation: setWidth
  //! Parameter:
  //! @param newWidth:
  //! @param b_updateObject:
  void setWidth(uint16_t newWidth, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) {
    vtObjectPolygon_c::setWidth (newWidth, b_updateObject, b_enableReplaceOfCmd);
  }

  //  Operation: setHeight
  //! Parameter:
  //! @param newHeight:
  //! @param b_updateObject:
  void setHeight(uint16_t newHeight, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) {
    vtObjectPolygon_c::setHeight (newHeight, b_updateObject, b_enableReplaceOfCmd);
  }

  //  Operation: setLineAttributes
  //! Parameter:
  //! @param newLineAttributes:
  //! @param b_updateObject:
  void setLineAttributes(IsoAgLib::iVtObjectLineAttributes_c* newLineAttributes, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) {
    vtObjectPolygon_c::setLineAttributes (newLineAttributes, b_updateObject, b_enableReplaceOfCmd);
  }

  //  Operation: setFillAttributes
  //! Parameter:
  //! @param newFillAttributes:
  //! @param b_updateObject:
  void setFillAttributes(IsoAgLib::iVtObjectFillAttributes_c* newFillAttributes, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) {
    vtObjectPolygon_c::setFillAttributes (newFillAttributes, b_updateObject, b_enableReplaceOfCmd);
  }

  //  Operation: setPolygonType
  //! Parameter:
  //! @param newPolygonType:
  //! @param b_updateObject:
  void setPolygonType(uint8_t newPolygonType, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) {
    vtObjectPolygon_c::setPolygonType (newPolygonType, b_updateObject, b_enableReplaceOfCmd);
  }

  //  Operation: setSize
  //! Parameter:
  //! @param newWidth:
  //! @param newHeight:
  //! @param b_updateObject:
  void setSize(uint16_t newWidth, uint16_t newHeight, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) {
    vtObjectPolygon_c::setSize(newWidth, newHeight, b_updateObject, b_enableReplaceOfCmd);
  }

  // ///////////////////////// getter for attributes
  /** that attribute is in parentheses in the spec, so commented out here
  uint8_t updateObjectType() const { return vtObjectPolygon_c::updateObjectType(); }
  */

  uint16_t updateWidth(bool b_SendRequest=false) {
    return vtObjectPolygon_c::updateWidth(b_SendRequest);
  }

  uint16_t updateHeight(bool b_SendRequest=false) {
    return vtObjectPolygon_c::updateHeight(b_SendRequest);
  }

  uint16_t updateLineAttributes(bool b_SendRequest=false) {
    return vtObjectPolygon_c::updateLineAttributes(b_SendRequest);
  }

  uint16_t updateFillAttributes(bool b_SendRequest=false) {
    return vtObjectPolygon_c::updateFillAttributes(b_SendRequest);
  }

  uint8_t updatePolygonType(bool b_SendRequest=false) {
    return vtObjectPolygon_c::updatePolygonType(b_SendRequest);
  }
};

} // end namepsace IsoAgLibb

#endif