/***************************************************************************
              target_extension_can_server_A1_binary.cpp -
                    can server communicates with clients through message queues
                    read/write operation for /dev/wecan<bus_no>
                    message forwarding to other clients
                    #define SIMULATE_BUS_MODE for operation without can device (operation based on message forwarding)
                    use "can_server_a1 --help" for input parameters (log, replay mode)

                             -------------------
    begin                : Tue Oct 2 2001
    copyright            : (C) 1999 - 2004 Dipl.-Inform. Achim Spangler
    email                : a.spangler@osb-ag:de
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
 * Copyright (C) 1999 - 2004 Dipl.-Inform. Achim Spangler                  *
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

// do not use can bus, operation is based on message forwarding in server
#ifndef SYSTEM_A1
#define SIMULATE_BUS_MODE 1
#endif

#include "can_target_extensions.h"
#include <cstring>
#include <cstdio>
#include <cctype>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>

#include <list>
#include <string>

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/msg.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <sys/times.h>
#include <unistd.h>
#include <sys/stat.h>
#include <time.h>
#include <signal.h>
#include <errno.h>

#include <pthread.h>
#include <linux/version.h>

#include "can_server.h"
#include "can_msq.h"

using namespace __HAL;


/////////////////////////////////////////////////////////////////////////
// Globals
/** if the following define is active, the can_server writes important logging info to the given path.
 *  comment this define out to eliminate this */
#ifdef WIN32
  #define CAN_SERVER_LOG_PATH ".\\can_server.log"
#elif defined( SYSTEM_A1 )
  #define CAN_SERVER_LOG_PATH "/sd0/can_server.log"
#else
  #define CAN_SERVER_LOG_PATH "./can_server.log"
#endif



// CAN Globals
static int apiversion;

// IO address for LPT and ISA ( PCI and PCMCIA use automatic adr )
const int32_t LptIsaIoAdr = 0x378;

#define USE_CAN_CARD_TYPE c_CANA1BINARY




////////////////////////////////////////////////////////////////////////////////////////
// Wachendorff stuff...

#define MSGTYPE_EXTENDED        0x02            /* extended frame */
#define MSGTYPE_STANDARD        0x00            /* standard frame */

/* ioctl request codes */
#define CAN_MAGIC_NUMBER        'z'
#define MYSEQ_START             0x80

// kernel 2.6 needs type for third argument and not sizeof()
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,0)
#define CAN_WRITE_MSG   _IOW(CAN_MAGIC_NUMBER, MYSEQ_START + 1, sizeof(canmsg))
#define CAN_READ_MSG    _IOR(CAN_MAGIC_NUMBER, MYSEQ_START + 2, sizeof(canmsg))
#else
#define CAN_WRITE_MSG   _IOW(CAN_MAGIC_NUMBER, MYSEQ_START + 1, canmsg)
#define CAN_READ_MSG    _IOR(CAN_MAGIC_NUMBER, MYSEQ_START + 2, canmsg)
#endif

struct CANmsg {
        unsigned        id;
        int             msg_type;
        int             len;
        unsigned char   data[8];
        unsigned long   time;           /* timestamp in msec, at read only */
};
typedef struct CANmsg canmsg;



server_c::server_c()
  : b_logMode(FALSE), b_inputFileMode(FALSE), i32_lastPipeId(0)
{
  memset(f_canOutput, 0, sizeof(f_canOutput));
  memset(ui16_globalMask, 0, sizeof(ui16_globalMask));
}


/////////////////////////////////////////////////////////////////////////
// Function Declarations

static int ca_InitApi_1 ();
static int ca_ResetCanCard_1(void);
static int ca_InitCanCard_1 (uint32_t channel, int btr0, int btr1, server_c* pc_serverData);
static int ca_TransmitCanCard_1(tSend* ptSend, uint8_t ui8_bus, server_c* pc_serverData);

/////////////////////////////////////////////////////////////////////////
// Local Data

static int  canBusIsOpen[cui32_maxCanBusCnt];
static FILE*  canBusFp[cui32_maxCanBusCnt];


namespace __HAL {

int32_t getClientTime( client_s& ref_receiveClient )
{
  // use gettimeofday for native LINUX system
  static const unsigned int clock_t_per_sec = sysconf(_SC_CLK_TCK);

  struct timeval now;
  gettimeofday(&now, 0);

  // we use the resulting clock_t from times() call to get SECONDS
  // as these are independend from settimeofday calls
  const uint32_t cui32_now_clock_t = times(NULL)-ref_receiveClient.t_startTimeClock;

  int32_t i32_result =
      ( ((uint64_t(cui32_now_clock_t)*10ULL) / clock_t_per_sec ) * 100 )
      + ( ( now.tv_usec / 1000 ) % 100 );

  static int32_t si32_lastTime = 0;

  if (  ref_receiveClient.i32_lastTimeStamp_msec > i32_result ) i32_result = ref_receiveClient.i32_lastTimeStamp_msec;
  else ref_receiveClient.i32_lastTimeStamp_msec = i32_result;

  return i32_result;
}


} // end namespace

