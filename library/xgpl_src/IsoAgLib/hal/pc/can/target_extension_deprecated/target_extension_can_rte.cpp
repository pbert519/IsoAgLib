/*
  target_extension_can_rte.cpp: source for the real time server with
    CAN communication through client-server call semantics

  (C) Copyright 2009 - 2012 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/
#define  STRICT

#define CVS_REV "$Revision$"
#define RTE_CLIENT

#include "rte_global.h"
#include "can_target_extensions.h"
#include <stdlib.h>
#include <ctype.h>
#include <IsoAgLib/hal/hal_system.h>
#include <IsoAgLib/hal/hal_can.h>

// RTE Includes
#include <rte_client.h>   // be an RTE client
#include <iostream>
#include <iomanip.h>

#include <list>
#include <cstdlib>


//#include <IsoAgLib/hal/generic_utils/can/icanfifo.h>
#include <IsoAgLib/hal/generic_utils/can/write_central_fifo.h>

namespace __HAL {
/////////////////////////////////////////////////////////////////////////
// Globals


struct can_data { // evtl. anpassen
  int32_t i32_time;
  int32_t i32_ident;
  uint8_t b_dlc;
  uint8_t b_xtd;
  uint8_t pb_data[8];
};

// IsoAgLib counting for BUS-NR and MsgObj starts both in C-Style with 0
// -> all needed offsets shall be added at the lowest possible layer
//    ( i.e. direct in the BIOS/OS call)
static const uint32_t cui32_maxCanBusCnt = ( HAL_CAN_MAX_BUS_NR + 1 );
static can_c* rteCan_c [cui32_maxCanBusCnt];
static rtd_can_bus_state_t rteCanBusState[cui32_maxCanBusCnt];

typedef struct {

can_data* rec_buf;
int16_t rec_bufCnt;
int16_t rec_bufSize;
int16_t rec_bufOut;
int16_t rec_bufIn;
bool rec_bufXtd;
uint32_t rec_bufFilter;
bool b_canBufferLock;

bool b_canObjConfigured;

uint32_t ui32_mask_xtd;
uint16_t ui16_mask_std;

} tMsgObj;

//typedef STL_NAMESPACE::vector<tMsgObj> ArrMsgObj;
//ArrMsgObj arrMsgObj[cui32_maxCanBusCnt];
typedef STL_NAMESPACE::vector<tMsgObj>  ArrMsgOb_t;
static ArrMsgOb_t* arrMsgObj;

int32_t i32_lastReceiveTime;

static uint16_t ui16_globalMask[cui32_maxCanBusCnt];
static uint32_t ui32_globalMask[cui32_maxCanBusCnt];
static uint32_t ui32_lastMask[cui32_maxCanBusCnt];
static bool b_busOpened[cui32_maxCanBusCnt];

class RteCanPkg {
  private:
    uint32_t id;
    uint8_t  b_xtd;
    uint8_t  size;
    uint8_t  data[8];
  public:
    RteCanPkg( uint32_t aui32_id = 0, uint8_t aui8_xtd = 0, uint8_t aui8_size = 0, const uint8_t* apui8_data = NULL )
      { id = aui32_id; b_xtd = aui8_xtd; size = ( aui8_size <= 8 )?aui8_size:8; memcpy( data, apui8_data, size ); };

    bool operator==( const RteCanPkg& acrc_src ) const
      { return ( ( id == acrc_src.id ) && ( b_xtd == acrc_src.b_xtd ) && ( size == acrc_src.size ) && ( memcmp( data, acrc_src.data, size ) == 0 ) ); } ;

    bool operator!=( const RteCanPkg& acrc_src ) const
      { return ( ( id != acrc_src.id ) || ( b_xtd != acrc_src.b_xtd ) || ( size != acrc_src.size ) || ( memcmp( data, acrc_src.data, size ) != 0 ) ); } ;

    const RteCanPkg& operator=( const RteCanPkg& acrc_src )
      { id = acrc_src.id; b_xtd = acrc_src.b_xtd; size = acrc_src.size; memcpy( data, acrc_src.data, size ); return *this;};
};

static STL_NAMESPACE::list<RteCanPkg> arr_canQueue[cui32_maxCanBusCnt];



// globale Filehandle fuer simulierte CAN Input
static FILE* canlogDat[cui32_maxCanBusCnt];

// offset time
// static rte_time_t t_rteOffset = RTE_NEVER;

/** there's no send-delay recognition of RTE */
int32_t getMaxSendDelay(uint8_t aui8_busNr) { return 0; }

