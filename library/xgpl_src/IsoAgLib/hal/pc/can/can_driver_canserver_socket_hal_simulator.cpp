/*
  can_driver_canserver_socket_hal_simulator.cpp:
    source-additions for HAL simulator for CAN communication.

  NOTE: The defines below will be injected into the original
  can_client_sock-module at the end of the file to modify it
  slightly (i.e. instrument it).

  (C) Copyright 2009 - 2019 by OSB AG

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Usage under Commercial License:
  Licensees with a valid commercial license may use this file
  according to their commercial license agreement. (To obtain a
  commercial license contact OSB AG via <http://isoaglib.com/en/contact>)

  Usage under GNU General Public License with exceptions for ISOAgLib:
  Alternatively (if not holding a valid commercial license)
  use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/

// See if the HALSimulator_c wants to insert a can msg for this channel
#define ENTRY_POINT_FOR_INSERT_RECEIVE_CAN_MSG                                                            \
{                                                                                                         \
  uint8_t bBusNumber = channel;                                                                           \
  uint8_t bXtd = 0;                                                                                       \
  uint32_t dwId = 0;                                                                                      \
  uint8_t bDlc = 0;                                                                                       \
  uint8_t abData[8] = { 0,0,0,0,0,0,0,0 };                                                                \
                                                                                                          \
  if( __HAL::halSimulator().InsertReceiveCanMsg( bBusNumber, &bXtd, &dwId, &bDlc, abData ) == true )      \
  {                                                                                                       \
    isoaglib_assert( bDlc <= 8 );                                                                         \
                                                                                                          \
    const ecutime_t now = getTime();                                                                        \
    const __IsoAgLib::Ident_c::identType_t type = (bXtd != 0)                                             \
      ? __IsoAgLib::Ident_c::ExtendedIdent                                                                \
      : __IsoAgLib::Ident_c::StandardIdent;                                                               \
                                                                                                          \
    static __IsoAgLib::CanPkg_c msg;                                                                      \
    msg.setIdent( dwId, type );                                                                           \
    msg.setLen( bDlc );                                                                                   \
    msg.setTime( now );                                                                                   \
    memcpy( msg.getUint8DataPointer(), abData, bDlc );                                                    \
                                                                                                          \
    HAL::CanFifos_c::get( channel ).push( msg );                                                          \
  }                                                                                                       \
}                                                                                                         \


#define ENTRY_POINT_FOR_RECEIVE_CAN_MSG \
  __HAL::halSimulator().ReceiveCanMsg( s_transferBuf.s_data.ui8_bus, s_transferBuf.s_data.i32_sendTimeStamp, (msg.identType() == __IsoAgLib::Ident_c::ExtendedIdent ) ? 1 : 0, msg.ident(), msg.getLen(), msg.getUint8DataPointer() );

#define ENTRY_POINT_FOR_SEND_CAN_MSG \
  __HAL::halSimulator().SendCanMsg( s_transferBuf.s_data.ui8_bus, s_transferBuf.s_data.i32_sendTimeStamp, (msg.identType() == __IsoAgLib::Ident_c::ExtendedIdent ) ? 1 : 0, msg.ident(), msg.getLen(), msg.getUint8DataConstPointer() );

  
#include "can_driver_canserver_socket.cpp"