static void enqueue_msg(uint32_t DLC, uint32_t ui32_id, uint32_t b_bus, uint8_t b_xtd, uint8_t* pui8_data, int32_t i32_clientID, server_c* pc_serverData)
{

  can_data* pc_data;
  std::list<client_s>::iterator iter, iter_delete = 0;

  // semaphore to prevent client list modification already set in calling function

  // prepare msqRead_s for each potential broadcast send
  msqRead_s msqReadBuf;
  pc_data = &(msqReadBuf.s_canData);
  // preset constant fields of msqReadBuf
  pc_data->i32_ident = ui32_id;
  pc_data->b_dlc = DLC;
  pc_data->b_xtd = b_xtd;
  memcpy( pc_data->pb_data, pui8_data, DLC );

  for (iter = pc_serverData->l_clients.begin(); iter != pc_serverData->l_clients.end(); iter++) {

    // send signal 0 (no signal is send, but error handling is done) to check is process is alive
    if (kill(iter->i32_clientID, 0) == -1) {
      // client dead!
      DEBUG_PRINT1("client with ID %d no longer alive!\n", iter->i32_clientID);
      // remove this client from list after iterator loop
      iter_delete = iter;
      continue;
    }

    // i32_clientID != 0 in forwarding mode during send, do not enqueue this message for sending client
    if (i32_clientID && (iter->i32_clientID == i32_clientID))
      continue;

    // now search for MsgObj queue on this b_bus, where new message from b_bus maps
    for (int32_t i32_obj = 1; i32_obj < cui8_maxCanObj; i32_obj++) {

      if (!iter->b_canObjConfigured[b_bus][i32_obj])
        continue;

      if ( iter->b_canBufferLock[b_bus][i32_obj] ) {
        // don't even check this MsgObj as it shall not receive messages
        DEBUG_PRINT2("lock bus %d, obj %d\n", b_bus, i32_obj);
        continue;
      }

      if ( (iter->ui8_bMsgType[b_bus][i32_obj] != RX )
        || (iter->ui16_size[b_bus][i32_obj] == 0     ) )
      { // this MsgObj is no candidate for message receive
        continue;
      }

      // compare received msg with filter
      if
        (
         (
          ( i32_obj < cui8_maxCanObj - 1 )
          && (
               ( (iter->ui8_bufXtd[b_bus][i32_obj] == 1)
                  && (b_xtd == 1)
                  && ( (ui32_id & iter->ui32_globalMask[b_bus]) == ((iter->ui32_filter[b_bus][i32_obj]) & iter->ui32_globalMask[b_bus]) )
               )
               ||
               ( (iter->ui8_bufXtd[b_bus][i32_obj] == 0)
                  && (b_xtd == 0)
                  && ( (ui32_id & iter->ui16_globalMask[b_bus]) == (iter->ui32_filter[b_bus][i32_obj] & iter->ui16_globalMask[b_bus]) )
                 )
               )
          )
         || (
             ( i32_obj == cui8_maxCanObj - 1)
             && (
                  ( (iter->ui8_bufXtd[b_bus][i32_obj] == 1)
                    && (b_xtd == 1)
                    && ( (ui32_id & iter->ui32_globalMask[b_bus] & iter->ui32_lastMask[b_bus]) ==  ((iter->ui32_filter[b_bus][i32_obj]) & iter->ui32_globalMask[b_bus] & iter->ui32_lastMask[b_bus]) )
                    )
                  ||
                  ( (iter->ui8_bufXtd[b_bus][i32_obj] == 0)
                     && (b_xtd == 0)
                     && ( (ui32_id & iter->ui16_globalMask[b_bus] & iter->ui32_lastMask[b_bus]) ==  (iter->ui32_filter[b_bus][i32_obj] & iter->ui16_globalMask[b_bus] & iter->ui32_lastMask[b_bus]) )
                  )
                )
             )
         )
        { // received msg fits actual filter
          DEBUG_PRINT("queueing message\n");

          pc_data->i32_time = getClientTime(*iter);

          DEBUG_PRINT1("mtype: 0x%08x\n", assemble_mtype(iter->i32_clientID, b_bus, i32_obj));
          msqReadBuf.i32_mtype = assemble_mtype(iter->i32_clientID, b_bus, i32_obj);

          int i_rcSnd=msgsnd(pc_serverData->msqDataServer.i32_rdHandle, &msqReadBuf, sizeof(msqRead_s) - sizeof(int32_t), IPC_NOWAIT);
          if (i_rcSnd == -1)
          {
            DEBUG_PRINT1("error in msgsnd (errno: %d)\n", errno);
            #ifdef CAN_SERVER_LOG_PATH
            std::ofstream logging( CAN_SERVER_LOG_PATH );
            logging << "error in msgsnd (errno: " << errno << ")" << std::endl;
            #endif
            if (errno == EAGAIN)
            { // queue is full => remove oldest msg and try again
              msqWrite_s msqWriteBuf;
              DEBUG_PRINT4("message queue for CAN Ident: %d with Filter: %d, global Mask: %d for MsgObj: %d is full => try to remove oldest msg and send again!!\n",
                           ui32_id, iter->ui32_filter[b_bus][i32_obj], iter->ui32_globalMask[b_bus], i32_obj );
              #ifdef CAN_SERVER_LOG_PATH
              logging << "message queue for CAN Ident: " << ui32_id << " with Filter: "
                << iter->ui32_filter[b_bus][i32_obj] << ", global Mask: " << iter->ui32_globalMask[b_bus]
                << " for MsgObj NR: " << i32_obj
                << " is full => try to remove oldest msg and send again!!" << std::endl;
              #endif
              int i_rcRcv = msgrcv(pc_serverData->msqDataServer.i32_rdHandle, &msqWriteBuf, sizeof(msqWrite_s) - sizeof(int32_t), 0,IPC_NOWAIT);
              if ( i_rcRcv > 0 )
              { // number of received bytes > 0 => msgrcv successfull => try again
                DEBUG_PRINT("oldest msg from queue removed!!\n");
                #ifdef CAN_SERVER_LOG_PATH
                logging << "oldest msg from queue removed!!" << std::endl;
                #endif
                int i_rcSnd=msgsnd(pc_serverData->msqDataServer.i32_rdHandle, &msqReadBuf, sizeof(msqRead_s) - sizeof(int32_t), IPC_NOWAIT);
                if (i_rcSnd == 0)
                {
                  DEBUG_PRINT("message sent again after queue full!!\n");
                  #ifdef CAN_SERVER_LOG_PATH
                  logging << "message sent again after queue full!!" << std::endl;
                  #endif
                }
              }
            }
          }

          if (iter->i32_pipeHandle) {
            uint8_t ui8_buf[16];

            // clear pipe (is done also in client)
            int16_t rc=read(iter->i32_pipeHandle, &ui8_buf, 16);

            // trigger wakeup
            rc=write(iter->i32_pipeHandle, &ui8_buf, 1);
          }

          // don't check following objects if message is already enqueued for this client
          break;

        } // if fit
    } // for objNr
  }// for iter

  if (iter_delete != 0)
    // clear read/write queue for this client, close pipe, remove from client list
    releaseClient(pc_serverData, iter_delete);

}