int16_t can_startDriver()
{

  arrMsgObj = new ArrMsgOb_t [cui32_maxCanBusCnt];

  // open the driver
  for ( uint32_t ind = 0; ind < cui32_maxCanBusCnt; ind++ )
  {
    rteCanBusState[ind] = rtd_can_bus_state_active;
    rteCan_c[ind] = NULL;
    canlogDat[ind] = NULL;
    b_busOpened[ind] = false;
#ifndef SYSTEM_WITH_ENHANCED_CAN_HAL
    // configure vector arrMsgObj[ind] with 15 elements
    arrMsgObj[ind].resize(15);
    for (uint8_t ui8_nr = 0; ui8_nr < arrMsgObj[ind].size(); ui8_nr++)
    {
      arrMsgObj[ind][ui8_nr].rec_bufSize = 0;
      arrMsgObj[ind][ui8_nr].rec_bufCnt = 0;
    }
#endif
  }

  if ( ! rte_is_init() ) {
    std::string c_rteServer = CONFIG_HAL_PC_RTE_DEFAULT_SERVER;
    if ( getenv( "RTE_HOST" ) != NULL ) c_rteServer = getenv( "RTE_HOST" );

    if (rte_connect( c_rteServer.c_str() ) < 0) {
      cerr << "Unable to connect " << c_rteServer << " server." << endl;
      exit(1);
    } else {
      cout << "Connected to " << c_rteServer << " server." << endl;
    }
  }

  for ( uint32_t ind = 0; ind < cui32_maxCanBusCnt; ind++ )
  {
    rteCan_c [ind] = new can_c ();
  }

  // clear list for CAN Pkg
//  arr_canQueue[0].list();
//  arr_canQueue[1].list();

  return HAL_NO_ERR;
}

int16_t can_stopDriver()
{
  for ( uint32_t ind = 0; ind < cui32_maxCanBusCnt; ind++ )
  {
    if (canlogDat[ind] != NULL){
      fclose(canlogDat[ind]);
      canlogDat[ind] = NULL;
    }
    delete (rteCan_c [ind]);
    rteCan_c[ind] = NULL;
  }

  for( uint32_t i=0; i<cui32_maxCanBusCnt; i++ )
      arrMsgObj[i].clear();

  delete[] arrMsgObj;
  arrMsgObj = NULL;

  return HAL_NO_ERR;
}

/** get last timestamp of CAN receive */
int32_t can_lastReceiveTime()
{
  checkMsg();
  return i32_lastReceiveTime;
}


int16_t getCanMsgBufCount(uint8_t bBusNumber,uint8_t bMsgObj)
{
  checkMsg();
  return ((bBusNumber < cui32_maxCanBusCnt)&&(bMsgObj < arrMsgObj[bBusNumber].size()))?arrMsgObj[bBusNumber][bMsgObj].rec_bufCnt:0;
};

bool waitUntilCanReceiveOrTimeout( uint16_t aui16_timeoutInterval )
{
  unsigned int busInd = 0;
  int openBus = -1;
  for ( ; busInd < cui32_maxCanBusCnt; busInd++ )
  {
    if ( b_busOpened[busInd] )
    {
      if (openBus < 0) openBus = busInd;
      for ( unsigned int msgInd = 0; msgInd < arrMsgObj[busInd].size(); msgInd++) if ( arrMsgObj[busInd][msgInd].rec_bufCnt > 0 ) return true;
    }
  }
  if ( openBus >= 0 )
  { // an open CAN BUS found
    uint32_t n = 1;
    /** @todo ON REQUEST - DEPRECATED HAL: how to handle TWO open BUSSes -> how can wait be called then */
    if ( rteCan_c[openBus]->wait( &n, RTE_ONE_MILLISECOND*rte_time_t(aui16_timeoutInterval), false ) == rte_ret_error( timeout ) ) return false;
    else return true;
  }
  else
  { // no CAN BUS opened
    usleep( aui16_timeoutInterval * 1000 );
    return false;
  }
}


/**
  check if MsgObj is currently locked
  @param aui8_busNr number of the BUS to check
  @param aui8_msgobjNr number of the MsgObj to check
  @return true -> MsgObj is currently locked
*/
bool getCanMsgObjLocked( uint8_t aui8_busNr, uint8_t aui8_msgobjNr )
{
  if ( ( aui8_busNr > 1 ) || ( aui8_msgobjNr> arrMsgObj[aui8_busNr].size()-1 ) ) return true;
  else if ( arrMsgObj[aui8_busNr][aui8_msgobjNr].b_canBufferLock ) return true;
  else return false;
}


