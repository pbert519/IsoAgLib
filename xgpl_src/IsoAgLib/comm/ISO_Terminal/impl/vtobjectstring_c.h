/***************************************************************************
                          isoterminalstringmss_c.h
                             -------------------
    begin                : Don Sep 4 2003
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

#ifndef VTOBJECTSTRING_C_H
#define VTOBJECTSTRING_C_H


// +X2C includes
#include <IsoAgLib/comm/Multipacket/impl/multisendstreamer_c.h>
#include "IsoAgLib/comm/Multipacket/impl/multisendpkg_c.h"
#include "vtobject_c.h"
// ~X2C


namespace __IsoAgLib {

/** helper class to avoid multiple inheritance by vtObjectString_c */
class vtObjectStringStreamer_c : public MultiSendStreamer_c
{
 public:

  //  Operation: setDataNextStreamPart
  //! Parameter:
  //! @param mspData:
  //! @param bytes:
  void setDataNextStreamPart(__IsoAgLib::MultiSendPkg_c* mspData,
                             uint8_t bytes);

  //  Operation: resetDataNextStreamPart
  void resetDataNextStreamPart();

  //  Operation: saveDataNextStreamPart
  void saveDataNextStreamPart();

  //  Operation: restoreDataNextStreamPart
  void restoreDataNextStreamPart();

  //  Operation: getStreamSize
  uint32_t getStreamSize();

  uint8_t getFirstByte () { return 179; /* Command: "Command" --- Parameter: "Change String Value"; */ };

  const char* getStringToStream() { return pc_stringToStream; };

  void set5ByteCommandHeader(uint8_t* destinBuffer);

  void setStringToStream( const char* rpc_stringToStream ) { pc_stringToStream = rpc_stringToStream;};
  void setStrLenToSend( uint16_t rui16_strLenToSend ) { ui16_strLenToSend = rui16_strLenToSend;};

  //  Operation: getID
  uint16_t getID() { return vtObject_a_ID; };
  //  Operation: setID
  void setID( uint16_t rui16_id ) { vtObject_a_ID = rui16_id; };
 private:
  // ID from the connected __IsoAgLib::vtObject_c
  uint16_t vtObject_a_ID;
  // those both will be set before multisending is getting started.
  const char* pc_stringToStream;
  uint16_t ui16_strLenToSend;

  //  Attribute: streamPosition
  uint32_t streamPosition;

  //  Attribute: uploadBuffer [7+1]
  uint8_t uploadBuffer [7+1]; /* "+1" for safety only */ 

  //  Attribute: streamPositionStored
  uint32_t streamPositionStored;

}; // ~X2C

//  +X2C Class 241 : vtObjectString
//!  Stereotype: class
class vtObjectString_c : public __IsoAgLib::vtObject_c
{
 public:
	vtObjectStringStreamer_c* getStreamer( void ) { return &c_streamer;};
 protected:

  // those both will be set before multisending is getting started.
	void setStringToStream( const char* rpc_stringToStream );
	void setStrLenToSend( uint16_t rui16_strLenToSend );

 private:
	// streaming helper class which is called by the sending class
	vtObjectStringStreamer_c c_streamer;
}; // ~X2C

} // end namespace __IsoAgLib

#endif // -X2C