/////////////////////////////////////////////////////////////////////////
static void* can_write_thread_func(void* ptr)
{
  int16_t i16_rc;
  int32_t i32_error = 0;
  msqWrite_s msqWriteBuf;

  int local_semaphore_id;

  server_c* pc_serverData = static_cast<server_c*>(ptr);

  if ((local_semaphore_id = open_semaphore_set(SERVER_SEMAPHORE)) == -1) {
    perror("semaphore not available");
    exit(1);
  }

  for (;;) {

    if((i16_rc = msgrcv(pc_serverData->msqDataServer.i32_wrHandle, &msqWriteBuf, sizeof(msqWrite_s) - sizeof(int32_t), 0, 0)) == -1) {
      perror("msgrcv");
      usleep(1000);
      continue;
    }

    // acquire semaphore (prevents concurrent read/write access to can driver and modification of client list during execution of enqueue_msg
    if (get_semaphore(local_semaphore_id, -1) == -1) {
      perror("aquire semaphore error");
      exit(1);
    }

    DEBUG_PRINT("+");
    i16_rc = ca_TransmitCanCard_1(&(msqWriteBuf.s_sendData), msqWriteBuf.ui8_bus, pc_serverData);
    DEBUG_PRINT1("send: %d\n", i16_rc);

    if (pc_serverData->b_logMode) {
      dumpCanMsg (msqWriteBuf.ui8_bus, msqWriteBuf.ui8_obj, &(msqWriteBuf.s_sendData), pc_serverData->f_canOutput[msqWriteBuf.ui8_bus]);
    }

    // i16_rc == 1: send was ok
    if (!i16_rc)
      i32_error = HAL_OVERFLOW_ERR;
    else
      // message forwarding
      // get clientID from msqWriteBuf.i32_mtype
      enqueue_msg(msqWriteBuf.s_sendData.bDlc, msqWriteBuf.s_sendData.dwId, msqWriteBuf.ui8_bus, msqWriteBuf.s_sendData.bXtd, &msqWriteBuf.s_sendData.abData[0], disassemble_client_id(msqWriteBuf.i32_mtype), pc_serverData);

    send_command_ack(disassemble_client_id(msqWriteBuf.i32_mtype), i32_error, &(pc_serverData->msqDataServer));

    // release semaphore
    if (get_semaphore(local_semaphore_id, 1) == -1) {
      perror("release semaphore error");
      exit(1);
    }

  }

  return NULL;
}


