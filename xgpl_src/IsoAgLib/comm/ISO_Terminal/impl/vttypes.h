/***************************************************************************
                          vttypes.h
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

#ifndef VTTYPES_H
#define VTTYPES_H


#include <IsoAgLib/typedef.h>

#define FLAG_BIT_IN_RAM 0
#define FLAG_BIT_ORIGIN_SKM 1
#define FLAG_BIT_STRING_IN_RAM 2
#define FLAG_BIT_OBJECTS2FOLLOW_IN_RAM 3

#define FLAG_IN_RAM (1<<FLAG_BIT_IN_RAM)
#define FLAG_ORIGIN_SKM (1<<FLAG_BIT_ORIGIN_SKM)
#define FLAG_STRING_IN_RAM (1<<FLAG_BIT_STRING_IN_RAM)
#define FLAG_OBJECTS2FOLLOW_IN_RAM (1<<FLAG_BIT_OBJECTS2FOLLOW_IN_RAM)


namespace __IsoAgLib {

// Forward declarations
class vtObject_c;
class vtObjectWorkingSet_c;
class vtObjectDataMask_c;
class vtObjectAlarmMask_c;
class vtObjectContainer_c;
class vtObjectSoftKeyMask_c;
class vtObjectKey_c;
class vtObjectButton_c;
class vtObjectInputBoolean_c;
class vtObjectInputString_c;
class vtObjectInputNumber_c;
class vtObjectInputList_c;
class vtObjectOutputString_c;
class vtObjectOutputNumber_c;
class vtObjectLine_c;
class vtObjectRectangle_c;
class vtObjectEllipse_c;
class vtObjectPolygon_c;
class vtObjectMeter_c;
class vtObjectLinearBarGraph_c;
class vtObjectArchedBarGraph_c;
class vtObjectPictureGraphic_c;
class vtObjectNumberVariable_c;
class vtObjectStringVariable_c;
class vtObjectFontAttributes_c;
class vtObjectLineAttributes_c;
class vtObjectFillAttributes_c;
class vtObjectInputAttributes_c;
class vtObjectObjectPointer_c;
class vtObjectMacro_c;
class vtObjectArchedBarGraph_c;

class vtObjectAuxiliaryFuntion_c;
class vtObjectAuxiliaryInput_c;

} // end namespace __IsoAgLib


#define MACRO_localVars \
    MACRO_vtObjectTypeS* MACRO_vtObjectTypeA = (MACRO_vtObjectTypeS *) vtObject_a;

#define MACRO_streamLocalVars \
    MACRO_localVars \
    /* current bytes written */ \
    uint16_t curBytes=0;

#define MACRO_scaleLocalVarVtDimension \
    uint32_t vtDimension=__IsoAgLib::getIsoTerminalInstance().getVtHardwareDimension();

#define MACRO_scaleLocalVarOpDimension \
    uint32_t opDimension=__IsoAgLib::getIsoTerminalInstance().getVtObjectPoolDimension();

#define MACRO_scaleLocalVars \
    MACRO_scaleLocalVarOpDimension \
    MACRO_scaleLocalVarVtDimension

#define MACRO_scaleSKLocalVars \
    int32_t opSoftKeyWidth  = __IsoAgLib::getIsoTerminalInstance().getVtObjectPoolSoftKeyWidth(); \
    int32_t opSoftKeyHeight = __IsoAgLib::getIsoTerminalInstance().getVtObjectPoolSoftKeyHeight(); \
    int32_t vtSoftKeyWidth  = __IsoAgLib::getIsoTerminalInstance().getVtCapabilities ()->skWidth; \
    int32_t vtSoftKeyHeight = __IsoAgLib::getIsoTerminalInstance().getVtCapabilities ()->skHeight; \
    int32_t factorX = (vtSoftKeyWidth  << 20) / opSoftKeyWidth; \
    int32_t factorY = (vtSoftKeyHeight << 20) / opSoftKeyHeight; \
    int32_t factor  = (factorX < factorY) ? factorX : factorY;


#define MACRO_streamObject(bytesBefore) \
    uint16_t nrObject = (sourceOffset-(bytesBefore)) / 2; \
    while ((sourceOffset >= (bytesBefore)) && (sourceOffset < ((bytesBefore)+2*MACRO_vtObjectTypeA->numberOfObjectsToFollow)) && ((curBytes+2) <= maxBytes)) { \
      /* write out an object */ \
      if(MACRO_vtObjectTypeA->objectsToFollow [nrObject].vtObject != NULL) { \
        destMemory [curBytes]   = MACRO_vtObjectTypeA->objectsToFollow [nrObject].vtObject->getID() & 0xFF; \
        destMemory [curBytes+1] = MACRO_vtObjectTypeA->objectsToFollow [nrObject].vtObject->getID() >> 8; \
      } else { \
      destMemory [curBytes]   = 0xFF; \
        destMemory [curBytes+1] = 0xFF; \
      } \
      nrObject++; \
      curBytes += 2; \
      sourceOffset += 2; \
    }

