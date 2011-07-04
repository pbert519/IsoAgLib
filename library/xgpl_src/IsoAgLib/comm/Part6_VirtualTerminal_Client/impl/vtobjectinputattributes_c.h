/*
  vtobjectinputattributes_c.h

  (C) Copyright 2009 - 2011 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/
#ifndef VTOBJECTINPUTATTRIBUTES_C_H
#define VTOBJECTINPUTATTRIBUTES_C_H

#include "../ivtobjectstring_c.h"
#include "vtclientservercommunication_c.h"

// Begin Namespace __IsoAgLib
namespace __IsoAgLib {

class vtObjectInputAttributes_c : public IsoAgLib::iVtObjectString_c
{
public:
  /// Operation: stream
  /// @param destMemory
  /// @param maxBytes don't stream out more than that or you'll overrun the internal upload-buffer
  /// @param sourceOffset
  int16_t stream(uint8_t* destMemory,
                 uint16_t maxBytes,
                 objRange_t sourceOffset);

  ///  Operation: init
  /// @param vtObjectInputAttributesSROM
  /// @param b_initPointer
  void init(const iVtObjectInputAttributes_s* vtObjectInputAttributesSROM MULTITON_INST_PARAMETER_DEF_WITH_COMMA)
  { vtObject_c::init ((iVtObject_s*) vtObjectInputAttributesSROM MULTITON_INST_PARAMETER_USE_WITH_COMMA); }

  //  Operation: get_vtObjectInputAttributes_a
  iVtObjectInputAttributes_s* get_vtObjectInputAttributes_a() { return (iVtObjectInputAttributes_s *)&(get_vtObject_a()); }

  //  Operation: vtObjectInputAttributes_c
  vtObjectInputAttributes_c();

  //  Operation: size
  uint32_t fitTerminal() const;

  //  Operation: getString
  const char* getString();

  /// Operation: setValidationStringCopy
  /// @param newValidationString
  /// @param b_updateObject default:false
  /// @param b_enableReplaceOfCmd default:false
  void setValidationStringCopy(const char* newValidationString, bool b_updateObject= false, bool b_enableReplaceOfCmd=false);

  //  Operation: setValidationStringRef
  //! @param newValidationString:
  //! @param b_updateObject:
  void setValidationStringRef(const char* newValidationString, bool b_updateObject= false, bool b_enableReplaceOfCmd=false);
#ifdef USE_ISO_TERMINAL_GETATTRIBUTES
  /** these attributes are in parentheses in the spec, so commented out here
  uint8_t updateObjectType() const { return 26; }

  uint8_t updateValidationType(bool b_SendRequest=false);
  */

  void saveReceivedAttribute (uint8_t attrID, uint8_t* pui8_attributeValue);
#endif
};

} // end namespace __IsoAgLib

#endif
