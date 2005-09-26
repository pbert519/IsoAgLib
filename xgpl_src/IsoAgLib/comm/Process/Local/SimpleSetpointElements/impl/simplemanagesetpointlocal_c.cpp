/***************************************************************************
                          simplemanagesetpointlocal_c.cpp - managing of simple
                                                       local process data object
                             -------------------
    begin                : Fri Apr 07 2000
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
 * - Call of a (member-) function                                          *
 * - Instantiation of a variable in a datatype from internal namespace     *
 * Allowed is:                                                             *
 * - Instatiation of a variable in a datatype from interface namespace,    *
 *   even if this is derived from a base class inside an internal namespace*
 * - Call of member functions which are defined in the interface class     *
 *   definition ( header )                                                 *
 * Pairing of internal and interface classes:                              *
 * - Internal implementation in an :impl: subdirectory                     *
 * - Interface in the parent directory of the corresponding internal class *
 * - Interface class name IsoAgLib::iFoo_c maps to the internal class      *
 *   __IsoAgLib::Foo_c                                                     *
 *                                                                         *
 * AS A RULE: Use only classes with names beginning with small letter :i:  *
 ***************************************************************************/

/* *************************************** */
/* ********** include headers ************ */
/* *************************************** */
#include "simplemanagesetpointlocal_c.h"
#include "../../../impl/process_c.h"
#include "../../../processdatachangehandler_c.h"

