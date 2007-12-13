/***************************************************************************
                          vtobjectinputnumber_c.cpp
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
#include "vtobjectinputnumber_c.h"

#if not defined PRJ_ISO_TERMINAL_OBJECT_SELECTION1 || defined USE_VTOBJECT_inputnumber
#include <IsoAgLib/util/impl/util_funcs.h>
#include "isoterminal_c.h"

// Begin Namespace __IsoAgLib
namespace __IsoAgLib {
// Operation : stream
//! @param:destMemory:
//! @param maxBytes: don't stream out more than that or you'll overrun the internal upload-buffer
//! @param sourceOffset:
int16_t
vtObjectInputNumber_c::stream(uint8_t* destMemory,
                              uint16_t maxBytes,
                              objRange_t sourceOffset)
{
#define MACRO_vtObjectTypeA vtObjectInputNumber_a
#define MACRO_vtObjectTypeS iVtObjectInputNumber_s
    MACRO_streamLocalVars;
    MACRO_scaleLocalVars;

    if (sourceOffset == 0) { // dump out constant sized stuff
      destMemory [0] = vtObject_a->ID & 0xFF;
      destMemory [1] = vtObject_a->ID >> 8;
      destMemory [2] = 9; // Object Type = Input Number
      destMemory [3] = (((uint32_t) vtObjectInputNumber_a->width*vtDimension)/opDimension) & 0xFF;
      destMemory [4] = (((uint32_t) vtObjectInputNumber_a->width*vtDimension)/opDimension) >> 8;
      destMemory [5] = (((uint32_t) vtObjectInputNumber_a->height*vtDimension)/opDimension) & 0xFF;
      destMemory [6] = (((uint32_t) vtObjectInputNumber_a->height*vtDimension)/opDimension) >> 8;
      destMemory [7] = __IsoAgLib::getIsoTerminalInstance4Comm().getClientByID (s_properties.clientId).getUserClippedColor (vtObjectInputNumber_a->backgroundColour, this, IsoAgLib::BackgroundColour);
      destMemory [8] = vtObjectInputNumber_a->fontAttributes->getID() & 0xFF;
      destMemory [9] = vtObjectInputNumber_a->fontAttributes->getID() >> 8;
      destMemory [10] = vtObjectInputNumber_a->options;
      if (vtObjectInputNumber_a->variableReference != NULL) {
          destMemory [11] = vtObjectInputNumber_a->variableReference->getID() & 0xFF;
          destMemory [12] = vtObjectInputNumber_a->variableReference->getID() >> 8;
      } else {
          destMemory [11] = 0xFF;
          destMemory [12] = 0xFF;
      }
      destMemory [13] = (vtObjectInputNumber_a->value) & 0xFF;
      destMemory [14] = (vtObjectInputNumber_a->value >> 8) & 0xFF;
      destMemory [15] = (vtObjectInputNumber_a->value >> 16) & 0xFF;
      destMemory [16] = (vtObjectInputNumber_a->value >> 24) & 0xFF;

      destMemory [17] = (vtObjectInputNumber_a->minValue) & 0xFF;
      destMemory [18] = (vtObjectInputNumber_a->minValue >> 8) & 0xFF;
      destMemory [19] = (vtObjectInputNumber_a->minValue >> 16) & 0xFF;
      destMemory [20] = (vtObjectInputNumber_a->minValue >> 24) & 0xFF;

      destMemory [21] = (vtObjectInputNumber_a->maxValue) & 0xFF;
      destMemory [22] = (vtObjectInputNumber_a->maxValue >> 8) & 0xFF;
      destMemory [23] = (vtObjectInputNumber_a->maxValue >> 16) & 0xFF;
      destMemory [24] = (vtObjectInputNumber_a->maxValue >> 24) & 0xFF;

      destMemory [25] = (vtObjectInputNumber_a->offset) & 0xFF;
      destMemory [26] = (vtObjectInputNumber_a->offset >> 8) & 0xFF;
      destMemory [27] = (vtObjectInputNumber_a->offset >> 16) & 0xFF;
      destMemory [28] = (vtObjectInputNumber_a->offset >> 24) & 0xFF;

      __IsoAgLib::floatVar2LittleEndianStream (&vtObjectInputNumber_a->scale, &destMemory[29]);

      destMemory [33] = vtObjectInputNumber_a->numberOfDecimals;
      destMemory [34] = vtObjectInputNumber_a->format;
      destMemory [35] = vtObjectInputNumber_a->horizontalJustification;
      destMemory [36] = vtObjectInputNumber_a->secondOptionsByte;
      destMemory [37] = vtObjectInputNumber_a->numberOfMacrosToFollow;
      sourceOffset += 38;
      curBytes += 38;
    }

    MACRO_streamEventMacro(38);
    return curBytes;
}


// Operation : vtObjectInputNumber_c
vtObjectInputNumber_c::vtObjectInputNumber_c() {}

// Operation : size
uint32_t
vtObjectInputNumber_c::fitTerminal() const
{
  MACRO_localVars;
  return 38+vtObjectInputNumber_a->numberOfMacrosToFollow*2;
}

// Operation : updateEnable
//! @param b_enableOrDisable:
void
vtObjectInputNumber_c::updateEnable(uint8_t aui8_enOrDis)
{
  saveValue8 (MACRO_getStructOffset(get_vtObjectInputNumber_a(), secondOptionsByte), sizeof(iVtObjectInputNumber_s), aui8_enOrDis);
}

// Operation : setValue
//! @param newValue:
//! @param b_updateObject:
void
vtObjectInputNumber_c::setValue(uint32_t newValue,
                                bool b_updateObject, bool b_enableReplaceOfCmd)
{
  if (get_vtObjectInputNumber_a()->variableReference == NULL) {
    if (b_updateObject) saveValue32 (MACRO_getStructOffset(get_vtObjectInputNumber_a(), value),  sizeof(iVtObjectInputNumber_s), newValue);

    __IsoAgLib::getIsoTerminalInstance4Comm().getClientByID (s_properties.clientId).sendCommandChangeNumericValue (this, newValue & 0xFF, (newValue >> 8) & 0xFF, (newValue >> 16) & 0xFF, newValue >> 24, b_enableReplaceOfCmd);
  }
}

void
vtObjectInputNumber_c::setSize(uint16_t newWidth, uint16_t newHeight, bool b_updateObject, bool b_enableReplaceOfCmd)
{
  if (b_updateObject) {
    saveValue16 (MACRO_getStructOffset(get_vtObjectInputNumber_a(), width),  sizeof(iVtObjectInputNumber_s), newWidth);
    saveValue16 (MACRO_getStructOffset(get_vtObjectInputNumber_a(), height), sizeof(iVtObjectInputNumber_s), newHeight);
  }

  __IsoAgLib::getIsoTerminalInstance4Comm().getClientByID (s_properties.clientId).sendCommandChangeSize (this, newWidth, newHeight, b_enableReplaceOfCmd);
}

#ifdef USE_ISO_TERMINAL_GETATTRIBUTES
uint16_t
vtObjectInputNumber_c::updateWidth(bool b_SendRequest)
{
  if (b_SendRequest)
    return getValue16GetAttribute(MACRO_getStructOffset(get_vtObjectInputNumber_a(), width), sizeof(iVtObjectInputNumber_s), 1);
  else
    return getValue16(MACRO_getStructOffset(get_vtObjectInputNumber_a(), width), sizeof(iVtObjectInputNumber_s));
}

uint16_t
vtObjectInputNumber_c::updateHeight(bool b_SendRequest)
{
  if (b_SendRequest)
    return getValue16GetAttribute(MACRO_getStructOffset(get_vtObjectInputNumber_a(), height), sizeof(iVtObjectInputNumber_s), 2);
  else
    return getValue16(MACRO_getStructOffset(get_vtObjectInputNumber_a(), height), sizeof(iVtObjectInputNumber_s));
}

uint8_t
vtObjectInputNumber_c::updateBackgroundColour(bool b_SendRequest)
{
  if (b_SendRequest)
    return getValue8GetAttribute(MACRO_getStructOffset(get_vtObjectInputNumber_a(), backgroundColour), sizeof(iVtObjectInputNumber_s), 3);
  else
    return getValue8(MACRO_getStructOffset(get_vtObjectInputNumber_a(), backgroundColour), sizeof(iVtObjectInputNumber_s));
}

uint16_t
vtObjectInputNumber_c::updateFontAttributes(bool b_SendRequest)
{
  if (b_SendRequest)
    return getValue16GetAttribute(MACRO_getStructOffset(get_vtObjectInputNumber_a(), fontAttributes), sizeof(iVtObjectInputNumber_s), 4);
  else
    return getValue16(MACRO_getStructOffset(get_vtObjectInputNumber_a(), fontAttributes), sizeof(iVtObjectInputNumber_s));
}

uint8_t
vtObjectInputNumber_c::updateOptions(bool b_SendRequest)
{
  if (b_SendRequest)
    return getValue8GetAttribute(MACRO_getStructOffset(get_vtObjectInputNumber_a(), options), sizeof(iVtObjectInputNumber_s), 5);
  else
    return getValue8(MACRO_getStructOffset(get_vtObjectInputNumber_a(), options), sizeof(iVtObjectInputNumber_s));
}

uint16_t
vtObjectInputNumber_c::updateVariableReference(bool b_SendRequest)
{
  if (b_SendRequest)
    return getValue16GetAttribute(MACRO_getStructOffset(get_vtObjectInputNumber_a(), variableReference), sizeof(iVtObjectInputNumber_s), 6);
  else
    return getValue16(MACRO_getStructOffset(get_vtObjectInputNumber_a(), variableReference), sizeof(iVtObjectInputNumber_s));
}

uint32_t
vtObjectInputNumber_c::updateMinValue(bool b_SendRequest)
{
  if (b_SendRequest)
    return getValue32GetAttribute(MACRO_getStructOffset(get_vtObjectInputNumber_a(), minValue), sizeof(iVtObjectInputNumber_s), 7);
  else
    return getValue32(MACRO_getStructOffset(get_vtObjectInputNumber_a(), minValue), sizeof(iVtObjectInputNumber_s));
}

uint32_t
vtObjectInputNumber_c::updateMaxValue(bool b_SendRequest)
{
  if (b_SendRequest)
    return getValue32GetAttribute(MACRO_getStructOffset(get_vtObjectInputNumber_a(), maxValue), sizeof(iVtObjectInputNumber_s), 8);
  else
    return getValue32(MACRO_getStructOffset(get_vtObjectInputNumber_a(), maxValue), sizeof(iVtObjectInputNumber_s));
}

int32_t
vtObjectInputNumber_c::updateOffset(bool b_SendRequest)
{
  if (b_SendRequest)
    return getSignedValue32GetAttribute(MACRO_getStructOffset(get_vtObjectInputNumber_a(), offset), sizeof(iVtObjectInputNumber_s), 9);
  else
    return getSignedValue32(MACRO_getStructOffset(get_vtObjectInputNumber_a(), offset), sizeof(iVtObjectInputNumber_s));
}

float
vtObjectInputNumber_c::updateScale(bool b_SendRequest)
{
  if (b_SendRequest)
    return getValueFloatGetAttribute(MACRO_getStructOffset(get_vtObjectInputNumber_a(), scale), sizeof(iVtObjectInputNumber_s), 10);
  else
    return getValueFloat(MACRO_getStructOffset(get_vtObjectInputNumber_a(), scale), sizeof(iVtObjectInputNumber_s));
}

uint8_t
vtObjectInputNumber_c::updateNumberOfDecimals(bool b_SendRequest)
{
  if (b_SendRequest)
    return getValue8GetAttribute(MACRO_getStructOffset(get_vtObjectInputNumber_a(), numberOfDecimals), sizeof(iVtObjectInputNumber_s), 11);
  else
    return getValue8(MACRO_getStructOffset(get_vtObjectInputNumber_a(), numberOfDecimals), sizeof(iVtObjectInputNumber_s));
}

uint8_t
vtObjectInputNumber_c::updateFormat(bool b_SendRequest)
{
  if (b_SendRequest)
    return getValue8GetAttribute(MACRO_getStructOffset(get_vtObjectInputNumber_a(), format), sizeof(iVtObjectInputNumber_s), 12);
  else
    return getValue8(MACRO_getStructOffset(get_vtObjectInputNumber_a(), format), sizeof(iVtObjectInputNumber_s));
}

uint8_t
vtObjectInputNumber_c::updateJustification(bool b_SendRequest)
{
  if (b_SendRequest)
    return getValue8GetAttribute(MACRO_getStructOffset(get_vtObjectInputNumber_a(), horizontalJustification), sizeof(iVtObjectInputNumber_s), 13);
  else
    return getValue8(MACRO_getStructOffset(get_vtObjectInputNumber_a(), horizontalJustification), sizeof(iVtObjectInputNumber_s));
}

/** these attributes are in parentheses in the spec, so commented out here
uint32_t
vtObjectInputNumber_c::updateValue(bool b_SendRequest)
{
  if (b_SendRequest)
    return getValue32GetAttribute(MACRO_getStructOffset(get_vtObjectInputNumber_a(), value), sizeof(iVtObjectInputNumber_s), 14);
  else
    return getValue32(MACRO_getStructOffset(get_vtObjectInputNumber_a(), value), sizeof(iVtObjectInputNumber_s));
}

uint8_t
vtObjectInputNumber_c::update2ndOptionsByte(bool b_SendRequest)
{
  if (b_SendRequest)
    return getValue8GetAttribute(MACRO_getStructOffset(get_vtObjectInputNumber_a(), secondOptionsByte), sizeof(iVtObjectInputNumber_s), 15);
  else
    return getValue8(MACRO_getStructOffset(get_vtObjectInputNumber_a(), secondOptionsByte), sizeof(iVtObjectInputNumber_s));
}
*/