int16_t closeCan ( uint8_t bBusNumber )
{
  if (canlogDat[bBusNumber] != NULL){
    fclose(canlogDat[bBusNumber]);
    canlogDat[bBusNumber] = NULL;
  }
  b_busOpened[bBusNumber] = false;

  for (int i=0; i < arrMsgObj[bBusNumber].size(); i++) {
    closeCanObj (bBusNumber, i);
  }

  // clear list for CAN Pkg
  arr_canQueue[0].clear();
  arr_canQueue[1].clear();
  arr_canQueue[2].clear();
  arr_canQueue[3].clear();

#ifdef SYSTEM_WITH_ENHANCED_CAN_HAL
  arrMsgObj[bBusNumber].clear();
#endif

  return HAL_NO_ERR;
};

int16_t chgCanObjPause ( uint8_t /* bBusNumber */, uint8_t /* bMsgObj */, uint16_t /* wPause */ )
{
/*
  printf("sende Pause auf BUS %d fuer CAN Objekt %d auf %d eingestellt\n",
  (int16_t)bBusNumber, (int16_t)bMsgObj, wPause);
  */
  return HAL_NO_ERR;
}

int16_t getCanBusStatus(uint8_t bBusNumber, tCanBusStatus* ptStatus )
{
  switch ( rteCanBusState[bBusNumber] )
  {
    case rtd_can_bus_state_offline:
    case rtd_can_bus_state_off:
      ptStatus->wCtrlStatusReg = CanStateOff;
      break;
    case rtd_can_bus_state_passive:
      ptStatus->wCtrlStatusReg = CanStateWarn;
      break;
    case rtd_can_bus_state_active:
      ptStatus->wCtrlStatusReg = CanStateNoErr;
      break;
  }
//  printf("getCanBusStatus fuer BUS %d\n", bBusNumber);
  return HAL_NO_ERR;
}

int16_t clearCanObjBuf(uint8_t bBusNumber, uint8_t bMsgObj)
{
  if (arrMsgObj[bBusNumber][bMsgObj].rec_bufCnt == -1)
  { // it's a send object -> call native clear transmit
//    ncdFlushTransmitQueue(gPortHandle, gChannelMask);
  }
  else
  { // set receive buffer to 0
    arrMsgObj[bBusNumber][bMsgObj].rec_bufCnt = 0;
    arrMsgObj[bBusNumber][bMsgObj].rec_bufOut = 0;
    arrMsgObj[bBusNumber][bMsgObj].rec_bufIn = 0;
  }
  return HAL_NO_ERR;
}

int16_t configCanObj ( uint8_t bBusNumber, uint8_t bMsgObj, tCanObjConfig * ptConfig )
{

#ifdef SYSTEM_WITH_ENHANCED_CAN_HAL
    if (bMsgObj >= arrMsgObj[bBusNumber].size()) {
        // add new elements in the vector with resize
        arrMsgObj[bBusNumber].resize(bMsgObj + 1);
    } else {
        // reconfigure element -> reinit rec_buf
        delete [] (arrMsgObj[bBusNumber][bMsgObj].rec_buf);
        arrMsgObj[bBusNumber][bMsgObj].rec_buf = NULL;
    }

  arrMsgObj[bBusNumber][bMsgObj].rec_bufSize = 0;
  arrMsgObj[bBusNumber][bMsgObj].rec_bufCnt = 0;

  arrMsgObj[bBusNumber][bMsgObj].b_canObjConfigured = true;
#endif

  arrMsgObj[bBusNumber][bMsgObj].b_canBufferLock = false;
  if (ptConfig->bMsgType == TX)
  { /* Sendeobjekt */
    arrMsgObj[bBusNumber][bMsgObj].rec_bufSize = -1;
  }
  else
  { // empfangsobjekt
    arrMsgObj[bBusNumber][bMsgObj].rec_bufXtd = ptConfig->bXtd;
    arrMsgObj[bBusNumber][bMsgObj].rec_buf = new can_data[ptConfig->wNumberMsgs*20];
    arrMsgObj[bBusNumber][bMsgObj].rec_bufSize = ptConfig->wNumberMsgs*20;
    arrMsgObj[bBusNumber][bMsgObj].rec_bufCnt = 0;
    arrMsgObj[bBusNumber][bMsgObj].rec_bufOut = 0;
    arrMsgObj[bBusNumber][bMsgObj].rec_bufIn = 0;
    arrMsgObj[bBusNumber][bMsgObj].rec_bufFilter = ptConfig->dwId;

#ifdef SYSTEM_WITH_ENHANCED_CAN_HAL
    if (ptConfig->bXtd == 0)
      arrMsgObj[bBusNumber][bMsgObj].ui16_mask_std = ptConfig->mask;
    else
      arrMsgObj[bBusNumber][bMsgObj].ui32_mask_xtd = ptConfig->mask;
#endif

    // clear list for CAN Pkg
    arr_canQueue[bBusNumber].clear();
    // set RTE time offset
    // t_rteOffset = rte_approx_server_now() - ( rte_time_t( getTime() ) * RTE_ONE_MILLISECOND );
  }
  return HAL_NO_ERR;
};

