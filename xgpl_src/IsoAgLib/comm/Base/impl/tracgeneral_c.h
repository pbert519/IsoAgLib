/***************************************************************************
               tracgeneral_c.h  - stores, updates  and
                                  delivers all base data informations
                                  from CANCustomer_c derived for CAN
                                  sending and receiving interaction;
                                  from BaseCommon_c derived for
                                  interaction with other IsoAgLib objects
                             -------------------
    begin                 Fri Apr 07 2000
    copyright            : (C) 2000 - 2004 by Dipl.-Inform. Achim Spangler
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
#ifndef TRACGENERAL_C_H
#define TRACGENERAL_C_H

#include <IsoAgLib/comm/Base/impl/basecommon_c.h>
#include <IsoAgLib/comm/SystemMgmt/ISO11783/impl/isorequestpgnhandler_c.h>

#include <ctime>

// Begin Namespace __IsoAgLib
namespace __IsoAgLib {

typedef struct
{
  /** indicates the transport state of an implement connected to a tractor or power unit */
  unsigned int inTransport: 2;
  /** indicates the state of an implement where it may be disconnected from a tractor or power unit */
  unsigned int inPark: 2;
  /** indicates that an implement is connected to a tractor or power unit and is ready for work */
  unsigned int inWork: 2;
} indicatedStateImpl_t;

class TracGeneral_c;
typedef SINGLETON_DERIVED (TracGeneral_c, BaseCommon_c) SingletonTracGeneral_c;
/** stores, updates  and delivers all base data informations;
    Derive from BaseCommon_c some fundamental funktionality for all base data
    Derive from ElementBase_c to register in Scheduler_c for timeEvent trigger
    Derive from CANCustomer to register FilterBox'es in CANIO_c to receive CAN messages
    Derive from SINGLETON to create a Singleton which manages one global accessible singleton
    per IsoAgLib instance (if only one IsoAgLib instance is defined in application config, no overhead is produced).
  */
