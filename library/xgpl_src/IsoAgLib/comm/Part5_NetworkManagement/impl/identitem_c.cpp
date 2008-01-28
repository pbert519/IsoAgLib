/***************************************************************************
                          identitem_c.cpp - object for managing identity/ies
                                          of the ECU
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
#include <IsoAgLib/typedef.h>
#include "identitem_c.h"
#include "../impl/isomonitor_c.h"
#include "../impl/isorequestpgn_c.h"
#if !defined(IGNORE_VTSERVER_NACK) && defined(USE_ISO_TERMINAL)
  #include "../../Part6_VirtualTerminal_Client/impl/isoterminal_c.h"
#endif

#ifdef USE_EEPROM_IO
#include <IsoAgLib/driver/eeprom/impl/eepromio_c.h>
#endif

#include <IsoAgLib/comm/Scheduler/impl/scheduler_c.h>
#include <IsoAgLib/driver/system/impl/system_c.h>
#include <IsoAgLib/driver/can/impl/canio_c.h>
#include <IsoAgLib/util/iliberr_c.h>


namespace __IsoAgLib {

/** A) default constructor:  will not do anything, create a not-yet-initialized identity. use "init" afterwards!
  * B) EEPROM address given: will read WantedSA/ISOName from EEPROM and start address claim for this identity
  * @param aui16_eepromAdr Address of EEPROM where the following 10 bytes have to be stored stored:
                            8 byte ISO-Name stored in LITTLE ENDIAN
                            1 byte preferred/last-used SA (0xFE for no preference)
                            1 byte flags, set to 0x00 initially (so the ISO-Name has a chance to change some of its
                                  instance(s) to avoid ISO-Name-conflicts on the bus BEFORE doing its initial address-claim
    @param ai_singletonVecKey optional key for selection of IsoAgLib instance, defaults to 0 at construction time!
  */
IdentItem_c::IdentItem_c (uint16_t aui16_eepromAdr, int ai_singletonVecKey)
  : BaseItem_c (System_c::getTime(), IState_c::IstateNull, ai_singletonVecKey)
  , mpc_isoItem (NULL)
  , mui16_eepromAdr (aui16_eepromAdr)
  , mui8_globalRunState (GlobalRunStateNeverClaimed)
#ifdef USE_WORKING_SET
  , mpvec_slaveIsoNames (NULL)
#endif
{
  init (NULL, 0xFF, aui16_eepromAdr,
#ifdef USE_WORKING_SET
        -1, NULL, // -1 indicates we're no working-set!
#endif
        ai_singletonVecKey);
}



/** constructor for ISO identity, which starts address claim for this identity
    @param aui8_indGroup        select the industry group, 2 == agriculture
    @param aui8_devClass	device class identifier
    @param aui8_devClassInst    instance of device class
    @param ab_func              function code of the member (25 = network interconnect)
    @param aui16_manufCode      11bit manufactor code
    @param aui32_serNo          21bit serial number
    @param aui8_preferredSa     preferred source adress (SA) of the ISO item (fixed SA or last time
                                SA for self conf ISO device) (default 254 for no special wish)
    @param aui16_eepromAdr      EEPROM adress, where the used IsoName / SA / flags are stored
                                (default 0xFFFF for NO EEPROM store)
    @param ab_funcInst          function instance of this member (default 0)
    @param ab_ecuInst           ECU instance of this member (default 0)
    @param ab_selfConf          true -> this member as a self configurable source adress
    @param ai8_slaveCount       amount of attached slave devices; default -1 == no master state;
                                in case an address claim for the slave devices shall be sent by this ECU, they
                                must get their own IdentItem_c instance ( then with default value -1 for ai8_slaveCount )
    @param apc_slaveIsoNameList pointer to list of IsoName_c values, where the slave devices are defined.
                                IsoAgLib will then send the needed "master indicates its slaves" messages on BUS
    @param ai_singletonVecKey   optional key for selection of IsoAgLib instance (default 0)
  */
