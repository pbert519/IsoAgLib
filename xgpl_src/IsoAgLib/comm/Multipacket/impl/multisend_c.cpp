/***************************************************************************
                          multisend_c.cpp - transfer of multi message data
                             -------------------
    begin                : Fri Sep 28 2001
    copyright            : (C) 2000 - 2004 by Dipl.-Inform. Achim Spangler
    email                : a.spangler@osb-ag:de
    type                 : Source
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

//#define IOP_OUTPUT

/* *************************************** */
/* ********** include headers ************ */
/* *************************************** */
#include "multisend_c.h"
#include <IsoAgLib/comm/Multipacket/imultisendstreamer_c.h>

#include <IsoAgLib/driver/system/impl/system_c.h>
#include <IsoAgLib/driver/can/impl/canio_c.h>
#include <IsoAgLib/comm/Scheduler/impl/scheduler_c.h>

#ifdef DEBUG
  #ifdef SYSTEM_PC
    #include <iostream>
  #else
    #include <supplementary_driver/driver/rs232/impl/rs232io_c.h>
  #endif
#endif

#ifdef USE_DIN_TERMINAL
  #include <IsoAgLib/comm/DIN_Terminal/impl/dinmaskupload_c.h>
#endif

#ifdef USE_ISO_11783
//  #include <IsoAgLib/comm/ISO_Terminal/ivtobjectstring_c.h>
  #include <IsoAgLib/comm/SystemMgmt/impl/systemmgmt_c.h>
#endif

#if defined( IOP_OUTPUT ) && defined( SYSTEM_PC )
  #include <fstream>
#endif



namespace __IsoAgLib {
#if defined( PRT_INSTANCE_CNT ) && ( PRT_INSTANCE_CNT > 1 )
  /** C-style function, to get access to the unique MultiSend_c singleton instance
    * if more than one CAN BUS is used for IsoAgLib, an index must be given to select the wanted BUS
    */
  MultiSend_c& getMultiSendInstance( uint8_t rui8_instance ) { return MultiSend_c::instance( rui8_instance );};
#else
  /** C-style function, to get access to the unique MultiSend_c singleton instance */
  MultiSend_c& getMultiSendInstance( void ) { return MultiSend_c::instance();};
#endif


#ifdef USE_ISO_11783

/** @todo set the wanted sensemaking retries/timeout values here!!! */
#define DEF_TimeOut_ChangeStringValue 1500   /* 1,5 seconds are stated in F.1 (page 96) */
#define DEF_TimeOut_ChangeChildPosition 1500 /* 1,5 seconds are stated in F.1 (page 96) */
#define DEF_Retries_TPCommands 2
#define DEF_Retries_NormalCommands 2


/** This is mostly used for debugging now... */
void SendUploadBase_c::set (uint8_t* rpui8_buffer, uint32_t rui32_bufferSize)
{
  /// Use BUFFER - NOT MultiSendStreamer!
  vec_uploadBuffer.clear();
  vec_uploadBuffer.reserve (rui32_bufferSize);

  uint32_t i=0;
  for (; i < rui32_bufferSize; i++) {
    vec_uploadBuffer.push_back (*rpui8_buffer);
    rpui8_buffer++;
  }
  for (; i < 8; i++) {
    vec_uploadBuffer.push_back (0xFF);
  }

  ui8_retryCount = 0; // hacked, no retry here!!!

  ui32_uploadTimeout = DEF_TimeOut_ChangeStringValue;

  #ifdef DEBUG_HEAP_USEAGE
  if ( vec_uploadBuffer.capacity() != sui16_lastPrintedBufferCapacity )
  {
    sui16_lastPrintedBufferCapacity = vec_uploadBuffer.capacity();
    getRs232Instance() << "ISOTerminal_c Buffer-Capa: " << sui16_lastPrintedBufferCapacity << "\r\n";
  }
  #endif
}


/**
  >>StringUpload<< Constructors ( Copy and Reference! )
*/
void SendUploadBase_c::set (uint16_t rui16_objId, const char* rpc_string, uint16_t overrideSendLength, uint8_t ui8_cmdByte)
{
  // if string is shorter than length, it's okay to send - if it's longer, we'll clip - as client will REJECT THE STRING (FINAL ISO 11783 SAYS: "String Too Long")
  uint16_t strLen = (CNAMESPACE::strlen(rpc_string) < overrideSendLength) ? CNAMESPACE::strlen(rpc_string) : overrideSendLength;

  /// Use BUFFER - NOT MultiSendStreamer!
  vec_uploadBuffer.clear();
  vec_uploadBuffer.reserve (((5+strLen) < 8) ? 8 : (5+strLen)); // DO NOT USED an UploadBuffer < 8 as ECU->VT ALWAYS has 8 BYTES!

  vec_uploadBuffer.push_back (ui8_cmdByte ); /* Default of ui8_cmdByte is: Command: Command --- Parameter: Change String Value (TP) */
  vec_uploadBuffer.push_back (rui16_objId & 0xFF);
  vec_uploadBuffer.push_back (rui16_objId >> 8);
  vec_uploadBuffer.push_back (strLen & 0xFF);
  vec_uploadBuffer.push_back (strLen >> 8);
  int i=0;
  for (; i < strLen; i++) {
    vec_uploadBuffer.push_back (*rpc_string);
    rpc_string++;
  }
  for (; i < 3; i++) {
    // at least 3 bytes from the string have to be written, if not, fill with 0xFF, so the pkg-len is 8!
    vec_uploadBuffer.push_back (0xFF);
  }

  if ((5+strLen) < 9)
    ui8_retryCount = DEF_Retries_NormalCommands;
  else
    ui8_retryCount = DEF_Retries_TPCommands;

  ui32_uploadTimeout = DEF_TimeOut_ChangeStringValue;

  #ifdef DEBUG_HEAP_USEAGE
  if ( vec_uploadBuffer.capacity() != sui16_lastPrintedBufferCapacity )
  {
    sui16_lastPrintedBufferCapacity = vec_uploadBuffer.capacity();
    getRs232Instance() << "ISOTerminal_c Buffer-Capa: " << sui16_lastPrintedBufferCapacity << "\r\n";
  }
  #endif
}



/**
  Constructor used for "normal" 8-byte CAN-Pkgs!
*/
void SendUploadBase_c::set (uint8_t byte1, uint8_t byte2, uint8_t byte3, uint8_t byte4, uint8_t byte5, uint8_t byte6, uint8_t byte7, uint8_t byte8, uint32_t rui32_timeout)
{
  vec_uploadBuffer.clear();
  vec_uploadBuffer.reserve (8);

  vec_uploadBuffer.push_back (byte1);
  vec_uploadBuffer.push_back (byte2);
  vec_uploadBuffer.push_back (byte3);
  vec_uploadBuffer.push_back (byte4);
  vec_uploadBuffer.push_back (byte5);
  vec_uploadBuffer.push_back (byte6);
  vec_uploadBuffer.push_back (byte7);
  vec_uploadBuffer.push_back (byte8);

  ui8_retryCount = DEF_Retries_NormalCommands;
  ui32_uploadTimeout = rui32_timeout;
}


/**
  Constructor used for "ChangeChildPosition" >> 9 <<-byte CAN-Pkgs!
  -- Parameter "timeOut" only there as else the signature would be the same compared to 8byte+timeOut constructor!
  -- simply always pass "DEF_TimeOut_ChangeChildPosition"
*/
void SendUploadBase_c::set (uint8_t byte1, uint8_t byte2, uint8_t byte3, uint8_t byte4, uint8_t byte5, uint8_t byte6, uint8_t byte7, uint8_t byte8, uint8_t byte9, uint32_t rui32_timeout)
{
  /// Use BUFFER - NOT MultiSendStreamer!
  vec_uploadBuffer.clear();
  vec_uploadBuffer.reserve (9);

  vec_uploadBuffer.push_back (byte1);
  vec_uploadBuffer.push_back (byte2);
  vec_uploadBuffer.push_back (byte3);
  vec_uploadBuffer.push_back (byte4);
  vec_uploadBuffer.push_back (byte5);
  vec_uploadBuffer.push_back (byte6);
  vec_uploadBuffer.push_back (byte7);
  vec_uploadBuffer.push_back (byte8);
  vec_uploadBuffer.push_back (byte9);

  ui8_retryCount = DEF_Retries_TPCommands;
  ui32_uploadTimeout = rui32_timeout;
}


const SendUploadBase_c& SendUploadBase_c::operator= (const SendUploadBase_c& ref_source)
{
  vec_uploadBuffer = ref_source.vec_uploadBuffer;
  ui8_retryCount = ref_source.ui8_retryCount;
  ui32_uploadTimeout = ref_source.ui32_uploadTimeout;
  return ref_source;
}


SendUploadBase_c::SendUploadBase_c (const SendUploadBase_c& ref_source)
  : vec_uploadBuffer (ref_source.vec_uploadBuffer)
  , ui8_retryCount (ref_source.ui8_retryCount)
  , ui32_uploadTimeout (ref_source.ui32_uploadTimeout)
{}

#endif ///< block for SendUploadBase_c only in case of USE_ISO_11783