#define MACRO_streamObjectXY(bytesBefore) \
    uint16_t nrObjectXY = (sourceOffset-(bytesBefore)) / 6; \
    while ((sourceOffset >= (bytesBefore)) && (sourceOffset < ((bytesBefore)+6*MACRO_vtObjectTypeA->numberOfObjectsToFollow)) && ((curBytes+6) <= maxBytes)) { \
      /* write out an object_X_Y pair */ \
      destMemory [curBytes]   = MACRO_vtObjectTypeA->objectsToFollow [nrObjectXY].vtObject->getID() & 0xFF; \
      destMemory [curBytes+1] = MACRO_vtObjectTypeA->objectsToFollow [nrObjectXY].vtObject->getID() >> 8; \
      int32_t xBlock, yBlock; \
      if (MACRO_vtObjectTypeA->objectsToFollow [nrObjectXY].vtObjectBlockFont != NULL) { \
        xBlock = MACRO_vtObjectTypeA->objectsToFollow [nrObjectXY].col * (MACRO_vtObjectTypeA->objectsToFollow [nrObjectXY].vtObjectBlockFont->getScaledWidthHeight () >> 8); \
        yBlock = MACRO_vtObjectTypeA->objectsToFollow [nrObjectXY].row * (MACRO_vtObjectTypeA->objectsToFollow [nrObjectXY].vtObjectBlockFont->getScaledWidthHeight () & 0xFF); \
      } else { \
        xBlock = 0; \
        yBlock = 0; \
      } \
      if (flags & FLAG_ORIGIN_SKM) { \
        destMemory [curBytes+2] = ((((int32_t) (MACRO_vtObjectTypeA->objectsToFollow [nrObjectXY].x) * factor) >> 20)+xBlock) & 0xFF; \
        destMemory [curBytes+3] = ((((int32_t) (MACRO_vtObjectTypeA->objectsToFollow [nrObjectXY].x) * factor) >> 20)+xBlock) >> 8; \
        destMemory [curBytes+4] = ((((int32_t) (MACRO_vtObjectTypeA->objectsToFollow [nrObjectXY].y) * factor) >> 20)+yBlock) & 0xFF; \
        destMemory [curBytes+5] = ((((int32_t) (MACRO_vtObjectTypeA->objectsToFollow [nrObjectXY].y) * factor) >> 20)+yBlock) >> 8; \
      } else { \
        destMemory [curBytes+2] = ((((int32_t) (MACRO_vtObjectTypeA->objectsToFollow [nrObjectXY].x)*vtDimension)/opDimension)+xBlock) & 0xFF; \
        destMemory [curBytes+3] = ((((int32_t) (MACRO_vtObjectTypeA->objectsToFollow [nrObjectXY].x)*vtDimension)/opDimension)+xBlock) >> 8; \
        destMemory [curBytes+4] = ((((int32_t) (MACRO_vtObjectTypeA->objectsToFollow [nrObjectXY].y)*vtDimension)/opDimension)+yBlock) & 0xFF; \
        destMemory [curBytes+5] = ((((int32_t) (MACRO_vtObjectTypeA->objectsToFollow [nrObjectXY].y)*vtDimension)/opDimension)+yBlock) >> 8; \
      } \
      nrObjectXY++; \
      curBytes += 6; \
      sourceOffset += 6; \
    }

#define MACRO_streamObjectXYcenteredInSoftKey(bytesBefore) \
    uint16_t nrObjectXY = (sourceOffset-(bytesBefore)) / 6; \
    MACRO_scaleSKLocalVars \
    int16_t centerX = (vtSoftKeyWidth -  ((opSoftKeyWidth *factor) >> 20)) >>1; \
    int16_t centerY = (vtSoftKeyHeight - ((opSoftKeyHeight*factor) >> 20)) >>1; \
    while ((sourceOffset >= (bytesBefore)) && (sourceOffset < ((bytesBefore)+6*MACRO_vtObjectTypeA->numberOfObjectsToFollow)) && ((curBytes+6) <= maxBytes)) { \
      /* write out an objectX_y pair */ \
      destMemory [curBytes]   = MACRO_vtObjectTypeA->objectsToFollow [nrObjectXY].vtObject->getID() & 0xFF; \
      destMemory [curBytes+1] = MACRO_vtObjectTypeA->objectsToFollow [nrObjectXY].vtObject->getID() >> 8; \
      destMemory [curBytes+2] = (MACRO_vtObjectTypeA->objectsToFollow [nrObjectXY].x+centerX) & 0xFF; \
      destMemory [curBytes+3] = (MACRO_vtObjectTypeA->objectsToFollow [nrObjectXY].x+centerX) >> 8; \
      destMemory [curBytes+4] = (MACRO_vtObjectTypeA->objectsToFollow [nrObjectXY].y+centerY) & 0xFF; \
      destMemory [curBytes+5] = (MACRO_vtObjectTypeA->objectsToFollow [nrObjectXY].y+centerY) >> 8; \
      nrObjectXY++; \
      curBytes += 6; \
      sourceOffset += 6; \
    }

