/***************************************************************************
                          vtobjectlinearbargraph_c.h
                             -------------------
    begin                : Mon Jun 30 14:41:54 2003
    copyright            : (C) 2003 by Martin Wodok
    email                : m.wodok@osb-ag:de
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
 * the main author Achim Spangler by a.spangler@osb-ag:de                  * 
 ***************************************************************************/ 

 /**************************************************************************
 *                                                                         * 
 *     ###    !!!    ---    ===    IMPORTANT    ===    ---    !!!    ###   * 
 * Each software module, which accesses directly elements of this file,    * 
 * is considered to be an extension of IsoAgLib and is thus covered by the * 
 * GPL license. Applications must use only the interface definition out-   * 
 * side :impl: subdirectories. Never access direct elements of __IsoAgLib  * 
 * and __HAL namespaces from applications which shouldnt be affected by    * 
 * the license. Only access their interface counterparts in the IsoAgLib   * 
 * and HAL namespaces. Contact a.spangler@osb-ag:de in case your applicat- * 
 * ion really needs access to a part of an internal namespace, so that the * 
 * interface might be extended if your request is accepted.                * 
 *                                                                         * 
 * Definition of direct access:                                            * 
 * - Instantiation of a variable with a datatype from internal namespace   * 
 * - Call of a (member-) function                                          * 
 * Allowed is:                                                             * 
 * - Instatiation of a variable with a datatype from interface namespace,  * 
 *   even if this is derived from a base class inside an internal namespace* 
 * - Call of member functions which are defined in the interface class     * 
 *   definition ( header )                                                 * 
 *                                                                         * 
 * Pairing of internal and interface classes:                              * 
 * - Internal implementation in an :impl: subdirectory                     * 
 * - Interface in the parent directory of the corresponding internal class * 
 * - Interface class name IsoAgLib::iFoo_c maps to the internal class      * 
 *   __IsoAgLib::Foo_c                                                     * 
 *                                                                         * 
 * AS A RULE: Use only classes with names beginning with small letter :i:  *
 ***************************************************************************/

#ifndef VTOBJECTLINEARBARGRAPH_C_H
#define VTOBJECTLINEARBARGRAPH_C_H

// +X2C includes
#include "vtobject_c.h"
#include "isoterminal_c.h"
// ~X2C

// Begin Namespace __IsoAgLib
namespace __IsoAgLib {


//  +X2C Class 119 : vtObjectLinearBarGraph_c
//!  Stereotype: Klasse
class vtObjectLinearBarGraph_c : public vtObject_c
{

public:

  //  Operation: stream
  //! Parameter:
  //! @param:destMemory:
  //! @param maxBytes: don't stream out more than that or you'll overrun the internal upload-buffer
  //! @param sourceOffset:
  int16_t stream(uint8_t* destMemory,
                 uint16_t maxBytes,
                 uint16_t sourceOffset);

  //  Operation: init
  //! Parameter:
  //! @param vtObjectLinearBarGraphSROM:
  //! @param b_initPointer:
  void init(const iVtObjectLinearBarGraph_s* vtObjectLinearBarGraphSROM) { vtObject_c::init ((iVtObject_s*) vtObjectLinearBarGraphSROM); };

  //  Operation: get_vtObjectLinearBarGraph_a
  iVtObjectLinearBarGraph_s* get_vtObjectLinearBarGraph_a() { return (iVtObjectLinearBarGraph_s *)vtObject_a; };

  //  Operation: vtObjectLinearBarGraph_c
  vtObjectLinearBarGraph_c();

  //  Operation: size
  uint32_t fitTerminal();

