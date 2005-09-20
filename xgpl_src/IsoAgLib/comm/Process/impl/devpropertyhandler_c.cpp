/***************************************************************************
                           devpropertyhandler_c.cpp
                             -------------------
    begin                : Tue Jun 23 2005
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
#include "devpropertyhandler_c.h"

#include <IsoAgLib/comm/Scheduler/impl/scheduler_c.h>
#include <IsoAgLib/driver/can/impl/canio_c.h>
#include <IsoAgLib/comm/Multipacket/impl/multisend_c.h>
#include <IsoAgLib/util/impl/singleton.h>
#include <supplementary_driver/driver/rs232/irs232io_c.h>

#ifdef USE_ISO_TERMINAL
  #include <IsoAgLib/comm/ISO_Terminal/impl/isoterminal_c.h>
#endif

//define length of every attribute in deviceObject
#define DEF_Transfer_Code 1
#define DEF_TableID 3
#define DEF_ObjectID 2
#define DEF_Designator_Length 1
#define DEF_Software_Version_Length 1
#define DEF_Workingset_MasterNAME 8
#define DEF_Serialnumber_Length 1
#define DEF_Structurelabel 7


/** This function delivers the offset to the structurelabel in every device description*/
uint8_t getLabelOffset (const uint8_t* pc_Array)
{
  uint8_t ui8_designatorLength =      pc_Array[DEF_Transfer_Code + DEF_TableID + DEF_ObjectID];
  uint8_t ui8_softwareVersionLength = pc_Array[DEF_Transfer_Code + DEF_TableID + DEF_ObjectID + DEF_Designator_Length + ui8_designatorLength];
  uint8_t ui8_serialNumberLength =    pc_Array[DEF_Transfer_Code + DEF_TableID + DEF_ObjectID + DEF_Designator_Length + ui8_designatorLength + DEF_Workingset_MasterNAME + DEF_Software_Version_Length + ui8_softwareVersionLength];
  uint8_t ui8_offsetLabel = ( DEF_Transfer_Code
                            + DEF_TableID
                            + DEF_ObjectID
                            + DEF_Designator_Length
                            + ui8_designatorLength
                            + DEF_Software_Version_Length
                            + ui8_softwareVersionLength //pc_Array[(DEF_Transfer_Code + DEF_TableID + DEF_ObjectID + DEF_Designator_Length + pc_Array[DEF_Transfer_Code + DEF_TableID + DEF_ObjectID + DEF_Designator_LengthDEF_TableID+DEF_ObjectID] + DEF_Designator_Length)]
                            + DEF_Workingset_MasterNAME
                            + DEF_Serialnumber_Length
                            + ui8_serialNumberLength
                            + DEF_Structurelabel
                            );
  return ui8_offsetLabel;
}

