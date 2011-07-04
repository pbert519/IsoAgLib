/*
  istate_c.h: object which manages state of item in monitor list

  (C) Copyright 2009 - 2011 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/
#ifndef ISTATE_H
#define ISTATE_H

#include <IsoAgLib/hal/hal_typedef.h>
#include <IsoAgLib/util/impl/singleton.h>

// Begin Namespace __IsoAgLib
namespace __IsoAgLib {

/**
  object which manages state of item in monitor list
  @author Dipl.-Inform. Achim Spangler
*/
class IState_c : public ClientBase {
public:
  /* *************************************** */
  /* ************* typedefs **************** */
  /* *************************************** */
  /** enum type for state of a monitor list item */
  enum itemState_t {
         IstateNull = 0x0,
         // only one state of following Line possible
         OffUnable = 0x2, OffExplicitly = 0x4, Active = 0x8,
         // only one state of following Line possible
         PreAddressClaim = 0x10, AddressClaim = 0x20, ClaimedAddress = 0x40, AddressLost = 0x80,
         // states of following Line independent parallel possible
         CausedConflict = 0x200,
         AffectedConflict = 0x400,
         Error = 0x800,
         Local = 0x4000,   ///< This flag identifies local items
         PossiblyOffline = 0x8000 ///< This flag indicates, that the node missed to anwer at least one AdrClaim request -> upon next miss it should be deleted from monitor list
  };

  /**
    constructor of IState_c which can set the state to given initial value
    @param ren_itemState optional wanted state information (default Active value)
  */
  IState_c(itemState_t ren_itemState = Active, int ai_multitonInst = 0);

  /**
    constructor of IState_c which can set the state to given initial value
    @param aui8_state optional wanted state information (default \<empty\> value)
  */
  IState_c(uint8_t aui8_state, int ai_multitonInst = 0);

  /**
    copy constructor for IState_c
    @param acrc_src source for this new instance
  */
  IState_c(const IState_c& acrc_src);

  /**
    set the state of an monitor list item and
    return the resulting state value

    set state to Active, and reset Off and OffUnable on
    Active, PreAddressClaim, AddressClaim, ClaimedAddress, FalseAlive;

    set: PreAddressClaim, AddressClaim and ClaimedAddress exclusive

    set: Off, OffUnable, Active exclusive
    @param ren_itemState state information
    @param ab_clearOld optional clear old value for complete new set (default no clear)
    @return resulting state information
  */
  itemState_t setItemState(itemState_t ren_itemState, bool ab_clearOld = false);

  /**
    check if specific state is exactly set
    @param ren_itemState state information to check
    @return true -> the given state is set
  */
  bool itemState(itemState_t ren_itemState) const
    {return ((en_itemState & ren_itemState) == ren_itemState);}

  /**
  check if specific state is partially set
  @param ren_itemState combined state information to check
  @return true -> one of the given combined states is set
   */
  bool itemStatePartialMatch(itemState_t ren_itemState) const
  {return ((en_itemState & ren_itemState) != 0);}

  /**
    deliver the state information
    @return state information of type itemState_t (with state informations coded by OR in enum)
  */
  itemState_t itemState() const
    {return en_itemState;}

  /**
    clear whole state or some specific flags
    @param ren_itemState optional flags to clear (default clear all)
  */
  void clearItemState(itemState_t ren_itemState = itemState_t(~IstateNull))
    {en_itemState = itemState_t(en_itemState & ~ren_itemState);}


private:
  /** state of this monitor item */
  itemState_t en_itemState;

};

/**
  extended state managing object, which stores additional information for Members
  @author Dipl.-Inform. Achim Spangler
*/
class IStateExt_c : public IState_c {
public:
  /** enum type for special command arguments */
  enum specialParameter_t { Request = -1, Incr = -2, Decr = -3};

  /**
    constructor of IStateExt_c which can set the state to given initial value
    @param ren_itemState optional wanted state information (default Off value)
  */
  IStateExt_c(itemState_t ren_itemState = OffExplicitly, int ai_multitonInst = 0);

  /**
    constructor of IState_c which can set the state to given initial value
    @param aui8_state optional wanted state information (default \<empty\> value)
  */
  IStateExt_c(uint8_t aui8_state, int ai_multitonInst = 0);

  /**
    copy constructor for IState_c
    @param acrc_src source for this new instance
  */
  IStateExt_c(const IStateExt_c& acrc_src);

  /**
    set and/or retreive the AddressClaim counter
    @param ac_cnt optional new AddressClaim counter  (default only Request)
    @return actual or resulting AddressClaim cnt
  */
  uint8_t addressClaimCnt(int8_t ac_cnt = Request);

  /**
    retreive the counter of caused conflicts
    @return actual count of caused conflict
  */
  uint8_t causedConflictCnt() const {return counter.b_causedConflictCnt;}

  /**
    set the counter of false alive msgs
    @param ac_cnt new false alive counter  (default only Request)
    @return actual or resulting false alive cnt
  */
  uint8_t causedConflictCnt(int8_t ac_cnt, int32_t ai32_time = -1);

  /**
    retreive the counter of Affected conflicts
    @return actual count of Affected conflict
  */
  uint8_t affectedConflictCnt() const {return counter.b_affectedConflictCnt;}

  /**
    set the counter of false alive msgs
    @param ac_cnt new false alive counter  (default only Request)
    @return actual or resulting false alive cnt
  */
  uint8_t affectedConflictCnt(int8_t ac_cnt, int32_t ai32_time = -1);

private:
  /** last timestamp (in seconds) of caused conflict */
  int16_t mi16_lastCausedConflictTime;
  /** last timestamp (in seconds) of Affected conflict */
  int16_t mi16_lastAffectedConflictTime;
  struct {
    uint16_t b_causedConflictCnt : 4;
    uint16_t b_affectedConflictCnt : 4;
    uint16_t b_addressClaimCnt : 4;
  //uint16_t b_falseAliveCnt : 4;
  } counter;
};

}

#endif
