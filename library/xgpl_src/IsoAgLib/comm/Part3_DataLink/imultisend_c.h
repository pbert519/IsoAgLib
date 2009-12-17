/*
  imultisend_c.h

  (C) Copyright 2009 - 2010 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/
#ifndef IMULTI_SEND_H
#define IMULTI_SEND_H

/* *************************************** */
/* ********** include headers ************ */
/* *************************************** */
#include "impl/multisend_c.h"


// Begin Namespace IsoAgLib
namespace IsoAgLib {
/**
  This class implements ISO 11783 (Enhanced) Transport
  Protocol and FastPacket for send of more than 8 bytes of data.

  NOTE: Application-programmers should not need to perform
        (E)TP/FastPacket-Transfers on their own.
        This class is only for ISO-Server-type applications
        like VT-/TC-/FS-Servers.

        This functionality is subject to be moved when the new
        interface for such usage is defined.

  @author Dipl.-Inform. Achim Spangler
  @author Dipl.-Inf. Martin Wodok
*/
class iMultiSend_c : private __IsoAgLib::MultiSend_c {
public:

  /**
    send an ISO target multipacket message with active retrieve of data-parts to send
    @param acrc_isoNameSender dynamic member no of sender
    @param acrc_isoNameReceiver dynamic member no of receiver
    @param apc_mss allow active build of data stream parts for upload by deriving data source class
                  from IsoAgLib::iMultiSendStreamer_c, which defines virtual functions to control the
                  retrieve of data to send. This is especially important for ISO_Terminal,
                  which assembles the data pool dependent on the terminal capabilities during upload
                  ( e.g. bitmap variants )
    @param ai32_pgn PGN to use for the upload
    @param rpen_sendSuccessNotify -> pointer to send state var, where the current state
            is written by MultiSend_c
    @return true -> MultiSend_c was ready -> mask is spooled to target
  */
  bool sendIsoTarget (const iIsoName_c& acrc_isoNameSender, const iIsoName_c& acrc_isoNameReceiver, iMultiSendStreamer_c* apc_mss, int32_t ai32_pgn, sendSuccess_t& rpen_sendSuccessNotify)
  { return MultiSend_c::sendIsoTarget (acrc_isoNameSender, acrc_isoNameReceiver, apc_mss, ai32_pgn, rpen_sendSuccessNotify);}

  /**
    send a ISO target multipacket message
    @param acrc_isoNameSender dynamic member no of sender
    @param acrc_isoNameReceiver dynamic member no of receiver
    @param rhpb_data HUGE_MEM pointer to the data
    @param ai32_dataSize size of the complete mask
    @param ai32_pgn PGN to use for the upload
    @param rpen_sendSuccessNotify -> pointer to send state var, where the current state
            is written by MultiSend_c
    @return true -> MultiSend_c was ready -> mask is spooled to target
  */
  bool sendIsoTarget (const iIsoName_c& acrc_isoNameSender, const iIsoName_c& acrc_isoNameReceiver, HUGE_MEM uint8_t* rhpb_data, int32_t ai32_dataSize, int32_t ai32_pgn, sendSuccess_t& rpen_sendSuccessNotify)
    { return MultiSend_c::sendIsoTarget (acrc_isoNameSender, acrc_isoNameReceiver, rhpb_data, ai32_dataSize, ai32_pgn, rpen_sendSuccessNotify ); }

  /**
    send a ISO broadcast multipacket message
    @param acrc_isoNameSender dynamic member no of sender
    @param rhpb_data HUGE_MEM pointer to the data
    @param ai32_dataSize size of the complete mask
    @param ai32_pgn PGN to use for the upload
    @return true -> MultiSend_c was ready -> mask is spooled to target
  */
  bool sendIsoBroadcast (const iIsoName_c& acrc_isoNameSender, HUGE_MEM uint8_t* rhpb_data, uint16_t aui16_dataSize, int32_t ai32_pgn, sendSuccess_t& rpen_sendSuccessNotify)
    { return MultiSend_c::sendIsoBroadcast (acrc_isoNameSender, rhpb_data, aui16_dataSize, ai32_pgn, rpen_sendSuccessNotify); }

  /**
   * Currently not supported (needed) in the interface
    send a ISO broadcast multipacket message. If the size is less than 9, the transport protocol won't be used
    @param acrc_isoNameSender dynamic member no of sender
    @param rhpb_data HUGE_MEM pointer to the data
    @param ai32_dataSize size of the complete mask
    @param ai32_pgn PGN to use for the upload
    @return true -> MultiSend_c was ready -> mask is spooled to target
  */
  //bool sendIsoBroadcastOrSinglePacket (const iIsoName_c& acrc_isoNameSender, HUGE_MEM uint8_t* rhpb_data, int32_t ai32_dataSize, int32_t ai32_pgn, sendSuccess_t& rpen_sendSuccessNotify)
  //  { return MultiSend_c::sendIsoBroadcastOrSinglePacket (acrc_isoNameSender, rhpb_data, ai32_dataSize, ai32_pgn, rpen_sendSuccessNotify); }




#if defined(ENABLE_MULTIPACKET_VARIANT_FAST_PACKET)
  /**
    send a FastPacket targeted multipacket message using a given uint8_t* buffer
    @param acrc_isoNameSender ISOName of sender
    @param acrc_isoNameReceiver ISOName of receiver
    @param apc_mss Allow active build of data stream parts for upload by deriving data source class
                   from IsoAgLib::iMultiSendStreamer_c, which defines virtual functions to control the
                   retrieve of data to send.
    @param ai32_pgn PGN of the data
    @param rpen_sendSuccessNotify Pointer to send state var, where the current state is written by MultiSend_c
    @return true -> MultiSend_c was ready -> Transfer was started
  */
  bool sendIsoFastPacket (const iIsoName_c& acrc_isoNameSender, const iIsoName_c& acrc_isoNameReceiver, HUGE_MEM uint8_t* rhpb_data, uint16_t aui16_dataSize, int32_t ai32_pgn, sendSuccess_t& rpen_sendSuccessNotify)
  { return MultiSend_c::sendIsoFastPacket(acrc_isoNameSender, acrc_isoNameReceiver, rhpb_data, aui16_dataSize, ai32_pgn, rpen_sendSuccessNotify); }