IdentItem_c::IdentItem_c (uint8_t aui8_indGroup, uint8_t aui8_devClass, uint8_t aui8_devClassInst,
  uint8_t ab_func, uint16_t aui16_manufCode, uint32_t aui32_serNo, uint8_t aui8_preferredSa, uint16_t aui16_eepromAdr,
  uint8_t ab_funcInst, uint8_t ab_ecuInst, bool ab_selfConf,
  #ifdef USE_WORKING_SET
  int8_t ai8_slaveCount, const IsoName_c* apc_slaveIsoNameList,
  #endif
  int ai_singletonVecKey)
  : BaseItem_c (System_c::getTime(), IState_c::IstateNull, ai_singletonVecKey) /// needs to be init'ed, so double "init()" can be detected!
  , mpc_isoItem (NULL)
  , mui8_globalRunState (GlobalRunStateNeverClaimed)
#ifdef USE_WORKING_SET
  , mpvec_slaveIsoNames (NULL)
#endif
{
  init (aui8_indGroup, aui8_devClass, aui8_devClassInst, ab_func, aui16_manufCode, aui32_serNo,
        aui8_preferredSa, aui16_eepromAdr, ab_funcInst, ab_ecuInst, ab_selfConf,
        #ifdef USE_WORKING_SET
        ai8_slaveCount, apc_slaveIsoNameList,
        #endif
        ai_singletonVecKey);
}