/////////////////////////////////////////////////////////////////////////
static void can_read(server_c* pc_serverData)
{
  can_recv_data receiveData;
  uint32_t DLC;
  bool b_moreToRead = TRUE;
  int local_semaphore_id;

  if ((local_semaphore_id = open_semaphore_set(SERVER_SEMAPHORE)) == -1) {
    perror("semaphore not available");
    exit(1);
  }

  for (;;) {

    DEBUG_PRINT(".");

    bool b_processMsg = FALSE;

#ifndef SIMULATE_BUS_MODE

    fd_set rfds;
    int retval;
    uint32_t channel, channel_with_change;
    int maxfd = 0;

    // get maxfd
    for (channel=0; channel<cui32_maxCanBusCnt; channel++ ) {
      if (canBusIsOpen[channel] && pc_serverData->can_device[channel]>maxfd)
        maxfd=pc_serverData->can_device[channel];
    }

    maxfd++;

    FD_ZERO(&rfds);

    for (channel=0; channel<cui32_maxCanBusCnt; channel++ ) {
      if (canBusIsOpen[channel]) {
        FD_SET(pc_serverData->can_device[channel], &rfds);
      }
    }

    // we have open devices
    if (maxfd > 1) {
      retval = select(maxfd, &rfds, NULL, NULL, NULL);  // wait infinitely for next CAN msg

      if(retval == -1) {
        DEBUG_PRINT("Error Occured in select\n");
        break;
      } else if (retval == 0) {
        break;
      } else {

        // get device with changes
        for (channel_with_change=0; channel_with_change<cui32_maxCanBusCnt; channel_with_change++ ) {
          if (canBusIsOpen[channel_with_change]) {
            if (FD_ISSET(pc_serverData->can_device[channel_with_change], &rfds) == 1) {
              DEBUG_PRINT2("change on channel %d, FD_ISSET %d\n", channel_with_change, FD_ISSET(pc_serverData->can_device[channel_with_change], &rfds));
              b_processMsg = TRUE;
              break;
            }
          }
        }
      }
    }

    if (b_processMsg) {

      // acquire semaphore to prevent concurrent read/write to can driver
      if (get_semaphore(local_semaphore_id, -1) == -1) {
        perror("aquire semaphore error");
        exit(1);
      }

      CANmsg msg;
      int ret = ioctl(pc_serverData->can_device[channel_with_change], CAN_READ_MSG, &msg);

      if (ret < 0) {
        /* nothing to read or interrupted system call */
#ifdef DEBUG
        perror("ioctl");
#endif
        DEBUG_PRINT1("CANRead error: %i\n", ret);
        b_processMsg = FALSE;

        // release semaphore
        if (get_semaphore(local_semaphore_id, 1) == -1) {
          perror("release semaphore error");
          exit(1);
        }

      } else {
        receiveData.b_bus = channel_with_change;
        receiveData.msg.i32_ident = msg.id;
        receiveData.msg.b_xtd = (msg.msg_type & MSGTYPE_EXTENDED) == MSGTYPE_EXTENDED;
        receiveData.msg.b_dlc = msg.len;
        receiveData.msg.i32_time = msg.time;
        memcpy( receiveData.msg.pb_data, msg.data, msg.len );
      }
    }

#else

    if (pc_serverData->b_inputFileMode && b_moreToRead) {
      b_moreToRead = readCanDataFile(pc_serverData, &receiveData);
      b_processMsg = TRUE;

      // acquire semaphore to prevent modification of client list during execution of enqueue_msg
      if (get_semaphore(local_semaphore_id, -1) == -1) {
        perror("aquire semaphore error");
        exit(1);
      }

    } else
      for (;;)
        sleep(1000);
#endif

    if (!b_processMsg) {
      usleep(10000);
      continue;
    }

    DLC = ( receiveData.msg.b_dlc & 0xF );
    if ( DLC > 8 ) DLC = 8;
    const uint32_t ui32_id = receiveData.msg.i32_ident;
    const uint32_t b_bus = receiveData.b_bus;
    const uint8_t b_xtd = receiveData.msg.b_xtd;

    DEBUG_PRINT4("DLC %d, ui32_id 0x%08x, b_bus %d, b_xtd %d\n", DLC, ui32_id, b_bus, b_xtd);

    if (ui32_id >= 0x7FFFFFFF) {
      DEBUG_PRINT1("!!Received of malformed message with undefined CAN ident: %x\n", ui32_id);
    } else
      enqueue_msg(DLC, ui32_id, b_bus, b_xtd, &receiveData.msg.pb_data[0], 0, pc_serverData);

    // release semaphore
    if (get_semaphore(local_semaphore_id, 1) == -1) {
      perror("release semaphore error");
      exit(1);
    }

  }
}