  /**
    Send a FastPacket targeted multipacket message using a given MultiSendStreamer
    @param acrc_isoNameSender ISOName of sender
    @param acrc_isoNameReceiver ISOName of receiver
    @param rhpb_data HUGE_MEM pointer to the data
    @param aui32_dataSize size of the complete data (should be >= 9 of course)
    @param ai32_pgn PGN of the data
    @param rpen_sendSuccessNotify Pointer to send state var, where the current state is written by MultiSend_c
    @return true -> MultiSend_c was ready -> Transfer was started
  */
  bool sendIsoFastPacket (const iIsoName_c& acrc_isoNameSender, const iIsoName_c& acrc_isoNameReceiver, IsoAgLib::iMultiSendStreamer_c* apc_mss, int32_t ai32_pgn, sendSuccess_t& rpen_sendSuccessNotify)
  { return MultiSend_c::sendIsoFastPacket(acrc_isoNameSender, acrc_isoNameReceiver, apc_mss, ai32_pgn, rpen_sendSuccessNotify); }

  /**
    Send a FastPacket broadcast multipacket message using a given data-buffer
    @param acrc_isoNameSender ISOName of sender
    @param rhpb_data HUGE_MEM pointer to the data
    @param aui16_dataSize Size of the complete buffer (should be >= 9 of course)
    @param ai32_pgn PGN of the data
    @param rpen_sendSuccessNotify Pointer to send state var, where the current state is written by MultiSend_c
    @return true -> MultiSend_c was ready -> Transfer was started
  */
  bool sendIsoFastPacketBroadcast (const iIsoName_c& acrc_isoNameSender, HUGE_MEM uint8_t* rhpb_data, uint16_t aui16_dataSize, int32_t ai32_pgn, sendSuccess_t& rpen_sendSuccessNotify)
    { return MultiSend_c::sendIsoFastPacketBroadcast (acrc_isoNameSender, rhpb_data, aui16_dataSize, ai32_pgn, rpen_sendSuccessNotify); }

  /**
    Send a FastPacket broadcast multipacket message using a given MultiSendStreamer
    @param acrc_isoNameSender ISOName of sender
    @param apc_mss Allow active build of data stream parts for upload by deriving data source class
                   from IsoAgLib::iMultiSendStreamer_c, which defines virtual functions to control the
                   retrieve of data to send.
    @param ai32_pgn PGN of the data
    @param rpen_sendSuccessNotify Pointer to send state var, where the current state is written by MultiSend_c
    @return true -> MultiSend_c was ready -> Transfer was started
  */
  bool sendIsoFastPacketBroadcast (const iIsoName_c& acrc_isoNameSender, IsoAgLib::iMultiSendStreamer_c* apc_mss, int32_t ai32_pgn, sendSuccess_t& rpen_sendSuccessNotify)
    { return MultiSend_c::sendIsoFastPacketBroadcast (acrc_isoNameSender, apc_mss, ai32_pgn, rpen_sendSuccessNotify); }
#endif

  /** check if at least one multisend stream is running */
  bool isMultiSendRunning() const { return MultiSend_c::isMultiSendRunning(); }

  /** user function for explicit abort of any running matching stream. */
  void abortSend (const iIsoName_c& acrc_isoNameSender, const iIsoName_c& acrc_isoNameReceiver)
    { MultiSend_c::abortSend (acrc_isoNameSender, acrc_isoNameReceiver); }


private:
  /** allow getIMultiSendInstance() access to shielded base class.
      otherwise __IsoAgLib::getMultiSendInstance() wouldn't be accepted by compiler
    */
  #if defined( PRT_INSTANCE_CNT ) && ( PRT_INSTANCE_CNT > 1 )
  friend iMultiSend_c& getIMultiSendInstance( uint8_t aui8_instance );
  #else
  friend iMultiSend_c& getIMultiSendInstance( void );
  #endif
}; // end class


#if defined( PRT_INSTANCE_CNT ) && ( PRT_INSTANCE_CNT > 1 )
  /** C-style function, to get access to the unique MultiSend_c singleton instance
    * if more than one CAN BUS is used for IsoAgLib, an index must be given to select the wanted BUS
    */
  inline iMultiSend_c& getIMultiSendInstance( uint8_t aui8_instance = 0 )
  { return static_cast<iMultiSend_c&>(__IsoAgLib::getMultiSendInstance(aui8_instance)); }
#else
  /** C-style function, to get access to the unique MultiSend_c singleton instance */
  inline iMultiSend_c& getIMultiSendInstance( void )
  { return static_cast<iMultiSend_c&>(__IsoAgLib::getMultiSendInstance()); }
#endif

} // end namespace IsoAgLib
#endif