#ifndef SYSTEM_WITH_ENHANCED_CAN_HAL
int16_t chgCanObjId ( uint8_t bBusNumber, uint8_t bMsgObj, uint32_t dwId, uint8_t bXtd )
#else
int16_t chgCanObjId ( uint8_t bBusNumber, uint8_t bMsgObj, uint32_t dwId, uint32_t mask, uint8_t bXtd )
#endif
{
  arrMsgObj[bBusNumber][bMsgObj].b_canBufferLock = false;
  if (arrMsgObj[bBusNumber][bMsgObj].rec_bufSize > -1)
  { // active receive object
    arrMsgObj[bBusNumber][bMsgObj].rec_bufFilter = dwId;
    arrMsgObj[bBusNumber][bMsgObj].rec_bufXtd = bXtd;
  }

#ifdef SYSTEM_WITH_ENHANCED_CAN_HAL
  if (bXtd == 0)
    arrMsgObj[bBusNumber][bMsgObj].ui16_mask_std = mask;
  else
    arrMsgObj[bBusNumber][bMsgObj].ui32_mask_xtd = mask;
#endif

  return HAL_NO_ERR;
}

/**
  lock a MsgObj to avoid further placement of messages into buffer.
  @param aui8_busNr number of the BUS to config
  @param aui8_msgobjNr number of the MsgObj to config
  @param ab_doLock true==lock(default); false==unlock
  @return HAL_NO_ERR == no error;
          HAL_CONFIG_ERR == BUS not initialised or ident can't be changed
          HAL_RANGE_ERR == wrong BUS or MsgObj number
  */
int16_t lockCanObj( uint8_t aui8_busNr, uint8_t aui8_msgobjNr, bool ab_doLock )
{ // first get waiting messages
  rte_poll();
//  if (b_busOpened[aui8_busNr])
//  {
//    rteCan_c [aui8_busNr]->poll ();
//  }
  arrMsgObj[aui8_busNr][aui8_msgobjNr].b_canBufferLock = ab_doLock;
  return HAL_NO_ERR;
}


int16_t closeCanObj ( uint8_t bBusNumber,uint8_t bMsgObj )
{
  arrMsgObj[bBusNumber][bMsgObj].b_canObjConfigured = false;
  arrMsgObj[bBusNumber][bMsgObj].b_canBufferLock = false;
  if (arrMsgObj[bBusNumber][bMsgObj].rec_bufSize == -1)
  { /* Sendeobjekt */
    arrMsgObj[bBusNumber][bMsgObj].rec_bufSize = -1;
  }
  else
  { // empfangsobjekt
    delete [] (arrMsgObj[bBusNumber][bMsgObj].rec_buf);
    arrMsgObj[bBusNumber][bMsgObj].rec_buf = NULL;

    arrMsgObj[bBusNumber][bMsgObj].rec_bufSize = -1;
    arrMsgObj[bBusNumber][bMsgObj].rec_bufCnt = 0;
    arrMsgObj[bBusNumber][bMsgObj].rec_bufOut = 0;
    arrMsgObj[bBusNumber][bMsgObj].rec_bufIn = 0;

    // clear list for CAN Pkg
    arr_canQueue[bBusNumber].clear();
  }

#ifdef SYSTEM_WITH_ENHANCED_CAN_HAL
    // erase element if it is the last in the vector, otherwise it can stay there
  while (arrMsgObj[bBusNumber].back().b_canObjConfigured == false)
      arrMsgObj[bBusNumber].pop_back();
#endif

  return HAL_NO_ERR;
};