// private
void IdentItem_c::init (IsoName_c* apc_isoNameParam, uint8_t aui8_preferredSa, uint16_t aui16_eepromAdr,
  #ifdef USE_WORKING_SET
  int8_t ai8_slaveCount, const IsoName_c* apc_slaveIsoNameList,
  #endif
  int ai_singletonVecKey)
{
  /// Check if Item was already ACTIVE (detect double init()-call!)
  if (itemState (IState_c::Active))
  { // For init again, you'd first have to stop the identity - this feature is to come somewhen (when needed)...
    getILibErrInstance().registerError( iLibErr_c::Precondition, iLibErr_c::System );
    #if defined(DEBUG) && defined(SYSTEM_PC)
    INTERNAL_DEBUG_DEVICE << "ERROR: Double initialization of IdentItem_c detected!!!!" << INTERNAL_DEBUG_DEVICE_ENDL;
    abort();
    #else
    return;
    #endif
  }

  // Do start up the complete system first (if not yet done - call is idempotent!, so okay to do here!)
  getSchedulerInstance4Comm().startSystem();

  // set all other member variables depending on the EEPROM-Address parameter/member variable
  mui16_eepromAdr = aui16_eepromAdr;

  /// Default to SetActive=TRUE. This'll be just set FALSE when initialization is postponed by the user!
  bool b_setActive = true;

  bool b_useParameters;
  if (aui16_eepromAdr == 0xFFFF)
  { // Not using EEPROM
    b_useParameters = true;
  }
  else
  { // Using EEPROM
   #ifdef USE_EEPROM_IO
    /// FIRST, default to EEPROM values
    EepromIo_c& rc_eeprom = getEepromInstance();
    uint8_t p8ui8_isoNameEeprom [8];
    rc_eeprom.setg (mui16_eepromAdr);
    rc_eeprom >> mui8_globalRunState >> mui8_preferredSa;
    rc_eeprom.readString (p8ui8_isoNameEeprom, 8);
    mc_isoName = IsoName_c (p8ui8_isoNameEeprom);

#ifdef DEBUG_NETWORK_MANAGEMENT
      INTERNAL_DEBUG_DEVICE << "Read global run state " << (int)mui8_globalRunState << " from EEPROM.";
#endif

    // use fallback to free definition, when the EEPROM has only invalid SA
    if ( mui8_preferredSa == 0xFF ) mui8_preferredSa = 0xFE;

    if (apc_isoNameParam == NULL)
    { // no parameter given, so don't even try to figure out if we'd want to use them!
      b_useParameters = false;
    }
    else
    { /// SECOND, decide on the GlobalRunState and validity of the EEPROM values
      if (mui8_globalRunState == GlobalRunStateNeverClaimed)
      { // FIRST ECU power-up, try with given program parameters (eeprom is only for storage of claimed iso-name!)
        b_useParameters = true;

#ifdef DEBUG_NETWORK_MANAGEMENT
      INTERNAL_DEBUG_DEVICE << " GlobalRunStateNeverClaimed" << INTERNAL_DEBUG_DEVICE_ENDL;
#endif
      }
      else if (mui8_globalRunState == GlobalRunStateAlreadyClaimed)
      { // FURTHER ECU power-up, use claimed name stored in EEPROM
        // but only if not a new firmware was flashed with new ISO-Name and the EEPROM was NOT reset!

#ifdef DEBUG_NETWORK_MANAGEMENT
      INTERNAL_DEBUG_DEVICE << " GlobalRunStateAlreadyClaimed" << INTERNAL_DEBUG_DEVICE_ENDL;
#endif

        if (mc_isoName.isEqualRegardingNonInstFields (*apc_isoNameParam))
        { // no firmware change, so go ahead!
          b_useParameters=false; // use EEPROM values - fine...
        }
        else
        { // firmware changed, we have different parameter-IsoName as in EEPROM, so use the parameters instead!
          b_useParameters=true; // override with most likely newer firmware(=parameter)
        }
      }
      else
      { // Illegal value in EEPROM
        b_useParameters=true;

#ifdef DEBUG_NETWORK_MANAGEMENT
      INTERNAL_DEBUG_DEVICE << " Illegal value in EEPROM." << INTERNAL_DEBUG_DEVICE_ENDL;
#endif
      }
    }
   #else
    // ERROR: Using EEPROM Address but IsoAgLib is NOT compiled with USE_EEPROM_IO !!!!
    getILibErrInstance().registerError( iLibErr_c::ElNonexistent, iLibErr_c::Eeprom );
    #if defined(DEBUG) && defined(SYSTEM_PC)
    INTERNAL_DEBUG_DEVICE << "ERROR: Using EEPROM Address in IdentItem_c() construction but IsoAgLib is NOT compiled with USE_EEPROM_IO !!!!" << INTERNAL_DEBUG_DEVICE_ENDL;
    abort();
    #else
    b_useParameters = true; // fallback because we can't read out the eeprom!
    #endif
   #endif
  }

  /// THIRD, use parameters if SECOND told us to do OR we were running without EEPROM anyway
  if (b_useParameters)
  {
    if (apc_isoNameParam == NULL)
    { /// NO Parameter-IsoName is given AND NO EEPROM address given, so initialize this IdentItem empty to be initialized later with "init"
      mc_isoName.setUnspecified();
      b_setActive = false; // only case where we don't start the address-claim procedure!
    }
    else
    { /// Parameter-IsoName is given and should be used: use it!
      mc_isoName = *apc_isoNameParam;
    }
    mui8_preferredSa = aui8_preferredSa; // 0xFF in case of "mc_isoName = IsoName_c::IsoNameUnspecified;"
    mui8_globalRunState = GlobalRunStateNeverClaimed; // 0
  }

#ifdef USE_WORKING_SET
  /// store SLAVE ISONAMEs
  if (mpvec_slaveIsoNames)
  { // first we need to remove the old ones
    delete mpvec_slaveIsoNames;
    mpvec_slaveIsoNames = NULL;
  }
  if (ai8_slaveCount >= 0)
  { // we're workingset-master!
    mpvec_slaveIsoNames = new STL_NAMESPACE::vector<IsoName_c>();
    // which is indicated by "mpvec_slaveIsoNames != NULL"
    if (ai8_slaveCount > 0)
    { // we have Slaves
      mpvec_slaveIsoNames->reserve (ai8_slaveCount);
      if (apc_slaveIsoNameList == NULL)
      { // Catch bad case and to avoid dereferencing NULL we set ai8_slaveCount to 0.
        getILibErrInstance().registerError( iLibErr_c::Precondition, iLibErr_c::System );
        ai8_slaveCount = 0;
      }
      for (int i=0; i<ai8_slaveCount; i++)
      { // copy the given Slave-IsoNames to the member stl-vector variable
        mpvec_slaveIsoNames->push_back (apc_slaveIsoNameList [i]);
      }
    }
  }
#endif

  if (b_setActive)
  { /// Set Item to ACTIVE
    BaseItem_c::set (System_c::getTime(), (IState_c::itemState_t (IState_c::Active |
                                                                  IState_c::PreAddressClaim |
                                                                  IState_c::Local)), ai_singletonVecKey);
    getIsoMonitorInstance4Comm().registerClient (this);
    getIsoMonitorInstance4Comm().sendRequestForClaimedAddress();
  }
}


