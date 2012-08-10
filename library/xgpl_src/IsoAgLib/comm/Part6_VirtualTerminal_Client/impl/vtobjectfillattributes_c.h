/*
  vtobjectfillattributes_c.h

  (C) Copyright 2009 - 2012 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/
#ifndef VTOBJECTFILLATTRIBUTES_C_H
#define VTOBJECTFILLATTRIBUTES_C_H

#include "vtobject_c.h"
#include "../ivtobjectpicturegraphic_c.h"

// Begin Namespace __IsoAgLib
namespace __IsoAgLib {

class vtObjectFillAttributes_c : public vtObject_c
{
public:
  //  Operation: stream
  //! @param destMemory:
  //! @param maxBytes: don't stream out more than that or you'll overrun the internal upload-buffer
  //! @param sourceOffset:
  int16_t stream(uint8_t* destMemory,
                 uint16_t maxBytes,
                 objRange_t sourceOffset);

  /// Operation: init
  /// @param vtObjectFillAttributesSROM
  /// @param b_initPointer
  void init(const iVtObjectFillAttributes_s* vtObjectFillAttributesSROM MULTITON_INST_PARAMETER_DEF_WITH_COMMA)
  { vtObject_c::init ((iVtObject_s*) vtObjectFillAttributesSROM MULTITON_INST_PARAMETER_USE_WITH_COMMA); }

  //  Operation: get_vtObjectFillAttributes_a
  iVtObjectFillAttributes_s* get_vtObjectFillAttributes_a() { return (iVtObjectFillAttributes_s *)&(get_vtObject_a()); }

  //  Operation: vtObjectFillAttributes_c
  vtObjectFillAttributes_c();

  //  Operation: size
  uint32_t fitTerminal() const;

  /// Operation: setFillType
  /// @param newValue
  /// @param b_updateObject default:false
  /// @param b_enableReplaceOfCmd default:false
  void setFillType(uint8_t newValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) {
    saveValue8SetAttribute ((b_updateObject) ? MACRO_getStructOffset(get_vtObjectFillAttributes_a(), fillType) : 0, sizeof(iVtObjectFillAttributes_s), 1 /* "Fill Type" */, newValue, newValue, b_enableReplaceOfCmd);
  }

  /// Operation: setFillColour
  /// @param newValue
  /// @param b_updateObject default:false
/// @param b_enableReplaceOfCmd default:false
  void setFillColour(uint8_t newValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) {
    saveValue8SetAttribute ((b_updateObject) ? MACRO_getStructOffset(get_vtObjectFillAttributes_a(), fillColour) : 0, sizeof(iVtObjectFillAttributes_s), 2 /* "Fill Colour" */, newValue, __IsoAgLib::getVtClientInstance4Comm().getClientByID (s_properties.clientId).getUserClippedColor (newValue, this, IsoAgLib::FillColour), b_enableReplaceOfCmd);
  }

  /// Operation: setFillPattern
  /// @param newValue
  /// @param b_updateObject default:false
  /// @param b_enableReplaceOfCmd default:false
  void setFillPattern(IsoAgLib::iVtObjectPictureGraphic_c* newValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) {
    saveValuePSetAttribute ((b_updateObject) ? MACRO_getStructOffset(get_vtObjectFillAttributes_a(), fillPatternObject) : 0, sizeof(iVtObjectFillAttributes_s), 3 /* "Fill Pattern" */, newValue, b_enableReplaceOfCmd);
  }

  //  Operation: changeFillAttributes
  //! @param newFillType
  //! @param newFillColour
  //! @param newFillPattern
  //! @param b_updateObject default:false
  /// @param b_enableReplaceOfCmd default:false
  void setFillAttributes(uint8_t newFillType, uint8_t newFillColour, IsoAgLib::iVtObjectPictureGraphic_c* newFillPattern, bool b_updateObject=false, bool b_enableReplaceOfCmd=false);

#ifdef USE_ISO_TERMINAL_GETATTRIBUTES
  /** that attribute is in parentheses in the spec, so commented out here
  uint8_t updateObjectType() const { return 25; }
  */

  uint8_t updateFillType(bool b_SendRequest=false);

  uint8_t updateFillColour(bool b_SendRequest=false);

  uint16_t updateFillPattern(bool b_SendRequest=false);

  void saveReceivedAttribute (uint8_t attrID, uint8_t* pui8_attributeValue);
#endif
};

} // end namespace
#endif