  // //////////////////////////////////
  // All special Attribute-Set methods
  void setWidth(uint16_t newValue, bool b_updateObject=false) {
    saveValue16SetAttribute ((b_updateObject) ? MACRO_getStructOffset(get_vtObjectLinearBarGraph_a(), width) : 0, sizeof(iVtObjectLinearBarGraph_s), 1, newValue);
  }
  void setHeight(uint16_t newValue, bool b_updateObject=false) {
    saveValue16SetAttribute ((b_updateObject) ? MACRO_getStructOffset(get_vtObjectLinearBarGraph_a(), height) : 0, sizeof(iVtObjectLinearBarGraph_s), 2, newValue);
  }
  void setColour(uint8_t newValue, bool b_updateObject=false) {
    saveValue8SetAttribute ((b_updateObject) ? MACRO_getStructOffset(get_vtObjectLinearBarGraph_a(), colour) : 0, sizeof(iVtObjectLinearBarGraph_s), 3, newValue, __IsoAgLib::getIsoTerminalInstance().getUserClippedColor (newValue, this, IsoAgLib::Colour));
  }
  void setTargetLineColour(uint8_t newValue, bool b_updateObject=false) {
    saveValue8SetAttribute ((b_updateObject) ? MACRO_getStructOffset(get_vtObjectLinearBarGraph_a(), targetLineColour) : 0, sizeof(iVtObjectLinearBarGraph_s), 4, newValue, __IsoAgLib::getIsoTerminalInstance().getUserClippedColor (newValue, this, IsoAgLib::TargetLineColour));
  }
  void setOptions(uint8_t newValue, bool b_updateObject=false) {
    saveValue8SetAttribute ((b_updateObject) ? MACRO_getStructOffset(get_vtObjectLinearBarGraph_a(), options) : 0, sizeof(iVtObjectLinearBarGraph_s), 5, newValue, newValue);
  }
  void setNumberOfTicks(uint8_t newValue, bool b_updateObject=false) {
    saveValue8SetAttribute ((b_updateObject) ? MACRO_getStructOffset(get_vtObjectLinearBarGraph_a(), numberOfTicks) : 0, sizeof(iVtObjectLinearBarGraph_s), 6, newValue, newValue);
  }
  void setMinValue(uint16_t newValue, bool b_updateObject=false) {
    saveValue16SetAttribute ((b_updateObject) ? MACRO_getStructOffset(get_vtObjectLinearBarGraph_a(), minValue) : 0, sizeof(iVtObjectLinearBarGraph_s), 7, newValue);
  }
  void setMaxValue(uint16_t newValue, bool b_updateObject=false) {
    saveValue16SetAttribute ((b_updateObject) ? MACRO_getStructOffset(get_vtObjectLinearBarGraph_a(), maxValue) : 0, sizeof(iVtObjectLinearBarGraph_s), 8, newValue);
  }
  void setVariableReference(IsoAgLib::iVtObject_c* newValue, bool b_updateObject=false) {
    saveValuePSetAttribute ((b_updateObject) ? MACRO_getStructOffset(get_vtObjectLinearBarGraph_a(), variableReference) : 0, sizeof(iVtObjectLinearBarGraph_s), 9, newValue);
  }
  
  void setValue(uint16_t newValue, bool b_updateObject= false);

  void setTargetValueVariableReference(IsoAgLib::iVtObject_c* newValue, bool b_updateObject=false) {
    saveValuePSetAttribute ((b_updateObject) ? MACRO_getStructOffset(get_vtObjectLinearBarGraph_a(), targetValueVariableReference) : 0, sizeof(iVtObjectLinearBarGraph_s), 10, newValue);
  }
  void setTargetValue(uint16_t newValue, bool b_updateObject=false) {
    saveValue16SetAttribute ((b_updateObject) ? MACRO_getStructOffset(get_vtObjectLinearBarGraph_a(), targetValue) : 0, sizeof(iVtObjectLinearBarGraph_s), 11, newValue);
  }

  void setSize(uint16_t newWidth, uint16_t newHeight, bool b_updateObject=false);

}; // ~X2C

} // end of namespace __IsoAgLib

#endif // -X2C
