/***************************************************************************
                 tracmove_c.h -   stores, updates and delivers all moving
                                  data information from CanCustomer_c
                                  derived for CAN sending and receiving
                                  interaction;
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
#ifndef TRACMOVE_C_H
#define TRACMOVE_C_H

#include <IsoAgLib/comm/Base/impl/basecommon_c.h>



// Begin Namespace __IsoAgLib
namespace __IsoAgLib {

#define GROUND_BASED_SPEED_DIST_PGN_DISABLE_MASK       0x0001LU
#define WHEEL_BASED_SPEED_DIST_PGN_DISABLE_MASK        0x0002LU
#define SELECTED_SPEED_MESSAGE_DISABLE_MASK            0x0004LU

  class TracMove_c;
  typedef SINGLETON_DERIVED(TracMove_c,BaseCommon_c) SingletonTracMove_c;
  /** stores, updates  and delivers all moving data information;
      Derive from BaseCommon_c some fundamental funktionality for all base data
      Derive from ElementBase_c to register in Scheduler_c for timeEvent trigger
      Derive from CANCustomer to register FilterBox'es in CanIo_c to receive CAN messages
      Derive from SINGLETON to create a Singleton which manages one global accessible singleton
      per IsoAgLib instance (if only one IsoAgLib instance is defined in application config, no overhead is produced).
    */

  class TracMove_c : public SingletonTracMove_c
  {
  public:// Public methods

    /** initialize directly after the singleton instance is created.
      this is called from singleton.h and should NOT be called from the user again.
      users please use init(...) instead.
    */
    void singletonInit();

    /** initialise element which can't be done during construct;
        above all create the needed FilterBox_c instances
        possible errors:
          * dependant error in CanIo_c problems during insertion of new FilterBox_c entries for IsoAgLibBase
        @param apc_isoName optional pointer to the ISOName variable of the responsible member instance (pointer enables automatic value update if var val is changed)
        @param ai_singletonVecKey singleton vector key in case PRT_INSTANCE_CNT > 1
        @param at_identMode either IsoAgLib::IdentModeImplement or IsoAgLib::IdentModeTractor
      */
    virtual void init_base (const IsoName_c*, int ai_singletonVecKey, IsoAgLib::IdentMode_t at_identMode = IsoAgLib::IdentModeImplement);
    /** config the TracMove_c object after init -> set pointer to isoName and
        config send/receive of a moving msg type
        @param apc_isoName pointer to the ISOName variable of the responsible member instance (pointer enables automatic value update if var val is changed)
        @param at_identMode either IsoAgLib::IdentModeImplement or IsoAgLib::IdentModeTractor
      */
    virtual bool config_base (const IsoName_c* apc_isoName, IsoAgLib::IdentMode_t at_identMode, uint16_t aui16_suppressMask = 0);

    /** update selected speed with actually best available speed
        @param t_speedSrc  from which source is the speed available
      */
    void updateSpeed(IsoAgLib::SpeedSource_t t_speedSrc);

    /** update distance and direction with the actually best available distance and direction
        @param t_distanceSrc  from which source is the distance and direction available
      */
    void updateDistanceDirection(IsoAgLib::DistanceDirectionSource_t t_distanceSrc);

    /** destructor for TracMove_c which has nothing to do */
    virtual ~TracMove_c() { BaseCommon_c::close();}

    /* ******************************************* */
    /** \name Set Values for periodic send on BUS  */
    /*@{*/
    /** set the theoretical (gear calculated) driven distance
        @param rreflVal value to store as theoretical (gear calculated) driven distance
      */
    void setDistTheor(const uint32_t& rreflVal) { mui32_distTheor = rreflVal; }

    /** set the real (radar measured) driven distance
        @param rreflVal value to store as real radar measured distance
      */
    void setDistReal(const uint32_t& rreflVal) { mui32_distReal = rreflVal; }

    /** set the value of real speed (measured by radar)
        @param ai16_val value to store as real radar measured speed
      */
    void setSpeedReal(const int32_t& ai32_val)
			{mi32_speedReal = ai32_val; if(mt_selectedSpeedSource == IsoAgLib::IsoGroundBasedSpeed) mi32_selectedSpeed = ai32_val;}

    /** set the value of theoretical speed (calculated from gear)
        @param ai16_val value to store as theoretical gear calculated speed
      */
    void setSpeedTheor(const int32_t& ai32_val)
			{mi32_speedTheor = ai32_val; if(mt_selectedSpeedSource == IsoAgLib::IsoWheelBasedSpeed) mi32_selectedSpeed = ai32_val;}

    /** set measured signal indicating either forward or reverse as the real (radar measured) direction of travel
        @return  direction of travel
      */
    void setDirectionReal(IsoAgLib::IsoDirectionFlag_t t_val)
			{mt_directionReal = t_val; if(mt_selectedSpeedSource == IsoAgLib::IsoGroundBasedSpeed) mt_selectedDirection = t_val;}

    /** set measured signal indicating either forward or reverse as the theoretical (gear calculated) direction of travel
        @return  direction of travel
      */
    void setDirectionTheor(IsoAgLib::IsoDirectionFlag_t t_val)
			{mt_directionTheor = t_val; if(mt_selectedSpeedSource == IsoAgLib::IsoWheelBasedSpeed) mt_selectedDirection = t_val;}

    /** set parameter which indicates whetcher the reported direction is reversed from the perspective of the operator
        @param at_val  indicates direction (IsoInactive = not reversed; IsoActive = reversed)
      */
    void setOperatorDirectionReversed(const IsoAgLib::IsoOperatorDirectionFlag_t at_val) { mt_operatorDirectionReversed = at_val;}

    /** start/stop state BE AWARE THIS IS A DUMMY BECAUSE DESCRIPTION IS NOT TO FIND IN AMENDMENT 1*/
    void setStartStopState(const IsoAgLib::IsoActiveFlag_t at_val) {mt_startStopState = at_val;}

    /** set actual distance traveled by the machine based on the value of selected machine speed
        @param i32_val  actual distance
      */
    void setSelectedDistance(const uint32_t& i32_val) {mui32_selectedDistance = i32_val;}

    /** set current direction of travel of the machine
        @param t_val  current direction of travel
      */
    void setSelectedDirection(IsoAgLib::IsoDirectionFlag_t t_val) {mt_selectedDirection = t_val;}

    /** get current value of the speed as determined from a number of sources by the machine
        @param i32_val  current value of speed
      */
    void setSelectedSpeed(const int32_t& i32_val)  {mi32_selectedSpeed = i32_val;}

    /** set speed source that is currently being reported in the machine speed parameter
        @param t_val  actual speed source
      */
    void setSelectedSpeedSource(IsoAgLib::IsoSpeedSourceFlag_t t_val) {mt_selectedSpeedSource = t_val;}

    /** present limit status of selected speed
        @param t_val  limit status
      */
    void setSelectedSpeedLimitStatus(const IsoAgLib::IsoLimitFlag_t t_val) {mt_selectedSpeedLimitStatus = t_val;}
    /*@}*/


    /* ****************************************************** */
    /** \name Retrieve Values which are sent from other ECUs  */
    /*@{*/
    /** get the real driven distance with int16_t val
        @return actual radar measured driven distance value
      */
    uint32_t distReal() const { return mui32_distReal;}

    /** get the real driven distance with int16_t val
        @return actual gear calculated driven distance value
      */
    uint32_t distTheor() const { return mui32_distTheor;}

    /** get the value of real speed (measured by radar)
        @return actual radar measured speed value
      */
    int32_t speedReal() const { return mi32_speedReal;}

    /** is looking for a valid speed value
        @return true if speed is valid otherwise false
      */
    bool isRealSpeedUsable() const;

    /** is looking for a missing speed value
        @return true if speed is missing otherwise false
      */
    bool isRealSpeedMissing() const { return (mi32_speedReal == NO_VAL_32S)?true:false;}

    /** is looking for a erroneous speed value
        @return true if speed is erroneous otherwise false
      */
    bool isRealSpeedErroneous() const { return (mi32_speedReal == ERROR_VAL_32S)?true:false;}

    /** get the value of theoretical speed (calculated from gear)
        @return theoretical gear calculated speed value
      */
    int32_t speedTheor() const { return mi32_speedTheor;}

    /** is looking for a valid speed value
        @return true if speed is valid otherwise false
      */
    bool isTheorSpeedUsable() const;

    /** is looking for a missing speed value
        @return true if speed is missing otherwise false
      */
    bool isTheorSpeedMissing() const { return (mi32_speedTheor == NO_VAL_32S)?true:false;}

    /** is looking for a erroneous speed value
        @return true if speed is erroneous otherwise false
      */
    bool isTheorSpeedErroneous() const { return (mi32_speedTheor == ERROR_VAL_32S)?true:false;}

    /** get measured signal indicating either forward or reverse as the theoretical (gear calculated) direction of travel
        @return  direction of travel
      */
    IsoAgLib::IsoDirectionFlag_t directionTheor() {return mt_directionTheor;}

    /** get measured signal indicating either forward or reverse as the real (radar measured) direction of travel
        @return  direction of travel
      */
    IsoAgLib::IsoDirectionFlag_t directionReal() {return mt_directionReal;}

    /** get parameter which indicates whetcher the reported direction is reversed from the perspective of the operator
        @return indicates direction (IsoInactive = not reversed; IsoActive = reversed)
      */
    IsoAgLib::IsoOperatorDirectionFlag_t operatorDirectionReversed()const { return mt_operatorDirectionReversed;}

    /** start/stop state BE AWARE THIS IS A DUMMY BECAUSE DESCRIPTION IS NOT TO FIND IN AMENDMENT 1*/
    IsoAgLib::IsoActiveFlag_t startStopState() const {return mt_startStopState;}

    /** get actual distance traveled by the machine based on the value of selected machine speed
        @return  actual distance traveled
      */
    uint32_t selectedDistance() const {return mui32_selectedDistance;}

    /** get current direction of travel of the machine
        @return  current direction of travel
      */
    IsoAgLib::IsoDirectionFlag_t selectedDirection() const {return mt_selectedDirection;}

    /** get current value of the speed as determined from a number of sources by the machine
        @return  current value of speed
      */
    int32_t selectedSpeed() const {return mi32_selectedSpeed;}

    /** is looking for a valid speed value
        @return true if speed is valid otherwise false
      */
    bool isSelectedSpeedUsable() const;

    /** is looking for a missing speed value
        @return true if speed is missing otherwise false
      */
    bool isSelectedSpeedMissing() const { return (mi32_selectedSpeed == NO_VAL_32S)?true:false; }

    /** is looking for a erroneous speed value
        @return true if speed is erroneous otherwise false
      */
    bool isSelectedSpeedErroneous() const { return (mi32_selectedSpeed == ERROR_VAL_32S)?true:false; }

    /** present limit status of selected speed
        @return  limit status
      */
    IsoAgLib::IsoLimitFlag_t selectedSpeedLimitStatus() const {return mt_selectedSpeedLimitStatus;}

    /** get speed source that is currently being reported in the machine speed parameter
        @return  speed source that is currently being reported
      */
    IsoAgLib::IsoSpeedSourceFlag_t selectedSpeedSource() const {return mt_selectedSpeedSource;}


  virtual const char* getTaskName() const;
  /** dummy implementation */
  virtual bool processMsgRequestPGN (uint32_t aui32_pgn, IsoItem_c* apc_isoItemSender, IsoItem_c* apc_isoItemReceiver);

  private:
    // Private methods
    friend class SINGLETON_DERIVED(TracMove_c,BaseCommon_c);
    /** HIDDEN constructor for a TracMove_c object instance which can optional
        set the configuration for send/receive for a moving msg
        NEVER instantiate a variable of type TracMove_c within application
        only access TracMove_c via getTracMoveInstance() or getTracMoveInstance( int riLbsBusNr ) in case more than one BUS is used for IsoAgLib
      */
    TracMove_c() {}

    /** check if filter boxes shall be created - create only filters based
        on active local idents which has already claimed an address
        --> avoid to much Filter Boxes
      */
    virtual void checkCreateReceiveFilter( );

    /** send a ISO11783 moving information PGN.
      * this is only called when sending ident is configured and it has already claimed an address
        @pre  function is only called in tractor mode
        @see  BaseCommon_c::timeEvent()
      */
    virtual bool timeEventTracMode();

    /** send a ISO11783 moving information PGN.
      * this is only called when sending ident is configured and it has already claimed an address
        @pre  function is only called in implement mode
        @see  BaseCommon_c::timeEvent()
      */
    virtual bool timeEventImplMode();

    /** process a ISO11783 base information PGN
        @pre  sender of message is existent in monitor list
        @see  CanPkgExt_c::resolveSendingInformation()
      */
    virtual bool processMsg();

    /** send moving data with ground&theor speed&dist
        @see  CanIo_c::operator<<
      */
    void sendMovingTracMode( );

  private:
    // Private attributes
    /** actually selected distance and direction source */
    IsoAgLib::DistanceDirectionSource_t mt_distDirecSource;

    /** last time when selected speed data was processed */
    uint32_t mui32_lastUpdateTimeSpeed;

    /** last time when direction and distance data was processed */
    uint32_t mui32_lastUpdateTimeDistDirec;


    /************ DISTANCE **************/

    /** real distance as int32_t value (cumulates 16bit overflows) */
    uint32_t mui32_distReal;

    /** theoretical distance as int32_t value (cumulates 16bit overflows)*/
    uint32_t mui32_distTheor;

    /** last 16bit real distance to cope with 16bit overflows */
    //uint16_t ui32_lastDistReal;

    /** last 16bit theoretical distance to cope with 16bit overflows */
    //uint16_t ui32_lastDistTheor;

    /** actual distance traveled by the machine based on the value of selected machine speed */
    uint32_t mui32_selectedDistance;

    /** start/stop state BE AWARE THIS IS A DUMMY BECAUSE DESCRIPTION IS NOT TO FIND IN AMENDMENT 1*/
    IsoAgLib::IsoActiveFlag_t mt_startStopState;


    /************ DIRECTION *************/
    /** parameter indicates whetcher the reported direction is reversed from the perspective of the operator */
    IsoAgLib::IsoOperatorDirectionFlag_t mt_operatorDirectionReversed;

    /** measured signal indicating either forward or reverse as the direction of travel */
    IsoAgLib::IsoDirectionFlag_t mt_directionTheor;

    /** measured signal indicating either forward or reverse as the direction of travel */
    IsoAgLib::IsoDirectionFlag_t mt_directionReal;

    /** indicates the current direction of travel of the machine */
    IsoAgLib::IsoDirectionFlag_t mt_selectedDirection;


    /************ SPEED ***************/
    /** actually selected speed source */
    IsoAgLib::SpeedSource_t mt_speedSource;

    /** real speed */
    int32_t mi32_speedReal;

    /** theoretical speed */
    int32_t mi32_speedTheor;

    /** current value of the speed as determined from a number of sources by the machine */
    int32_t mi32_selectedSpeed;

    /** present limit status of selected speed */
    IsoAgLib::IsoLimitFlag_t mt_selectedSpeedLimitStatus;

    /** indicates the speed source that is currently being reported in the machine speed parameter */
    IsoAgLib::IsoSpeedSourceFlag_t mt_selectedSpeedSource;
  };

  #if defined(PRT_INSTANCE_CNT) && (PRT_INSTANCE_CNT > 1)
  /** C-style function, to get access to the unique Base_c singleton instance
    * if more than one CAN BUS is used for IsoAgLib, an index must be given to select the wanted BUS
    */
  TracMove_c& getTracMoveInstance(uint8_t aui8_instance = 0);
  #else
  /** C-style function, to get access to the unique Base_c singleton instance */
  TracMove_c& getTracMoveInstance(void);
  #endif
}
#endif