/////////////////////////////////////////////////////////////////////////
static void* command_thread_func(void* ptr)
{

  int16_t i16_rc;
  int32_t i32_error;
  msqCommand_s msqCommandBuf;
  int local_semaphore_id;
  uint16_t ui16_busRefCnt[cui32_maxCanBusCnt];

  server_c* pc_serverData = static_cast<server_c*>(ptr);

  if ((local_semaphore_id = open_semaphore_set(SERVER_SEMAPHORE)) == -1) {
    perror("semaphore not available");
    exit(1);
  }

  memset(ui16_busRefCnt, 0, sizeof(ui16_busRefCnt));

  for (;;) {

    // check command queue
    /* The length is essentially the size of the structure minus sizeof(mtype) */
    if((i16_rc = msgrcv(pc_serverData->msqDataServer.i32_cmdHandle, &msqCommandBuf, sizeof(msqCommand_s) - sizeof(int32_t), 0, 0)) == -1) {
      perror("msgrcv");
      usleep(1000);
      continue;
    }

    DEBUG_PRINT1("cmd %d\n", msqCommandBuf.i16_command);

    i32_error = 0;

    // acquire semaphore
    if (get_semaphore(local_semaphore_id, -1) == -1) {
      perror("aquire semaphore error");
      exit(1);
    }

    // get client
    std::list<client_s>::iterator iter_client = NULL, tmp_iter;
    for (tmp_iter = pc_serverData->l_clients.begin(); tmp_iter != pc_serverData->l_clients.end(); tmp_iter++)
      if (tmp_iter->i32_clientID == msqCommandBuf.i32_mtype) {
        iter_client = tmp_iter;
        break;
      }

    switch (msqCommandBuf.i16_command) {

      case COMMAND_REGISTER:
      {
        client_s s_tmpClient;

        DEBUG_PRINT("command start driver\n");

        // do check for dead clients before queueing any new message
        for (std::list<client_s>::iterator iter_deadClient = pc_serverData->l_clients.begin(); iter_deadClient != pc_serverData->l_clients.end();) {

          // send signal 0 (no signal is send, but error handling is done) to check is process is alive
          if (kill(iter_deadClient->i32_clientID, 0) == -1) {
            // client dead!
            DEBUG_PRINT1("client with ID %d no longer alive!\n", iter_deadClient->i32_clientID);
            // clear read/write queue for this client, close pipe, remove from client list, iter_deadClient is incremented
            releaseClient(pc_serverData, iter_deadClient);
          } else
            // increment iter_deadClient manually (not in for statement)
            iter_deadClient++;
        }


        // initialize
        memset(&s_tmpClient, 0, sizeof(client_s));

        // client process id is used as clientID
        s_tmpClient.i32_clientID = msqCommandBuf.i32_mtype;

        s_tmpClient.t_startTimeClock = msqCommandBuf.s_startTimeClock.t_clock;
        s_tmpClient.i32_lastTimeStamp_msec = 0;

        DEBUG_PRINT1("client start up time (absolute value in clocks): %d\n", s_tmpClient.t_startTimeClock);

        char pipe_name[255];
        sprintf(pipe_name, "%s%d", PIPE_PATH, ++(pc_serverData->i32_lastPipeId));

        umask(0);
        // if not existing already
        mknod(pipe_name, S_IFIFO|0666, 0);

        // open pipe in read/write mode to allow read access to clean the pipe and to avoid SIGPIPE when client dies and this client was the only reader to the pipe
        if ((s_tmpClient.i32_pipeHandle = open(pipe_name, O_NONBLOCK | O_RDWR, 0)) == -1)
          i32_error = HAL_UNKNOWN_ERR;

        if (!i32_error)
          pc_serverData->l_clients.push_back(s_tmpClient);

        // transmit current pipeId to client (for composition of pipe name)
        send_command_ack(msqCommandBuf.i32_mtype, i32_error, &(pc_serverData->msqDataServer), pc_serverData->i32_lastPipeId);

        break;
      }
      case COMMAND_DEREGISTER:

        DEBUG_PRINT("command stop driver\n");

        // @todo: is queue clearing necessary?
        if (iter_client != NULL) {

          if (iter_client->i32_pipeHandle)
            close(iter_client->i32_pipeHandle);

          pc_serverData->l_clients.erase(iter_client);

        } else
          i32_error = HAL_CONFIG_ERR;

        send_command_ack(msqCommandBuf.i32_mtype, i32_error, &(pc_serverData->msqDataServer));

        break;


      case COMMAND_INIT:

        int fdata[16];
        int btr0;
        int btr1;
        int16_t i16_init_rc;

        if (msqCommandBuf.s_config.ui8_bus > HAL_CAN_MAX_BUS_NR)
          i32_error = HAL_RANGE_ERR;

        if (!i32_error && !ui16_busRefCnt[msqCommandBuf.s_init.ui8_bus]) {
          // first init command for current bus

          memset(fdata, 0, sizeof(fdata));

          if (iter_client != NULL) {

            // open log file only once per bus
            if (pc_serverData->b_logMode) {
              char file_name[255];
              snprintf(file_name, 255, "%s_%hx", pc_serverData->logFileBase.c_str(), msqCommandBuf.s_init.ui8_bus);
              if ((pc_serverData->f_canOutput[msqCommandBuf.s_init.ui8_bus] = fopen(file_name, "a+")) == NULL ) {
                perror("fopen");
                exit(1);
              }
            }

            btr0 = 0;
            btr1 = 1;

            switch ( msqCommandBuf.s_init.ui16_wBitrate ) {
              case 100: { btr0 = 0xc3; btr1 = 0x3e;} break;
              case 125: { btr0 = 0xc3; btr1 = 0x3a;} break;
              case 250: { btr0 = 0xc1; btr1 = 0x3a;} break;
              case 500: { btr0 = 0xc0; btr1 = 0x3a;} break;
            }

            i16_init_rc = ca_InitCanCard_1(msqCommandBuf.s_init.ui8_bus,  // 0 for CANLPT/ICAN, else 1 for first BUS
                                           btr0,  // BTR0
                                           btr1, // BTR1
                                           pc_serverData);

            if (!i16_init_rc) {
              printf("can't initialize CAN\n");
              i32_error = HAL_CONFIG_ERR;
            }

          } else
            i32_error = HAL_CONFIG_ERR;

        }

        if (!i32_error)
          ui16_busRefCnt[msqCommandBuf.s_init.ui8_bus]++;

        // do rest of init handling in next case statement (no break!)

     case COMMAND_CHG_GLOBAL_MASK:

        if (!i32_error) {
          iter_client->ui16_globalMask[msqCommandBuf.s_init.ui8_bus] = msqCommandBuf.s_init.ui16_wGlobMask;
          iter_client->ui32_globalMask[msqCommandBuf.s_init.ui8_bus] = msqCommandBuf.s_init.ui32_dwGlobMask;
          iter_client->ui32_lastMask[msqCommandBuf.s_init.ui8_bus] = msqCommandBuf.s_init.ui32_dwGlobMaskLastmsg;
        }

        send_command_ack(msqCommandBuf.i32_mtype, i32_error, &(pc_serverData->msqDataServer));

        break;


      case COMMAND_CLOSE:

        if (msqCommandBuf.s_config.ui8_bus > HAL_CAN_MAX_BUS_NR )
          i32_error = HAL_RANGE_ERR;
        else {

          if (iter_client != NULL) {

            if (ui16_busRefCnt[msqCommandBuf.s_init.ui8_bus])
              ui16_busRefCnt[msqCommandBuf.s_init.ui8_bus]--;

            for (uint8_t i=0; i<cui32_maxCanBusCnt; i++)
              for (uint8_t j=0; j<cui8_maxCanObj; j++) {
                clearReadQueue(i, j, pc_serverData->msqDataServer.i32_rdHandle, iter_client->i32_clientID);
                clearWriteQueue(i, j, pc_serverData->msqDataServer.i32_wrHandle, iter_client->i32_clientID);
              }

            if (!ui16_busRefCnt[msqCommandBuf.s_init.ui8_bus] && pc_serverData->b_logMode && pc_serverData->f_canOutput[msqCommandBuf.s_init.ui8_bus]) {
              fclose(pc_serverData->f_canOutput[msqCommandBuf.s_init.ui8_bus]);
              pc_serverData->f_canOutput[msqCommandBuf.s_init.ui8_bus] = 0;
            }

          } else
            i32_error = HAL_CONFIG_ERR;
        }

        send_command_ack(msqCommandBuf.i32_mtype, i32_error, &(pc_serverData->msqDataServer));

        break;


      case COMMAND_CONFIG:
      case COMMAND_CHG_CONFIG:

        if ((msqCommandBuf.s_config.ui8_bus > HAL_CAN_MAX_BUS_NR ) || ( msqCommandBuf.s_config.ui8_obj > cui8_maxCanObj-1 ))
          i32_error = HAL_RANGE_ERR;
        else {
          if (iter_client != NULL) {

            iter_client->b_canObjConfigured[msqCommandBuf.s_config.ui8_bus][msqCommandBuf.s_config.ui8_obj] = TRUE;

            iter_client->ui8_bufXtd[msqCommandBuf.s_config.ui8_bus][msqCommandBuf.s_config.ui8_obj] = msqCommandBuf.s_config.ui8_bXtd;
            iter_client->ui32_filter[msqCommandBuf.s_config.ui8_bus][msqCommandBuf.s_config.ui8_obj] = msqCommandBuf.s_config.ui32_dwId;

            if (msqCommandBuf.i16_command == COMMAND_CONFIG) {

              clearReadQueue(msqCommandBuf.s_config.ui8_bus, msqCommandBuf.s_config.ui8_obj, pc_serverData->msqDataServer.i32_rdHandle, iter_client->i32_clientID);
              clearWriteQueue(msqCommandBuf.s_config.ui8_bus, msqCommandBuf.s_config.ui8_obj, pc_serverData->msqDataServer.i32_wrHandle, iter_client->i32_clientID);

              iter_client->ui8_bMsgType[msqCommandBuf.s_config.ui8_bus][msqCommandBuf.s_config.ui8_obj] = msqCommandBuf.s_config.ui8_bMsgType;
              iter_client->ui16_size[msqCommandBuf.s_config.ui8_bus][msqCommandBuf.s_config.ui8_obj] = msqCommandBuf.s_config.ui16_wNumberMsgs;
              iter_client->b_canBufferLock[msqCommandBuf.s_config.ui8_bus][msqCommandBuf.s_config.ui8_obj] = false;
            }
          } else
            i32_error = HAL_CONFIG_ERR;
        }

        send_command_ack(msqCommandBuf.i32_mtype, i32_error, &(pc_serverData->msqDataServer));

        break;


      case COMMAND_LOCK:
      case COMMAND_UNLOCK:

        if ((msqCommandBuf.s_config.ui8_bus > HAL_CAN_MAX_BUS_NR ) || ( msqCommandBuf.s_config.ui8_obj > cui8_maxCanObj-1 ))
          i32_error = HAL_RANGE_ERR;
        else {
          if (iter_client != NULL) {
            if (msqCommandBuf.i16_command == COMMAND_LOCK) {
              iter_client->b_canBufferLock[msqCommandBuf.s_config.ui8_bus][msqCommandBuf.s_config.ui8_obj] = TRUE;
              DEBUG_PRINT2("locked buf %d, obj %d\n", msqCommandBuf.s_config.ui8_bus, msqCommandBuf.s_config.ui8_obj);
            } else {
              iter_client->b_canBufferLock[msqCommandBuf.s_config.ui8_bus][msqCommandBuf.s_config.ui8_obj] = FALSE;
              DEBUG_PRINT2("unlocked buf %d, obj %d\n", msqCommandBuf.s_config.ui8_bus, msqCommandBuf.s_config.ui8_obj);
            }
          } else
            i32_error = HAL_CONFIG_ERR;
        }

        send_command_ack(msqCommandBuf.i32_mtype, i32_error, &(pc_serverData->msqDataServer));

        break;


      case COMMAND_QUERYLOCK:

        if ((msqCommandBuf.s_config.ui8_bus > HAL_CAN_MAX_BUS_NR ) || ( msqCommandBuf.s_config.ui8_obj > cui8_maxCanObj-1 ))
          i32_error = HAL_RANGE_ERR;
        else {
          if (iter_client != NULL) {
            msqCommandBuf.s_config.ui16_queryLockResult = iter_client->b_canBufferLock[msqCommandBuf.s_config.ui8_bus][msqCommandBuf.s_config.ui8_obj];
          } else
            i32_error = HAL_CONFIG_ERR;
        }

        send_command_ack(msqCommandBuf.i32_mtype, i32_error, &(pc_serverData->msqDataServer));

        break;


      case COMMAND_CLOSEOBJ:

        if ((msqCommandBuf.s_config.ui8_bus > HAL_CAN_MAX_BUS_NR ) || ( msqCommandBuf.s_config.ui8_obj > cui8_maxCanObj-1 ))
          i32_error = HAL_RANGE_ERR;
        else {
          if (iter_client != NULL) {

            iter_client->b_canObjConfigured[msqCommandBuf.s_config.ui8_bus][msqCommandBuf.s_config.ui8_obj] = FALSE;

            iter_client->b_canBufferLock[msqCommandBuf.s_config.ui8_bus][msqCommandBuf.s_config.ui8_obj] = FALSE;
            iter_client->ui16_size[msqCommandBuf.s_config.ui8_bus][msqCommandBuf.s_config.ui8_obj] = 0;
            clearReadQueue(msqCommandBuf.s_config.ui8_bus, msqCommandBuf.s_config.ui8_obj, pc_serverData->msqDataServer.i32_rdHandle, iter_client->i32_clientID);
            clearWriteQueue(msqCommandBuf.s_config.ui8_bus, msqCommandBuf.s_config.ui8_obj, pc_serverData->msqDataServer.i32_wrHandle, iter_client->i32_clientID);
          } else
            i32_error = HAL_CONFIG_ERR;
        }

        send_command_ack(msqCommandBuf.i32_mtype, i32_error, &(pc_serverData->msqDataServer));

        break;


      default:
        send_command_ack(msqCommandBuf.i32_mtype, HAL_UNKNOWN_ERR, &(pc_serverData->msqDataServer));

    } // end switch


    // release semaphore
    if (get_semaphore(local_semaphore_id, 1) == -1) {
      perror("release semaphore error");
      exit(1);
    }

  } // end for

}



