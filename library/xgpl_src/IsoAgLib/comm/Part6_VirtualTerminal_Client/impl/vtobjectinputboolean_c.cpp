/***************************************************************************
                          vtobjectinputboolean_c.cpp
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
#include "vtobjectinputboolean_c.h"

#include "isoterminal_c.h"

// Begin Namespace __IsoAgLib
namespace __IsoAgLib {
// Operation : stream
//! @param:destMemory:
//! @param maxBytes: don't stream out more than that or you'll overrun the internal upload-buffer
//! @param sourceOffset:
int16_t
vtObjectInputBoolean_c::stream(uint8_t* destMemory,
                               uint16_t maxBytes,
                               objRange_t sourceOffset)
{
#define MACRO_vtObjectTypeA vtObjectInputBoolean_a
#define MACRO_vtObjectTypeS iVtObjectInputBoolean_s
    MACRO_streamLocalVars;
    MACRO_scaleLocalVars;

    if (sourceOffset == 0) { // dump out constant sized stuff
      destMemory [0] = vtObject_a->ID & 0xFF;
      destMemory [1] = vtObject_a->ID >> 8;
      destMemory [2] = 7; // Object Type = Input Boolean
      destMemory [3] = __IsoAgLib::getIsoTerminalInstance4Comm().getClientByID (s_properties.clientId).getUserClippedColor (vtObjectInputBoolean_a->backgroundColour, this, IsoAgLib::BackgroundColour);
      destMemory [4] = (((uint32_t) vtObjectInputBoolean_a->width*vtDimension)/opDimension) & 0xFF;
      destMemory [5] = (((uint32_t) vtObjectInputBoolean_a->width*vtDimension)/opDimension) >> 8;
      destMemory [6] = vtObjectInputBoolean_a->foregroundColour->getID() & 0xFF;
      destMemory [7] = vtObjectInputBoolean_a->foregroundColour->getID() >> 8;
      if (vtObjectInputBoolean_a->variableReference != NULL) {
        destMemory [8] = vtObjectInputBoolean_a->variableReference->getID() & 0xFF;
        destMemory [9] = vtObjectInputBoolean_a->variableReference->getID() >> 8;
      } else {
        destMemory [8] = 0xFF;
        destMemory [9] = 0xFF;
      }
      destMemory [10] = vtObjectInputBoolean_a->value;
      destMemory [11] = vtObjectInputBoolean_a->enabled;
      destMemory [12] = vtObjectInputBoolean_a->numberOfMacrosToFollow;
      sourceOffset += 13;
      curBytes += 13;
    }

    MACRO_streamEventMacro(13);
    return curBytes;
}


// Operation : vtObjectInputBoolean_c
vtObjectInputBoolean_c::vtObjectInputBoolean_c() {}

// Operation : size
uint32_t
vtObjectInputBoolean_c::fitTerminal() const
{
  MACRO_localVars;
  return 13+vtObjectInputBoolean_a->numberOfMacrosToFollow*2;
}

// Operation : updateEnable
//! @param b_enableOrDisable:
void
vtObjectInputBoolean_c::updateEnable(uint8_t aui8_enOrDis)
{
  saveValue8 (MACRO_getStructOffset(get_vtObjectInputBoolean_a(), enabled), sizeof(iVtObjectInputBoolean_s), aui8_enOrDis);
}

// Operation : setValue
//! @param newValue:
//! @param b_updateObject:
void
vtObjectInputBoolean_c::setValue(bool newValue, bool b_updateObject, bool b_enableReplaceOfCmd)
{
  if (get_vtObjectInputBoolean_a()->variableReference == NULL) {
    if (b_updateObject) saveValue8 (MACRO_getStructOffset(get_vtObjectInputBoolean_a(), value),  sizeof(iVtObjectInputBoolean_s), newValue);
    __IsoAgLib::getIsoTerminalInstance4Comm().getClientByID (s_properties.clientId).sendCommandChangeNumericValue (this, newValue?1:0, 0x00, 0x00, 0x00, b_enableReplaceOfCmd);
  }
}

uint8_t
vtObjectInputBoolean_c::updateBackgroundColour(bool b_SendRequest)
{
  if (b_SendRequest)
    return getValue8GetAttribute(MACRO_getStructOffset(get_vtObjectInputBoolean_a(), backgroundColour), sizeof(iVtObjectInputBoolean_s), 1);
  else
    return getValue8(MACRO_getStructOffset(get_vtObjectInputBoolean_a(), backgroundColour), sizeof(iVtObjectInputBoolean_s));
}

uint16_t
vtObjectInputBoolean_c::updateWidth(bool b_SendRequest)
{
  if (b_SendRequest)
    return getValue16GetAttribute(MACRO_getStructOffset(get_vtObjectInputBoolean_a(), width), sizeof(iVtObjectInputBoolean_s), 2);
  else
    return getValue16(MACRO_getStructOffset(get_vtObjectInputBoolean_a(), width), sizeof(iVtObjectInputBoolean_s));
}

uint16_t
vtObjectInputBoolean_c::updateForegroundColour(bool b_SendRequest)
{
  if (b_SendRequest)
    return getValue16GetAttribute(MACRO_getStructOffset(get_vtObjectInputBoolean_a(), foregroundColour), sizeof(iVtObjectInputBoolean_s), 3);
  else
    return getValue16(MACRO_getStructOffset(get_vtObjectInputBoolean_a(), foregroundColour), sizeof(iVtObjectInputBoolean_s));
}

uint16_t
vtObjectInputBoolean_c::updateVariableReference(bool b_SendRequest)
{
  if (b_SendRequest)
    return getValue16GetAttribute(MACRO_getStructOffset(get_vtObjectInputBoolean_a(), variableReference), sizeof(iVtObjectInputBoolean_s), 4);
  else
    return getValue16(MACRO_getStructOffset(get_vtObjectInputBoolean_a(), variableReference), sizeof(iVtObjectInputBoolean_s));
}

/** these attributes are in parentheses in the spec, so commented out here
uint8_t
vtObjectInputBoolean_c::updateValue(bool b_SendRequest)
{
  if (b_SendRequest)
    return getValue8GetAttribute(MACRO_getStructOffset(get_vtObjectInputBoolean_a(), value), sizeof(iVtObjectInputBoolean_s), 5);
  else
    return getValue8(MACRO_getStructOffset(get_vtObjectInputBoolean_a(), value), sizeof(iVtObjectInputBoolean_s));
}

uint8_t
vtObjectInputBoolean_c::updateEnabled(bool b_SendRequest)
{
  if (b_SendRequest)
    return getValue8GetAttribute(MACRO_getStructOffset(get_vtObjectInputBoolean_a(), enabled), sizeof(iVtObjectInputBoolean_s), 6);
  else
    return getValue8(MACRO_getStructOffset(get_vtObjectInputBoolean_a(), enabled), sizeof(iVtObjectInputBoolean_s));
}
*/