namespace __IsoAgLib {

// **************************************************************************************
// *************************** SimpleManageSetpointLocal_c ***************************
// **************************************************************************************


/**
  initialise this SimpleManageSetpointLocal_c instance to a well defined initial state
  @param rpc_processData optional pointer to containing ProcessData instance
*/
void SimpleManageSetpointLocal_c::init( ProcDataBase_c *const rpc_processData )
{
  ProcessElementBase_c::set( rpc_processData );
  #if !defined(HANDLE_SETPOINT_MEASURE_EQUIVALENT)
    i32_setpointMasterVal = 0;
  #endif
}
/** copy constructor */
SimpleManageSetpointLocal_c::SimpleManageSetpointLocal_c( const SimpleManageSetpointLocal_c& rrefc_src )
: ProcessElementBase_c( rrefc_src )
{
  #if !defined(HANDLE_SETPOINT_MEASURE_EQUIVALENT)
    i32_setpointMasterVal = rrefc_src.i32_setpointMasterVal;
  #endif
}
/** assignment operator */
const SimpleManageSetpointLocal_c& SimpleManageSetpointLocal_c::operator=( const SimpleManageSetpointLocal_c& rrefc_src )
{
  ProcessElementBase_c::operator=( rrefc_src );
  #if !defined(HANDLE_SETPOINT_MEASURE_EQUIVALENT)
    i32_setpointMasterVal = rrefc_src.i32_setpointMasterVal;
  #endif

  return *this;
}
/** process a setpoint message */
void SimpleManageSetpointLocal_c::processSetpoint(){
  // for simple setpoint the message is process here
  ProcessPkg_c& c_pkg = getProcessInstance4Comm().data();
  const DevKey_c& cc_senderDevKey = c_pkg.memberSend().devKey();
  // DIN pd=0
  if (c_pkg.c_generalCommand.getCommand() == GeneralCommand_c::setValue)
  { // setpoint set
    bool b_change = false;
    switch (c_pkg.c_generalCommand.getValueGroup())
    {
      case GeneralCommand_c::exactValue: // set actual setpoint
      case GeneralCommand_c::minValue: // min -> simply set exact
      case GeneralCommand_c::maxValue: // max -> simply set exact
      case GeneralCommand_c::defaultValue: // max -> simply set exact
        #ifdef USE_FLOAT_DATA_TYPE
        if ( ( c_pkg.valType() == float_val)
          && ( setpointMasterValFloat() != c_pkg.dataFloat() ) ) {
          setSetpointMasterVal(c_pkg.dataFloat());
          b_change = true;
        }
        else if ( setpointMasterVal() != c_pkg.dataLong() ) {
        #else
        if ( setpointMasterVal() != c_pkg.dataLong() ) {
        #endif
          setSetpointMasterVal(c_pkg.dataLong());
          b_change = true;
        }
        break;
    }
    // call handler function if handler class is registered
    if ( processDataConst().getProcessDataChangeHandler() != NULL )
      processDataConst().getProcessDataChangeHandler()->processSetpointSet( pprocessData(), c_pkg.dataLong(), c_pkg.memberSend().devKey(), b_change );
  }
  #ifndef SIMPLE_RESPOND_ON_SET
  // if no auto-response on setpoint set is want
  // send MOD dependent value on request
  // if SIMPLE_RESPOND_ON_SET is defined, the actual setpoint shall be resend
  // on receive of a new setpoint --> "else" is NOT used in this case
  else
  #endif
  {
    sendSetpointMod(cc_senderDevKey, Proc_c::progType_t( c_pkg.pri() ),
                    c_pkg.c_generalCommand.getValueGroup(), GeneralCommand_c::setValue );
  }

}

/**
  send a sub-setpoint (selected by MOD) to a specified target (selected by GPT)
  @param rui8_mod select sub-type of setpoint
  @param rc_targetDevKey DevKey of target
  @param ren_type optional PRI specifier of the message (default Proc_c::Target )
  @param en_valueGroup: min/max/exact/default
  @param en_command
  @return true -> successful sent
*/
bool SimpleManageSetpointLocal_c::sendSetpointMod(const DevKey_c& rc_targetDevKey,
                                                  Proc_c::progType_t ren_progType,
                                                  GeneralCommand_c::ValueGroup_t en_valueGroup,
                                                  GeneralCommand_c::CommandType_t en_command ) const {
  // prepare general command in process pkg
  getProcessInstance4Comm().data().c_generalCommand.setValues(true /* isSetpoint */, false, /* isRequest */
                                                              en_valueGroup, en_command);
  //if ( rui8_mod != 1 ) {
    // not percent
    // DIN: pd=0, mod=rui8_mod
    #ifdef USE_FLOAT_DATA_TYPE
    if (valType() == float_val)
      return processDataConst().sendValDevKey(ren_progType, rc_targetDevKey, setpointMasterValFloat());
    else
    #endif
      return processDataConst().sendValDevKey(ren_progType, rc_targetDevKey, setpointMasterVal());
  //}
}


/**
  retreive simple master setpoint
  @return actual received setpoint value
*/
int32_t SimpleManageSetpointLocal_c::setpointMasterVal() const
{
#ifndef HANDLE_SETPOINT_MEASURE_EQUIVALENT
return i32_setpointMasterVal;
#else // HANDLE_SETPOINT_MEASURE_EQUIVALENT
ProcDataLocalBase_c& c_localProcBase = static_cast<ProcDataLocalBase_c&>(processData());
uint32_t ui32_masterVal = c_localProcBase.masterVal();
return i32_masterVal;
#endif // HANDLE_SETPOINT_MEASURE_EQUIVALENT
}
  #ifdef USE_FLOAT_DATA_TYPE
/**
  retreive simple master setpoint
  @return actual received setpoint value
*/
float SimpleManageSetpointLocal_c::setpointMasterValFloat() const
{
#ifndef HANDLE_SETPOINT_MEASURE_EQUIVALENT
return f_setpointMasterVal;
#else // HANDLE_SETPOINT_MEASURE_EQUIVALENT
ProcDataLocalBase_c& c_localProcBase = static_cast<ProcDataLocalBase_c&>(processData());
float f_masterVal = c_localProcBase.masterValFloat();
return f_masterVal;
#endif // HANDLE_SETPOINT_MEASURE_EQUIVALENT
}
  #endif // USE_FLOAT_DATA_TYPE

/**
  set the setpoint value
  @param ri32_val new setpoint value
*/
void SimpleManageSetpointLocal_c::setSetpointMasterVal(int32_t ri32_val)
{
#ifndef HANDLE_SETPOINT_MEASURE_EQUIVALENT
processData().setValType(i32_val);
i32_setpointMasterVal = ri32_val;
#else // HANDLE_SETPOINT_MEASURE_EQUIVALENT
ProcDataLocalBase_c& c_localProcBase = static_cast<ProcDataLocalBase_c&>(processData());
c_localProcBase.setMasterVal( ri32_val );
#endif // HANDLE_SETPOINT_MEASURE_EQUIVALENT
}
#ifdef USE_FLOAT_DATA_TYPE
/**
  set the setpoint value as float value
  @param rf_val new setpoint value
*/
void SimpleManageSetpointLocal_c::setSetpointMasterVal(float rf_val)
{
#ifndef HANDLE_SETPOINT_MEASURE_EQUIVALENT
processData().setValType(float_val);
f_setpointMasterVal = rf_val;
#else // HANDLE_SETPOINT_MEASURE_EQUIVALEN
ProcDataLocalBase_c& c_localProcBase = static_cast<ProcDataLocalBase_c&>(processData());
c_localProcBase.setMasterVal( rf_val );
#endif // HANDLE_SETPOINT_MEASURE_EQUIVALEN
}
#endif // USE_FLOAT_DATA_TYPE


} // end of namespace __IsoAgLib
