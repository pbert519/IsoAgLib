/*
  ivtobjectkey_c.h

  (C) Copyright 2009 - 2011 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/
#ifndef IVTOBJECTKEY_C_H
#define IVTOBJECTKEY_C_H

// +X2C includes
#include "impl/vtobjectkey_c.h"
// ~X2C
#ifdef USE_VTOBJECT_key

// Begin Namespace IsoAgLib
namespace IsoAgLib {


//  +X2C Class 86 : iVtObjectKey_c
//!  Stereotype: Klasse
class iVtObjectKey_c : public __IsoAgLib::vtObjectKey_c
{

public:

  //  Operation: init
  //! Parameter:
  //! @param vtObjectKeySROM:
  //! @param b_initPointer:
  void init(const iVtObjectKey_s* vtObjectKeySROM MULTITON_INST_PARAMETER_DEF_WITH_COMMA) {
    vtObjectKey_c::init (vtObjectKeySROM MULTITON_INST_PARAMETER_USE_WITH_COMMA);
  }

  //  Operation: get_vtObjectKey_a
  const iVtObjectKey_s& get_vtObjectKey_a() { return *vtObjectKey_c::get_vtObjectKey_a(); }

  //  Operation: setBackgroundColour
  //! Parameter:
  //! @param newValue:
  //! @param b_updateObject:
  void setBackgroundColour(uint8_t newValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) {
    vtObjectKey_c::setBackgroundColour (newValue, b_updateObject, b_enableReplaceOfCmd);
  }

  //  Operation: setKeyCode
  //! Parameter:
  //! @param newValue: Range 1-255, KeyCode 0 is reserved for KeyCode:"ESC"
  //! @param b_initPointer:
  void setKeyCode(uint8_t newValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) {
    vtObjectKey_c::setKeyCode (newValue, b_updateObject, b_enableReplaceOfCmd);
  }

  //  Operation: moveChildLocation
  //! Parameter:
  //! @param apc_childObject:
  //! @param dx:
  //! @param dy:
  //! @param b_updateObject:
  bool moveChildLocation(IsoAgLib::iVtObject_c* apc_childObject, int8_t dx, int8_t dy, bool b_updateObject=false) {
    return vtObjectKey_c::moveChildLocation(apc_childObject, dx, dy, b_updateObject);
  }

  //  Operation: setChildPosition
  //! @param apc_childObject:
  //! @param dx:
  //! @param dy:
  bool setChildPosition(IsoAgLib::iVtObject_c* apc_childObject, int16_t dx, int16_t dy, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) {
    return vtObjectKey_c::setChildPosition(apc_childObject, dx, dy, b_updateObject, b_enableReplaceOfCmd);
  }
#ifdef USE_ISO_TERMINAL_GETATTRIBUTES
  // ///////////////////////// getter for attributes
  /** that attribute is in parentheses in the spec, so commented out here
  uint8_t updateObjectType() const {
  return vtObjectKey_c::updateObjectType();
  }
   */

  uint8_t updateBackgroundColour(bool b_SendRequest=false) {
    return vtObjectKey_c::updateBackgroundColour(b_SendRequest);
  }

  uint8_t updateKeyCode(bool b_SendRequest=false) {
    return vtObjectKey_c::updateKeyCode(b_SendRequest);
  }
#endif
};

} // end of namespace IsoAgLib

#endif
#endif