void
vtObjectInputBoolean_c::saveReceivedAttribute(uint8_t attrID, uint8_t* pui8_attributeValue)
{
  switch (attrID)
  {
    case 1: saveValue8(MACRO_getStructOffset(get_vtObjectInputBoolean_a(), backgroundColour), sizeof(iVtObjectInputBoolean_s), convertLittleEndianStringUi8(pui8_attributeValue)); break;
    case 2: saveValue16(MACRO_getStructOffset(get_vtObjectInputBoolean_a(), width), sizeof(iVtObjectInputBoolean_s), convertLittleEndianStringUi16(pui8_attributeValue)); break;
    case 3: saveValue16(MACRO_getStructOffset(get_vtObjectInputBoolean_a(), foregroundColour), sizeof(iVtObjectInputBoolean_s), convertLittleEndianStringUi16(pui8_attributeValue)); break;
    case 4: saveValue16(MACRO_getStructOffset(get_vtObjectInputBoolean_a(), variableReference), sizeof(iVtObjectInputBoolean_s), convertLittleEndianStringUi16(pui8_attributeValue)); break;
    /** these attributes are in parentheses in the spec, so commented out here
    case 5: saveValue8(MACRO_getStructOffset(get_vtObjectInputBoolean_a(), value), sizeof(iVtObjectInputBoolean_s), convertLittleEndianStringUi8(pui8_attributeValue)); break;
    case 6: saveValue8(MACRO_getStructOffset(get_vtObjectInputBoolean_a(), enabled), sizeof(iVtObjectInputBoolean_s), convertLittleEndianStringUi8(pui8_attributeValue)); break;
    */
    default: break;
  }
}

} // end of namespace __IsoAgLib