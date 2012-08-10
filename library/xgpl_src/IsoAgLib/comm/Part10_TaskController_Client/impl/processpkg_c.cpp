/*
  processpkg_c.cpp: data object for Process-Data messages

  (C) Copyright 2009 - 2012 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/

/* *************************************** */
/* ********** include headers ************ */
/* *************************************** */
#include "processpkg_c.h"
#include <IsoAgLib/driver/can/impl/ident_c.h>
#include <IsoAgLib/driver/system/impl/system_c.h>
#include <IsoAgLib/util/impl/util_funcs.h>
#include "proc_c.h"

namespace __IsoAgLib {

ProcessPkg_c::ProcessPkg_c( const ProcessPkg_c& arc_src )
  : CanPkgExt_c( arc_src ),
    mc_processCmd( arc_src.mc_processCmd ),
    mi32_pdValue( arc_src.mi32_pdValue ),
    bit_data( arc_src.bit_data )
{
}

ProcessPkg_c::ProcessPkg_c( const CanPkg_c& arc_src, int ai_multitonInst )
  : CanPkgExt_c( arc_src, ai_multitonInst )
  , mc_processCmd()
  , mi32_pdValue( 0 )
  , bit_data()
{
  // call string2flags function that was done formerly in the filterbox
  string2Flags();
}

/** default constructor which has nothing to do */
ProcessPkg_c::ProcessPkg_c( int /*ai_multitonInst*/ )
  : CanPkgExt_c( )
  , mc_processCmd()
  , mi32_pdValue( 0 )
  , bit_data()
{
}

/** default constructor which has nothing to do */
ProcessPkg_c::~ProcessPkg_c(){
}

/**
  check if the 4byte value of the message has a special command of type
  proc_specCmd_t: setpointReleaseCmd, setpointErrCmd, noVal_32s, errVal_32s
  (extension to ISO specification)
  @param ren_checkCmd special command to check for
  @return true -> the given cmd was received
*/
bool ProcessPkg_c::isSpecCmd(proc_specCmd_t ren_checkCmd)const
{
  bool b_result = false;

  const int32_t ci32_test = mi32_pdValue;

  if (mc_processCmd.getCommand() == ProcessCmd_c::setValue &&
      mc_processCmd.checkIsSetpoint())
  { // setpoint value -> special commands are possible for exact, min, max, default setpopints
      if (((ren_checkCmd & setpointReleaseCmd) != 0)
       &&(ci32_test == static_cast<int32_t>(SETPOINT_RELEASE_COMMAND)))
      {
        b_result = true;
      }
      else
      {
        if (((ren_checkCmd & setpointErrCmd) != 0)
         && (ci32_test == static_cast<int32_t>(SETPOINT_ERROR_COMMAND))) b_result = true;
      }
  }

  if (mc_processCmd.getCommand() == ProcessCmd_c::setValue &&
      !mc_processCmd.checkIsSetpoint())
  { // measure value: conversion if: actual, min, max, integ, med
    // check for command values
    if (((ren_checkCmd & noVal_32s) != 0)
      &&(ci32_test == static_cast<int32_t>(NO_VAL_32S)))
    {
      b_result = true;
    }
    else
    {
      if (((ren_checkCmd & errVal_32s) != 0)
      && (ci32_test == static_cast<int32_t>(ERROR_VAL_32S))) b_result = true;
    }
  }
  return b_result;
}


/**
  set one of the special commands of type proc_specCmd_t:
  setpointReleaseCmd, setpointErrCmd, noVal_32s, errVal_32s
  with specified data type specifying flag in CAN data string
  @param ren_procSpecCmd special command to send
*/
void ProcessPkg_c::setData(proc_specCmd_t ren_procSpecCmd)
{
  switch (ren_procSpecCmd)
  {
    case setpointReleaseCmd:
      mi32_pdValue = SETPOINT_RELEASE_COMMAND;
      break;

    case setpointErrCmd:
      mi32_pdValue = SETPOINT_ERROR_COMMAND;
      break;

    case noVal_32s:
      mi32_pdValue = NO_VAL_32S;
      break;

    case errVal_32s:
      mi32_pdValue = ERROR_VAL_32S;
      break;
  }
}


/**
  overloaded virtual function to translate the string data into flag values;
  needed for assigning informations from another CanPkg_c or CANPkgExt
  @see CanPkg_c::operator=
  @see CanPkgExt_c::operator=
*/
void ProcessPkg_c::string2Flags()
{
  set_Cmd(mc_data[0] & 0xf);
  uint16_t mui16_element = 0;
  mui16_element = uint16_t(mc_data[1]) << 4;
  mui16_element |= ((mc_data[0] & 0xF0)>>4);
  set_Element(mui16_element);

  uint16_t newDDI = 0;
  newDDI |= mc_data[3];
  newDDI = newDDI << 8;
  newDDI |= mc_data[2];
  set_DDI(newDDI);

  mi32_pdValue = mc_data.getInt32Data(4);
};

/**
  overloaded virtual function to translate flag values to data string;
  needed for sending informations from this object via CanIo_c on CAN BUS,
  because CanIo_c doesn't know anything about the data format of this type of msg
  so that it can only use an unformated data string from CANPkg
  @see CanPkg_c::getData
  @see CanPkgExt_c::getData
*/
void ProcessPkg_c::flags2String()
{
  uint8_t ui8_cmd;
  switch (mc_processCmd.getCommand()) {
    case ProcessCmd_c::requestConfiguration:                  ui8_cmd = 0; break;
    case ProcessCmd_c::configurationResponse:                 ui8_cmd = 1; break;
    case ProcessCmd_c::requestValue:                          ui8_cmd = 2; break;
    case ProcessCmd_c::setValue:                              ui8_cmd = 3; break;
    case ProcessCmd_c::measurementTimeValueStart:             ui8_cmd = 4; break;
    case ProcessCmd_c::measurementDistanceValueStart:         ui8_cmd = 5; break;
    case ProcessCmd_c::measurementMinimumThresholdValueStart: ui8_cmd = 6; break;
    case ProcessCmd_c::measurementMaximumThresholdValueStart: ui8_cmd = 7; break;
    case ProcessCmd_c::measurementChangeThresholdValueStart:  ui8_cmd = 8; break;
    case ProcessCmd_c::nack:                                  ui8_cmd = 0xd; break;
    case ProcessCmd_c::taskControllerStatus:                  ui8_cmd = 0xe; break;
    case ProcessCmd_c::workingsetMasterMaintenance:           ui8_cmd = 0xf; break;
    // map reset command to setValue command
    case ProcessCmd_c::measurementReset:                      ui8_cmd = 3; break;
    default: ui8_cmd = 0xFF;
  }

  mc_data.setUint8Data(0, (ui8_cmd & 0xf) | ( (element() & 0xf) << 4) );
  mc_data.setUint8Data(1, element() >> 4 );
  mc_data.setUint8Data(2, DDI() & 0x00FF );
  mc_data.setUint8Data(3, (DDI()& 0xFF00) >> 8 );
  // for ISO the ident is directly read and written

  mc_data.setInt32Data(4, mi32_pdValue);

  setLen(8);
};


/**
  extract data from ISO commands and save it to member class
  @param rl_elementDDI
*/
bool ProcessPkg_c::resolveCommandTypeForISO(uint16_t aui16_ddi)
{
  bool b_isSetpoint = false;
  bool b_isRequest = false;
  ProcessCmd_c::ValueGroup_t en_valueGroup = ProcessCmd_c::noValue;
  ProcessCmd_c::CommandType_t en_command = ProcessCmd_c::noCommand;

  if ( identType() != Ident_c::StandardIdent) {
    // ISO command
    switch (cmd()) {
      case 0x00:
        en_command = ProcessCmd_c::requestConfiguration;
        break;
      case 0x01:
        en_command = ProcessCmd_c::configurationResponse;
        break;
      case 0x02:
        en_command = ProcessCmd_c::requestValue;
        b_isRequest = true;
        break;
      case 0x03:
        en_command = ProcessCmd_c::setValue;
        break;
      case 0x04:
        en_command = ProcessCmd_c::measurementTimeValueStart;
        break;
      case 0x05:
        en_command = ProcessCmd_c::measurementDistanceValueStart;
        break;
      case 0x06:
        en_command = ProcessCmd_c::measurementMinimumThresholdValueStart;
        break;
      case 0x07:
        en_command = ProcessCmd_c::measurementMaximumThresholdValueStart;
        break;
      case 0x08:
        en_command = ProcessCmd_c::measurementChangeThresholdValueStart;
        break;
      case 0x0d:
        en_command = ProcessCmd_c::nack;
        break;
      case 0x0e:
        en_command = ProcessCmd_c::taskControllerStatus;
        break;
      case 0x0f:
        en_command = ProcessCmd_c::workingsetMasterMaintenance;
        break;
    }
  }

#if CHECK_IF_REMOVE_NECESSARY
  if ( aui16_ddi == DDI() )
  {
    b_isSetpoint = rl_elementDDI.b_isSetpoint;
    en_valueGroup = rl_elementDDI.en_valueGroup;
  }
#endif

  if (en_command != ProcessCmd_c::noCommand)
  {
    mc_processCmd.setValues(b_isSetpoint, b_isRequest, en_valueGroup, en_command);
    return true;
  }
  else return false;
}

} // end of namespace __IsoAgLib