namespace __IsoAgLib {

#define DEF_TimeOut_GetVersion 5000
#define DEF_TimeOut_OPTransfer 10000
#define DEF_TimeOut_EndOfDevicePool 10000
#define DEF_WaitFor_Reupload 5000
#define DEF_TimeOut_ChangeDesignatorValue 1500
#define DEF_TimeOut_NormalCommand 1500

//define helper class LanguageLabel_c
//===================================================================
/** class to stored the language label from a device description */
LanguageLabel_c::LanguageLabel_c (const char* label)
{
  CNAMESPACE::memcpy(str, label, 2);
};

LanguageLabel_c::LanguageLabel_c (const LanguageLabel_c& c_langLabel)
{
  CNAMESPACE::memcpy(str, c_langLabel.str, 2);
};

const LanguageLabel_c& LanguageLabel_c::operator=(const LanguageLabel_c& c_langLabel)
{
  CNAMESPACE::memcpy(str, c_langLabel.str, 2);
  return c_langLabel;
};

const LanguageLabel_c& LanguageLabel_c::operator=(const uint8_t* ui8_str)
{
  CNAMESPACE::memcpy(str, ui8_str, 2);
  return *this;
};

bool LanguageLabel_c::operator<(const LanguageLabel_c& c_langLabel) const
{
  return CNAMESPACE::strcmp(str, c_langLabel.str);
};

bool LanguageLabel_c::operator==(const LanguageLabel_c& c_langLabel)
{
  return (CNAMESPACE::strcmp(str, c_langLabel.str)==0)?true:false;
};
//end of definition LanguageLabel_c
//===================================================================


//define helper class DevicePool_c
//===================================================================
/** class to stored the device description byte stream and its length */
DevicePool_c::DevicePool_c (const uint8_t* pcui8_bytestream, const uint32_t ui32_length)
{
  p_DevicePool = pcui8_bytestream;
  devicePoolLength = ui32_length;
}

const DevicePool_c& DevicePool_c::operator=(const DevicePool_c& c_devicePool)
{
  p_DevicePool = c_devicePool.p_DevicePool;
  devicePoolLength = c_devicePool.devicePoolLength;
  return *this;
}
//end of definition DevicePool_c
//===================================================================


/** process received can messages
    @return true -> message was processed; else the received CAN message will be served to other matching CANCustomer_c
  */
bool
DevPropertyHandler_c::processMsg()
{
  if ((data().isoPgn() & 0x1FF00) != PROCESS_DATA_PGN)
    //should never be the case
    return FALSE;

  // set FALSE if "default" is selected in switch case 
  bool b_rc = TRUE;
     
  //handling of status message
  if ((data().getUint8Data(0) & 0xF) == 0xE)
  {
    tcState_lastReceived = HAL::getTime();
    //tcState_saOfActiveWorkingSetMaster = data().getUint8Data (1); //do we need it???
    tcSourceAddress = data().isoSa();
    #ifdef DEBUG
      EXTERNAL_DEBUG_DEVICE << "Received status message..." << EXTERNAL_DEBUG_DEVICE_ENDL;
    #endif
  }

  //handling of nack
  //-> means that no device description is uploaded before
  //-> so extract all necessary information from the byte-stream (structure and localization label)
  //-> add the stream to the map
  if ((data().getUint8Data(0) & 0xF) == 0xD)
  {
    //here starts nack handling
    // these two cmds could be answered with a NACK
    switch ((data().getUint8Data(0) >> 4) & 0xF)
    {
      case 0x0: //NACK Request StructureLabel
        if (en_uploadState == StatePresettings && en_uploadStep == UploadWaitForStructureLabelResponse)
        {
          en_uploadStep = UploadWaitForUploadInit;
          b_receivedStructureLabel = false;
          #ifdef DEBUG
          EXTERNAL_DEBUG_DEVICE << "Received NACK for structure label..." << EXTERNAL_DEBUG_DEVICE_ENDL;
          #endif
        }
        break;
      case 0x2: //NACK Request LocalizationLabel
        if (en_uploadState == StatePresettings && en_uploadStep == UploadWaitForLocalizationLabelResponse)
        {
          en_uploadStep = UploadWaitForUploadInit;
          b_receivedLocalizationLabel = false;
          #ifdef DEBUG
            EXTERNAL_DEBUG_DEVICE << "Received NACK for localization label..." << EXTERNAL_DEBUG_DEVICE_ENDL;
          #endif
        }
        break;
      default:
        b_rc = FALSE;
    }
  }

  uint8_t i;
  switch (data().getUint8Data (0))
  {
    case procCmdPar_VersionMsg:
      if (en_uploadState == StateUploadInit && en_uploadStep == UploadWaitForVersionResponse)
      {
        ui8_versionLabel = data().getUint8Data(1);
        #ifdef DEBUG
          EXTERNAL_DEBUG_DEVICE << "Received version response..." << EXTERNAL_DEBUG_DEVICE_ENDL;
        #endif
        startUpload();
      }
      break;
    case procCmdPar_StructureLabelMsg:
      if (en_uploadState == StatePresettings && en_uploadStep == UploadWaitForStructureLabelResponse)
      {
        //store structureLabel for later compare in StateUploadInit
        for (i=1; i<8; i++) pch_structureLabel[i] = char(data().getUint8Data(i));
        b_receivedStructureLabel = true;
        // send Request Localization Label message
        pc_data->setExtCanPkg8 (3, 0, 203, tcSourceAddress, pc_wsMasterIdentItem->getIsoItem()->nr(),
                          procCmdPar_RequestLocalizationLabelMsg, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff);
        getCanInstance4Comm() << *pc_data;
        ui32_uploadTimestamp = HAL::getTime();
        ui32_uploadTimeout = DEF_TimeOut_NormalCommand;
        #ifdef DEBUG
          EXTERNAL_DEBUG_DEVICE << "Received structure label response..." << EXTERNAL_DEBUG_DEVICE_ENDL;
        #endif
      }
      break;
    case procCmdPar_LocalizationLabelMsg:
      if (en_uploadState == StatePresettings && en_uploadStep == UploadWaitForLocalizationLabelResponse)
      {
        //store localizationLabel for later compare in StateUploadInit
        for (i=1; i<8; i++) pch_localizationLabel[i] = char(data().getUint8Data(i));
        b_receivedLocalizationLabel = true;
        #ifdef DEBUG
          EXTERNAL_DEBUG_DEVICE << "Received localization response..." << EXTERNAL_DEBUG_DEVICE_ENDL;
        #endif
      }
      break;
    case procCmdPar_RequestOPTransferRespMsg:
      if (en_uploadState == StateUploadPool && en_uploadStep == UploadWaitForRequestOPTransferResponse)
      {
        if (data().getUint8Data(1) == 0)
        {
          en_uploadStep = UploadUploading;
          getMultiSendInstance().sendIsoTarget(pc_wsMasterIdentItem->getIsoItem()->nr(), tcSourceAddress,
          pc_devPoolForUpload->p_DevicePool, pc_devPoolForUpload->devicePoolLength, PROCESS_DATA_PGN, en_sendSuccess);
        }
        else
        {
          ui8_commandParameter = procCmdPar_RequestOPTransferRespMsg;
          outOfMemory();
        }
        #ifdef DEBUG
          EXTERNAL_DEBUG_DEVICE << "Received response for request OP transfer..." << EXTERNAL_DEBUG_DEVICE_ENDL;
        #endif
      }
      break;
    case procCmdPar_OPTransferRespMsg:
      // if timeEvent not yet called => set uploadStep right here
      en_uploadStep = UploadWaitForOPTransferResponse;
      if (en_uploadState == StateUploadPool)
      {
        if (data().getUint8Data(1) == 0)
        {
          en_uploadStep = UploadWaitForOPActivateResponse;
          pc_data->setExtCanPkg8 (3, 0, 203, tcSourceAddress, pc_wsMasterIdentItem->getIsoItem()->nr(),
                          procCmdPar_OPActivateMsg, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff);
          getCanInstance4Comm() << *pc_data;
          ui32_uploadTimestamp = HAL::getTime();
          ui32_uploadTimeout = DEF_TimeOut_NormalCommand;
          #ifdef DEBUG
            EXTERNAL_DEBUG_DEVICE << "Received positive response for OP transfer..." << EXTERNAL_DEBUG_DEVICE_ENDL;
          #endif
        }
        else
        {
          ui8_commandParameter = procCmdPar_OPTransferRespMsg;
          outOfMemory();
          #ifdef DEBUG
          EXTERNAL_DEBUG_DEVICE << "Received negative response for OP transfer..." << EXTERNAL_DEBUG_DEVICE_ENDL;
          #endif
        }
      }
      break;
    case procCmdPar_OPActivateRespMsg:
      #ifdef DEBUG
      EXTERNAL_DEBUG_DEVICE << "Received activate response..." << EXTERNAL_DEBUG_DEVICE_ENDL;
      #endif
      if (en_uploadState == StateUploadPool && en_uploadStep == UploadWaitForOPActivateResponse)
      {
        if (data().getUint8Data(1) == 0)
        {
          en_uploadState = StateIdle;
          en_uploadStep = UploadNone;
          en_poolState = OPSuccessfullyUploaded;
        }
        else
        {
          /** @todo further output for user to locate the error which caused the upload fail*/
          en_uploadStep = UploadFailed;
          en_poolState = OPCannotBeUploaded;
          ui8_commandParameter = procCmdPar_OPActivateRespMsg;
          #if defined(DEBUG) && defined(SYSTEM_PC)
          EXTERNAL_DEBUG_DEVICE << (uint16_t) data().getUint8Data(1) << EXTERNAL_DEBUG_DEVICE_ENDL;
          EXTERNAL_DEBUG_DEVICE << "upload failed, activate with error..." << EXTERNAL_DEBUG_DEVICE_ENDL;
          #endif
        }
      }
      break;
    case procCmdPar_OPDeleteMsg:
      if (en_uploadState == StateUploadPool && en_uploadStep == UploadWaitForDeleteResponse)
      {
        if (data().getUint8Data(1) == 0)
        {
          pc_data->setExtCanPkg8 (3, 0, 203, tcSourceAddress, pc_wsMasterIdentItem->getIsoItem()->nr(),
                            procCmdPar_RequestVersionMsg, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff);
          getCanInstance4Comm() << *pc_data;
          ui32_uploadTimestamp = HAL::getTime();
          ui32_uploadTimeout = DEF_TimeOut_NormalCommand;
          en_uploadStep = UploadWaitForVersionResponse;
        }
        else
        {
          /** @todo if the pool couldn't be deleted: fail upload or just ignore it??? */
          pc_data->setExtCanPkg8 (3, 0, 203, tcSourceAddress, pc_wsMasterIdentItem->getIsoItem()->nr(),
                            procCmdPar_RequestVersionMsg, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff);
          getCanInstance4Comm() << *pc_data;
          ui32_uploadTimestamp = HAL::getTime();
          ui32_uploadTimeout = DEF_TimeOut_NormalCommand;
          en_uploadStep = UploadWaitForVersionResponse;
        }
        #ifdef DEBUG
        EXTERNAL_DEBUG_DEVICE << "Received delete response..." << EXTERNAL_DEBUG_DEVICE_ENDL;
        #endif
      }
      break;
    /* UploadCommandWaitingForCommandResponse and UploadMultiSendCommandWaitingForCommandResponse
     * are states for the same cmd but different ways of sending (via CAN pkg and via mss)
     */
    case procCmdPar_ChangeDesignatorRespMsg:
      if (en_uploadState == StateUploadCommand &&
         (en_uploadCommand == UploadCommandWaitingForCommandResponse || UploadMultiSendCommandWaitingForCommandResponse))
      {
        //no matter if successful or faulty, finish upload command
        finishUploadCommandChangeDesignator();
        #ifdef DEBUG
        EXTERNAL_DEBUG_DEVICE << "ChangeDesignatorRespMsg..." << EXTERNAL_DEBUG_DEVICE_ENDL;
        #endif
      }
      break;
    default:
      b_rc = FALSE;
  }
  
  return b_rc;
}


/** initialise element which can't be done during construct
  */
void
DevPropertyHandler_c::init(ProcessPkg_c *rpc_data)
{
  static bool b_basicInit = false;
  pc_data = rpc_data;

  if (!b_basicInit)
  {
    tcState_lastReceived = 0;
    tcSourceAddress = 0x7F;

    en_poolState = OPNotRegistered;
    en_uploadState = StateIdle;
    en_uploadStep = UploadStart;
    pc_devPoolForUpload = NULL;
  }

  b_basicInit = true;
}

/** periodically event -> call timeEvent for all  identities and parent objects
    @return true -> all planned activities performed in allowed time
  */
bool
DevPropertyHandler_c::timeEvent( void )
{
  bool tcAliveOld = tcAliveNew;
  tcAliveNew = isTcActive();

  if (tcAliveOld != tcAliveNew) {
    // react on vt alive change "false->true"
    if (tcAliveNew == true) {
      if (en_poolState != OPNotRegistered) {
        en_poolState = OPRegistered;
      }
      en_uploadState = StatePresettings;
      en_uploadStep = UploadStart;
    }
  }

  // ### Do nothing if there's no TC active ###
  if (!isTcActive()) return true;

  if (! getSystemMgmtInstance4Comm().existActiveLocalIsoMember()) return true;

  if ((en_poolState == OPNotRegistered) || (en_poolState == OPCannotBeUploaded)) return true;

  //now TC is active, so send your msgs to TC
  switch(en_uploadStep)
  {
    case UploadStart:
      //first of all: get the structure label (if available)
      pc_data->setExtCanPkg8 (3, 0, 203, tcSourceAddress, pc_wsMasterIdentItem->getIsoItem()->nr(),
                            procCmdPar_RequestStructureLabelMsg, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff);
      getCanInstance4Comm() << *pc_data;
      ui32_uploadTimestamp = HAL::getTime();
      ui32_uploadTimeout = DEF_TimeOut_NormalCommand;
      en_uploadStep = UploadWaitForStructureLabelResponse;
      #ifdef DEBUG
      EXTERNAL_DEBUG_DEVICE << "Wait for structure label..." << EXTERNAL_DEBUG_DEVICE_ENDL;
      #endif
      break;
    case UploadWaitForVersionResponse:
      if (((uint32_t) HAL::getTime()) > (ui32_uploadTimeout + ui32_uploadTimestamp))
      {
        /** @todo set versionLabel to a default??? */
        #ifdef DEBUG
        EXTERNAL_DEBUG_DEVICE << "No version available..." << EXTERNAL_DEBUG_DEVICE_ENDL;
        #endif
        startUpload();
      }
      //else std::cout << "still waiting for timeout or response..." << EXTERNAL_DEBUG_DEVICE_ENDL;
      break;
    case UploadWaitForStructureLabelResponse:
      //Timed out???
      if (((uint32_t) HAL::getTime()) > (ui32_uploadTimeout + ui32_uploadTimestamp))
      {
        b_receivedStructureLabel = false;
        en_uploadStep = UploadWaitForUploadInit;
        #ifdef DEBUG
        EXTERNAL_DEBUG_DEVICE << "Wait for structure label timed out, go to wait for upload init..." << EXTERNAL_DEBUG_DEVICE_ENDL;
        #endif
      }
      break;
    case UploadWaitForLocalizationLabelResponse:
      //Timed out???
      if (((uint32_t) HAL::getTime()) > (ui32_uploadTimeout + ui32_uploadTimestamp))
      {
        b_receivedLocalizationLabel = false;
        en_uploadStep = UploadWaitForUploadInit;
        #ifdef DEBUG
        EXTERNAL_DEBUG_DEVICE << "Wait for upload init..." << EXTERNAL_DEBUG_DEVICE_ENDL;
        #endif
      }
      break;
    case UploadWaitForUploadInit:
      if (pc_devDefaultDeviceDescription == NULL) {/*wait*/}
      if (pc_devDefaultDeviceDescription != NULL)
      {
        en_uploadState = StateUploadInit;
        if (!b_receivedStructureLabel)
        {
          #ifdef DEBUG
          EXTERNAL_DEBUG_DEVICE << "get pool from isoterminal or via default..." << EXTERNAL_DEBUG_DEVICE_ENDL;
          #endif
          getPoolForUpload();
        }
        else
        {
          #ifdef DEBUG
          EXTERNAL_DEBUG_DEVICE << "init upload..." << EXTERNAL_DEBUG_DEVICE_ENDL;
          #endif
          initUploading();
        }

        if (b_receivedStructureLabel)
        {
          pc_data->setExtCanPkg8 (3, 0, 203, tcSourceAddress, pc_wsMasterIdentItem->getIsoItem()->nr(),
                                  procCmdPar_OPDeleteMsg, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff);
          getCanInstance4Comm() << *pc_data;
          ui32_uploadTimestamp = HAL::getTime();
          ui32_uploadTimeout = DEF_TimeOut_NormalCommand;
          en_uploadStep = UploadWaitForDeleteResponse;
          #ifdef DEBUG
          EXTERNAL_DEBUG_DEVICE << "Wait for delete response..." << EXTERNAL_DEBUG_DEVICE_ENDL;
          #endif
        }
        else
        {
          pc_data->setExtCanPkg8 (3, 0, 203, tcSourceAddress, pc_wsMasterIdentItem->getIsoItem()->nr(),
                                procCmdPar_RequestVersionMsg, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff);
          getCanInstance4Comm() << *pc_data;
          ui32_uploadTimestamp = HAL::getTime();
          ui32_uploadTimeout = DEF_TimeOut_NormalCommand;
          en_uploadStep = UploadWaitForVersionResponse;
          #ifdef DEBUG
          EXTERNAL_DEBUG_DEVICE << "Wait for version response..." << EXTERNAL_DEBUG_DEVICE_ENDL;
          #endif
        }
      }
      break;
    case UploadWaitForDeleteResponse:
      if (((uint32_t) HAL::getTime()) > (ui32_uploadTimeout + ui32_uploadTimestamp))
      {
        en_uploadState = StateIdle;
        en_uploadStep = UploadFailed;
        ui8_commandParameter = procCmdPar_OPDeleteRespMsg;
        ui32_uploadTimestamp = HAL::getTime();
        ui32_uploadTimeout = DEF_WaitFor_Reupload;
        #ifdef DEBUG
        EXTERNAL_DEBUG_DEVICE << "Upload timed out when deleting old pool, wait for re-upload..." << EXTERNAL_DEBUG_DEVICE_ENDL;
        #endif
      }
      break;
    case UploadWaitForRequestOPTransferResponse:
      if (((uint32_t) HAL::getTime()) > (ui32_uploadTimeout + ui32_uploadTimestamp))
      {
        en_uploadState = StateIdle;
        en_uploadStep = UploadFailed;
        ui8_commandParameter = procCmdPar_RequestOPTransferRespMsg;
        ui32_uploadTimestamp = HAL::getTime();
        ui32_uploadTimeout = DEF_WaitFor_Reupload;
        #ifdef DEBUG
        EXTERNAL_DEBUG_DEVICE << "Upload timed out, wait for re-upload..." << EXTERNAL_DEBUG_DEVICE_ENDL;
        #endif
      }
      break;
    case UploadRetryUpload:
      startUpload();
      break;
    case UploadUploading:
      switch (en_sendSuccess) {
        case __IsoAgLib::MultiSend_c::Running: {
          // wait
        } break;
        case __IsoAgLib::MultiSend_c::SendAborted: {
          // aborted sending
          en_uploadState = StateIdle;
          en_uploadStep = UploadFailed;
          ui8_commandParameter = procCmdPar_OPTransferMsg;
          ui32_uploadTimestamp = HAL::getTime();
          ui32_uploadTimeout = DEF_WaitFor_Reupload;
        } break;
        case __IsoAgLib::MultiSend_c::SendSuccess: {
          en_uploadStep = UploadWaitForOPTransferResponse;
          ui32_uploadTimestamp = HAL::getTime();
          ui32_uploadTimeout = DEF_TimeOut_EndOfDevicePool;
          #ifdef DEBUG
          EXTERNAL_DEBUG_DEVICE << "Upload successful, wait for transfer response..." << EXTERNAL_DEBUG_DEVICE_ENDL;
          #endif
        }
      }
      break;
    case UploadWaitForOPTransferResponse:
      if (((uint32_t) HAL::getTime()) > (ui32_uploadTimeout + ui32_uploadTimestamp))
      {
        en_uploadState = StateIdle;
        en_uploadStep = UploadFailed;
        ui8_commandParameter = procCmdPar_OPTransferRespMsg;
        ui32_uploadTimestamp = HAL::getTime();
        ui32_uploadTimeout = DEF_WaitFor_Reupload;
        #ifdef DEBUG
        EXTERNAL_DEBUG_DEVICE << "Upload failed, timeout when waiting for transfer response,wait for re-upload..." << EXTERNAL_DEBUG_DEVICE_ENDL;
        #endif
      }
      break;
    case UploadWaitForOPActivateResponse:
      if (((uint32_t) HAL::getTime()) > (ui32_uploadTimeout + ui32_uploadTimestamp))
      {
        en_uploadState = StateIdle;
        en_uploadStep = UploadFailed;
        ui8_commandParameter = procCmdPar_OPActivateMsg;
        ui32_uploadTimestamp = HAL::getTime();
        ui32_uploadTimeout = DEF_WaitFor_Reupload;
        #ifdef DEBUG
        EXTERNAL_DEBUG_DEVICE << "Upload failed when waiting for activating pool, wait for re-upload..." << EXTERNAL_DEBUG_DEVICE_ENDL;
        #endif
      }
      break;
    case UploadFailed:
      if (((uint32_t) HAL::getTime()) > (ui32_uploadTimeout + ui32_uploadTimestamp))
      {
        switch (ui8_commandParameter)
        {
          case procCmdPar_OPActivateRespMsg:
          case procCmdPar_OPActivateMsg:
          case procCmdPar_OPTransferMsg:
          case procCmdPar_OPTransferRespMsg:
          case procCmdPar_RequestOPTransferRespMsg:
          case procCmdPar_RequestOPTransferMsg:
            en_uploadState = StateUploadPool;
            en_uploadStep = UploadRetryUpload;
            break;
          case procCmdPar_OPDeleteRespMsg:
            pc_data->setExtCanPkg8 (3, 0, 203, tcSourceAddress, pc_wsMasterIdentItem->getIsoItem()->nr(),
                                  procCmdPar_OPDeleteMsg, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff);
            getCanInstance4Comm() << *pc_data;
            ui32_uploadTimestamp = HAL::getTime();
            ui32_uploadTimeout = DEF_TimeOut_NormalCommand;
            en_uploadStep = UploadWaitForDeleteResponse;
            break;
          default:
            en_uploadState = StatePresettings;
            en_uploadStep = UploadStart;
            break;
        }
      }
      break;
    case UploadNone:
      //do nothing
      break;
  }

  /////////////////////////////
  /// UPLOADING --> COMMAND <--
  // Can only be done if the Object-Pool is successfully uploaded!
  if (en_poolState != OPSuccessfullyUploaded) return true;

  /// FROM HERE THE OBJECT-POOL >>IS<< UPLOADED SUCCESSFULLY
  /// NOW HERE THE RUNTIME COMMANDS ARE BEING HANDLED

  if (en_uploadState == StateUploadCommand)
  {
    if (en_uploadCommand == UploadCommandWaitingForCommandResponse)
    {
      // Waiting for an answer - Did it time out?
      if (((uint32_t) HAL::getTime()) > (ui32_uploadTimeout + ui32_uploadTimestamp)) {
        en_uploadCommand = UploadCommandTimedOut;
      }
    }
    else if (en_uploadCommand == UploadMultiSendCommandWaitingForCommandResponse){
      if (((uint32_t) HAL::getTime()) < (ui32_uploadTimeout + ui32_uploadTimestamp))
      {
        switch (en_sendSuccess) {
        case __IsoAgLib::MultiSend_c::Running: {
          // wait
          } break;
        case __IsoAgLib::MultiSend_c::SendAborted: {
          // aborted sending
          en_uploadState = StateIdle;
          en_uploadCommand = UploadCommandTimedOut;
          } break;
        case __IsoAgLib::MultiSend_c::SendSuccess: {
          en_uploadState = StateIdle;
          }
        }
      }
    }
    // last Upload failed?
    if (en_uploadCommand == UploadCommandTimedOut)
    {
      if (ui8_uploadRetry > 0) {
        ui8_uploadRetry--;
        startUploadCommandChangeDesignator();
      } else {
        // No more retries, simply finish this job and go Idle!
        //finishUploadCommandChangeDesignator(); // will pop the SendUpload, as it can't be correctly sent after <retry> times. too bad.
      }
    }
  } // UploadCommand

  // ### Is a) no Upload running and b) some Upload to do?
  if ((en_uploadState == StateIdle) && !l_sendUpload.empty()) {
    // Set Retry & Start Uploading
    ui8_uploadRetry = (*(l_sendUpload.begin())).ui8_retryCount;
    startUploadCommandChangeDesignator ();
  }

  return true;
}


/** store every device description with its length in the map with its lanuage label
    @return true => device description successfully stored
  */
bool
DevPropertyHandler_c::queuePoolInMap (const uint8_t* rpc_devicePoolByteArray, uint32_t rui32_bytestreamlength, bool b_setToDefault)
{
  /** @todo should we test for minimum size of the pool??? (1 DeviceObject + 1 DeviceElementObject)*/

  LanguageLabel_c langLabel;
  DevicePool_c devicePool (rpc_devicePoolByteArray, rui32_bytestreamlength);

  langLabel = &(rpc_devicePoolByteArray[getLabelOffset(rpc_devicePoolByteArray)]);
  std::map<LanguageLabel_c, DevicePool_c>::iterator c_iterNew = map_deviceDescription.insert(map_deviceDescription.begin(), std::pair<LanguageLabel_c, DevicePool_c>(langLabel, devicePool));

  //set DeviceDescription to default
  //if nothing is default, take the first mapped pointer
  if ((pc_devDefaultDeviceDescription == NULL) || (b_setToDefault) )
  {
    pc_devDefaultDeviceDescription = &(c_iterNew->second);
    #if 0
    std::map<LanguageLabel_c, DevicePool_c>::iterator it_default;
    it_default = map_deviceDescription.begin();
    pc_devDefaultDeviceDescription = &it_default->second;
    #endif
  }
  return true;
}


/** every possible device description is stored in a maps
    @return true => if pool was successfully stored
  */
bool
DevPropertyHandler_c::registerDevicePool(const IsoAgLib::iIdentItem_c* rpc_wsMasterIdentItem, const uint8_t* rpc_devicePoolByteArray, const uint32_t rui32_bytestreamlength, bool b_setToDefault)
{
  //no double registration for one device description
  if (en_poolState != OPNotRegistered) return false;

  pc_wsMasterIdentItem = rpc_wsMasterIdentItem;

  if (!(queuePoolInMap(rpc_devicePoolByteArray, rui32_bytestreamlength, b_setToDefault)))
  {
    return false;
  }

  en_poolState = OPRegistered;      // try to upload until state == UploadedSuccessfully || CannotBeUploaded
  return true;
}


/**
  call to check if at least one tc_statusMessage has arrived so we know if the terminal is there.
  @return true -> >= 1 tc_statusMessages have arrived -> task controller is there.
*/
bool
DevPropertyHandler_c::isTcActive ()
{
  uint32_t curTime = HAL::getTime();
  if (tcState_lastReceived) {
    if ((curTime-tcState_lastReceived) <= 6000) {
      return true;
    } else {
      return false;
    }
  } else {
    return false;
  }
}


/** compare the received structure label from TC with that from the pool which should be uploaded
    if they are different, try a match via the local settings from the ISOTerminal
  */
void
DevPropertyHandler_c::initUploading()
{
  //compare received structurelabel
  uint8_t ui8_offset = getLabelOffset(pc_devDefaultDeviceDescription->p_DevicePool);
  if (CNAMESPACE::strncmp(pch_structureLabel, (const char*)&pc_devDefaultDeviceDescription->p_DevicePool[ui8_offset], 7) != 0)
  {
    getPoolForUpload();
  }
  else
  {
    if (b_receivedLocalizationLabel)
    {
      char ch_temp[2] = { 'e', 'n' };
      #ifdef USE_ISO_TERMINAL
      ch_temp[0] = ((getIsoTerminalInstance().getLocalSettings()->languageCode) >> 8) & 0xFF;
      ch_temp[1] = (getIsoTerminalInstance().getLocalSettings()->languageCode) & 0xFF;
      #endif
      if (CNAMESPACE::strncmp(pch_localizationLabel, ch_temp, 2) == 0)
      {
        en_uploadState = StateIdle;
        en_poolState = OPSuccessfullyUploaded;
      }
      else
        getPoolForUpload();
    }
    else getPoolForUpload();
  }
}


/** if no structure label is found, choose a appropriate pool for upload
  */
void
DevPropertyHandler_c::getPoolForUpload()
{
  #ifdef USE_ISO_TERMINAL
  //if there are no local settings in ISOTerminal just take the default pool from the map
  if (getIsoTerminalInstance().getLocalSettings()->lastReceived != 0) {
    //get local language from ISOTerminal
    char pc_langCode [2];
    pc_langCode[0] = ((getIsoTerminalInstance().getLocalSettings()->languageCode) >> 8) & 0xFF;
    pc_langCode[1] = (getIsoTerminalInstance().getLocalSettings()->languageCode) & 0xFF;
    //compare with all stored pools -> take the first found pool
    std::map<LanguageLabel_c, DevicePool_c>::iterator it_maps;
    for (it_maps = map_deviceDescription.begin();it_maps !=map_deviceDescription.end(); it_maps++)
    {
      if (CNAMESPACE::strncmp(it_maps->first.get(), pc_langCode, 2) == 0)
      {
        pc_devPoolForUpload = &it_maps->second;
        return;
      }
    }
    //if no match (priority-list):
    //compare units
    for (it_maps = map_deviceDescription.begin();it_maps !=map_deviceDescription.end(); it_maps++)
    {
      //get all units from localization label
      if (getIsoTerminalInstance().getLocalSettings()->uDistance == (uint8_t)((it_maps->second.p_DevicePool[getLabelOffset(it_maps->second.p_DevicePool)+4] >> 6) & 0x3))
      {
        pc_devPoolForUpload = &it_maps->second;
        return;
      }
      if (getIsoTerminalInstance().getLocalSettings()->uArea == (uint8_t)((it_maps->second.p_DevicePool[getLabelOffset(it_maps->second.p_DevicePool)+4] >> 4) & 0x3))
      {
        pc_devPoolForUpload = &it_maps->second;
        return;
      }
      if (getIsoTerminalInstance().getLocalSettings()->uVolume == (uint8_t)((it_maps->second.p_DevicePool[getLabelOffset(it_maps->second.p_DevicePool)+4] >> 2) & 0x3))
      {
        pc_devPoolForUpload = &it_maps->second;
        return;
      }
      if (getIsoTerminalInstance().getLocalSettings()->uMass == (uint8_t)((it_maps->second.p_DevicePool[getLabelOffset(it_maps->second.p_DevicePool)+4] >> 2) & 0x3))
      {
        pc_devPoolForUpload = &it_maps->second;
        return;
      }
    }
    //compare date format
    for (it_maps = map_deviceDescription.begin();it_maps !=map_deviceDescription.end(); it_maps++)
    {
      if (getIsoTerminalInstance().getLocalSettings()->dFormat == (uint8_t)(it_maps->second.p_DevicePool[getLabelOffset(it_maps->second.p_DevicePool)+3] & 0xFF))
      {
        pc_devPoolForUpload = &it_maps->second;
        return;
      }
    }
    //compare time format
    for (it_maps = map_deviceDescription.begin();it_maps !=map_deviceDescription.end(); it_maps++)
    {
      if (getIsoTerminalInstance().getLocalSettings()->nTimeFormat == (uint8_t)((it_maps->second.p_DevicePool[getLabelOffset(it_maps->second.p_DevicePool)+2] >> 4) & 0x3))
      {
        pc_devPoolForUpload = &it_maps->second;
        return;
      }
    }
  }
  #endif
  //if no matching pool was found just take the default pool
  if (pc_devPoolForUpload == NULL) pc_devPoolForUpload = pc_devDefaultDeviceDescription;
}


/** send a request for Object Pool Transfer
  */
void
DevPropertyHandler_c::startUpload()
{
  //estimate size of bytestream
  uint32_t ui32_byteStreamLength = pc_devPoolForUpload->devicePoolLength;
  pc_data->setExtCanPkg8 (3, 0, 203, tcSourceAddress, pc_wsMasterIdentItem->getIsoItem()->nr(),
                            procCmdPar_RequestOPTransferMsg, (ui32_byteStreamLength & 0xff), (ui32_byteStreamLength >> 8) & 0xff, (ui32_byteStreamLength >> 16) & 0xff, 0xff, 0xff, 0xff, 0xff);
  getCanInstance4Comm() << *pc_data;
  ui32_uploadTimestamp = HAL::getTime();
  ui32_uploadTimeout = DEF_TimeOut_OPTransfer;

  en_uploadState = StateUploadPool;
  en_uploadStep = UploadWaitForRequestOPTransferResponse;
  #ifdef DEBUG
  EXTERNAL_DEBUG_DEVICE << "Wait for response for request OP transfer..." << EXTERNAL_DEBUG_DEVICE_ENDL;
  #endif
}


/** if a pool couldn't be uploaded, register error in IsoTerminal and set the necessary states
  */
void
DevPropertyHandler_c::outOfMemory()
{ // can't (up)load the pool.
  getLbsErrInstance().registerError( LibErr_c::IsoTerminalOutOfMemory, LibErr_c::IsoTerminal );
  en_uploadStep = UploadFailed; // no timeout needed
  en_poolState = OPCannotBeUploaded;
  #ifdef DEBUG
  EXTERNAL_DEBUG_DEVICE << "upload failed, no retry" << EXTERNAL_DEBUG_DEVICE_ENDL;
  #endif
}


/** put a new change designator command in the send-queue
    @return true, if command was queued
  */
bool
DevPropertyHandler_c::sendCommandChangeDesignator(uint16_t rui16_objectID, const char* rpc_newString, uint8_t stringLength)
{
  // if string is shorter than length, it's okay to send - if it's longer, we'll clip - as client will REJECT THE STRING
  uint8_t strLen = (CNAMESPACE::strlen(rpc_newString) < stringLength) ? CNAMESPACE::strlen(rpc_newString) : stringLength;
  if (CNAMESPACE::strlen(rpc_newString) <= 32)
  {
    l_sendUpload.push_back(SendUploadBase_c (rui16_objectID, rpc_newString, strLen, procCmdPar_ChangeDesignatorMsg));
    return true;
  }
  //DEBUG OUT
  //std::cout << "New Designator value exceeds a length of 32 characters! Choose a shorter name!"
  return false;
}


/** start Upload from the change designator value
    decide either using a single CAN Pkg or the mulitsendstreamer for strings longer than 4 characters
  */
void
DevPropertyHandler_c::startUploadCommandChangeDesignator()
{
  // Set new state
  en_uploadState = StateUploadCommand;

  // Get first element from queue
  SendUploadBase_c* actSend = &l_sendUpload.front();

  // Set time-out values
  ui32_uploadTimeout = actSend->ui32_uploadTimeout;
  ui32_uploadTimestamp = HAL::getTime();

  if ((actSend->vec_uploadBuffer.size() < 9)) {
    /// Fits into a single CAN-Pkg!
    pc_data->setExtCanPkg8 (3, 0, 203, tcSourceAddress, pc_wsMasterIdentItem->getIsoItem()->nr(),
                          actSend->vec_uploadBuffer [0], actSend->vec_uploadBuffer [1],
                          actSend->vec_uploadBuffer [2], actSend->vec_uploadBuffer [3],
                          actSend->vec_uploadBuffer [4], actSend->vec_uploadBuffer [5],
                          actSend->vec_uploadBuffer [6], actSend->vec_uploadBuffer [7]);
    getCanInstance4Comm() << *pc_data;
    en_uploadCommand = UploadCommandWaitingForCommandResponse;
  }
  else
  {
    /// Use multi CAN-Pkgs [(E)TP], doesn't fit into a single CAN-Pkg!
    getMultiSendInstance().sendIsoTarget(pc_wsMasterIdentItem->getIsoItem()->nr(), tcSourceAddress,
           &actSend->vec_uploadBuffer.front(), actSend->vec_uploadBuffer.size(), PROCESS_DATA_PGN, en_sendSuccess);
    en_uploadCommand = UploadMultiSendCommandWaitingForCommandResponse;
  }
}


/** finish the upload of a single command and delete it from the queue
  */
void
DevPropertyHandler_c::finishUploadCommandChangeDesignator()
{
  en_uploadState = StateIdle;
  en_uploadStep = UploadNone;

  //delete first processed cmd of the queue
  l_sendUpload.pop_front();
}

};
