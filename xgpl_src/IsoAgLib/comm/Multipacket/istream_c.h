/***************************************************************************
                          istream_c.h -
                             -------------------
    class                : ::iStream_c
    project              : IsoAgLib
    begin                : Tue Jan 25 17:39:02 2005
    copyright            : (C) 2005 by Achim Spangler (a.spangler@osb-ag.de)
    email                : a.spangler@osb-ag:de
    $Id: stream_c.h 1228 2005-06-03 10:32:42Z wodok $

    $Log$
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

#ifndef ISTREAM_C_H
#define ISTREAM_C_H

// IsoAgLib
#include <supplementary_driver/driver/datastreams/streaminput_c.h>



// Begin Namespace IsoAgLib
namespace IsoAgLib {




/**
  struct for stream identification
*/
class ReceiveStreamIdentifier_c {
public:
  ReceiveStreamIdentifier_c (uint16_t rui16_pgn, uint8_t rui8_da, uint8_t rui8_sa)
                          : ui16_pgn (rui16_pgn), ui8_da (rui8_da), ui8_sa (rui8_sa) {};

  ReceiveStreamIdentifier_c (const ReceiveStreamIdentifier_c& rc_rsi)
                          : ui16_pgn (rc_rsi.ui16_pgn), ui8_da (rc_rsi.ui8_da), ui8_sa (rc_rsi.ui8_sa) {};

  bool operator == (const ReceiveStreamIdentifier_c& rc_rsi)
  {
    return (rc_rsi.ui8_sa == ui8_sa) && (rc_rsi.ui8_da == ui8_da) && (rc_rsi.ui16_pgn == ui16_pgn);
  };

  bool matchSaDa(uint8_t sa, uint8_t da)
  {
    return (sa == ui8_sa) && (da == ui8_da);
  };

  /** test if the client's da/pgn match this RCI, so the incoming stream is to be handled by this client.
      the client also HAS to handle Broadcasts, so in case of (da==0xFF) he's also responsible!
    */
  bool matchDaPgn(uint8_t da, uint16_t pgn)
  {
    return ((da == 0xFF) || (da == ui8_da)) && (pgn == ui16_pgn);
  };

  uint8_t  getSa()  const { return ui8_sa; };
  uint8_t  getDa()  const { return ui8_da; };
  uint16_t getPgn() const { return ui16_pgn; };

private:

  /** PGN of the stream */
  uint16_t ui16_pgn;

  /** Destination of the stream, normall "part" of the PGN, i.e. PS-field */
  uint8_t ui8_da;

  /** source address of the corresponding sender */
  uint8_t ui8_sa;
};




//  +X2C Class 754 : Stream_c
class iStream_c : public StreamInput_c
{

public:

  iStream_c() : b_streamInvalid(false) {};

  virtual uint32_t getNotParsedSize()=0;

  virtual uint8_t getNextNotParsed()=0;

  virtual uint8_t getNotParsed(uint16_t ui16_notParsedRelativeOffset)=0;

  virtual uint32_t getByteTotalSize() const=0;

  virtual uint32_t getByteAlreadyReceived() const=0;

  virtual ReceiveStreamIdentifier_c& getIdent()=0;

  virtual uint8_t getFirstByte() const=0;

  virtual void setStreamFinishedJustKept()=0;

  void setStreamInvalid()       { b_streamInvalid = true; };
  bool getStreamInvalid() const { return b_streamInvalid; };


protected:

  // Has this stream been marked invalid while parsing?
  bool b_streamInvalid;

}; // ~X2C

} // end namespace __IsoAgLib
#endif // -X2C