int16_t sendCanMsg ( uint8_t bBusNumber,uint8_t msgObj/* bMsgObj */, tSend * ptSend )
{

  int error;

#ifndef SYSTEM_WITH_ENHANCED_CAN_HAL
#ifdef USE_CAN_SEND_DELAY_MEASUREMENT
/** call the IRQ function*/

  __HAL::tCanMsgReg tCanregister;
  IRQ_TriggerSend(bBusNumber,msgObj,&tCanregister);
#endif
#endif
  // set RTE time offset
  // t_rteOffset = rte_approx_server_now() - ( rte_time_t( getTime() ) * RTE_ONE_MILLISECOND );

  rteCan_c[bBusNumber]->set_delta (0); // NOW!
  rteCan_c[bBusNumber]->set_count (1); // send only once

  if (ptSend->bXtd) {
    rteCan_c[bBusNumber]->send (rtd_can_type_xtd_msg, ptSend->dwId, ptSend->bDlc, ptSend->abData);
  } else {
    rteCan_c[bBusNumber]->send (rtd_can_type_std_msg, ptSend->dwId, ptSend->bDlc, ptSend->abData);
  }

  error = 0;

  if (error)
  {
    return HAL_OVERFLOW_ERR;
  }
  else
  {
    #if 1
    // long l_time = ( ( rte_approx_server_now() - t_rteOffset ) / RTE_ONE_MILLISECOND);
    fprintf(canlogDat[bBusNumber], "Send    %d.%03d: %x  %hx %hx %hx %hx %hx %hx %hx %hx\n", (getTime()/1000), (getTime()%1000), ptSend->dwId,
    ptSend->abData[0], ptSend->abData[1], ptSend->abData[2],
    ptSend->abData[3], ptSend->abData[4], ptSend->abData[5],
    ptSend->abData[6], ptSend->abData[7]);
    fflush( canlogDat[bBusNumber] );
    // insert sent msg in array to detect later receive of self sent msg
    // arr_canQueue[bBusNumber].push_back( RteCanPkg( ptSend->dwId, ptSend->bXtd, ptSend->bDlc, ptSend->abData ) );
    #endif
    return HAL_NO_ERR;
  }
};

int16_t getCanMsg ( uint8_t bBusNumber,uint8_t bMsgObj, tReceive * ptReceive )
{
#ifndef SYSTEM_WITH_ENHANCED_CAN_HAL
  uint8_t ui8_useMsgObj = bMsgObj;
#else
  uint8_t ui8_useMsgObj = bMsgObj;
  if (bMsgObj == COMMON_MSGOBJ_IN_QUEUE) {
    int32_t i32_minReceivedTime = -1;
    int32_t i32_compareTime;
    int16_t i16_tmp;
    for (int16_t i16_obj = 0; i16_obj < arrMsgObj[bBusNumber].size(); i16_obj++) {
        if (arrMsgObj[bBusNumber][i16_obj].rec_bufSize == -1)
        { // object is used for send
          continue;
        }
        i16_tmp = arrMsgObj[bBusNumber][i16_obj].rec_bufOut;
        i32_compareTime = arrMsgObj[bBusNumber][i16_obj].rec_buf[i16_tmp].i32_time;
        if ((arrMsgObj[bBusNumber][i16_obj].rec_bufCnt > 0)
                && (i32_compareTime < i32_minReceivedTime || i32_minReceivedTime == -1))
        {
            i32_minReceivedTime = i32_compareTime;
            ui8_useMsgObj = i16_obj;
        }
    }
    if (i32_minReceivedTime == -1)
        return HAL_UNKNOWN_ERR;
  }
#endif

  if (arrMsgObj[bBusNumber][ui8_useMsgObj].rec_bufCnt > 0)
  { // data received
    i32_lastReceiveTime = getTime();
    // get access to data
    int16_t i16_out = arrMsgObj[bBusNumber][ui8_useMsgObj].rec_bufOut;
    can_data* pc_data = &(arrMsgObj[bBusNumber][ui8_useMsgObj].rec_buf[i16_out]);
    // copy data
    ptReceive->dwId = pc_data->i32_ident;
    ptReceive->bDlc = pc_data->b_dlc;
    ptReceive->tReceiveTime.l1ms = pc_data->i32_time;
    ptReceive->bXtd = pc_data->b_xtd;
#ifdef SYSTEM_WITH_ENHANCED_CAN_HAL
    ptReceive->bMsgObj = ui8_useMsgObj;
#endif
    memcpy(ptReceive->abData, pc_data->pb_data, pc_data->b_dlc);
    // update ringbuffer
    arrMsgObj[bBusNumber][ui8_useMsgObj].rec_bufOut = ((i16_out + 1) % arrMsgObj[bBusNumber][ui8_useMsgObj].rec_bufSize);
    arrMsgObj[bBusNumber][ui8_useMsgObj].rec_bufCnt -= 1;
  }
  return HAL_NO_ERR;
};


int16_t checkMsg()
{
  rte_poll();
  /*
  for (uint8_t b_bus = 0; b_bus < cui32_maxCanBusCnt; b_bus++)
  {
    if (b_busOpened[b_bus])
    {
      rteCan_c [b_bus]->poll ();
    }
  }
  */
  return 0;
}