/** init function for later start of address claim of an ISO identity (this can be only called once upon a default-constructed object)
    @param aui8_indGroup        select the industry group, 2 == agriculture
    @param aui8_devClass	device class identifier
    @param aui8_devClassInst	device class instance 
    @param ab_func              function code of the member (25 = network interconnect)
    @param aui16_manufCode      11bit manufactor code
    @param aui32_serNo          21bit serial number
    @param aui8_preferredSa      preferred source adress (SA) of the ISO item (fixed SA or last time
                                SA for self conf ISO device) (default 254 for no special wish)
    @param aui16_eepromAdr      EEPROM adress, where the used IsoName / SA / flags are stored
                                (default 0xFFFF for NO EEPROM store)
    @param ab_funcInst          function instance of this member (default 0)
    @param ab_ecuInst           ECU instance of this member (default 0)
    @param ab_selfConf          true -> this member as a self configurable source adress
    @param ai8_slaveCount       amount of attached slave devices; default -1 == no master state;
                                in case an address claim for the slave devices shall be sent by this ECU, they
                                must get their own IdentItem_c instance ( then with default value -1 for ai8_slaveCount )
    @param apc_slaveIsoNameList pointer to list of IsoName_c values, where the slave devices are defined.
                                IsoAgLib will then send the needed "master indicates its slaves" messages on BUS
    @param ai_singletonVecKey   optional key for selection of IsoAgLib instance (DEFAULTS HERE TO -1 which will NOT modify the instance set construction time!!)
  */
void IdentItem_c::init(
  uint8_t aui8_indGroup, uint8_t aui8_devClass, uint8_t aui8_devClassInst, uint8_t ab_func, uint16_t aui16_manufCode,
  uint32_t aui32_serNo, uint8_t aui8_preferredSa, uint16_t aui16_eepromAdr, uint8_t ab_funcInst, uint8_t ab_ecuInst, bool ab_selfConf,
  #ifdef USE_WORKING_SET
  int8_t ai8_slaveCount, const IsoName_c* apc_slaveIsoNameList,
  #endif
  int ai_singletonVecKey)
{
  // temporary to assemble the single parameters to one IsoName so it gets better handled in the following function call
  IsoName_c c_isoNameParam (ab_selfConf, aui8_indGroup, aui8_devClass, aui8_devClassInst,
                            ab_func, aui16_manufCode, aui32_serNo, ab_funcInst, ab_ecuInst);

  init (&c_isoNameParam, aui8_preferredSa, aui16_eepromAdr,
        #ifdef USE_WORKING_SET
        ai8_slaveCount, apc_slaveIsoNameList,
        #endif
        ai_singletonVecKey);
}



/** restart SA claim procedure.
    When ISONAME conflict gets detected between creation of corresponding IsoItem_c and sending of SA-Claim,
    a complete restart of IdentItem_c life cycle beginning with PreAddressClaim.
  */
void IdentItem_c::restartWithPreAddressClaim( void )
{
  if (mpc_isoItem != NULL)
  { // item is online
    // ->delete item from memberList
    getIsoMonitorInstance4Comm().deleteIsoMemberISOName (isoName());
    mpc_isoItem = NULL;
  }
  setItemState( IState_c::PreAddressClaim );
}


