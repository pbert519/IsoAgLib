/*
  ivtobjectlinearbargraph_c.h

  (C) Copyright 2009 - 2010 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/
#ifndef IVTOBJECTLINEARBARGRAPH_C_H
#define IVTOBJECTLINEARBARGRAPH_C_H

// +X2C includes
#include "impl/vtobjectlinearbargraph_c.h"
// ~X2C
#ifdef USE_VTOBJECT_linearbargraph

// Begin Namespace IsoAgLib
namespace IsoAgLib {


//  +X2C Class 119 : iVtObjectLinearBarGraph_c
//!  Stereotype: Klasse
class iVtObjectLinearBarGraph_c : public __IsoAgLib::vtObjectLinearBarGraph_c
{

public:

  //  Operation: init
  //! Parameter:
  //! @param vtObjectLinearBarGraphSROM:
  //! @param b_initPointer:
  void init(const iVtObjectLinearBarGraph_s* vtObjectLinearBarGraphSROM MULTITON_INST_PARAMETER_DEF_WITH_COMMA) {
    vtObjectLinearBarGraph_c::init (vtObjectLinearBarGraphSROM MULTITON_INST_PARAMETER_USE_WITH_COMMA);
  }

  //  Operation: get_vtObjectLinearBarGraph_a
  const iVtObjectLinearBarGraph_s& get_vtObjectLinearBarGraph_a() { return *vtObjectLinearBarGraph_c::get_vtObjectLinearBarGraph_a(); }

  //  Operation: setValue
  //! Parameter:
  //! @param newValue:
  //! @param b_updateObject:
  void setValue(uint16_t newValue, bool b_updateObject= false, bool b_enableReplaceOfCmd=true) {
    vtObjectLinearBarGraph_c::setValue (newValue, b_updateObject, b_enableReplaceOfCmd);
  }

  //  Operation: setWidth
  //! Parameter:
  //! @param newValue:
  //! @param b_updateObject:
  void setWidth(uint16_t newValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) {
    vtObjectLinearBarGraph_c::setWidth (newValue, b_updateObject, b_enableReplaceOfCmd);
  }

  //  Operation: setHeight
  //! Parameter:
  //! @param newValue:
  //! @param b_updateObject:
  void setHeight(uint16_t newValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) {
    vtObjectLinearBarGraph_c::setHeight (newValue, b_updateObject, b_enableReplaceOfCmd);
  }

  //  Operation: setColour
  //! Parameter:
  //! @param newValue:
  //! @param b_updateObject:
  void setColour(uint8_t newValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) {
    vtObjectLinearBarGraph_c::setColour (newValue, b_updateObject, b_enableReplaceOfCmd);
  }

  //  Operation: setTargetLineColour
  //! Parameter:
  //! @param newValue:
  //! @param b_updateObject:
  void setTargetLineColour(uint8_t newValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) {
    vtObjectLinearBarGraph_c::setTargetLineColour (newValue, b_updateObject, b_enableReplaceOfCmd);
  }

  //  Operation: setOptions
  //! Parameter:
  //! @param newValue:
  //! @param b_updateObject:
  void setOptions(uint8_t newValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) {
    vtObjectLinearBarGraph_c::setOptions (newValue, b_updateObject, b_enableReplaceOfCmd);
  }

  //  Operation: setNumberOfTicks
  //! Parameter:
  //! @param newValue:
  //! @param b_updateObject:
  void setNumberOfTicks(uint8_t newValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) {
    vtObjectLinearBarGraph_c::setNumberOfTicks (newValue, b_updateObject, b_enableReplaceOfCmd);
  }

  //  Operation: setMinValue
  //! Parameter:
  //! @param newValue:
  //! @param b_updateObject:
  void setMinValue(uint16_t newValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) {
    vtObjectLinearBarGraph_c::setMinValue (newValue, b_updateObject, b_enableReplaceOfCmd);
  }

  //  Operation: setMaxValue
  //! Parameter:
  //! @param newValue:
  //! @param b_updateObject:
  void setMaxValue(uint16_t newValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) {
    vtObjectLinearBarGraph_c::setMaxValue (newValue, b_updateObject, b_enableReplaceOfCmd);
  }

  //  Operation: setVariableReference
  //! Parameter:
  //! @param newValue:
  //! @param b_updateObject:
  void setVariableReference(iVtObject_c* newValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) {
    vtObjectLinearBarGraph_c::setVariableReference (newValue, b_updateObject, b_enableReplaceOfCmd);
  }

  //  Operation: setTargetValueVariableReference
  //! Parameter:
  //! @param newValue:
  //! @param b_updateObject:
  void setTargetValueVariableReference(iVtObject_c* newValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) {
    vtObjectLinearBarGraph_c::setTargetValueVariableReference (newValue, b_updateObject, b_enableReplaceOfCmd);
  }

  //  Operation: setTargetValue
  //! Parameter:
  //! @param newValue:
  //! @param b_updateObject:
  void setTargetValue(uint16_t newValue, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) {
    vtObjectLinearBarGraph_c::setTargetValue (newValue, b_updateObject, b_enableReplaceOfCmd);
  }

  //  Operation: setSize
  //! Parameter:
  //! @param newWidth:
  //! @param newHeight:
  //! @param b_updateObject:
  void setSize(uint16_t newWidth, uint16_t newHeight, bool b_updateObject=false, bool b_enableReplaceOfCmd=false) {
    vtObjectLinearBarGraph_c::setSize(newWidth, newHeight, b_updateObject, b_enableReplaceOfCmd);
  }
#ifdef USE_ISO_TERMINAL_GETATTRIBUTES
  // ///////////////////////// getter for attributes
  /** that attribute is in parentheses in the spec, so commented out here
  uint8_t updateObjectType() const { return vtObjectLinearBarGraph_c::updateObjectType(); }
  */

  uint16_t updateWidth(bool b_SendRequest=false) {
    return vtObjectLinearBarGraph_c::updateWidth(b_SendRequest);
  }

  uint16_t updateHeight(bool b_SendRequest=false) {
    return vtObjectLinearBarGraph_c::updateHeight(b_SendRequest);
  }

  uint8_t updateColour(bool b_SendRequest=false) {
    return vtObjectLinearBarGraph_c::updateColour(b_SendRequest);
  }

  uint8_t updateTargetLineColour(bool b_SendRequest=false) {
    return vtObjectLinearBarGraph_c::updateTargetLineColour(b_SendRequest);
  }

  uint8_t updateOptions(bool b_SendRequest=false) {
    return vtObjectLinearBarGraph_c::updateOptions(b_SendRequest);
  }

  uint8_t updateNumberOfTicks(bool b_SendRequest=false) {
    return vtObjectLinearBarGraph_c::updateNumberOfTicks(b_SendRequest);
  }

  uint16_t updateMinValue(bool b_SendRequest=false) {
    return vtObjectLinearBarGraph_c::updateMinValue(b_SendRequest);
  }

  uint16_t updateMaxValue(bool b_SendRequest=false) {
    return vtObjectLinearBarGraph_c::updateMaxValue(b_SendRequest);
  }

  uint16_t updateVariableReference(bool b_SendRequest=false) {
    return vtObjectLinearBarGraph_c::updateVariableReference(b_SendRequest);
  }

  uint16_t updateTargetValueVariableReference(bool b_SendRequest=false) {
    return vtObjectLinearBarGraph_c::updateTargetValueVariableReference(b_SendRequest);
  }

  uint16_t updateTargetValue(bool b_SendRequest=false) {
    return vtObjectLinearBarGraph_c::updateTargetValue(b_SendRequest);
  }

  /** that attribute is in parentheses in the spec, so commented out here
  uint16_t updateValue(bool b_SendRequest=false) {
    return vtObjectLinearBarGraph_c::getValue(b_SendRequest);
  }
  */
#endif
};

} // end of namespace IsoAgLib

#endif
#endif