//####################################################################

/*! Handler for identification reply */
static int ident_hdl(rtd_handler_para_t* para, char* ident) {
  if ((para == 0) || (ident == 0))
    return rte_set_error( null_pointer );

  return 0;
}

/*! Handler for acknowledge reply */
static int ackHdl(rtd_handler_para_t*, rte_return_t ) {
  return 0;
}

/*! Handler for incoming CAN messages. */
static int send(rtd_handler_para_t* para, rtd_can_type_t type, uint32_t id, uint8_t size, const void *data,
                rte_client_t cid, rtd_can_event_flag_t flag, uint8_t /*code*/)
{
  static rte_time_t lastTime;
  uint8_t b_bus = para->rtd_msg->channel;
  uint8_t b_xtd = ( type == rtd_can_type_xtd_msg )?1:0;

  if (!b_busOpened[b_bus])
    return -1;


  if (0 != flag)
  { /* ERROR has been reported. */
    return -1;
  }

  if ( rteCan_c [b_bus]->get_cid() == cid )
  { // echo of this rte client
    return -1;
  }

  for (int16_t i16_obj = 0; i16_obj < arrMsgObj[b_bus].size(); i16_obj++)
  { // compare received msg with filter
    int16_t i16_in;
    can_data* pc_data;
#ifdef SYSTEM_WITH_ENHANCED_CAN_HAL
    if ( !arrMsgObj[b_bus][i16_obj].b_canObjConfigured )
      continue;
#endif

    if ( arrMsgObj[b_bus][i16_obj].b_canBufferLock )
    { // don't even check this MsgObj as it shall not receive messages
      continue;
    }
    if
    (
#ifndef SYSTEM_WITH_ENHANCED_CAN_HAL
      (
        ( i16_obj < 14 )
      && (
          ( (arrMsgObj[b_bus][i16_obj].rec_bufXtd == 1)
          && (b_xtd == 1)
          && (arrMsgObj[b_bus][i16_obj].rec_bufSize > 0)
          && ( (id & ui32_globalMask[b_bus]) ==  ((arrMsgObj[b_bus][i16_obj].rec_bufFilter) & ui32_globalMask[b_bus]) )
          )
        || ( (arrMsgObj[b_bus][i16_obj].rec_bufXtd == 0)
          && (b_xtd == 0)
          && (arrMsgObj[b_bus][i16_obj].rec_bufSize > 0)
          && ( (id & ui16_globalMask[b_bus]) ==  (arrMsgObj[b_bus][i16_obj].rec_bufFilter & ui16_globalMask[b_bus]) )
          )
        )
      )
    || (
        ( i16_obj == 14 )
      && (
          ( (arrMsgObj[b_bus][i16_obj].rec_bufXtd == 1)
          && (b_xtd == 1)
          && (arrMsgObj[b_bus][i16_obj].rec_bufSize > 0)
          && ( (id & ui32_globalMask[b_bus] & ui32_lastMask[b_bus]) == ((arrMsgObj[b_bus][i16_obj].rec_bufFilter) & ui32_globalMask[b_bus] & ui32_lastMask[b_bus]) )
          )
        || ( (arrMsgObj[b_bus][i16_obj].rec_bufXtd == 0)
          && (b_xtd == 0)
          && (arrMsgObj[b_bus][i16_obj].rec_bufSize > 0)
          && ( (id & ui16_globalMask[b_bus] & ui32_lastMask[b_bus]) == (arrMsgObj[b_bus][i16_obj].rec_bufFilter & ui16_globalMask[b_bus] & ui32_lastMask[b_bus]) )
          )
        )
      )
#else
    (
     ( (arrMsgObj[b_bus][i16_obj].rec_bufXtd == 1)
       && (b_xtd == 1)
       && (arrMsgObj[b_bus][i16_obj].rec_bufSize > 0)
       && ( (id & arrMsgObj[b_bus][i16_obj].ui32_mask_xtd) ==  (arrMsgObj[b_bus][i16_obj].rec_bufFilter & arrMsgObj[b_bus][i16_obj].ui32_mask_xtd) )
     )
     ||
     ( (arrMsgObj[b_bus][i16_obj].rec_bufXtd == 0)
       && (b_xtd == 0)
       && (arrMsgObj[b_bus][i16_obj].rec_bufSize > 0)
       && ( (id & arrMsgObj[b_bus][i16_obj].ui16_mask_std) ==  (arrMsgObj[b_bus][i16_obj].rec_bufFilter & arrMsgObj[b_bus][i16_obj].ui16_mask_std) )
     )
    )
#endif
    )
    { // received msg fits actual filter
#ifndef SYSTEM_WITH_ENHANCED_CAN_HAL
      if ( i16_obj == 14 ) INTERNAL_DEBUG_DEVICE << "\n\n Put MSG in LAST-MSG-OBJ\n" << INTERNAL_DEBUG_DEVICE_ENDL;
#endif
      i16_in = arrMsgObj[b_bus][i16_obj].rec_bufIn;
      arrMsgObj[b_bus][i16_obj].rec_bufIn = ((i16_in + 1) % arrMsgObj[b_bus][i16_obj].rec_bufSize);
      if (arrMsgObj[b_bus][i16_obj].rec_bufCnt >= arrMsgObj[b_bus][i16_obj].rec_bufSize)
      { // overflow -> insert new, and overwrite oldest msg in buffer
        arrMsgObj[b_bus][i16_obj].rec_bufOut = arrMsgObj[b_bus][i16_obj].rec_bufIn;
      }
      else
      {
        arrMsgObj[b_bus][i16_obj].rec_bufCnt += 1;
      }
      pc_data = &(arrMsgObj[b_bus][i16_obj].rec_buf[i16_in]);
      pc_data->i32_time = getTime();
      pc_data->i32_ident = id;
      pc_data->b_dlc = size;
      pc_data->b_xtd = (type == rtd_can_type_xtd_msg)?1:0;
      memcpy(pc_data->pb_data, data, pc_data->b_dlc);


      #ifndef SYSTEM_WITH_ENHANCED_CAN_HAL
       __HAL::tCanMsgReg* retValue;
      __HAL::tCanMsgReg irqInValue;


        irqInValue.tArbit.dw = pc_data->i32_ident; /** extended or standard **/
        irqInValue.tMessageCtrl.w = pc_data->b_xtd;
        irqInValue.tCfg_D0.b[0] = pc_data->b_dlc;
        irqInValue.tCfg_D0.b[1] = pc_data->pb_data[0];
        irqInValue.tD1_D4.b[0] = pc_data->pb_data[1];
        irqInValue.tD1_D4.b[1] = pc_data->pb_data[2];
        irqInValue.tD1_D4.b[2] = pc_data->pb_data[3];
        irqInValue.tD5_D7.b[0] = pc_data->pb_data[4];
        irqInValue.tD5_D7.b[1] = pc_data->pb_data[5];
        irqInValue.tD5_D7.b[2] = pc_data->pb_data[6];
        irqInValue.tD5_D7.b[3] = pc_data->pb_data[7];

/** the input of the IwriteCentralCanfifo is the CAN bus number which start from 1, -> we have to +1 make **/
      retValue =  IwriteCentralCanfifo(b_bus, i16_obj+1, &irqInValue);
      #endif
      #if 0
      if (  ((id & 0x700) == 0x700)
          || ((id & 0x700) == 0x500)
          || ((id & 0x700) == 0x200)
          || ((id & 0x700) == 0x000)
//        || ( pc_data->b_xtd )
          )
      {
        printf("Empfang %d.%03d: %x  %hx %hx %hx %hx %hx %hx %hx %hx\n", pc_data->i32_time/1000,pc_data->i32_time%1000, id,
        pc_data->pb_data[0], pc_data->pb_data[1], pc_data->pb_data[2],
        pc_data->pb_data[3], pc_data->pb_data[4], pc_data->pb_data[5],
        pc_data->pb_data[6], pc_data->pb_data[7]);

        fprintf(canlogDat[b_bus], "Empfang %d.%03d: %x  %hx %hx %hx %hx %hx %hx %hx %hx\n", pc_data->i32_time/1000,pc_data->i32_time%1000, id,
        pc_data->pb_data[0], pc_data->pb_data[1], pc_data->pb_data[2],
        pc_data->pb_data[3], pc_data->pb_data[4], pc_data->pb_data[5],
        pc_data->pb_data[6], pc_data->pb_data[7]);
      }
      #endif

    } // if fit
  } // for objNr
  can_c *c;
  if (rte_get_class<can_c>( para, c ) == 0) return rte_set_error( illegal_para );
  rte_time_t t = para->time;

  lastTime = t;
  return 0;
}