class TracGeneral_c : public SingletonTracGeneral_c
                    , public ISORequestPGNHandler_c
{
public: // Public methods
  /* ********************************************* */
  /** \name Management Functions for class TracGeneral_c  */
  /*@{*/

  /** initialise element which can't be done during construct;
      above all create the needed FilterBox_c instances
      possible errors:
        * dependant error in CANIO_c problems during insertion of new FilterBox_c entries for IsoAgLibBase
      @param rpc_isoName optional pointer to the ISOName variable of the responsible member instance (pointer enables automatic value update if var val is changed)
      @param rt_identMode either IsoAgLib::IdentModeImplement or IsoAgLib::IdentModeTractor
    */
  virtual void init_base (const ISOName_c* rpc_isoName = NULL, IsoAgLib::IdentMode_t rt_identMode = IsoAgLib::IdentModeImplement);

  /** config the TracGeneral_c object after init -> set pointer to isoName and
      config send/receive of different general base msg types
      @param rpc_isoName pointer to the ISOName variable of the ersponsible member instance (pointer enables automatic value update if var val is changed)
      @param rt_identMode either IsoAgLib::IdentModeImplement or IsoAgLib::IdentModeTractor
      @return true -> configuration was successfull
    */
  virtual bool config_base (const ISOName_c* rpc_isoName, IsoAgLib::IdentMode_t rt_identMode);

  /** destructor for TracGeneral_c which has nothing to do */
  virtual ~TracGeneral_c() { BaseCommon_c::close();};

  bool processMsgRequestPGN (uint32_t rui32_pgn, ISOItem_c* rpc_isoItemSender, ISOItem_c* rpc_isoItemReceiver);

  /** force a request for pgn for language information */
  bool sendRequestUpdateLanguage();

  /*@}*/

  /* ******************************************* */
  /** \name Set Values for periodic send on BUS  */
  /*@{*/
  /**
  set rear hitch
  @param rb_val uint8_t value to store as position of rear hitch [%]
   */
  void setHitchRear(uint8_t rb_val)
  { ui8_hitchRear = rb_val;}

  /**
  set front hitch
  @param rb_val uint8_t value to store as position of front hitch [%]
   */
  void setHitchFront(uint8_t rb_val)
  { ui8_hitchFront = rb_val;}

  /** set front hitch draft
    * @return front hitch draft [-320.000N;322.550N]; 1N/bit
    */
  void setHitchFrontDraft(int32_t ri32_val) { ui16_frontDraft = ((ri32_val + 320000) / 10);}

  /** set rear hitch draft
    * @return rear hitch draft [-320.000N;322.550N]; 1N/bit
    */
  void setHitchRearDraft(int32_t ri32_val) { ui16_rearDraft = ((ri32_val + 320000) / 10);}

  /** set front hitch nominal link force
    * @return front nominal link force [-100%;100%]; 1 promille per bit
    */
  void setHitchFrontLowerLinkForce(int16_t ri16_val) { ui8_frontLinkForce = ((ri16_val + 1000) / 8);}

  /** set rear hitch nominal link force
    * @return rear nominal link force [-100%;100%]; 1 promille per bit
    */
  void setHitchRearLowerLinkForce(int16_t ri16_val) { ui8_rearLinkForce = ((ri16_val + 1000) / 8);}

  /**
    * set the ISO key switch state of the tractor
    * @param rt_val IsoActive -> key switch ON
    */
  void setKeySwitch(IsoAgLib::IsoActiveFlag_t rt_val) { t_keySwitch = rt_val; }

  /** set the maximum power time of the tractor in [min]
    * @return maximum power time of the tractor in [min]
    */
  void setMaxPowerTime(uint8_t rui8_val) { ui8_maxPowerTime = rui8_val;}

  /** set state of implement in transport state
    * @param rt_val state of implement in transport state
    */
  void setMaintainPowerForImplInTransport(IsoAgLib::IsoImplTransportFlag_t rt_val) { implState.inTransport = rt_val; }

  /** set state of implement in park state
    * @param rt_val state of implement in park state
    */
  void setMaintainPowerForImplInPark(IsoAgLib::IsoImplParkFlag_t rt_val) { implState.inPark = rt_val;}

  /** set state of implement in work
    * @param rt_val state of implement in work state
    */
  void setMaintainPowerForImplInWork(IsoAgLib::IsoImplWorkFlag_t rt_val) { implState.inWork = rt_val;}

  /** set present limit status of the front hitch position
      @param rt_val  limit status of the front hitch position
    */
  void setFrontHitchPosLimitStatus(const IsoAgLib::IsoLimitFlag_t rt_val) {t_frontHitchPosLimitStatus = rt_val;}

  /** set present limit status of the rear hitch position
      @param rt_val  limit status of the rear hitch position
    */
  void setRearHitchPosLimitStatus(const IsoAgLib::IsoLimitFlag_t rt_val) {t_rearHitchPosLimitStatus = rt_val;}
  /*@}*/

  /* ****************************************************** */
  /** \name Retrieve Values which are sent from other ECUs  */
  /*@{*/
  /** get rear hitch
      @return actual position of rear hitch [%]
    */
  uint8_t hitchRear() const {return ui8_hitchRear;}

  /** get front hitch
      @return actual position of front hitch [%]
    */
  uint8_t hitchFront() const {return ui8_hitchFront;}

  /** deliver front hitch draft
    * @return front hitch draft [-320.000N;322.550N]; 1N/bit
    */
  int32_t hitchFrontDraft() const { return static_cast<int32_t>( (ui16_frontDraft * 10) - 320000 );}

  /** deliver rear hitch draft
    * @return rear hitch draft [-320.000N;322.550N]; 1N/bit
    */
  int32_t hitchRearDraft() const { return static_cast<int32_t>( (ui16_rearDraft * 10) - 320000 );}

  /** deliver front hitch nominal link force
    * @return front nominal link force [-100%;100%]; 1 promille per bit
    */
  int16_t hitchFrontLowerLinkForce() const { return ((static_cast<int16_t>(ui8_frontLinkForce) * 8) - 1000);}

  /** deliver rear hitch nominal link force
    * @return rear nominal link force [-100%;100%]; 1 promille per bit
    */
  int16_t hitchRearLowerLinkForce() const { return ((static_cast<int16_t>(ui8_rearLinkForce) * 8) - 1000);}

  /** deliver the ISO key switch state of the tractor
    * @return IsoActive -> key switch ON
    */
  IsoAgLib::IsoActiveFlag_t keySwitch() const { return t_keySwitch;}

  /** deliver the maximum power time of the tractor in [min]
    * @return maximum power time of the tractor in [min]
    */
  uint8_t maxPowerTime() const { return ui8_maxPowerTime;}
  /** deliver last receive time of maintain power request
    * @return time in [ms] since system start -> comparable to system time
    */
  int32_t lastMaintainPowerRequest() const { return ui32_lastMaintainPowerRequest;}

  /** get present limit status of the front hitch position
      @return  limit status of front hitch position
    */
  IsoAgLib::IsoLimitFlag_t frontHitchPosLimitStatus()const {return t_frontHitchPosLimitStatus;}

  /** get present limit status of the rear hitch position
      @return  limit status of rear hitch position
    */
  IsoAgLib::IsoLimitFlag_t rearHitchPosLimitStatus()const {return t_rearHitchPosLimitStatus;}

  /** check whether maintenance of ECU power was requested */
  bool maintainEcuPower() const { return b_maintainEcuPower;}

  /** check whether maintenance of actuator power was requested */
  bool maintainActuatorPower() const { return b_maintainActuatorPower;}

  /** check whether maintenance of power
    * for implement in transport state was requested */
  IsoAgLib::IsoImplTransportFlag_t maintainPowerForImplInTransport() const
  { return IsoAgLib::IsoImplTransportFlag_t(implState.inTransport); }

  /** check whether maintenance of power
    * for implement in park state was requested */
  IsoAgLib::IsoImplParkFlag_t maintainPowerForImplInPark() const {return IsoAgLib::IsoImplParkFlag_t(implState.inPark);}

  /** check whether maintenance of power
    * for implement in work state was requested */
  IsoAgLib::IsoImplWorkFlag_t maintainPowerForImplInWork() const {return IsoAgLib::IsoImplWorkFlag_t(implState.inWork);}

  bool isVtLanguageReceived()   const { return b_languageVtReceived; }
  bool isTecuLanguageReceived() const { return b_languageTecuReceived; }

  const uint8_t* getVtLanguage()   const { return p8ui8_languageVt; }
  const uint8_t* getTecuLanguage() const { return p8ui8_languageTecu; }

  /** send iso language data msg
      @see  TracGeneral_c::processMsgRequestPGN
      @see  CANIO_c::operator<<
    */
  void sendLanguage();

  ///  Used for Debugging Tasks in Scheduler_c
  virtual const char* getTaskName() const;


  /** force maintain power from tractor
      @see  CANIO_c::operator<<
      @param rb_ecuPower true -> maintain ECU power
      @param rb_actuatorPower true-> maintain actuator power
      @param rt_implState in which state is the implement (transport, park, work)
    */
  void forceMaintainPower( bool rb_ecuPower, bool rb_actuatorPower, IsoAgLib::IsoMaintainPower_t rt_implState);
  /*@}*/

private:
  // Private methods
  friend class SINGLETON_DERIVED(TracGeneral_c,BaseCommon_c);
  /** HIDDEN constructor for a TracGeneral_c object instance which can optional
      set the configuration for send/receive for general base msg
      NEVER instantiate a variable of type TracGeneral_c within application
      only access TracGeneral_c via getTracGeneralInstance() or getTracGeneralInstance( int riLbsBusNr )
      in case more than one BUS is used for IsoAgLib
    */
  TracGeneral_c() {};

  /** check if filter boxes shall be created - create only filters based
      on active local idents which has already claimed an address
      --> avoid to much Filter Boxes
    */
  virtual void checkCreateReceiveFilter();

  /** send a ISO11783 general base information PGN.
    * this is only called when sending ident is configured and it has already claimed an address
      @pre  function is only called in tractor mode
      @see  BaseCommon_c::timeEvent()
    */
  virtual bool timeEventTracMode();

  /** process a ISO11783 base information PGN
      @pre  sender of message is existent in monitor list
      @see  CANPkgExt_c::resolveSendingInformation()
    */
  bool processMsg();

  /** send front hitch and rear hitch data msg
      @see  CANIO_c::operator<<
    */
  void sendMessage();

private:
  // Private attributes
  /** engine speed */
  int16_t i16_engine;

  /** front hitch data */
  uint8_t ui8_hitchFront;

  /** rear hitch data */
  uint8_t ui8_hitchRear;


  /** VT language information */
  uint8_t p8ui8_languageVt[8];

  /** TECU language information */
  uint8_t p8ui8_languageTecu[8];

  /** VT language reception information */
  bool b_languageVtReceived;

  /** TECU language reception information */
  bool b_languageTecuReceived;


  /// General
  /** last time of ISO GPS msg [msec] */
  int32_t i32_lastIsoPositionSimple;

  /** key switch state */
  IsoAgLib::IsoActiveFlag_t t_keySwitch;

  /** maximum time of tractor power in [min] */
  uint8_t ui8_maxPowerTime;

  /** front hitch draft [-320.000;322.550N], res: 10N/bit, offset: -320.000N  */
  uint16_t ui16_frontDraft;

  /** rear hitch draft [-320.000;322.550N], res: 10N/bit, offset: -320.000N  */
  uint16_t ui16_rearDraft;

  /** front nominal lower link force [-100%;100%], res: 0.8%/bit, offset: -100% */
  uint8_t ui8_frontLinkForce;

  /** rear nominal lower link force [-100%;100%], res: 0.8%/bit, offset: -100% */
  uint8_t ui8_rearLinkForce;

  /** set reported the tractor ECU's present limit status of the front hitch position */
  IsoAgLib::IsoLimitFlag_t t_frontHitchPosLimitStatus;

  /** set reported the tractor ECU's present limit status of the rear hitch position */
  IsoAgLib::IsoLimitFlag_t t_rearHitchPosLimitStatus;

  /** last time of maintain power request [ms] */
  uint32_t ui32_lastMaintainPowerRequest;

  /** state whether maintenance of ECU power was requested */
  bool b_maintainEcuPower;

  /** state whether maintenance of actuator power was requested */
  bool b_maintainActuatorPower;

  /** indicated state of an implement */
  indicatedStateImpl_t implState;
};

  #if defined( PRT_INSTANCE_CNT ) && ( PRT_INSTANCE_CNT > 1 )
  /** C-style function, to get access to the unique TracGeneral_c singleton instance
    * if more than one CAN BUS is used for IsoAgLib, an index must be given to select the wanted BUS
    */
  TracGeneral_c& getTracGeneralInstance(uint8_t rui8_instance = 0);
  #else
  /** C-style function, to get access to the unique TracGeneral_c singleton instance */
  TracGeneral_c& getTracGeneralInstance(void);
  #endif

} // End Namespace __IsoAgLib
#endif
