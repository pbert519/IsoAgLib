/***************************************************************************
                          dinmonitor_c.h - object for monitoring members
                                             (list of DINItem_c)
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
#ifndef MEMBER_MONITOR_H
#define MEMBER_MONITOR_H

/* *************************************** */
/* ********** include headers ************ */
/* *************************************** */
#include <IsoAgLib/typedef.h>
#include <IsoAgLib/util/config.h>
#include <IsoAgLib/util/impl/cancustomer_c.h>
#include <IsoAgLib/util/impl/singleton.h>
#include <IsoAgLib/util/impl/elementbase_c.h>
#include "dinsystempkg_c.h"
#include "adrvecttrusted_c.h"
#include "dinitem_c.h"

#if defined(SYSTEM_PC) && !defined(SYSTEM_PC_VC)
  #include <ext/slist>
  namespace std { using __gnu_cxx::slist;};
#else
  #include <slist>
#endif

#ifndef EXCLUDE_RARE_DIN_SYSTEM_CMD
  #include "dinstopmanager_c.h"
#endif
namespace IsoAgLib { class iDINMonitor_c;}

// Begin Namespace __IsoAgLib
namespace __IsoAgLib {
class DINMonitor_c;
typedef SINGLETON(DINMonitor_c) SingletonDINMonitor_c;
/**
  this object manages a monitor list of all
  members including inserting and administration of local own members.
  Derive from ElementBase_c to register in Scheduler_c for timeEvent trigger
  Derive from CANCustomer to register FilterBox'es in CANIO_c to receive CAN messages
  Derive from SINGLETON to create a Singleton which manages one global accessible singleton
   per IsoAgLib instance (if only one IsoAgLib instance is defined in application config, no overhead is produced).
  @short Manager for members of Scheduler_c (DINItem_c)
  @see MemberItem
  @see ServiceMonitor
  @author Dipl.-Inform. Achim Spangler
*/
class DINMonitor_c
: public ElementBase_c,
  public CANCustomer_c,
  public SingletonDINMonitor_c,
  public AdrVectTrusted_c
{
private:
  #ifndef EXCLUDE_RARE_DIN_SYSTEM_CMD
  typedef std::slist<DINStopManager_c,std::__malloc_alloc_template<0> > ArrStop;
  typedef std::slist<DINStopManager_c,std::__malloc_alloc_template<0> >::iterator ArrStopIterator;
  #endif
  typedef std::slist<DINItem_c,std::__malloc_alloc_template<0> > Vec_Member;
protected:
  typedef std::slist<DINItem_c,std::__malloc_alloc_template<0> >::iterator Vec_MemberIterator;

public:
  /** initialisation for DINMonitor_c
  */
  void init( void );
  /** every subsystem of IsoAgLib has explicit function for controlled shutdown
    */
  void close( void );
  /**
    performs periodically actions,
    here: update trusted adrVect every 3sec;

    possible errors:
      * partial error caused by one of the memberItems

    @return true -> all planned activities performed in allowed time
  */
  bool timeEvent( void );


  /** default destructor which has nothing to do */
  virtual ~DINMonitor_c() { close();};

  /**
    sends system message, which requests all active
    members to send their names (send own name too, as protocol demands)

    possible errors:
      * Err_c::lbsSysNoActiveLocalMember on missing own active ident
    @return true -> member name command successful sent
  */
  bool requestDinMemberNames();

  /**
    deliver reference to data pkg
    @return reference to the CAN communication member object c_data (DINSystemPkg_c)
    @see CANPkgExt
  */
  DINSystemPkg_c& data() {return c_data;};
  /**
    deliver reference to data pkg as reference to CANPkgExt_c
    to implement the base virtual function correct
  */
  virtual CANPkgExt_c& dataBase();
  /**
    process system msg with informations which are
    important for managing of members
    (this function is only called if NO conflict is detected)
    @return true -> message processed by DINMonitor_c; false -> process msg by ServiceMonitor
  */
  bool processMsg();
  /**
    deliver amount of DIN members in monitor list which optional (!!)
    match the condition of address claim state
    @param rb_forceClaimedAddress true -> only members with claimed address are used
          (optional, default false)
    @return amount of DIN members with claimed address
  */
  uint8_t dinMemberCnt(bool rb_forceClaimedAddress = false)
    {return dinMemberGetyCnt(0xFF, rb_forceClaimedAddress);};
  /**
    deliver the n'th DIN member in monitor list which optional (!!)
    match the condition of address claim state
    check first with dinMemberCnt if enough members are registered
    in Monitor-List
    @see dinMemberCnt

    possible errors:
      * Err_c::range there exist less than rui8_ind members with claimed address
    @param rui8_ind position of the wanted member in the
                 sublist of members (first item has rui8_ind == 0 !!)
                 with the wanted property
    @param rb_forceClaimedAddress true -> only members with claimed address are used
          (optional, default false)
    @return reference to searched element
  */
  DINItem_c& dinMemberInd(uint8_t rui8_ind, bool rb_forceClaimedAddress = false)
    {return dinMemberGetyInd(0xFF, rui8_ind, rb_forceClaimedAddress);};

  /**
    deliver the count of members in the Monitor-List with given GETY (variable POS)
    which optional (!!) match the condition of address claim state
    @param rb_gety searched GETY code
    @param rb_forceClaimedAddress true -> only members with claimed address are used
          (optional, default false)
    @return count of members in Monitor-List with GETY == rb_gety
  */
  uint8_t dinMemberGetyCnt(uint8_t rb_gety, bool rb_forceClaimedAddress = false);
  /**
    deliver one of the members with specific GETY
    which optional (!!) match the condition of address claim state
    check first with dinMemberGetyCnt if enough members with wanted GETY and
    optional (!!) property are registered in Monitor-List
    @see dinMemberGetyCnt

    possible errors:
      * Err_c::range there exist less than rui8_ind members with GETY rb_gety
   @param rb_gety searched GETY
   @param rui8_ind position of the wanted member in the
                 sublist of member with given GETY (first item has rui8_ind == 0 !!)
   @param rb_forceClaimedAddress true -> only members with claimed address are used
         (optional, default false)
   @return reference to searched element
  */
  DINItem_c& dinMemberGetyInd(uint8_t rb_gety, uint8_t rui8_ind, bool rb_forceClaimedAddress = false);
  /**
    check if a memberItem with given GETY_POS exist
    which optional (!!) match the condition of address claim state
    and update local pc_dinMemberCache
    @param rc_gtp searched GETY_POS
    @param rb_forceClaimedAddress true -> only members with claimed address are used
          (optional, default false)
    @return true -> searched member exist
  */
  bool existDinMemberGtp(GetyPos_c rc_gtp, bool rb_forceClaimedAddress = false);

  /**
    check if a member with given number exist
    which optional (!!) match the condition of address claim state
    and update local pc_dinMemberCache
    @param rui8_nr searched member number
    @param rb_forceClaimedAddress true -> only members with claimed address are used
          (optional, default false)
    @return true -> item found
  */
  bool existDinMemberNr(uint8_t rui8_nr, bool rb_forceClaimedAddress = false);

  /**
    check if member is in member list with wanted GETY_POS,
    adopt POS if member with claimed address with other POS exist
    @param refc_gtp GETY_POS to search (-> it's updated if member with claimed address with other POS is found)
    @return true -> member with claimed address with given GETY found (and refc_gtp has now its GETY_POS)
  */
  bool dinGety2GtpClaimedAddress(GetyPos_c &refc_gtp);


  /**
    insert a new DINItem_c in the list; with unset rui8_nr the member is initiated as
    address claim state; otherwise the given state can be given or state Active is used

    possible errors:
      * Err_c::badAlloc not enough memory to insert new DINItem_c isntance
      * Err_c::busy another member with same ident exists already in the list

    @param rc_gtp GETY_POS of the member
    @param rpb_name pointer to 7 uint8_t name string
    @param rui8_nr member number
    @param rui16_adrvect AdrVect_c used by the member
    @param ren_status wanted status
    @return true -> the DINItem_c was inserted
  */
  bool insertDinMember(GetyPos_c rc_gtp, const uint8_t* rpb_name = NULL, uint8_t rui8_nr = 0xFF,
                     uint16_t rAdrvect = 0, IState_c::itemState_t ren_state = IState_c::Active);

  /**
    deliver member item with given gtp
    (check with existDinMemberGtp before access to not defined item)

    possible errors:
      * Err_c::elNonexistent on failed search

    @param rc_gtp searched GETY_POS
    @return reference to searched MemberItem
     @exception containerElementNonexistant
  */
  DINItem_c& dinMemberGtp(GetyPos_c rc_gtp);

  /**
    deliver member item with given nr
    (check with existDinMemberNr before access to not defined item)

    possible errors:
      * Err_c::elNonexistent on failed search

    @param rui8_nr searched number
    @return reference to searched MemberItem
     @exception containerElementNonexistant
  */
  DINItem_c& dinMemberNr(uint8_t rui8_nr);

  /**
    deliver member item with given GETY_POS, set pointed bool var to true on success
    and set a Member Array Iterator to the result
    @param rc_gtp searched GETY_POS
    @param pb_success bool pointer to store the success (true on success)
    @param pbc_iter optional member array iterator which points to searched DINItem_c on success
    @return reference to the searched item
  */
  DINItem_c& dinMemberGtp(GetyPos_c rc_gtp, bool *const pb_success, Vec_MemberIterator *const pbc_iter = NULL);

  /**
    delete item with specified gtp

    possible errors:
      * Err_c::elNonexistent no member with given GETY_POS exists

    @param rc_gtp GETY_POS of to be deleted member
    @param rb_send-release true -> send adress release msg (optional, default = false)
  */
  bool deleteDinMemberGtp(GetyPos_c rc_gtp, bool rb_sendRelease = false);
  /**
    delete item with specified member number

    possible errors:
      * Err_c::elNonexistent no member with given GETY_POS exists

    @param rui8_nr number of to be deleted member
    @param rb_send-release true -> send adress release msg (optional, default = false)
  */
  bool deleteDinMemberNr(uint8_t rui8_nr, bool rb_sendRelease = false);

   /**
    check, if one free adre is available;
    if all numbers are used:
    a) numbers which are reported as used, but not found in monitor list are deleted
    b) deletable, because >3sec. inactive member, are searched and deleted
    @param rb_eraseInactiveItem optional command deletion of inactive item, if no numberis free (default false)
    @return true -> a member number is free
  */
   bool freeNrAvailable(bool rb_eraseInactiveItem = false);

  /**
    check if ECU with gtp is allowed to claim an adress number
    @param rc_gtp GETY_POS of member which is tested
    @return true -> member is allowed to claim number
  */
  bool canClaimNr(GetyPos_c rc_gtp);

  /**
    change gtp if actual gtp isn't unique
    (search possible free POS to given GETY)

    possible errors:
      * Err_c::busy no other POS code leads to unique GETY_POS code

    @param refc_gtp reference to GETY_POS var (is changed directly if needed!!)
    @return true -> referenced GETY_POS is now unique
  */
  bool unifyDinGtp(GetyPos_c& refc_gtp);
  /**
    sets given number as used at least in internal adrvect;
    if claim is allowed, the number is set used in trusted and the
    published version too

    possible errors:
      * Err_c::busy another member with given nr is already registered in AdrVect

    @param rc_gtp GETY_POS of the used setting member
    @param rui8_nr number to register as used for the given member
    @return true -> the wanted nr was registered successful in AdrVect
  */
  bool setUsedAdr(GetyPos_c rc_gtp, uint8_t rui8_nr);
#ifndef EXCLUDE_RARE_DIN_SYSTEM_CMD
  /* *********************************************************** */
  /** \name Seldom used DIN 9684 system cmds
    * Some system cmds about commanding and requesting state,
    * device error etc. are seldom used -> unnecessary Flash
    * memory waste, if not needed for special cases.             */
  /* *********************************************************** */
  /*\@{*/

  /**
    starts or release stop command for specific member;
    IMPORTANT: as protocol demands the stop command is repeated till explicit stop-release function call

    possible errors:
      * Err_c::elNonexistent the commanded member GETY_POS doesn't exist in member list
      * Err_c::lbsSysNoActiveLocalMember on missing own active ident
    @param rc_gtpTarget GETY_POS of the member, which should be stopped
    @param rb_toStop true -> start sending STOP commands; false -> release STOP sending mode
    @return true -> stop command sent without errors
  */
  bool commandStop(GetyPos_c rc_gtpTarget, bool rb_toStop = true);

  /**
    starts or release stop command for all system members;
    IMPORTANT: as protocol demands the stop command is repeated till explicit stop-release function call

    possible errors:
      * Err_c::lbsSysNoActiveLocalMember on missing own active ident for sending the command
    @param rb_toStop true -> star sending STOP commands; false -> release STOP sending mode
    @return true -> one of the own identities was active with claimed address to send the global stop correctly
  */
  bool commandGlobalStop(bool rb_toStop = true);

  /**
    send ISO11783 or DIN9684 system msg to command status request for another member to switch to given mode

    possible errors:
      * Err_c::elNonexistent the commanded member GETY_POS doesn't exist in member list
      * Err_c::lbsSysNoActiveLocalMember on missing own active ident
    @param rc_gtp GETY_PSO of member, which switch state to OFF
    @param ren_itemState wanted state of item
    @return true -> stop command sent without errors
  */
  bool commandItemState(GetyPos_c rc_gtp, IState_c::itemState_t ren_itemState);
  /*\@}*/
#endif
private:
  friend class SINGLETON(DINMonitor_c);
  friend class IsoAgLib::iDINMonitor_c;
  /**
    HIDDEN constructor for a DINMonitor_c object instance which can optional
    set the configuration for send/receive for base msg type 1,2
    and calendar
    NEVER instantiate a variable of type DINMonitor_c within application
    only access DINMonitor_c via getBaseInstance() or getBaseInstance( int riLbsBusNr ) in case more than one ISO11783 or DIN9684 BUS is used for IsoAgLib
    */
  DINMonitor_c() { init(); };

// Private Methods
#ifndef EXCLUDE_RARE_DIN_SYSTEM_CMD
  /**
    internal inline function to send stop command with given gtp,send,verw,empf,xxx
    @param rc_gtp GETY_POS of sending member identity
    @param rb_verw VERW code of the system command
    @param rb_send SEND code of the system command
    @param rb_empf EMPF code of the system command (target member no)
    @param rb_xxxx XXXX code of the system command
    @return true -> send without errors
  */
  inline bool sendStopIntern(const GetyPos_c& rc_gtp, const uint8_t& rb_verw, const uint8_t& rb_send,
      const uint8_t& rb_empf, const uint8_t& rb_xxxx);


  /**
    check if a periodic stop command repetition
    should be sent and do this
    @return true -> send of stop commands performed without send errors
  */
  bool sendPeriodicStop( void );
#endif
  /** process a message with an address claim information */
  void processAddressClaimMsg();
  /**
    check if a received msg contains an adress conflict
    -> only interprete the msg, if no conflict is reported
    @return true -> the actual received conflict causes a conflict
  */
  bool adressConflict();
private: // Private attributes
  #ifndef EXCLUDE_RARE_DIN_SYSTEM_CMD
  /** dynamic array for actual stop commands for other members */
  ArrStop vec_stop;
  /** time of last sent stop command */
  int32_t i32_lastStopSent;
  /** single DINStopManager_c element for global stop commands */
  DINStopManager_c c_globalStop;
  /** system global state */
  bool b_globalSystemState;
  #endif
  /** temp data where received and to be sent data is put */
  DINSystemPkg_c c_data;
  /** temporary memberItem instance for better inserting of new elements */
  DINItem_c c_tempDinMemberItem;
  /**
    dynamic array of memberItems for handling
    of single member informations
  */
  Vec_Member vec_dinMember;
  /**
    cache pointer to speed serial of access
    to the same MemberItem
  */
  Vec_MemberIterator pc_dinMemberCache;
  /** sec. counter to send name request every 3sec if needed */
  uint8_t b_lastNameRequest;
  /** last time of building an trusted adrvect */
  int32_t i32_lastTrusted;
};
#if defined( PRT_INSTANCE_CNT ) && ( PRT_INSTANCE_CNT > 1 )
  /** C-style function, to get access to the unique DINMonitor_c singleton instance
    * if more than one CAN BUS is used for IsoAgLib, an index must be given to select the wanted BUS
    */
  DINMonitor_c& getDinMonitorInstance( uint8_t rui8_instance = 0 );
#else
  /** C-style function, to get access to the unique DINMonitor_c singleton instance */
  DINMonitor_c& getDinMonitorInstance( void );
#endif

}
#endif