  /*************************************/
 /**** MultiSend_c Implementation ****/
/*************************************/

/// SendStream subclass implementation
//////////////////////////////////////

void
MultiSend_c::SendStream_c::init (uint8_t rb_send, uint8_t rb_empf, const HUGE_MEM uint8_t* rhpb_data, int32_t ri32_dataSize, uint16_t rui16_msgSize, sendSuccess_t& rrefen_sendSuccessNotify, IsoAgLib::iMultiSendStreamer_c* rpc_mss, msgType_t ren_msgType, bool rb_ext, uint16_t rui16_delay)
{
  b_send = rb_send;
  b_empf = rb_empf;
  hpb_data = rhpb_data;
  i32_dataSize = ri32_dataSize;   // initialise data for begin
  // LBS+ file uploads often demand a message size which is no
  // multiplicative from 7 -> %e.g. 1024 for Msk-Upload where 1029 is a factor of 7
  // but as only 1024 bytes have to be sent per message, and the rest must be
  // filled with 0xFF, the value 1024 is correct
  ui16_msgSize = rui16_msgSize;

  pc_mss = rpc_mss;
  en_msgType = ren_msgType;
  b_ext = rb_ext;

  i32_DC = i32_ack_DC = 0;
  i32_timestamp = 0;

  pc_multiSend->setDelay(rui16_delay);

  pen_sendSuccessNotify = &rrefen_sendSuccessNotify;
  en_sendState = SendRts;
  *pen_sendSuccessNotify = Running;
}


void
MultiSend_c::SendStream_c::initIso (uint8_t rb_send, uint8_t rb_empf, const HUGE_MEM uint8_t* rhpb_data, int32_t ri32_dataSize, sendSuccess_t& rrefen_sendSuccessNotify, int32_t ri32_pgn, IsoAgLib::iMultiSendStreamer_c* rpc_mss, msgType_t ren_msgType
  #if defined(NMEA_2000_FAST_PACKET)
  , bool rb_useFastPacket
  #endif
)
{
  // 1) initialise data from parameters
  init (rb_send, rb_empf, rhpb_data, ri32_dataSize, ri32_dataSize, rrefen_sendSuccessNotify, rpc_mss, ren_msgType, (ri32_dataSize >= 1786), scui8_isoCanPkgDelay);

  // 2) set initial values for ISO transfer
  //b_abortOnTimeout = <only needed in DIN case!>
  //b_fileCmd = <only needed in DIN case!>
  //b_try = 1; <only needed in DIN case!>
  i32_pgn = ri32_pgn;

#if defined (NMEA_2000_FAST_PACKET)
  b_fp = rb_useFastPacket;
  if ( rb_useFastPacket ) en_sendState = SendFpFirstFrame;
  ui8_FpSequenceCounter = 0;
#endif
}

void
MultiSend_c::SendStream_c::initDin (uint8_t rb_send, uint8_t rb_empf, const HUGE_MEM uint8_t* rhpb_data, int32_t ri32_dataSize, uint16_t rui16_msgSize, sendSuccess_t& rrefen_sendSuccessNotify, uint16_t rb_fileCmd, bool rb_abortOnTimeout)
{
  // 1) initialise data from parameters
  init (rb_send, rb_empf, rhpb_data, ri32_dataSize, rui16_msgSize, rrefen_sendSuccessNotify, NULL /* NOT supported by DIN */, Din, false, 4); // DIN with short delay between single pkg - must be >= 4msec

  // 2) set initial values for DIN transfer
  b_abortOnTimeout = rb_abortOnTimeout;
  b_fileCmd = rb_fileCmd;
  b_try = 1;
  //i32_pgn = ri32_pgn; // only needed for ISO!
}



void
MultiSend_c::singletonInit()
{
  setAlreadyClosed(); // so init() will init ;-)
  init();
};



/** initialisation for MultiSend_c */
void
MultiSend_c::init(void)
{ // clear state of b_alreadyClosed, so that close() is called one time
  // only init if closed (constructor "closes" it so it gets init'ed initially!
  if (checkAlreadyClosed())
  {
    // clear state of b_alreadyClosed, so that close() is called one time AND no more init()s are performed!
    clearAlreadyClosed();
    // first register in Scheduler_c
    getSchedulerInstance4Comm().registerClient( this );
    c_data.setSingletonKey( getSingletonVecKey() );

    // set BYTE-ORDER of MultiSendPkg_c
    data().setByteOrder(LSB_MSB);

    // detect if a new Filter is created, so that a reconfig of the CAN_IO
    // must be done at the end of this function
    bool b_isReconfigNeeded = false;

  #if defined(USE_ISO_11783)
    // register to get ISO monitor list changes
  __IsoAgLib::getIsoMonitorInstance4Comm().registerSaClaimHandler( this );
  #endif
  // ###################
  // Receive filters for ISO 11783 are created selectively in the SA claim handler, so that
  // only messages that are directed to a local SA are received
  // ###################
  #ifdef USE_DIN_TERMINAL
    // create filter to receive service to broadcast member messages from LBS+
    if (!getCanInstance4Comm().existFilter( *this, (uint16_t)0x700,(uint16_t)0x700))
    {
      getCanInstance4Comm().insertFilter( *this, 0x700,0x700, true);
      b_isReconfigNeeded = true;
    }
  #endif
    if ( b_isReconfigNeeded )
    { // at least one new Filter is created -> reconfig
      // CAN_IO receive structure in CANIO_c
      getCanInstance4Comm().reconfigureMsgObj();
    }
  }
}



/** every subsystem of IsoAgLib has explicit function for controlled shutdown
  */
void MultiSend_c::close( void ) {
  if ( ! checkAlreadyClosed() ) {
    // avoid another call
    setAlreadyClosed();
    // unregister from Scheduler_c
    getSchedulerInstance4Comm().unregisterClient( this );
  }
};

/** default destructor which has nothing to do */
MultiSend_c::~MultiSend_c(){
  close();
}



/**
  @return an "in-progress" stream or NULL if none active for this sa/da-key
*/
MultiSend_c::SendStream_c*
MultiSend_c::getSendStream(uint8_t rui8_sa, uint8_t rui8_da)
{
  for (std::list<SendStream_c>::iterator pc_iter=list_sendStream.begin(); pc_iter != list_sendStream.end(); pc_iter++)
  {
    if (pc_iter->matchSaDa(rui8_sa, rui8_da))
      return &*pc_iter;
  }
  return NULL;
}



/**
  use this function to add a new SendStream.
  IMPORTANT: Assure that the added SendStream is initialized right after this call!!
  @return reference to added SendStream ==> HAS TO BE INITIALIZED, because it may be a copy of the first (to avoid stack creation of new object)
*/
MultiSend_c::SendStream_c*
MultiSend_c::addSendStream(uint8_t rb_send, uint8_t rb_empf)
{
  SendStream_c* const pc_foundStream = getSendStream(rb_send, rb_empf);
  if (pc_foundStream)
  {
    if (!pc_foundStream->isFinished())
    {
      return NULL; // can't start a sendStream, one already active for this one..
    }
    else // use this finished one because it would be deleted anyway...
    {
      #ifdef DEBUG
      INTERNAL_DEBUG_DEVICE << "Using Stream from list which is already finished but not yet kicked from MultiSend_c::timeEvent()."<< INTERNAL_DEBUG_DEVICE_ENDL;
      #endif
      return pc_foundStream;
    }
  }

  if (list_sendStream.empty()) {
    list_sendStream.push_back (SendStream_c(*this SINGLETON_VEC_KEY_USE4CALL ));
  } else {
    list_sendStream.insert (list_sendStream.end(), list_sendStream.back()); // insert a copy of the first element (for performance reasons)
  }
  return &list_sendStream.back();
}



#ifdef USE_DIN_TERMINAL
/**
  send a DIN multipacket message for terminal accoring to LBS+
  @param rb_send dynamic member no of sender
  @param rb_empf dynamic member no of receiver
  @param hpb_data HUGE_MEM pointer to the data
  @param ri32_dataSize size of the complete mask
  @param rui16_msgSize size of one part message (mask is parted in different messages)
  @param rb_fileCmd terminal specific file command
  @param rb_abortOnTimeout choose if transfer should be aborted on timeout
        (instead of standard resend of last message) (default false)
  @return true -> MultiSend_c was ready -> mask is spooled to target
*/
bool MultiSend_c::sendDin(uint8_t rb_send, uint8_t rb_empf, const HUGE_MEM uint8_t* rhpb_data, int32_t ri32_dataSize, uint16_t rui16_msgSize, sendSuccess_t& rrefen_sendSuccessNotify, uint16_t rb_fileCmd, bool rb_abortOnTimeout)
{
  #if defined( DEBUG )
  INTERNAL_DEBUG_DEVICE << "MultiSend_c::sendDin with Len " << ri32_dataSize << "\n";
  #endif

  /// first check if new transfer can be started
  /// - check if there's already a SA/DA pair active (in this case NULL is returned!)
  /// - if not NULL is returned, it points to the newly generated stream.
  SendStream_c* pc_newSendStream = addSendStream(rb_send, rb_empf);

  if (pc_newSendStream)
  {
    pc_newSendStream->initDin (rb_send, rb_empf, rhpb_data, ri32_dataSize, rui16_msgSize, rrefen_sendSuccessNotify, rb_fileCmd, rb_abortOnTimeout);
    return true;
  }
  else
  {
    return false;
  }
}
#endif



#if defined(USE_ISO_11783)
/**
  send a ISO target multipacket message with active retrieve of data-parts to send
  @param rb_send dynamic member no of sender
  @param rb_empf dynamic member no of receiver
  @param rpc_mss allow active build of data stream parts for upload by deriving data source class
                 from IsoAgLib::iMultiSendStreamer_c, which defines virtual functions to control the
                 retrieve of data to send. This is especially important for ISO_Terminal,
                 which assembles the data pool dependent on the terminal capabilities during upload
                 ( e.g. bitmap variants )
  @param ri32_pgn PGN to use for the upload
  @param rrefen_sendSuccessNotify -> pointer to send state var, where the current state
          is written by MultiSend_c
  @return true -> MultiSend_c was ready -> mask is spooled to target
*/
bool
MultiSend_c::sendIsoTarget(uint8_t rb_send, uint8_t rb_empf, IsoAgLib::iMultiSendStreamer_c* rpc_mss, int32_t ri32_pgn, sendSuccess_t& rrefen_sendSuccessNotify)
{
  return sendIsoIntern(rb_send, rb_empf, NULL, rpc_mss->getStreamSize(), rrefen_sendSuccessNotify, ri32_pgn, rpc_mss, IsoTarget);
}



/**
  send a ISO target multipacket message
  @param rb_send dynamic member no of sender
  @param rb_empf dynamic member no of receiver
  @param rhpb_data HUGE_MEM pointer to the data
  @param ri32_dataSize size of the complete mask
  @param ri32_pgn PGN to use for the upload
  @param rrefen_sendSuccessNotify -> pointer to send state var, where the current state
          is written by MultiSend_c
  @return true -> MultiSend_c was ready -> mask is spooled to target
*/
bool
MultiSend_c::sendIsoTarget(uint8_t rb_send, uint8_t rb_empf, const HUGE_MEM uint8_t* rhpb_data, int32_t ri32_dataSize, int32_t ri32_pgn, sendSuccess_t& rrefen_sendSuccessNotify)
{
  return sendIsoIntern(rb_send, rb_empf, rhpb_data, ri32_dataSize, rrefen_sendSuccessNotify, ri32_pgn, NULL, IsoTarget);
}



/**
  send a ISO broadcast multipacket message
  @param rb_send dynamic member no of sender
  @param rb_empf dynamic member no of receiver
  @param hpb_data HUGE_MEM pointer to the data
  @param ri32_dataSize size of the complete mask
  @return true -> MultiSend_c was ready
*/
bool
MultiSend_c::sendIsoBroadcast(uint8_t rb_send, uint8_t rb_empf, const HUGE_MEM uint8_t* rhpb_data, int32_t ri32_dataSize, int32_t ri32_pgn, sendSuccess_t& rrefen_sendSuccessNotify)
{
  return sendIsoIntern(rb_send, rb_empf, rhpb_data, ri32_dataSize, rrefen_sendSuccessNotify, ri32_pgn, NULL /* NOT "yet" supported */, IsoBroadcast);
}

#if defined(NMEA_2000_FAST_PACKET)
bool MultiSend_c::sendIsoFastPacket(uint8_t rb_send, uint8_t rb_empf, HUGE_MEM uint8_t* rhpb_data, int32_t ri32_dataSize, int32_t ri32_pgn, sendSuccess_t& rrefen_sendSuccessNotify)
{ // no MSS here, not supported by IsoFastPacket
  return sendIsoIntern(rb_send, rb_empf, rhpb_data, ri32_dataSize, rrefen_sendSuccessNotify, ri32_pgn, NULL, IsoTarget, true);
}
bool MultiSend_c::sendIsoFastPacket(uint8_t rb_send, uint8_t rb_empf, IsoAgLib::iMultiSendStreamer_c* rpc_mss, int32_t ri32_pgn, sendSuccess_t& rrefen_sendSuccessNotify)
{ // o MSS here, not supported by IsoFastPacket
  return sendIsoIntern(rb_send, rb_empf, NULL, rpc_mss->getStreamSize(), rrefen_sendSuccessNotify, ri32_pgn, rpc_mss, IsoTarget, true);
}
#endif



/**
  internal function to send a ISO target multipacket message
  @param rb_send dynamic member no of sender
  @param rb_empf dynamic member no of receiver
  @param rhpb_data HUGE_MEM pointer to the data
  @param ri32_dataSize size of the complete mask
  @param ri32_pgn PGN to use for the upload
  @param rrefen_sendSuccessNotify -> pointer to send state var, where the current state
          is written by MultiSend_c
  @param rpc_mss allow active build of data stream parts for upload by deriving data source class
                 from IsoAgLib::iMultiSendStreamer_c, which defines virtual functions to control the
                 retrieve of data to send. This is especially important for ISO_Terminal,
                 which assembles the data pool dependent on the terminal capabilities during upload
                 ( e.g. bitmap variants )
  @return true -> MultiSend_c was ready -> mask is spooled to target
*/
bool
MultiSend_c::sendIsoIntern (uint8_t rb_send, uint8_t rb_empf, const HUGE_MEM uint8_t* rhpb_data, int32_t ri32_dataSize, sendSuccess_t& rrefen_sendSuccessNotify, int32_t ri32_pgn, IsoAgLib::iMultiSendStreamer_c* rpc_mss, msgType_t ren_msgType
  #if defined(NMEA_2000_FAST_PACKET)
  , bool rb_useFastPacket
  #endif
)
{
  /// first check if new transfer can be started
  /// - check if there's already a SA/DA pair active (in this case NULL is returned!)
  /// - if not NULL is returned, it points to the newly generated stream.
  SendStream_c* pc_newSendStream = addSendStream(rb_send, rb_empf);

  if (pc_newSendStream)
  {
    pc_newSendStream->initIso (rb_send, rb_empf, rhpb_data, ri32_dataSize, rrefen_sendSuccessNotify, ri32_pgn, rpc_mss, ren_msgType
    #if defined(NMEA_2000_FAST_PACKET)
    , rb_useFastPacket
    #endif
    );
    return true;
  }
  else
  {
    return false;
  }
}
#endif



/**
  perform periodical actions
  @return true -> all planned activities performed in allowed time
*/
#if defined( IOP_OUTPUT ) && defined(SYSTEM_PC)
static FILE *iopFile;
static int iopSequence;
#endif




/**
  @return true: stream finished
*/
bool
MultiSend_c::SendStream_c::timeEvent( uint8_t rui8_pkgCnt, int32_t ri32_time )
{
  uint8_t ui8_nettoDataCnt;
  uint8_t ui8_pkgInd;

  if (isFinished ())
  { // SendStream finished in processMsg(), so return true for deletion of SendStream.
    return true;
  }

  MultiSendPkg_c& refc_multiSendPkg = pc_multiSend->data();

  switch (en_sendState)
  {
#if defined (NMEA_2000_FAST_PACKET)
    case SendFpFirstFrame:
       // send Fast Packet First Frame
        if (b_fp)
        {
          refc_multiSendPkg.setUint8Data(0, static_cast<uint8_t>((ui8_FpSequenceCounter << 5) & 0xE0));
          refc_multiSendPkg.setUint8Data(1, static_cast<uint8_t>(getDataSize() & 0xFF));

          uint8_t ui8_nettoCnt = 6;
          if ( ui8_nettoCnt > getDataSize() ) ui8_nettoCnt = getDataSize();
          if (hpb_data != NULL) {
            refc_multiSendPkg.setFastPacketDataPart(hpb_data, 0, ui8_nettoCnt, 2);
          } else {
            pc_mss->setDataNextFastPacketStreamPart (&refc_multiSendPkg, ui8_nettoCnt, 2);
          }
          i32_DC = ui8_nettoCnt;
        }
        en_sendState = SendFpDataFrame;
        //ui32_lastNextPacketNumberToSend = 0xFFFFFFFF;
        i32_timestamp = ri32_time;
        ui32_offset = 0;
        b_pkgSent = 0;
        ui8_FpFrameNr = 0;

        // now data fields are set -> send
        sendIntern();
      break;
    case SendFpDataFrame:
    {
        uint8_t temp ;
        uint8_t ui8_freeCnt = getCanInstance4Comm().sendCanFreecnt(Ident_c::ExtendedIdent);

        // send only as much pkg as fits in send buffer (with spare of 2 for other use)
        if (ui8_freeCnt < 2)
           rui8_pkgCnt = 0;
        else if (rui8_pkgCnt > (ui8_freeCnt - 2))
           rui8_pkgCnt = (ui8_freeCnt - 2);

        if (rui8_pkgCnt == 0)        {
            #if defined( DEBUG )
            INTERNAL_DEBUG_DEVICE << "MultiSend_c::timeEvent --- pkgCnt == 0;\n";
            #endif
        }
        for (ui8_pkgInd = 0; ui8_pkgInd < rui8_pkgCnt; ui8_pkgInd++)
        {
            prepareSendMsg(ui8_nettoDataCnt);
            refc_multiSendPkg.setUint8Data(0, static_cast<uint8_t>(((ui8_FpSequenceCounter << 5) |(ui8_FpFrameNr & 0x1F)) ) );

            if (hpb_data != NULL) {
              refc_multiSendPkg.setFastPacketDataPart(hpb_data, i32_DC, ui8_nettoDataCnt, 1);
            } else {
              pc_mss->setDataNextFastPacketStreamPart (&refc_multiSendPkg, ui8_nettoDataCnt, 1);
            }
            sendIntern();
            i32_DC += ui8_nettoDataCnt;
            //b_pkgSent++; // sent on epkg so we know how much to increase the offset next time a DPO is sent
            // break if this message part is finished
            if (isCompleteData())
            {
                // isCompleteData()
                temp = ui8_FpSequenceCounter + 1; /** @todo check if is this needed as this Stream instance is afterwards deleted*/
                ui8_FpSequenceCounter = temp % 8; /** @todo check if is this needed as this Stream instance is afterwards deleted */
                b_fp = false; /** @todo check if is this needed as this Stream instance is afterwards deleted */
                *pen_sendSuccessNotify = SendSuccess;
                return true; // FINISHED SendStream, remove it from list please!
            }
        } // for
      }
      break;
#endif
    case SendRts:
      if ( isDelayEnd(ri32_time) )
      { // send RTS command
        if (b_ext) {
          refc_multiSendPkg.setUint8Data(0, static_cast<uint8_t>(eCM_RTS));
          refc_multiSendPkg.setData_2ByteInteger(1, getDataSize() & 0xFFFF);
          refc_multiSendPkg.setData_2ByteInteger(3, getDataSize() >> 16);
        } else {
          refc_multiSendPkg.setUint8Data(0, static_cast<uint8_t>(CM_RTS));
          refc_multiSendPkg.setData_2ByteInteger(1, getMsgSize());
          refc_multiSendPkg.setUint8Data(3, static_cast<uint8_t>(getPkgPerMsg()));
          refc_multiSendPkg.setUint8Data(4, static_cast<uint8_t>(0xFF));
        }
        en_sendState = AwaitCts;
        ui32_lastNextPacketNumberToSend = 0xFFFFFFFF; // so the first coming CTS is definitively NO repeated burst!
        i32_timestamp = ri32_time;
        ui32_offset = 0;
        b_pkgSent = 0;
        if (en_msgType == Din)
        {
          if (pc_multiSend->getMaxDelay() == 4)
            refc_multiSendPkg.setUint8Data(4, static_cast<uint8_t>(0xFF));
          else
            refc_multiSendPkg.setUint8Data(4, static_cast<uint8_t>((pc_multiSend->getMaxDelay() - 4)*2));
          refc_multiSendPkg.setUint8Data(5, static_cast<uint8_t>(b_fileCmd));
          refc_multiSendPkg.setData_2ByteInteger(6, getMsgNr());
        }
        else
        { // ISO multipacket
          #if defined( IOP_OUTPUT ) && defined( SYSTEM_PC )
          iopFile = fopen ("iop_dump.bin", "wb");
          iopSequence = 0;
          #endif
          if ( pc_mss != NULL ) pc_mss->resetDataNextStreamPart ();
          refc_multiSendPkg.setUint8Data (5, static_cast<uint8_t>(i32_pgn & 0xFF));
          refc_multiSendPkg.setUint16Data(6, static_cast<uint16_t>((i32_pgn >> 8) & 0xFFFFU));
          if (en_msgType != IsoTarget) refc_multiSendPkg.setUint8Data(0, static_cast<uint8_t>(CM_BAM)); // ISO_BAM cmd
          // is already set from above!!: else data().setUint8Data(0, CM_RTS); // ISO_RTS cmd
        }
        // now data fields are set -> send
        sendIntern();
      }
      break;
    case AwaitCts:
      switch (en_msgType)
      {
        case IsoBroadcast:
          if (isDelayEnd(ri32_time))
          {// after delay an ISO broadcast can start send immediately
            en_sendState = SendData;
          }
          break;
        case Din:
          if (isDelayEnd(ri32_time, 1000))
          { // force retry of RTS for DIN after 1sec.
            b_try += 1;
            if (b_try <= 3)
            { // do retry of RTS
              en_sendState = SendRts;
            }
            else
            { // terminate send
              abortSend();
              return true; // FINISHED SendStream, remove it from list please!
            }
          }
          break;
        case IsoTarget:
          if (isDelayEnd(ri32_time, 1250))
          { // abort send
            abortSend();
            return true; // FINISHED SendStream, remove it from list please!
          }
      }
      break;
    case AwaitEndofmsgack:
      switch (en_msgType)
      {
        case IsoTarget:
        case IsoBroadcast: // not usual - but to avoid compiler warning
          if (isDelayEnd(ri32_time, 1250))
          { // abort send
            abortSend();
            return true; // FINISHED SendStream, remove it from list please!
          }
        case Din:
          if (isDelayEnd(ri32_time, 5000))
          { // force resend of last message
            if (!b_abortOnTimeout)
            {  // -> start one uint8_t after last ack byte
              i32_DC = i32_ack_DC + 1;
              en_sendState = SendData;
            }
            else
            { // abort transfer
              abortSend();
              return true; // FINISHED SendStream, remove it from list please!
            }
          }
          break;
      }
      break;
    case SendPauseTillCts:
      if (isDelayEnd(ri32_time, 500))
      { // abort send
        abortSend();
        return true; // FINISHED SendStream, remove it from list please!
      }
      break;

    case SendData:
    {
      uint8_t ui8_freeCnt;
      if (rui8_pkgCnt == 0) rui8_pkgCnt = 1;
      if (en_msgType == Din) ui8_freeCnt = getCanInstance4Comm().sendCanFreecnt(Ident_c::StandardIdent);
      else ui8_freeCnt = getCanInstance4Comm().sendCanFreecnt(Ident_c::ExtendedIdent);
      // send only as much pkg as fits in send buffer (with spare of 2 for other use)
      if (ui8_freeCnt < 2)
        rui8_pkgCnt = 0;
      else if (rui8_pkgCnt > (ui8_freeCnt - 2))
        rui8_pkgCnt = (ui8_freeCnt - 2);

      #if 0 // defined( SYSTEM_PC )
      // slower down if program is debugged on PC
      if ( ( en_msgType == Din ) && ( ui8_pkgCnt > 1 ) ) ui8_pkgCnt -= 1;
      #endif

      if (rui8_pkgCnt == 0){
        #if defined( DEBUG )
        INTERNAL_DEBUG_DEVICE << "MultiSend_c::timeEvent --- pkgCnt == 0;\n";
        #endif
      }
      for (ui8_pkgInd = 0; ui8_pkgInd < rui8_pkgCnt; ui8_pkgInd++)
      {
        prepareSendMsg(ui8_nettoDataCnt);
        refc_multiSendPkg.setUint8Data(0, static_cast<uint8_t>(ui32_sequenceNr & 0xFF));
        if (hpb_data != NULL) {
          refc_multiSendPkg.setDataPart(hpb_data, i32_DC, ui8_nettoDataCnt);
        } else {
          pc_mss->setDataNextStreamPart (&refc_multiSendPkg, ui8_nettoDataCnt);
          #if defined( IOP_OUTPUT ) && defined( SYSTEM_PC )
          char iopBuffer [7];
          refc_multiSendPkg.getDataToString( 1, (uint8_t*)iopBuffer, 7 );
          if (iopSequence == 0) fwrite (iopBuffer+1, 1, ui8_nettoDataCnt-1, iopFile);
                           else fwrite (iopBuffer, 1, ui8_nettoDataCnt, iopFile);
          iopSequence++;
          #endif
        }
        sendIntern();
        i32_DC += ui8_nettoDataCnt;
        b_pkgSent++; // sent on epkg so we know how much to increase the offset next time a DPO is sent
        // break if this message part is finished
        if (isCompleteData() || isCompleteMsg())
        {
          if (!isCompleteData())
          {
            if (isCompleteMsg())
            {  // isCompleteMsg()
              switch (en_msgType)
              {
                case IsoTarget:
                case IsoBroadcast: // not usual - but to avoid compiler warning
                  // wait for CTS for next part of transfer
                  en_sendState = AwaitCts;
                  #if defined( DEBUG )
                  INTERNAL_DEBUG_DEVICE << "MultiSend_c::timeEvent --- after Sending now awaiting CTS!\n";
                  #endif
                  break;
                case Din:
                  en_sendState = AwaitEndofmsgack;
                  break;
              }
            }
          }
          else
          { // isCompleteData()
            if (en_msgType != IsoBroadcast)
            {
              #if defined( DEBUG )
              INTERNAL_DEBUG_DEVICE << "MultiSend_c::timeEvent --- after complete Sending now awaiting EOMACK!\n";
              #endif
              en_sendState = AwaitEndofmsgack;
            }
            else
            { // ISO Broadcast
              *pen_sendSuccessNotify = SendSuccess;
              return true; // FINISHED SendStream, remove it from list please!
            }
          }
          break;
        }
      } // for
      }
      break;
    case SendFileEnd:
      if (en_msgType == Din)
      { // send fileEnd cmd
        refc_multiSendPkg.setUint8Data(0, static_cast<uint8_t>(FileEnd));
        refc_multiSendPkg.setUint8Data(1,static_cast<uint8_t>(0xFF));
        refc_multiSendPkg.setUint16Data(2, static_cast<uint16_t>(0xFFFFU));
        refc_multiSendPkg.setUint32Data(4, uint32_t(0xFFFFFFFFUL));
        sendIntern();
        *pen_sendSuccessNotify = SendSuccess;
        return true; // FINISHED SendStream, remove it from list please!
      }
      break;
  }
  return false; // stream not yet finished!
}



bool
MultiSend_c::timeEvent( void )
{
  uint8_t ui8_pkgCnt;

  // nothing to do if no transfer is Running
  if (list_sendStream.empty()) return true;

  if ( Scheduler_c::getAvailableExecTime() == 0 ) {
    #if defined( DEBUG )
    INTERNAL_DEBUG_DEVICE << "MultiSend_c::timeEvent --- getAvailableExecTime() == 0;\n";
    #endif
    return false;
  }
  int32_t i32_time = Scheduler_c::getLastTimeEventTrigger();
  // store time of last call, to get time interval between execution
  static int32_t si32_lastCall = 0;
  // only send max 1 package for first call, when execution period can't be derived
  ui8_pkgCnt = ((si32_lastCall > 0 )&&(ui16_maxDelay > 0))?((i32_time - si32_lastCall)/ui16_maxDelay):1;
  // update last call time
  si32_lastCall = i32_time;

  const uint8_t cui8_pkgCntForEach = ui8_pkgCnt / list_sendStream.size(); // in case it gets 0 after division, it is set to 1 inside of SendStream's timeEvent().

  bool b_listShrunk=false;
  // Call each SendStream_c's timeEvent()
  for (std::list<SendStream_c>::iterator pc_iter=list_sendStream.begin(); pc_iter != list_sendStream.end();)
  {
    if (pc_iter->timeEvent (cui8_pkgCntForEach, i32_time))
    { // SendStream finished
      pc_iter = list_sendStream.erase (pc_iter);
      #ifdef DEBUG
      INTERNAL_DEBUG_DEVICE << "Kicked SendStream because it finished!\n";
      #endif
      b_listShrunk = true;
    }
    else
    { // SendStream not yet finished
      pc_iter++;
    }
  }


  if (b_listShrunk)
  {
    if (list_sendStream.size() == 0)
    { // (re-)set the CAN send pause to 0, because not a single SendStream is active anymore.
      setDelay(0);
      // restore default uint8_t order for 2 uint8_t integers
      data().setByteOrder(LSB_MSB); /** @todo is this neccessary? */
    }
    else
    { // there're still >= 1 streams, but as one left, set the delay back to the minimum, that's scui...
      /** @todo maybe ask all streams for their delay and set the max? */
      setDelay(scui8_isoCanPkgDelay);
    }
  }

  return true;
};



bool
MultiSend_c::SendStream_c::processMsg()
{
  /***/ MultiSendPkg_c&  refc_multiSendPkg = pc_multiSend->data();
  const MultiSendPkg_c& refcc_multiSendPkg = pc_multiSend->constData();

  switch (refc_multiSendPkg.getUint8Data( 0 ) )
  {
    case eCM_CTS:
    case CM_CTS:
      // upate i32_DC, ui8_sequenceNr and b_pkgToSend
      i32_DC = read_DC();
      #if defined( DEBUG )
      INTERNAL_DEBUG_DEVICE << "MultiSend_c::processMsg --- CTS received!\nFor Date from DC: " << i32_DC
        << "\n";
      #endif
      /** @todo WHY WAS THE SEND BUFFER CLEARED AT THIS POSITION??
        // clear send buffer
        getCanInstance4Comm().sendCanClearbuf();
      */
      if (   (en_sendState == AwaitCts)
          || (en_sendState == SendPauseTillCts)
          || (en_sendState == AwaitEndofmsgack)
          || (en_sendState == SendData)
         )
      { // awaited (or resent-) CTS received
        i32_timestamp = System_c::getTime();
        if (en_msgType == Din)
        {
          if (refc_multiSendPkg.getUint8Data(4) != 0xFF) {
            pc_multiSend->setDelay(4 + (refc_multiSendPkg.getUint8Data( 4) / 2));
          }
          // if last state was AwaitEndofmsgack then a CM_CTS means a 1sec. pause
          // change timestamp for wait to (1sec. - standardWait) ==> resulting 1sec. wait
          if (en_sendState == AwaitEndofmsgack)
          {
            i32_timestamp += (1000 - pc_multiSend->getMaxDelay());
          }
        }
        else
        { // only CTS reveived on IsoTarget (not IsoBroadcast)
          ui16_msgSize = uint16_t(refc_multiSendPkg.getUint8Data(1)) * 7;
          pc_multiSend->setDelay(scui8_isoCanPkgDelay);
        }
        // set counter of last acknowledged data byte
        if (en_sendState != SendData) i32_ack_DC = i32_DC - 1;

        if ( refc_multiSendPkg.getUint8Data(1) == 0)
        { // send pause commanded from receiver
          en_sendState = SendPauseTillCts;
        }
        else {
          // check if the same data as the last CTS is wanted?
          uint32_t ui32_pkgCTSd = uint32_t(refcc_multiSendPkg.getUint8Data(2));
          if (b_ext) {
             ui32_pkgCTSd += (uint32_t(refcc_multiSendPkg.getUint16Data(3)) << 8);
          }

          if ( pc_mss != NULL )
          {
            if (ui32_pkgCTSd == ui32_lastNextPacketNumberToSend) {
              pc_mss->restoreDataNextStreamPart ();
            } else {
              // we're streaming on, so save this position if it should happen we resend from here...
              pc_mss->saveDataNextStreamPart ();
            }
          }
          else
          { // nothing to do here, as "i32_DC = read_DC();" from above sets the "i32_DC" to where it wants data
            // in the case of "uint8_t[]" random access is possible, so that should be enough in my opinion --mjw
          }

          if ((ui32_pkgCTSd != ui32_lastNextPacketNumberToSend) && b_ext)
          { // increase DPO
            ui32_offset += b_pkgSent;
          }
          // update NextPacketNumberToSend!
          ui32_lastNextPacketNumberToSend = ui32_pkgCTSd;

          // send out Extended Connection Mode Data Packet Offset
          if (b_ext) {
            refc_multiSendPkg.setUint8Data(0, static_cast<uint8_t>(eCM_DPO));
            refc_multiSendPkg.setUint8Data(1, static_cast<uint8_t>(b_pkgToSend));
            refc_multiSendPkg.setUint16Data(2, static_cast<uint16_t>(ui32_offset & 0xFFFF));
            refc_multiSendPkg.setUint8Data(4, static_cast<uint8_t>(ui32_offset >> 16));
            // change order of setUint8Data(), setUint16Data() so that
            // setUint16Data() with equal position, so that a faster assignment is enabled
            refc_multiSendPkg.setUint8Data(5, static_cast<uint8_t>(i32_pgn & 0xFF));
            refc_multiSendPkg.setUint16Data(6, static_cast<uint16_t>(i32_pgn >> 8));
            ui32_sequenceNr = 0;
            b_pkgSent = 0;
            sendIntern();
          }
          // now receiver wants to receive new data
          #if defined( DEBUG )
          INTERNAL_DEBUG_DEVICE << "Start To Send Next Data Block\n";
          #endif
          en_sendState = SendData;
        } // end request to send
      } // awaited (or resent-) CTS received
      break;
    case CM_EndofMsgACK:
    case eCM_EndofMsgACK:
      if (en_sendState == AwaitEndofmsgack) {
        #if defined( DEBUG )
        INTERNAL_DEBUG_DEVICE << "MultiSend_c::processMsg --- EOMACK received!\n";
        #endif
        // CHECK HERE IF WE'RE AWAITING AN EOMACK!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
        if (en_msgType == Din)
        {
          if (refc_multiSendPkg.getUint8Data(4) != 0)
          { // ACK
            i32_ack_DC = readAck_DC();
            i32_DC = i32_ack_DC + 1;
            if (i32_DC < i32_dataSize)
            { // start sending next part message
              en_sendState = SendRts;
              b_try = 1;
            }
            else
            { // send fileEnd
              en_sendState = SendFileEnd;
            }
          }
          else
          { // NACK received -> resend last message (usual last 147 CAN pkg)
            // -> start one uint8_t after last ack byte
            i32_DC = i32_ack_DC + 1;
            en_sendState = SendData;
          }
        }
        else
        { // ISO target
          #if defined( IOP_OUTPUT ) && defined( SYSTEM_PC )
          fclose (iopFile);
          #endif
          *pen_sendSuccessNotify = SendSuccess; // will be kicked out after next timeEvent!
        }
      } else { // not awaiting end of message ack
        #if defined( DEBUG )
        INTERNAL_DEBUG_DEVICE << "MultiSend_c::processMsg --- EOMACK received without expecting it!\n";
        #endif
      }
      break;
    case ConnAbort:
      #if defined( DEBUG )
      INTERNAL_DEBUG_DEVICE << "MultiSend_c::processMsg --- ConnAbort received!\n";
      #endif
      *pen_sendSuccessNotify = SendAborted; // will be kicked out after next timeEvent!
      return false; // in case a MultiSend & MultiReceive are running parallel, then this ConnAbort should be for both!
    default:
      #ifdef USE_DIN_TERMINAL
      // give DINMaskUpload_c a try
      return getDinMaskuploadInstance4Comm().processMsg();
      #else
      return false;
      #endif
  }
  return true;
}



/**
  start processing of a process msg
  ignore all invalid messages where SEND is not of a member with claimed address,
  or where EMPF isn't valid

  possible errors:
    * Err_c::elNonexistent on SEND/EMPF not registered in Monitor-List
  @return true -> message was processed; else the received CAN message will be served to other matching CANCustomer_c
*/
bool
MultiSend_c::processMsg()
{
  if (data().getLen() != 8)
    return true; // All corrupted (E)TP-Packages should NOT be of interest for anybody...

  // don't process if no response from target of multi packet send
  // give DINMaskUpload_c a try
  SendStream_c* pc_sendStreamFound = getSendStream(data().empf(), data().send()); // sa/da swapped, of course ;-) !
  if (pc_sendStreamFound == NULL) {
    #ifdef USE_DIN_TERMINAL
    return getDinMaskuploadInstance4Comm().processMsg();
    #else
    return false;
    #endif
  }

  // found a matching SendStream, so call its processMsg()
  return pc_sendStreamFound->processMsg();
}



/**
  abort the multipacket send stream
  (important if original target isn't active any more)
  IMPORTANT: After calling abortSend() please exit timeEvent() with true, so the SendStream gets deleted!!
*/
void
MultiSend_c::SendStream_c::abortSend()
{
  MultiSendPkg_c& refc_multiSendPkg = pc_multiSend->data();

  refc_multiSendPkg.setUint8Data(0, static_cast<uint8_t>(ConnAbort));
  refc_multiSendPkg.setUint32Data(1, uint32_t(0xFFFFFFFFUL));
//refc_multiSendPkg.setUint32Data(1, uint32_t(0x53d0FFFFUL)); // debug-test
  bool b_performAbort = false;
  #ifdef USE_DIN_TERMINAL
  if (en_msgType == Din)
  { // send DIN abort msg
    refc_multiSendPkg.setUint8Data(5, static_cast<uint8_t>(b_fileCmd));
    refc_multiSendPkg.setData_2ByteInteger(6, getMsgNr());
    b_performAbort = true;
  }
  #endif
  #if defined(USE_ISO_11783)
  if ( en_msgType != Din )
  { // send ISO abort message
    // change order of setUint8Data(), setUint16Data() so that
    // setUint16Data() with equal position, so that a faster assignment is enabled
    refc_multiSendPkg.setUint8Data(5, static_cast<uint8_t>(i32_pgn & 0xFF));
    refc_multiSendPkg.setUint16Data(6, static_cast<uint16_t>(i32_pgn >> 8));
    b_performAbort = true;
  }
  #endif
  if ( b_performAbort )
  {
    en_sendState = SendFileEnd; // set it to anything OTHER than "SendData",
    sendIntern(); // so sendIntern() will send with the correct PGN: (E)TP_CONN_MANAGE_PGN
    *pen_sendSuccessNotify = SendAborted;
  }
}


#if defined(USE_ISO_11783)
/** this function is called by ISOMonitor_c when a new CLAIMED ISOItem_c is registered.
 * @param refc_devKey const reference to the item which ISOItem_c state is changed
 * @param rpc_newItem pointer to the currently corresponding ISOItem_c
 */
void MultiSend_c::reactOnMonitorListAdd( const DevKey_c& refc_devKey, const ISOItem_c* rpc_newItem )
{
  if ( getSystemMgmtInstance4Comm().existLocalMemberDevKey(refc_devKey) )
  { // lcoal ISOItem_c has finished adr claim
    bool b_isReconfigNeeded = false;
    uint32_t ui32_nr = rpc_newItem->nr();
    // only ISO msgs with own SA in PS (destination)
    uint32_t ui32_filter = ((static_cast<MASK_TYPE>(TP_CONN_MANAGE_PGN) | static_cast<MASK_TYPE>(ui32_nr)) << 8);
    if (!getCanInstance4Comm().existFilter( *this, (0x1FFFF00UL), ui32_filter, Ident_c::ExtendedIdent))
    { // create FilterBox
      getCanInstance4Comm().insertFilter( *this, (0x1FFFF00UL), ui32_filter, true, Ident_c::ExtendedIdent);
      b_isReconfigNeeded = true;
    }

    ui32_filter = ((static_cast<MASK_TYPE>(ETP_CONN_MANAGE_PGN) | static_cast<MASK_TYPE>(ui32_nr)) << 8);
    if (!getCanInstance4Comm().existFilter( *this, (0x1FFFF00UL), ui32_filter, Ident_c::ExtendedIdent))
    { // create FilterBox
      getCanInstance4Comm().insertFilter( *this, (0x1FFFF00UL), ui32_filter, true, Ident_c::ExtendedIdent);
      b_isReconfigNeeded = true;
    }
    if ( b_isReconfigNeeded ) getCanInstance4Comm().reconfigureMsgObj();
  }

  #ifdef DEBUG
  INTERNAL_DEBUG_DEVICE << "reactOnMonitorListAdd() handles CLAIM of ISOItem_c for device with DevClass: " << int(refc_devKey.getDevClass())
      << ", Instance: " << int(refc_devKey.getDevClassInst()) << ", and manufacturer ID: " << int(refc_devKey.getConstName().manufCode())
      << "NOW use SA: " << int(rpc_newItem->nr()) << "\n\n"
      << INTERNAL_DEBUG_DEVICE_ENDL;
  #endif
  // no resurrection here as we do NOT (yet) save the devKey/isoname to our SendStream_c instances in the list...
  // this can be done later if someone thinks that makes sense...

}

/** this function is called by ISOMonitor_c when a device looses its ISOItem_c.
 * @param refc_devKey const reference to the item which ISOItem_c state is changed
 * @param rui8_oldSa previously used SA which is NOW LOST -> clients which were connected to this item can react explicitly
 */
void MultiSend_c::reactOnMonitorListRemove( const DevKey_c& refc_devKey, uint8_t rui8_oldSa )
{
  if ( getSystemMgmtInstance4Comm().existLocalMemberDevKey(refc_devKey) )
  { // lcoal ISOItem_c has lost SA
    uint32_t ui32_nr = rui8_oldSa;
    // only ISO msgs with own SA in PS (destination)
    uint32_t ui32_filter = ((static_cast<MASK_TYPE>(TP_CONN_MANAGE_PGN) | static_cast<MASK_TYPE>(ui32_nr)) << 8);
    if (getCanInstance4Comm().existFilter( *this, (0x1FFFF00UL), ui32_filter, Ident_c::ExtendedIdent))
    { // create FilterBox
      getCanInstance4Comm().deleteFilter( *this, (0x1FFFF00UL), ui32_filter, Ident_c::ExtendedIdent);
    }
    ui32_filter = ((static_cast<MASK_TYPE>(ETP_CONN_MANAGE_PGN) | static_cast<MASK_TYPE>(ui32_nr)) << 8);
    if (getCanInstance4Comm().existFilter( *this, (0x1FFFF00UL), ui32_filter, Ident_c::ExtendedIdent))
    { // create FilterBox
      getCanInstance4Comm().deleteFilter( *this, (0x1FFFF00UL), ui32_filter, Ident_c::ExtendedIdent);
    }
  }
  #ifdef DEBUG
  INTERNAL_DEBUG_DEVICE << "reactOnMonitorListRemove() handles LOSS of ISOItem_c for device with DevClass: " << int(refc_devKey.getDevClass())
      << ", Instance: " << int(refc_devKey.getDevClassInst()) << ", and manufacturer ID: " << int(refc_devKey.getConstName().manufCode())
      << " and PREVIOUSLY used SA: " << int(rui8_oldSa) << "\n\n"
      << INTERNAL_DEBUG_DEVICE_ENDL;
  #endif
  for (std::list<SendStream_c>::iterator pc_iter=list_sendStream.begin(); pc_iter != list_sendStream.end();)
  {
    if (pc_iter->matchSa (rui8_oldSa) || pc_iter->matchDa (rui8_oldSa))
    { // SendStream found in list, abort and erase!
      pc_iter->abortSend();
      pc_iter = list_sendStream.erase (pc_iter);
    }
    else
    { // SendStream not yet found
      pc_iter++;
    }
  }
}
#endif



/**
  send a message -> set the ident and initiate sending to CAN
*/
void
MultiSend_c::SendStream_c::sendIntern()
{
  MultiSendPkg_c& refc_multiSendPkg = pc_multiSend->data();

  // set timestamp of last send action
  i32_timestamp = System_c::getTime();
  bool b_performSend = false;
  // set identifier
  #ifdef USE_DIN_TERMINAL
  if (en_msgType == Din)
  {
    uint16_t ui16_ident = (0x700 | (b_empf << 4) | b_send);
    // data().setIdent(ui16_ident, Ident_c::StandardIdent);
    // CANPkg_c::setIdent changed to static
    CANPkg_c::setIdent(ui16_ident, Ident_c::StandardIdent);
    b_performSend = true;
  }
  #endif
  #if defined(USE_ISO_11783)
  if ( en_msgType != Din )
  { // ISO
    refc_multiSendPkg.setIsoPri(6);
    #if defined (NMEA_2000_FAST_PACKET)
    if(b_fp)
    {
        refc_multiSendPkg.setIsoPgn(i32_pgn);  // For fast packet, the PGN itself tells us that the message is a fast packet message.
        // Since this is a non-destination specific PGN, not sure if we need to set b_empf or not -bac
        /** @todo determine when to set the b_empf for IsoPs and when not... */
        //data().setIsoPs(b_empf);
    }
    else
    #endif
    {
      if (en_sendState == SendData)
      {
        if (b_ext) refc_multiSendPkg.setIsoPgn(ETP_DATA_TRANSFER_PGN);
              else refc_multiSendPkg.setIsoPgn(TP_DATA_TRANSFER_PGN);
      }
      else
      {
      if (b_ext) refc_multiSendPkg.setIsoPgn(ETP_CONN_MANAGE_PGN);
            else refc_multiSendPkg.setIsoPgn(TP_CONN_MANAGE_PGN);
      }
      refc_multiSendPkg.setIsoPs(b_empf);
    }
    refc_multiSendPkg.setIsoSa(b_send);
    b_performSend = true;
  }
  #endif
  if ( !b_performSend )
  { // register error as selected protocol doesn't fit to compiled protocol
    getLbsErrInstance().registerError( LibErr_c::Precondition, LibErr_c::LbsMultipacket );
  }
  else
  {
    CANPkg_c::setLen( 8 );
    getCanInstance4Comm() << refc_multiSendPkg;
  }
}



/**
  check if actual message is complete
*/
bool
MultiSend_c::SendStream_c::isCompleteMsg() const
{
  return (b_pkgToSend == 0)?true:false;
}



/**
  check if send of all data is complete
  this function returns true only if both:<ul>
  <li>all netto data sent (i32_DC >= i32_dataSize)
  <li>last message after CTS with Byte stuffing is sent
  </ul>
  @return true -> i32_DC tell that i32_dataSize ist reached
*/
bool
MultiSend_c::SendStream_c::isCompleteData() const
{
#if defined (NMEA_2000_FAST_PACKET)
  if(b_fp)
    return (i32_DC >= i32_dataSize)?true:false;
  else
#endif
  return ( (i32_DC >= i32_dataSize)
        && (isCompleteMsg())
        )?true:false;
}



/**
  deliver the amount of messages for IsoAgLib+ data stream
  (no problem if called for ISO)
  @return amount of messages for stored complete data size
*/
uint16_t
MultiSend_c::SendStream_c::getMsgCnt()const
{
  uint16_t ui16_result = i32_dataSize / ui16_msgSize;
  return ((i32_dataSize % ui16_msgSize) != 0)?(ui16_result+1):ui16_result;
}



/**
  deliver the amount of CAN pkg per message
  @return CAN pkg amount per message
*/
uint16_t
MultiSend_c::SendStream_c::getPkgPerMsg()const
{
  uint16_t ui16_result = ui16_msgSize / 7;
  return ((ui16_msgSize % 7) != 0)?(ui16_result+1):ui16_result;
}



/**
  deliver the message number in LBS+ data stream
  @return message number to insert in CM_RTS
*/
uint16_t
MultiSend_c::SendStream_c::getMsgNr() const
{
  return ((i32_DC / ui16_msgSize)+1);
};



/**
  read the the commanded mesage size from CTS CAN pkg
  (for IsoAgLib+ a change of ui16_msgSize is only accepted, if
  new value results in increase of Pkg amount, because
  often the commanded ui16_msgSize is not round factor of 7)
  @return commanded message size
*/
uint16_t
MultiSend_c::SendStream_c::readMsgSize() const
{
  uint16_t ui16_tempSize = ((uint16_t(pc_multiSend->constData().getUint8Data(1)) * 7) - ui16_msgSize);
  if ( (en_msgType == Din)
    && (ui16_tempSize >= ui16_msgSize)
    && (ui16_tempSize - ui16_msgSize < 7)
     )
  { // no change in amount of pkg -> deliver old value
    return ui16_msgSize;
  }
  else
  {
    return ui16_tempSize;
  }
}



/**
  read the next to be sent data position from CM_CTS
  @return new i32_DC value
*/
int32_t
MultiSend_c::SendStream_c::read_DC()
{
  const MultiSendPkg_c& refcc_multiSendPkg = pc_multiSend->constData();

  // take sequence nr with decrease of 1 because prepareSendMsg increment by 1
  // before first send
  if (b_ext) {
    ui32_sequenceNr = (uint32_t(refcc_multiSendPkg.getUint16Data(2))) + (uint32_t(refcc_multiSendPkg.getUint8Data(4)) << 16) - 1UL;
  } else {
    ui32_sequenceNr = (refcc_multiSendPkg.getUint8Data(2) - 1UL);
  }
  b_pkgToSend = refcc_multiSendPkg.getUint8Data(1);
  int32_t i32_temp_DC = ui32_sequenceNr * 7L;
  if (en_msgType == Din) i32_temp_DC += ( uint32_t(refcc_multiSendPkg.getData_2ByteInteger(6) - 1) * uint32_t( ui16_msgSize ) );
  if (i32_temp_DC > i32_dataSize)
  {
    i32_temp_DC = i32_dataSize;
  }
  return i32_temp_DC;
}



/**
  read the acknowledged data amount from EndOfMsgAck
  @return amount of correct received data byte
*/
int32_t
MultiSend_c::SendStream_c::readAck_DC()
{
  const MultiSendPkg_c& refcc_multiSendPkg = pc_multiSend->constData();

  ui16_msgSize = refcc_multiSendPkg.getData_2ByteInteger(1);
  int32_t i32_tempAck_DC = ui16_msgSize;
  if (en_msgType == Din) i32_tempAck_DC *= refcc_multiSendPkg.getData_2ByteInteger(6);
  return i32_tempAck_DC;
}



/**
  calculate the actual sequence number and
  calculate the amount of data bytes which must be placed in new CAN pkg
  @param ui8_nettoDataCnt amount of data which should be sent within this msg
*/
void
MultiSend_c::SendStream_c::prepareSendMsg(uint8_t &ui8_nettoDataCnt)
{
  ui8_nettoDataCnt = 7;
  b_pkgToSend--;
#if defined (NMEA_2000_FAST_PACKET)
  if(b_fp)
  {
    ui8_FpFrameNr++;
  }
  else
#endif
  {
    ui32_sequenceNr++;
  }
  if (en_msgType == Din)
  {
    // check if last pkg of message has to be sent
    // -> check if dummy 0xFF have to be inserted
    if (b_pkgToSend == 0)
    { // netto data cnt of this pkg is message size - the
      // amount of already sent uint8_t == (ui8_sequenceNr - 1) pkg's
      uint16_t ui16_pkgLastByte = ((ui32_sequenceNr - 1) * 7);
      if (ui16_pkgLastByte < ui16_msgSize) ui8_nettoDataCnt = ui16_msgSize - ui16_pkgLastByte;
      else ui8_nettoDataCnt = 0;
    }
  }

  if (i32_DC + ui8_nettoDataCnt > i32_dataSize)
  {
    ui8_nettoDataCnt = i32_dataSize - i32_DC;
  }
}



/**
  set the delay between two sent messages
  @param rui16_delay wanted delay
*/
void
MultiSend_c::setSendStreamDelay(uint16_t rui16_delay)
{ // if 0xFF was delay setting in CAN, this function is called
  // with (0xFF / 2) + 4 ==> 131
  // if 0 was delay setting in CAN, this function is called with
  // (0 / 2) + 4 --> 4 ==> take it
  uint16_t ui16_tempDelay = (rui16_delay != 131)?rui16_delay:4;
  // NEW: Take the max, as we can't have different values "in parallel" if more than 1 stream is running!
  if (ui16_tempDelay > ui16_maxDelay)
  {
    setDelay(ui16_tempDelay);
  }
}



// private function only to be used from MultiSend_c to reset delay after a stream left!
void
MultiSend_c::setDelay(uint16_t rui16_delay)
{
  ui16_maxDelay = rui16_delay;
  // be a little bit slower as the CAN sending BIOS/OS may be a little bit quicker than commanded
  getCanInstance4Comm().setSendpause(ui16_maxDelay + 1);
}



/** user function for explicit abort of any running matching stream. */
void
MultiSend_c::abortSend (uint8_t rb_send, uint8_t rb_empf)
{
  // normally there should be only ONE match for this sa/da-pair, but loop anyway to be sure to clear the list ;)
  for (std::list<SendStream_c>::iterator pc_iter=list_sendStream.begin(); pc_iter != list_sendStream.end();)
  {
    if (pc_iter->matchSaDa (rb_send, rb_empf))
    { // SendStream found in list, abort and erase!
      pc_iter->abortSend();
      pc_iter = list_sendStream.erase (pc_iter);
    }
    else
    { // SendStream not yet found
      pc_iter++;
    }
  }
}

} // end namespace __IsoAgLib
