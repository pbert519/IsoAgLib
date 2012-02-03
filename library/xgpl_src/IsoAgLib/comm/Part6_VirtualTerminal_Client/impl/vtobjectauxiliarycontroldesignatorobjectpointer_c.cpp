/*
  vtobjectauxiliarycontroldesignatorobjectpointer_c.cpp

  (C) Copyright 2009 - 2011 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/

#include "vtobjectauxiliarycontroldesignatorobjectpointer_c.h"

#ifdef USE_VTOBJECT_auxiliarycontroldesignatorobjectpointer

#include "isoterminal_c.h"

// Begin Namespace __IsoAgLib
namespace __IsoAgLib {

// Operation : stream
//! @param destMemory:
//! @param maxBytes: don't stream out more than that or you'll overrun the internal upload-buffer
//! @param sourceOffset:
int16_t
vtObjectAuxiliaryControlDesignatorObjectPointer_c::stream(uint8_t* destMemory,
                                                          uint16_t /*maxBytes*/,
                                                          objRange_t sourceOffset)
{
#define MACRO_vtObjectTypeA vtObjectAuxiliaryControlDesignatorObjectPointer_a
#define MACRO_vtObjectTypeS iVtObjectAuxiliaryControlDesignatorObjectPointer_s
  MACRO_streamLocalVars;
  MACRO_scaleLocalVars;
  if (sourceOffset == 0) { // dump out constant sized stuff
    destMemory [0] = vtObject_a->ID & 0xFF;
    destMemory [1] = vtObject_a->ID >> 8;
    destMemory [2] = 33; // Object Type = Auxiliary Control Designator Object Pointer
    destMemory [3] = vtObjectAuxiliaryControlDesignatorObjectPointer_a->pointerType;
    if (vtObjectAuxiliaryControlDesignatorObjectPointer_a->value)
    {
      destMemory [4] = vtObjectAuxiliaryControlDesignatorObjectPointer_a->value->getID() & 0xFF;
      destMemory [5] = vtObjectAuxiliaryControlDesignatorObjectPointer_a->value->getID() >> 8;
    }
    else
    {
      destMemory [4] = 0xFF;
      destMemory [5] = 0xFF;
    }
    sourceOffset += 6;
    curBytes += 6;
  }
  
  return curBytes;
}


// Operation : vtObjectAuxiliaryControlDesignatorObjectPointer_c
vtObjectAuxiliaryControlDesignatorObjectPointer_c::vtObjectAuxiliaryControlDesignatorObjectPointer_c() {}

// Operation : size
uint32_t
vtObjectAuxiliaryControlDesignatorObjectPointer_c::fitTerminal() const
{
  return 6;
}


#ifdef USE_ISO_TERMINAL_GETATTRIBUTES
/** that attribute is in parentheses in the spec, so commented out here
uint16_t
vtObjectAuxiliaryControlDesignatorObjectPointer_c::updateValue(bool b_SendRequest)
{
  if (b_SendRequest)
    return getValue16GetAttribute(MACRO_getStructOffset(get_vtObjectAuxiliaryControlDesignatorObjectPointer_a(), value), sizeof(iVtObjectAuxiliaryControlDesignatorObjectPointer_s), 1);
  else
    return getValue16(MACRO_getStructOffset(get_vtObjectAuxiliaryControlDesignatorObjectPointer_a(), value), sizeof(iVtObjectAuxiliaryControlDesignatorObjectPointer_s));
}
*/

void
vtObjectAuxiliaryControlDesignatorObjectPointer_c::saveReceivedAttribute(uint8_t /*attrID*/, uint8_t* /*pui8_attributeValue*/)
{
  /** that attribute is in parentheses in the spec, so commented out here
  case 1: saveValue16(MACRO_getStructOffset(get_vtObjectAuxiliaryControlDesignatorObjectPointer_a(), value), sizeof(iVtObjectAuxiliaryControlDesignatorObjectPointer_s), convertLittleEndianStringUi16(pui8_attributeValue)); break;
  default: break;
  */
}
#endif

} // end of namespace __IsoAgLib
#endif