/** Go Offline by:
  * + reset IdentItem::IState_c to IState_c::Off / OffUnable 
  * + remove pointed IsoItem_c node and the respective pointer
 * @param ab_explicitlyOffByUser ("Off" if explicitly called by user (true), "OffUnable" if we are unable to keep a claimed address (false))
  */
void IdentItem_c::goOffline (bool ab_explicitlyOffByUser)
{
  if (mpc_isoItem != NULL)
  { // item is online
    // -> send unregister cmd
    // ->delete item from memberList
    getIsoRequestPgnInstance4Comm().unregisterLocalDevice( isoName() );
    getIsoMonitorInstance4Comm().deleteIsoMemberISOName (isoName());
    mpc_isoItem = NULL;
  }
  clearItemState( IState_c::ClaimedAddress );
  if (ab_explicitlyOffByUser) {
    setItemState( IState_c::OffExplicitly );
  } else {
    setItemState( IState_c::OffUnable );
  }
}


/** default destructor which has nothing to do */
IdentItem_c::~IdentItem_c()
{
  close();
}

/** every subsystem of IsoAgLib has explicit function for controlled shutdown
  * -> IdentItem_c::close() send address release for identities
  */
void IdentItem_c::close( void )
{ // delete the corresponding IsoItem_c instance in the respective monitoring list
  // and set state to Off (explicitly requested)
  goOffline (true);
  // unregister in IsoMonitor_c
  getIsoMonitorInstance4Comm().unregisterClient( this );
  // if we have a list of slaves, then
#ifdef USE_WORKING_SET
  if (mpvec_slaveIsoNames)
  { // delete list of slaves, too.
    delete mpvec_slaveIsoNames;
    mpvec_slaveIsoNames = NULL;
  }
#endif
}



/** periodically called functions do perform
    time dependent actions

    possible errors:
        * dependant memory error in SystemMgmt_c caused by inserting item in monitor list
    @see Scheduler_c::timeEvent()
    @see IsoMonitor_c::timeEvent()
    @see System_c::getTime()
    @return true -> all planned activities performed
  */
bool IdentItem_c::timeEvent( void )
{
  if (NULL != getIsoItem())
  { // If we have an IsoItem, see if it went off..
    if ( (getIsoItem()->itemState (IState_c::OffExplicitly))
      || (getIsoItem()->itemState (IState_c::OffUnable))
       )
    { // if the IsoItem dictates us to go offline, we're NOT offline by User of course...
      goOffline(false);
    }
  }
  if ( (itemState (IState_c::OffExplicitly))
    || (itemState (IState_c::OffUnable))
     )
  { // We're OFF! So nothing to do!
    return true;
  }
  else
  { // the system is not switched to off state
    // if state is prepare address claim check if isoName unique and insert item in list
    if (itemState(IState_c::PreAddressClaim))
      return timeEventPreAddressClaim();
    else
      return timeEventActive();
  }
}


/** periodically called functions do perform
    time dependent actions in prepare address claim state
    -> unify ISOName (Device Class / Device Class Instance)
    -> insert item in appropriate monitor lists and initiate address claim

    possible errors:
        * dependant memory error in SystemMgmt_c caused by inserting item in monitor list
    @see Scheduler_c::timeEvent
    @see SystemMgmt_c::timeEvent
    @see System_c::getTime
    @return true -> all planned activities performed
  */