/////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////
//
// METHOD:  ca_InitApi_1
// PURPOSE: To load and initialize the CAN driver and return the CAN API version
// RETURNS: API version greater than 0
//      0 on error (no CAN device available)
// NOTES: In this case, we will simply return an API version of 1.0  (0x0100)
//      Highversion is in highbyte, Lowversion is in lowbyte
//
// FUTURE:  This should be finished to actually check if /dev/wecan0 and
//      /dev/wecan1 are available and the drivers installed correctly or not
//
/////////////////////////////////////////////////////////////////////////
static int ca_InitApi_1 ()
{
  for( uint32_t i=0; i<cui32_maxCanBusCnt; i++ )
    {
    canBusIsOpen[i] = false;
    canBusFp[i] = NULL;
    }

  return 0x0100;
}

/////////////////////////////////////////////////////////////////////////
//
// METHOD:  ca_ResetCanCard_1
// PURPOSE: To reset the alreay initialized CAN driver (especially clear its
//      buffers and any error conditions)
// RETURNS: non-zero if CAN device was reset ok
//      0 on error
// NOTES: In this case, we will simply return 1
// FUTURE:  This should be finished to actually check the return values of fflush()
//
/////////////////////////////////////////////////////////////////////////
static int ca_ResetCanCard_1(void)
{
  for( uint32_t i=0; i<cui32_maxCanBusCnt; i++ )
    if( canBusIsOpen[i] )
      fflush( canBusFp[i] );

  return 1;
}