#define MACRO_streamEventMacro(bytesBefore) \
    uint16_t nrEventMacro = (sourceOffset-(bytesBefore)) / 2; \
    while ((sourceOffset >= (bytesBefore)) && (sourceOffset < ((bytesBefore)+2*MACRO_vtObjectTypeA->numberOfMacrosToFollow)) && ((curBytes+2) <= maxBytes)) { \
      /* write out an objectX_y pair */ \
      destMemory [curBytes]   = MACRO_vtObjectTypeA->macrosToFollow [nrEventMacro].event; \
      destMemory [curBytes+1] = MACRO_vtObjectTypeA->macrosToFollow [nrEventMacro].vtObjectMacro->getID() & 0xFF; /* Macro ObjID must be 0-255 !! */ \
      nrEventMacro++; \
      curBytes += 2; \
      sourceOffset += 2; \
    }

#define MACRO_streamLanguages(bytesBefore)\
    uint16_t nrLanguageCode = (sourceOffset-(bytesBefore)) / 2; \
    while ((sourceOffset >= (bytesBefore)) && (sourceOffset < ((bytesBefore)+2*MACRO_vtObjectTypeA->numberOfLanguagesToFollow)) && ((curBytes+2) <= maxBytes)) { \
      /* write out a two character Language Code pair */ \
      destMemory [curBytes]   = MACRO_vtObjectTypeA->languagesToFollow [nrLanguageCode].language[0]; \
      destMemory [curBytes+1] = MACRO_vtObjectTypeA->languagesToFollow [nrLanguageCode].language[1]; \
      nrLanguageCode++; \
      curBytes += 2; \
      sourceOffset += 2; \
    }

/** @todo: polygon positions with x/yBlock!
      if (MACRO_vtObjectTypeA->objectsToFollow [nrObjectXY].vtObjectBlockFont != NULL) { \
        xBlock = MACRO_vtObjectTypeA->objectsToFollow [nrObjectXY].col * (MACRO_vtObjectTypeA->objectsToFollow [nrObjectXY].vtObjectBlockFont->getScaledWidthHeight () >> 8); \
        yBlock = MACRO_vtObjectTypeA->objectsToFollow [nrObjectXY].row * (MACRO_vtObjectTypeA->objectsToFollow [nrObjectXY].vtObjectBlockFont->getScaledWidthHeight () & 0xFF); \
      } else { \
      } \
*/
/* M.Wodok's Version... */
#define MACRO_streamPolygonPoints(bytesBefore) \
    uint16_t nrPointXY = (sourceOffset-(bytesBefore)) >>2; \
    while ((sourceOffset >= (bytesBefore)) && (sourceOffset < ((bytesBefore)+4*MACRO_vtObjectTypeA->numberOfPoints)) && ((curBytes+4) <= maxBytes)) { \
      int32_t xBlock, yBlock; \
        xBlock = 0; \
        yBlock = 0; \
      if (flags & FLAG_ORIGIN_SKM) { \
        destMemory [curBytes  ] = ((((uint32_t) (MACRO_vtObjectTypeA->pointsToFollow [nrPointXY].x) * factor) >> 20)+xBlock) & 0xFF; \
        destMemory [curBytes+1] = ((((uint32_t) (MACRO_vtObjectTypeA->pointsToFollow [nrPointXY].x) * factor) >> 20)+xBlock) >> 8; \
        destMemory [curBytes+2] = ((((uint32_t) (MACRO_vtObjectTypeA->pointsToFollow [nrPointXY].y) * factor) >> 20)+yBlock) & 0xFF; \
        destMemory [curBytes+3] = ((((uint32_t) (MACRO_vtObjectTypeA->pointsToFollow [nrPointXY].y) * factor) >> 20)+yBlock) >> 8; \
      } else { \
        destMemory [curBytes  ] = ((((uint32_t) (MACRO_vtObjectTypeA->pointsToFollow [nrPointXY].x)*vtDimension)/opDimension)+xBlock) & 0xFF; \
        destMemory [curBytes+1] = ((((uint32_t) (MACRO_vtObjectTypeA->pointsToFollow [nrPointXY].x)*vtDimension)/opDimension)+xBlock) >> 8; \
        destMemory [curBytes+2] = ((((uint32_t) (MACRO_vtObjectTypeA->pointsToFollow [nrPointXY].y)*vtDimension)/opDimension)+yBlock) & 0xFF; \
        destMemory [curBytes+3] = ((((uint32_t) (MACRO_vtObjectTypeA->pointsToFollow [nrPointXY].y)*vtDimension)/opDimension)+yBlock) >> 8; \
      } \
      nrPointXY++; \
      curBytes += 4; \
      sourceOffset += 4; \
    }





#define MACRO_getStructOffset(structPointer,structElement) \
  ((uint16_t) ((uint8_t*)(&structPointer->structElement) - ((uint8_t*)structPointer)))

#endif
