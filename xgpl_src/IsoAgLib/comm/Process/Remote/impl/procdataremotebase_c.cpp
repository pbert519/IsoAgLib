/***************************************************************************
                          procdataremotebase_c.cpp - managing of remote
                                                       process data object
                             -------------------
    begin                : Fri Apr 07 2000
    copyright            : (C) 2000 - 2004 by Dipl.-Inform. Achim Spangler
    email                : a.spangler@osb-ag:de
    type                 : Header
    $LastChangedDate$
    $LastChangedRevision$
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

/* *************************************** */
/* ********** include headers ************ */
/* *************************************** */
#include "procdataremotebase_c.h"
#include "../../impl/process_c.h"

namespace __IsoAgLib {

/** initialise this ProcDataRemoteBase_c instance to a well defined initial state
    ISO parameter
    @param ps_elementDDI optional pointer to array of structure IsoAgLib::ElementDDI_s which contains DDI, element, isSetpoint and ValueGroup
                         (array is terminated by ElementDDI_s.ui16_element == 0xFFFF)
    common parameter
    @param rc_isoName optional ISOName code of this instance
    @param rc_ownerISOName optional ISOName of the owner
    @param rpc_commanderISOName pointer to updated ISOName variable of commander
    @param rpc_processDataChangeHandler optional pointer to handler class of application
    @param ri_singletonVecKey optional key for selection of IsoAgLib instance (default 0)
  */
void ProcDataRemoteBase_c::init(  const IsoAgLib::ElementDDI_s* ps_elementDDI, uint16_t rui16_element,
                                  const ISOName_c& rc_isoName, const ISOName_c& rc_ownerISOName,
                                  const ISOName_c* rpc_commanderISOName,
                                  IsoAgLib::ProcessDataChangeHandler_c *rpc_processDataChangeHandler,
                                  int ri_singletonVecKey)
{
  ProcDataBase_c::init( ps_elementDDI, rui16_element,
                        rc_isoName, rc_ownerISOName, NULL, rpc_processDataChangeHandler);

  setSingletonKey( ri_singletonVecKey );
  setCommanderISOName(rpc_commanderISOName);

  getProcessInstance4Comm().registerRemoteProcessData( this );
}

/** assignment operator for this object
  @param rrefc_src source instance
  @return reference to source instance for cmd like "prog1 = prog2 = prog3;"
*/
const ProcDataRemoteBase_c& ProcDataRemoteBase_c::operator=(const ProcDataRemoteBase_c& rrefc_src){
  // call the assignment operator for the base class
  ProcDataBase_c::operator=(rrefc_src);
  pc_isoName = rrefc_src.pc_isoName;
  // return reference to source
  return *this;
}

/** copy constructor for IsoAgLibProcDataRemote
  @param rrefc_src source instance
*/
ProcDataRemoteBase_c::ProcDataRemoteBase_c(const ProcDataRemoteBase_c& rrefc_src)
  : ProcDataBase_c(rrefc_src)
{ // now copy the element var
  pc_isoName = rrefc_src.pc_isoName;

  // now register the pointer to this instance in Process_c
  getProcessInstance4Comm().registerRemoteProcessData( this );
}

/** default destructor which has nothing to do */
ProcDataRemoteBase_c::~ProcDataRemoteBase_c(){
  // call unregisterRemoteProcessData in last derived class because unregister does again message processing!
}

/** set the pointer to the commander ident isoName
  @param rpbisoName pointer to ISOName var of local member used for
              sending commands to remote owner member
*/
void ProcDataRemoteBase_c::setCommanderISOName(const ISOName_c* rpc_isoName)
{
    pc_isoName = rpc_isoName;
}

/** perform periodic actions
  ProcDataRemoteBase_c::timeEvent
  -> adapt here the ownerISOName to an existing item, when DevClass/-Instance are matching, but the other fields are
     differen ( don't change anything, if there is an item with identic ISOName setting
  @return true -> all planned executions performed
*/
bool ProcDataRemoteBase_c::timeEvent( void )
{
  return true;
}

bool ProcDataRemoteBase_c::sendValISOName(const ISOName_c& rc_varISOName, int32_t ri32_val) const
{
  setRemoteSendFlags (rc_varISOName);

  return ProcDataBase_c::sendValISOName (rc_varISOName, ri32_val);
}

#ifdef USE_FLOAT_DATA_TYPE
bool ProcDataRemoteBase_c::sendValISOName(const ISOName_c& rc_varISOName, float rf_val) const
{
  setRemoteSendFlags (rc_varISOName);

  return ProcDataBase_c::sendValISOName (rc_varISOName, rf_val);
}
#endif

void ProcDataRemoteBase_c::setRemoteSendFlags(const ISOName_c& rc_varISOName) const
{
  ProcessPkg_c& c_data = getProcessPkg();

  c_data.setISONameForDA(ownerISOName());
  c_data.setISONameForSA(rc_varISOName);
}

} // end of namespace __IsoAgLib