/////////////////////////////////////////////////////////////////////////
//
// METHOD:  ca_InitCanCard_1
// PURPOSE: To initialize the specified CAN BUS to begin sending/receiving msgs
// PARAMS:  channel   // 0 for CANLPT/ICAN, else 1 for first BUS
//      msgnr   // msg-nr / 0 for CANLPT/ICAN
//      accode    // Acceptance Code to receive everything for ICAN
//      accmask   // Acceptance Mask to receive everything for ICAN
//      fullcanmask // filter array of int[16];
//      btr0    // BTR0
//      btr1    // BTR1
//      octrl   // reserved
//      typ     // typ 0 = 2 x 32 Bit, 1 = 4 x 16 Bit,
//            // 2 = 8 x 8 Bit, 3 = kein durchlass
//      extended  // reserved
// RETURNS: non-zero if CAN BUS was initialized ok
//      0 on error
// NOTES: In this case, we will simply return 1
//
// FUTURE:  This should be finished to actually use the parameters like baud
//      rate and extended IDs during the initialization.  Right now they
//      are hardcoded and the passed in params are ignored.
//      Should check return value of write() to make sure the baud rate
//      was set correctly before proceeding.
//
/////////////////////////////////////////////////////////////////////////
static int ca_InitCanCard_1 (uint32_t channel, int btr0, int btr1, server_c* pc_serverData)
{

  DEBUG_PRINT1("init can channel %d\n", channel);

  if( !canBusIsOpen[channel] ) {
    DEBUG_PRINT1("Opening CAN BUS channel=%d\n", channel);

    char fname[32];
#ifndef SIMULATE_BUS_MODE
    sprintf( fname, "/dev/wecan%u", channel );
#else
    sprintf( fname, "/dev/null");
#endif

    DEBUG_PRINT1("open( \"%s\", O_RDRWR)\n", fname);

    pc_serverData->can_device[channel] = open(fname, O_RDWR | O_NONBLOCK);

    if (!pc_serverData->can_device[channel]) {
      DEBUG_PRINT1("Could not open CAN bus%d\n",channel);
      return 0;
    }

    // Set baud rate to 250 and turn on extended IDs
    // For Opus A1, it is done by sending the following string to the can_device
    {
      char buf[16];
      sprintf( buf, "i 0x%2x%2x e\n", btr0 & 0xFF, btr1 & 0xFF );     //, (extended?" e":" ") extended is not being passed in! Don't use it!

      DEBUG_PRINT3("write( device-\"%s\"\n, \"%s\", %d)\n", fname, buf, strlen(buf));
      write(pc_serverData->can_device[channel], buf, strlen(buf));
    }

    canBusFp[channel] = fdopen( pc_serverData->can_device[channel], "w+" );
    if( !canBusFp[channel] ) {
      // open succeeded, but fdopen failed!!!
      // close the open file handle created with open()
      if( pc_serverData->can_device[channel] )
        close(pc_serverData->can_device[channel]);

      DEBUG_PRINT1("Could not fdopen CAN bus%d\n",channel);

      return 0;
    }

    canBusIsOpen[channel] = true;
    return 1;

  } else
    return 1; // already initialized and files are already open
}

