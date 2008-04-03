/***************************************************************************
                          specialparsingbaseproptag_c.h
                             -------------------
    begin                : Mon Mar 31 2003
    copyright            : (C) 2003 - 2004 by Dipl.-Inf.(FH) Martina Winkler
    email                : m.winkler@osb-ag:de
    type                 : Header
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
 ***************************************************************************/
#ifndef SPECIALPARSINGBASEPROPTAG_C_H
#define SPECIALPARSINGBASEPROPTAG_C_H

#include <xercesc/dom/DOM.hpp>
#include <xercesc/dom/DOMDocument.hpp>
#include <xercesc/dom/DOMDocumentType.hpp>
#include <xercesc/dom/DOMElement.hpp>
#include <xercesc/dom/DOMImplementation.hpp>
#include <xercesc/dom/DOMImplementationLS.hpp>
#include <xercesc/dom/DOMNodeIterator.hpp>
#include <xercesc/dom/DOMNodeList.hpp>
#include <xercesc/dom/DOMText.hpp>

#include <xercesc/parsers/XercesDOMParser.hpp>
#include <xercesc/util/XMLUni.hpp>

#include "vt2iso-defines.hpp"
#include "vt2iso.hpp"

#ifndef SYSTEM_PC_VC
#include <stdint.h>
#else
  typedef unsigned char uint8_t;
  typedef signed char int8_t;
  typedef unsigned short uint16_t;
  typedef short int16_t;
  typedef unsigned int uint32_t;
  typedef int int32_t;
  typedef unsigned long long int uint64_t;
  typedef long int long int64_t;
#endif

#include <string>

XERCES_CPP_NAMESPACE_USE

class SpecialParsingBasePropTag_c
{
 public:
  /** check if found tag is invalid at this position */
  virtual bool checkTag(DOMNode* ap_Node, unsigned int aui_objectType, uint64_t a_ui64_ombType) = 0;

  /** that function tries to parse an unknown tag from vt2iso
    * @return returns 0 if successful else anything unequal to 0
    * @param ap_Node pointer to element in xml file
    * @param aui_objectType object type of pNode
    * @param rpcch_objName pointer to object name character string
  */
  virtual bool parseUnknownTag (DOMNode* ap_Node, unsigned int aui_objectType, char* rpcch_objName, vt2iso_c* pc_vt2iso) = 0;

  /// returns the object type from a given DOMNode if type is known and sets it
  virtual uint16_t getObjType (const char* node_name) { return ui8_objType; }

  /// if a tag has unknown attributes, set that flag
  virtual void setUnknownAttributes (bool b_hasUnknownAttr) {}

  /// simple setter function to set the object type
  void setObjType (uint8_t aui8_objType) { ui8_objType = aui8_objType; }

  /// set the object id for the current handled tag element
  virtual void setObjID (uint16_t aui16_objID) = 0;

  /** that functions creates all necessary files and prints out all collected data */
  virtual void outputCollectedData2Files(const char attrString[maxAttributeNames] [stringLength+1], const bool attrIsGiven[maxAttributeNames]) {}

  /** that functions writes to already existing files and prints out a record during runtime of vt2iso */
  virtual bool outputData2FilesPiecewise(const char attrString[maxAttributeNames] [stringLength+1], const bool attrIsGiven[maxAttributeNames], vt2iso_c* pc_vt2iso) {return true;}

  /// returns if the current parsing module contains proprietary object types which can be handled like basic object types
  virtual bool checkForProprietaryOrBasicObjTypes() = 0;

  virtual void setOmcType (uint64_t& apui64_omcType, const uint64_t* apui64_ombType, uint16_t aui16_objType)=0;

  virtual bool objHasArrayEventMacro (uint16_t objType) const =0;

  virtual bool objHasArrayObjectXY (uint16_t objType) const =0;

  virtual bool objHasArrayObject (uint16_t objType) const =0;

  virtual bool objHasArrayPoints (uint16_t objType) const =0;

  virtual ~SpecialParsingBasePropTag_c() {}

  SpecialParsingBasePropTag_c(): ui8_objType(0xFF) {}

 private:
  uint8_t ui8_objType;
};

#endif