bool IdentItem_c::timeEventPreAddressClaim( void )
{
  bool b_isoNameSuccessfulUnified = false;
  // check if isoName is unique and change if needed (to avoid adress conflict on Scheduler_c BUS) and allowed!

  // fixIsoName=true: no unifying possible, calling "unifyIsoISOName" only to see if the isoname exists in the monitor yet or not...
  const bool cb_fixIsoName = (mui8_globalRunState == GlobalRunStateAlreadyClaimed); // If already claimed with this IsoName, we can't change away!
  b_isoNameSuccessfulUnified = getIsoMonitorInstance4Comm().unifyIsoISOName (mc_isoName, cb_fixIsoName);

  if (b_isoNameSuccessfulUnified)
  {
    // insert element in list
    mpc_isoItem = getIsoMonitorInstance4Comm().insertIsoMember (isoName(), mui8_preferredSa,
      IState_c::itemState_t(IState_c::Local | IState_c::PreAddressClaim), this, false); // do not yet announce. @todo SOON Maybe also announce right now?

    if (mpc_isoItem != NULL)
    {
      // register the new item for ISORequestPGN
      getIsoRequestPgnInstance4Comm().registerLocalDevice( isoName() );

      #ifdef USE_WORKING_SET
      mpc_isoItem->setMasterSlaves (mpvec_slaveIsoNames);
      #endif

      mpc_isoItem->timeEvent();
    }
  }

  // set ident_item state to claim address
  if (b_isoNameSuccessfulUnified) setItemState(IState_c::AddressClaim);
  return true;
}


/** periodically called functions do perform
    time dependent actions in active (address claim/claimed address) state
    -> call timeEvent for corresponding items in MemberMonitor/ISOMonitor
    -> initiate repeated address claim with changed Nr / ISOName if conflict with other item occured

    possible errors:
        * dependant memory error in SystemMgmt_c caused by inserting item in monitor list
    @see Scheduler_c::timeEvent
    @see SystemMgmt_c::timeEvent
    @see System_c::getTime
  */