/////////////////////////////////////////////////////////////////////////
//
// METHOD:  ca_TransmitCanCard_1
// PURPOSE: To send a msg on the specified CAN BUS
// PARAMS:  channel   // 0 for CANLPT/ICAN, else 1 for first BUS
//      extended  // extended Frame
//      ptSend    // can object
// RETURNS: non-zero if msg was sent ok
//      0 on error
// NOTES: In this case, we will simply return 1
//
/////////////////////////////////////////////////////////////////////////
static int ca_TransmitCanCard_1(tSend* ptSend, uint8_t ui8_bus, server_c* pc_serverData)
{
// Always Transmit to this format:
//  the letter 'm', extended/standard ID, CAN ID, Data Length, data bytes, timestamp
//  m e 0x0cf00300 8  0xff 0xfe 0x0b 0xff 0xff 0xff 0xff 0xff       176600
//
// Input is in this format:
// typedef struct
// {
//   uint32_t dwId;                          /** Identifier */
//   uint8_t bXtd;                           /** Laenge Bit Identifier */
//   uint8_t bDlc;                           /** Anzahl der empfangenen Daten */
//   uint8_t abData[8];                      /** Datenpuffer */
// } tSend;

//  fprintf(stderr,"Transmitting data to channel %d\n", channel);


  CANmsg msg;
  msg.id = ptSend->dwId;
  msg.msg_type = ( ptSend->bXtd ? MSGTYPE_EXTENDED : MSGTYPE_STANDARD );
  msg.len = ptSend->bDlc;
  msg.time = 0;

  for( int i=0; i<msg.len; i++ )
    msg.data[i] = ptSend->abData[i];

#ifndef SIMULATE_BUS_MODE

// select call should not be necessary during write
#if 0
  fd_set wfds;
  int retval;
  int maxfd = pc_serverData->can_device[ui8_bus]+1;

  FD_ZERO(&wfds);
  FD_SET(pc_serverData->can_device[ui8_bus], &wfds);

  struct timeval tv;
  tv.tv_sec = 0;    // tv_sec and tv_usec= 0: timeout immediately (only check if data is waiting when on a timer)
  tv.tv_usec = 10000;  // wait not more than 10 msec for the state: nonblocking write
  retval = select(maxfd, NULL, &wfds, NULL, &tv);
  if(retval == -1)
  {
    DEBUG_PRINT("Error Occured in select\n");
    return 0;

  } else if(retval == 0)
  {
    DEBUG_PRINT("Error can't write Occured in select\n");
    return 0;
  } else
  {
    if(FD_ISSET(pc_serverData->can_device[channel], &wfds) != 1)
    {
      DEBUG_PRINT("Not selecting right thing\n");
      return 0;
    }
  }
#endif

  int ret;

  if ((ui8_bus < HAL_CAN_MAX_BUS_NR) && canBusIsOpen[ui8_bus]) {
)
    ret = ioctl(pc_serverData->can_device[ui8_bus], CAN_WRITE_MSG, &msg);

#else
  int ret = 0;
#endif

  if (ret < 0) {
    perror("ca_TransmitCanCard_1 ioctl");
    /* nothing to read or interrupted system call */
  }

  return 1;
}



int main(int argc, char *argv[])
{

  pthread_t thread_registerClient, thread_canWrite;
  int i_registerClientThreadHandle, i_canWriteThreadHandle;
  server_c c_serverData;

  if (argc > 1 && strcmp(argv[1], "--help") == 0) {
    usage();
    exit(0);
  }

  for (int i=1; i<argc; i=i+2) {
    if (i==argc-1) {
      printf("error: option needs second parameter\n");
      exit(1);
    }
    if (strcmp(argv[i], "--log") == 0) {
      c_serverData.logFileBase = argv[i+1];
      c_serverData.b_logMode=true;
    }
    if (strcmp(argv[i], "--file-input") == 0) {
      c_serverData.inputFile = argv[i+1];
      c_serverData.b_inputFileMode=true;
    }
  }


  apiversion = ca_InitApi_1();
  if ( apiversion == 0 ) { // failure - nothing found
    DEBUG_PRINT1("FAILURE - No CAN card was found with automatic search with IO address %04X for manually configured cards\r\n", LptIsaIoAdr );
    exit(1);
  }


  // do the reset
  int i = ca_ResetCanCard_1();
  if (i) {
    DEBUG_PRINT("Reset CANLPT ok\n");
  } else   {
    DEBUG_PRINT("Reset CANLPT not ok\n");
    exit(1);
  }

  // wait to be shure that CAN card is clean reset
  //  usleep(100);

  int16_t i16_rc = ca_createMsqs(c_serverData.msqDataServer);

  if (i16_rc) {
    printf("error creating queues\n");
    exit(1);
  }

  printf("creating threads\n");

  i_registerClientThreadHandle = pthread_create( &thread_registerClient, NULL, &command_thread_func, &c_serverData);
  i_canWriteThreadHandle = pthread_create( &thread_canWrite, NULL, &can_write_thread_func, &c_serverData);

  printf("operating\n");

  can_read(&c_serverData);

}