void
vtObjectInputNumber_c::saveReceivedAttribute(uint8_t attrID, uint8_t* pui8_attributeValue)
{
  switch (attrID)
  {
    case 1: saveValue16(MACRO_getStructOffset(get_vtObjectInputNumber_a(), width), sizeof(iVtObjectInputNumber_s), convertLittleEndianStringUi16(pui8_attributeValue)); break;
    case 2: saveValue16(MACRO_getStructOffset(get_vtObjectInputNumber_a(), height), sizeof(iVtObjectInputNumber_s), convertLittleEndianStringUi16(pui8_attributeValue)); break;
    case 3: saveValue8(MACRO_getStructOffset(get_vtObjectInputNumber_a(), backgroundColour), sizeof(iVtObjectInputNumber_s), convertLittleEndianStringUi8(pui8_attributeValue)); break;
    case 4: saveValue16(MACRO_getStructOffset(get_vtObjectInputNumber_a(), fontAttributes), sizeof(iVtObjectInputNumber_s), convertLittleEndianStringUi16(pui8_attributeValue)); break;
    case 5: saveValue8(MACRO_getStructOffset(get_vtObjectInputNumber_a(), options), sizeof(iVtObjectInputNumber_s), convertLittleEndianStringUi8(pui8_attributeValue)); break;
    case 6: saveValue16(MACRO_getStructOffset(get_vtObjectInputNumber_a(), variableReference), sizeof(iVtObjectInputNumber_s), convertLittleEndianStringUi16(pui8_attributeValue)); break;
    case 7: saveValue32(MACRO_getStructOffset(get_vtObjectInputNumber_a(), minValue), sizeof(iVtObjectInputNumber_s), convertLittleEndianStringUi32(pui8_attributeValue)); break;
    case 8: saveValue32(MACRO_getStructOffset(get_vtObjectInputNumber_a(), maxValue), sizeof(iVtObjectInputNumber_s), convertLittleEndianStringUi32(pui8_attributeValue)); break;
    case 9: saveSignedValue32(MACRO_getStructOffset(get_vtObjectInputNumber_a(), offset), sizeof(iVtObjectInputNumber_s), convertLittleEndianStringI32(pui8_attributeValue)); break;
    case 10: saveValueFloat(MACRO_getStructOffset(get_vtObjectInputNumber_a(), scale), sizeof(iVtObjectInputNumber_s), convertLittleEndianStringFloat(pui8_attributeValue)); break;
    case 11: saveValue8(MACRO_getStructOffset(get_vtObjectInputNumber_a(), numberOfDecimals), sizeof(iVtObjectInputNumber_s), convertLittleEndianStringUi8(pui8_attributeValue)); break;
    case 12: saveValue8(MACRO_getStructOffset(get_vtObjectInputNumber_a(), format), sizeof(iVtObjectInputNumber_s), convertLittleEndianStringUi8(pui8_attributeValue)); break;
    case 13: saveValue8(MACRO_getStructOffset(get_vtObjectInputNumber_a(), horizontalJustification), sizeof(iVtObjectInputNumber_s), convertLittleEndianStringUi8(pui8_attributeValue)); break;
    /** these attributes are in parentheses in the spec, so commented out here
    case 14: saveValue32(MACRO_getStructOffset(get_vtObjectInputNumber_a(), value), sizeof(iVtObjectInputNumber_s), convertLittleEndianStringUi32(pui8_attributeValue)); break;
    case 15: saveValue8(MACRO_getStructOffset(get_vtObjectInputNumber_a(), secondOptionsByte), sizeof(iVtObjectInputNumber_s), convertLittleEndianStringUi8(pui8_attributeValue)); break;
    */
    default: break;
  }
}
#endif
} // end of namespace __IsoAgLib
#endif