bool IdentItem_c::timeEventActive( void )
{
  #ifdef CHANGE_DEV_CLASS_INST_ON_CONFLICT
  // only change dev class inst and repeated address claim on adress conflicts, if following define
  // is set in masterHeader
  /** @todo SOON USE THIS PRINCIPLE IN CASE WE RECEIVE MESSAGE WITH LOCAL SA -> detected in DataLinkLayer (CanPkgExt_c->resolving)
           --> we should change SA in such a case AND maybe send some sort of DIAGNOSE MESSAGE
    */
  // because of errors caused by terminals, which doesn't claim correctly an address
  // don't do conflict changing of POS
  if ((pc_memberItem->affectedConflictCnt() > 3) && (itemState(IState_c::Din)))
  { // repeated address claim needed
    // send an adress release for according number
    pc_memberItem->sendAdressRelease();
    // delete item from memberList
    getDinMonitorInstance4Comm().deleteDinMemberISOName(isoName());
    pc_memberItem = NULL;
    // restart with init
    init();
    // set time to individual wait time
    setIndividualWait();
  }
  else
  #endif

  bool b_configure = false;

  /// If we're in Activetimeevent, we always do have a valid mpc_isoItem!
  if (mpc_isoItem->itemState(IState_c::Local))
  {
    bool b_oldAddressClaimState = mpc_isoItem->itemState(IState_c::ClaimedAddress);

    mpc_isoItem->timeEvent();
    // check if IsoItem_c reports now to have finished address claim and store it in Ident_Item
    if ( (mpc_isoItem->itemState(IState_c::ClaimedAddress))
      && (!b_oldAddressClaimState) )
    { // item changed from address claim to claimed address state
      // -> create local filter for processs data
      setItemState(IState_c::ClaimedAddress);

      // set global run state because address claimed already
      mui8_globalRunState = GlobalRunStateAlreadyClaimed;

#ifdef DEBUG_NETWORK_MANAGEMENT
      INTERNAL_DEBUG_DEVICE << "Write global run state " << (int)mui8_globalRunState << " to EEPROM." << INTERNAL_DEBUG_DEVICE_ENDL;
#endif

      if (mui16_eepromAdr != 0xFFFF)
      {
        #ifdef USE_EEPROM_IO
        const uint8_t* pcui8_isoName = mc_isoName.outputString();
        EepromIo_c& rc_eeprom = getEepromInstance();
        rc_eeprom.setp (mui16_eepromAdr);
        rc_eeprom << mui8_globalRunState << mui8_preferredSa;
        rc_eeprom.writeString (pcui8_isoName, 8);

        #else
        // ERROR: Using EEPROM Address in IdentItem_c()'s timeEventActive but IsoAgLib is NOT compiled with USE_EEPROM_IO !!!!" << INTERNAL_DEBUG_DEVICE_ENDL;
        getILibErrInstance().registerError( iLibErr_c::ElNonexistent, iLibErr_c::Eeprom );
        #if defined(DEBUG) && defined(SYSTEM_PC)
        INTERNAL_DEBUG_DEVICE << "ERROR: Using EEPROM Address in IdentItem_c()'s timeEventActive but IsoAgLib is NOT compiled with USE_EEPROM_IO !!!!" << INTERNAL_DEBUG_DEVICE_ENDL;
        abort();
        #endif
        #endif
      }
    }
  }
  else
  { // remote ISO item has overwritten local item
    /// @todo SOON This case shouldn't happen anymore!!!!!!!! --> See conflict handling when some sends an ADDRESS_CLAIM with our ISONAME!
    // ->see if we can still live with our IsoName
    // ->if not, we'lost because we can't change our IsoName
    IsoMonitor_c& rc_isoMonitor = getIsoMonitorInstance4Comm();
    const bool cb_isoNameStillAvailable = !rc_isoMonitor.existIsoMemberISOName (mc_isoName);

    if (cb_isoNameStillAvailable)
    { // insert element in list
      mpc_isoItem =  rc_isoMonitor.insertIsoMember(isoName(), mui8_preferredSa,
        IState_c::itemState_t(IState_c::Local | IState_c::PreAddressClaim), this, false); // do not yet announce. @todo SOON Maybe also announce right now?
      if ( NULL != mpc_isoItem )
      {
        // register the new item for ISORequestPGN
        getIsoRequestPgnInstance4Comm().registerLocalDevice( isoName() );

        #ifdef USE_WORKING_SET
        mpc_isoItem->setMasterSlaves (mpvec_slaveIsoNames);
        #endif

        mpc_isoItem->timeEvent();
      }
    }
    else
    { /// IsoName now already used on the bus - we can't claim an address now anymore!
      /** @todo SOON insert new error-location/type for those cases! */
      getILibErrInstance().registerError( iLibErr_c::Busy, iLibErr_c::System );
      IsoItem_c& rc_foundIsoItemSameIsoName = rc_isoMonitor.isoMemberISOName (mc_isoName);
      if (rc_foundIsoItemSameIsoName.itemState (IState_c::Local))
      { // now the ISOName is used by some other member on the BUS
        mpc_isoItem = &rc_foundIsoItemSameIsoName; // seems to be our IsoItem although it's a case that shouldn't occur!
        #if defined(SYSTEM_PC) && defined(DEBUG)
        INTERNAL_DEBUG_DEVICE << "ERROR: IsoName stolen by other local member, take this IsoItem then, although this shouldn't happen!" << INTERNAL_DEBUG_DEVICE_ENDL;
        abort();
        #endif
      }
      else
      { // now the ISOName is used by some other member on the BUS
        // ==> conflict
        setItemState (OffUnable); // withdraw from action
        #if defined(SYSTEM_PC) && defined(DEBUG)
        INTERNAL_DEBUG_DEVICE << "WARNING: IsoName stolen by other member on the bus (remote), so we have to shut off forever!" << INTERNAL_DEBUG_DEVICE_ENDL;
        #endif
      }
    }
  }

  if (b_configure) {
    getCanInstance4Comm().reconfigureMsgObj();
  }

  return true;
}


/** check if given number is equal to member number of this item
    @param aui8_nr compared number
    @return true -> this item has same number
  */
bool IdentItem_c::equalNr(uint8_t aui8_nr)
{
  bool b_result = false;

  if (mpc_isoItem != NULL) b_result = (mpc_isoItem->nr() == aui8_nr)?true:false;

  return b_result;
}


} // end of namespace __IsoAgLib