/*! Handler for CAN bus state indications. */
static int setBusState(rtd_handler_para_t* para, rtd_can_bus_state_t state)
{
  rteCanBusState[para->rtd_msg->channel] = state;
  switch( state ) {
  case rtd_can_bus_state_offline:
    cout << "CAN node offline" << endl; break;
  case rtd_can_bus_state_active:
    cout << "CAN node in error-active state" << endl; break;
  case rtd_can_bus_state_passive:
    cout << "CAN node in error-passive state" << endl; break;
  case rtd_can_bus_state_off:
    cout << "CAN node in bus-off state" << endl; break;
  }
  return 0;
}

int16_t init_can ( uint8_t bBusNumber,uint16_t wGlobMask,uint32_t dwGlobMask,uint32_t dwGlobMaskLastmsg ,uint16_t wBitrate )
{
  if ( bBusNumber >= cui32_maxCanBusCnt ) return HAL_RANGE_ERR;

  rte_poll ();
  char logName[50];
#ifdef WIN32
  sprintf( logName, "..\\..\\..\\simulated_io\\can_send_%1hd.txt", bBusNumber );
#else
  sprintf( logName, "../../../simulated_io/can_send_%1hd.txt", bBusNumber );
#endif
  static bool b_initFirstLog = true;
  static bool firstOpenLog[cui32_maxCanBusCnt];
  if ( b_initFirstLog )
  {
    for ( uint32_t ind = 0; ind < cui32_maxCanBusCnt; ind++ ) firstOpenLog[ind] = true;
    b_initFirstLog = false;
  }
  if ( firstOpenLog[bBusNumber] ) {
    firstOpenLog[bBusNumber] = false;
    canlogDat[bBusNumber] = fopen( logName, "w");
    // BEGIN: Added by M.Wodok 6.12.04
    if (canlogDat [bBusNumber] == NULL) {
      // failed opening? Try in current directory then...
      sprintf( logName, "can_send_%1hd.txt", bBusNumber );
      canlogDat[bBusNumber] = fopen( logName, "w");
    }
    // END: Added by M.Wodok 6.12.04
  }
  else {
    // use appending open on succeding can inits during one program run
    canlogDat[bBusNumber] = fopen( logName, "a+");
    // BEGIN: Added by M.Wodok 6.12.04
    if (canlogDat [bBusNumber] == NULL) {
      // failed opening? Try in current directory then...
      sprintf( logName, "can_send_%1hd.txt", bBusNumber );
      canlogDat[bBusNumber] = fopen( logName, "a+");
    }
    // END: Added by M.Wodok 6.12.04
  }


  // Open a CAN messaging interface
  rteCan_c [bBusNumber]->set_channel( bBusNumber );
  rteCan_c [bBusNumber]->set_default_timeout( 60*RTE_ONE_SECOND );
  rteCan_c [bBusNumber]->set_echo( false );
  rteCan_c [bBusNumber]->init();

  // build parameter string with sprintf to handle different baudrates
  char parameterString[50];
  sprintf( parameterString, "C%d,R,B%d,E1,L0", bBusNumber, wBitrate );
  rteCan_c [bBusNumber]->set_parameters( parameterString );
  #if 0
  if (bBusNumber == 0)
    rteCan_c [0]->set_parameters( "C0,R,B250,E0,L0" );
  else
    rteCan_c [1]->set_parameters( "C1,R,B250,E0,L0" );
  #endif

  // Set handler functions.
  rteCan_c [bBusNumber]->set_acknowledge_handler( ackHdl, rteCan_c [bBusNumber]);
  rteCan_c [bBusNumber]->set_identify_handler( ident_hdl, rteCan_c [bBusNumber]);
  rteCan_c [bBusNumber]->set_send_handler( send, rteCan_c [bBusNumber] );
//  rteCan_c [bBusNumber]->set_set_bus_state_handler( setBusState, rteCan_c [bBusNumber] );

  // Set Global Masks
  ui16_globalMask[bBusNumber] = wGlobMask;
  ui32_globalMask[bBusNumber] = dwGlobMask;
  ui32_lastMask[bBusNumber] = dwGlobMaskLastmsg;

  // test if init was okay? (later)
  b_busOpened[bBusNumber] = true;

  return HAL_NO_ERR;
};

int16_t changeGlobalMask ( uint8_t bBusNumber,uint16_t wGlobMask,uint32_t dwGlobMask,uint32_t dwGlobMaskLastmsg)
{
  if ( bBusNumber >= cui32_maxCanBusCnt ) return HAL_RANGE_ERR;

  // Set Global Masks
  ui16_globalMask[bBusNumber] = wGlobMask;
  ui32_globalMask[bBusNumber] = dwGlobMask;
  ui32_lastMask[bBusNumber] = dwGlobMaskLastmsg;

  return HAL_NO_ERR;
};



} // end namespace __HAL
