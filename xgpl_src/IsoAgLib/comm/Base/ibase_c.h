/***************************************************************************
                          ibase.h  - working on Base Data Msg Type 1, 2
                                     and Calendar; stores, updates  and
                                     delivers all base data informations
                                     from CANCustomer_c derived for CAN
                                     sending and receiving interaction;
                                     from ElementBase_c derived for
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
#ifndef IBASE_H
#define IBASE_H

/* *************************************** */
/* ********** include headers ************ */
/* *************************************** */
#include "impl/base_c.h"
#include <IsoAgLib/util/igetypos_c.h>
#include "ibasetypes.h"

// Begin Namespace IsoAgLib
namespace IsoAgLib {

/**
  working on Base Data Msg Type 1, 2 and Calendar;
  stores, updates  and delivers all base data informations;
  Derive from ElementBase_c to register in Scheduler_c for timeEvent trigger
  Derive from CANCustomer to register FilterBox'es in CANIO_c to receive CAN messages
  Derive from SINGLETON to create a Singleton which manages one global accessible singleton
   per IsoAgLib instance (if only one IsoAgLib instance is defined in application config, no overhead is produced).
  *@author Dipl.-Inform. Achim Spangler
*/
class iBase_c : private __IsoAgLib::Base_c {
public:
  // Public methods
  /**
    initialise element which can't be done during construct;
    above all create the needed FilterBox_c instances, to receive
    the needed CAN msg with base msg type 1,2 and calendar

    possible errors:
        * dependant error in CANIO_c problems during insertion of new FilterBox_c entries for IsoAgLibBase
    @param rpc_gtp optional pointer to the GETY_POS variable of the responsible member instance (pointer enables automatic value update if var val is changed)
    @param rt_mySendSelection optional Bitmask of base data to send ( default send nothing )
  */
  void init(const iGetyPos_c* rpc_gtp = NULL, BaseDataGroup_t rt_mySendSelection = BaseDataNothing )
  {Base_c::init(rpc_gtp, rt_mySendSelection);};

  /**
    config the Base_c object after init -> set pointer to gtp and
    config send/receive of different base msg types
    @param rpc_gtp pointer to the GETY_POS variable of the responsible member instance (pointer enables automatic value update if var val is changed)
    @param rt_mySendSelection optional Bitmask of base data to send ( default send nothing )
  */
  void config(const iGetyPos_c* rpc_gtp, BaseDataGroup_t rt_mySendSelection )
  {Base_c::config(rpc_gtp, rt_mySendSelection );};

  #ifdef USE_ISO_11783
  /**
    send ISO11783 calendar PGN
    @param rc_gtp GETY_POS code off calling item which wants to send
    @parm ri32_time timestamp where calendar was last sent (default autodetect)

    possible errors:
        * dependant error in CANIO_c on CAN send problems
    @see CANPkg_c::getData
    @see CANPkgExt_c::getData
    @see CANIO_c::operator<<
  */
  void isoSendCalendar(const iGetyPos_c& rc_gtp) { Base_c::isoSendCalendar(rc_gtp);};
  #endif

  /* ******************************************* */
  /** \name Set Values for periodic send on BUS  */
  /*@{*/
  /**
    set the value of real speed (measured by radar)
    @param ri16_val value to store as real radar measured speed
  */
  void setSpeedReal(int16_t ri16_val)  { Base_c::setSpeedReal( ri16_val );};
  /**
    set the value of theoretical speed (calculated from gear)
    @param ri16_val value to store as theoretical gear calculated speed
  */
  void setSpeedTheor(int16_t ri16_val)  { Base_c::setSpeedTheor( ri16_val );};

  /**
    set the real (radar measured) driven distance with int16_t val
    @param ri16_val value to store as real radar measured distance
  */
  void setDistReal(int16_t ri16_val)
  { // use function to detect and handle 16bit integer overflows
    Base_c::setDistReal(ri16_val);
  };

  /**
    set the real (radar measured) driven distance with int32_t val
    @param rreflVal value to store as real radar measured distance
  */
  void setDistReal(const int32_t& rreflVal) { Base_c::setDistReal(rreflVal);};

  /**
    set the theoretical (gear calculated) driven distance with int16_t val
    @param ri16_val value to store as theoretical (gear calculated) driven distance
  */
  void setDistTheor(int16_t ri16_val) { Base_c::setDistTheor(ri16_val);};
  /**
    set the theoretical (gear calculated) driven distance with int32_t val
    @param rreflVal value to store as theoretical (gear calculated) driven distance
  */
  void setDistTheor(const int32_t& rreflVal){ Base_c::setDistTheor(rreflVal);};

  /**
    set rear PTO
    @param ri16_val value to store as the speed of the rear PTO
  */
  void setPtoRear(int16_t ri16_val) { Base_c::setPtoRear( ri16_val );};
  /**
    set front PTO
    @param ri16_val value to store as the speed of the front PTO
  */
  void setPtoFront(int16_t ri16_val) { Base_c::setPtoFront( ri16_val );};

  #ifdef USE_DIN_9684
  /** deliver rear left draft */
  void setRearLeftDraft( int16_t ri16_val ) { Base_c::setRearLeftDraft( ri16_val );};
  /** deliver rear right draft */
  void setRearRightDraft( int16_t ri16_val ) { Base_c::setRearRightDraft( ri16_val );};
  /** deliver rear total draft Newton */
  void setRearDraftNewton( int16_t ri16_val ) { Base_c::setRearDraftNewton( ri16_val );};
  /** deliver rear total draft Percent */
  void setRearDraftNominal( uint8_t rui8_val ) { Base_c::setRearDraftNominal( rui8_val );};
  /** deliver fuel consumption L/h */
  void setFuelRate( int16_t ri16_val ) { Base_c::setFuelRate( ri16_val );};
  /** deliver fuel temperature �C */
  void setFuelTemperature( uint8_t rui8_val ) { Base_c::setFuelTemperature( rui8_val );};
  #endif
  #ifdef USE_ISO_11783
  /** set front hitch draft
    * @return front hitch draft [-320.000N;322.550N]; 1N/bit
    */
  void setHitchFrontDraft(int32_t ri32_val) { Base_c::setHitchFrontDraft( ri32_val );};
  /** set rear hitch draft
    * @return rear hitch draft [-320.000N;322.550N]; 1N/bit
    */
  void setHitchRearDraft(int32_t ri32_val) { Base_c::setHitchRearDraft( ri32_val );};
  /** set front hitch nominal link force
    * @return front nominal link force [-100%;100%]; 1 promille per bit
    */
  void setHitchFrontLowerLinkForce(int16_t ri16_val) { Base_c::setHitchFrontLowerLinkForce( ri16_val );};
  /** set rear hitch nominal link force
    * @return rear nominal link force [-100%;100%]; 1 promille per bit
    */
  void setHitchRearLowerLinkForce(int16_t ri16_val) { Base_c::setHitchRearLowerLinkForce( ri16_val );};
  /** set explicit information whether front PTO is engaged
    * @param rc_val IsoActive -> PTO is engaged
    */
  void setPtoFrontEngaged(IsoActiveFlag_t rc_val)  { Base_c::setPtoFrontEngaged( rc_val );};
  /** set explicit information whether rear PTO is engaged
    * @param rc_val IsoActive -> PTO is engaged
    */
  void setPtoRearEngaged(IsoActiveFlag_t rc_val)  { Base_c::setPtoRearEngaged( rc_val );};
  /** set std RPM of front PTO (1000 n/min; 540 n/min)
    * @param rc_val IsoActive == 1000 n/min; false == 540 n/min
    */
  void setPtoFront1000(IsoActiveFlag_t rc_val)  { Base_c::setPtoFront1000( rc_val );};
  /** set std RPM of rear PTO (1000 n/min; 540 n/min)
    * @param rc_val IsoActive == 1000 n/min; false == 540 n/min
    */
  void setPtoRear1000(IsoActiveFlag_t rc_val)  { Base_c::setPtoRear1000( rc_val );};
  /** set economy mode of front PTO
    * @param rc_val IsoActive -> front PTO is in economy mode
    */
  void setPtoFrontEconomy(IsoActiveFlag_t rc_val)  { Base_c::setPtoFrontEconomy( rc_val );};
  /** set economy mode of rear PTO
    * @param rc_val IsoActive -> rear PTO is in economy mode
    */
  void setPtoRearEconomy(IsoActiveFlag_t rc_val)  { Base_c::setPtoRearEconomy( rc_val );};

  /**
    * set the ISO key switch state of the tractor
    * @param rc_val IsoActive -> key switch ON
    */
  void setKeySwitch(IsoActiveFlag_t rc_val)  { Base_c::setKeySwitch( rc_val );};
  /** set the maximum power time of the tractor in [min]
    * @return maximum power time of the tractor in [min]
    */
  void setMaxPowerTime(uint8_t rui8_val)  { Base_c::setMaxPowerTime( rui8_val );};
  /** force maintain power from tractor
    * @param rb_ecuPower true -> maintain ECU power
    * @param rb_actuatorPower -> maintain actuator power
    * @param rt_implTransport IsoActive -> implement is in transport state
    * @param rt_implPark IsoActive -> implement is in park state
    * @param rt_implWork IsoActive -> implement is in work state
    */
  void forceMaintainPower( bool rb_ecuPower, bool rb_actuatorPower, IsoActiveFlag_t rt_implTransport,
    IsoActiveFlag_t rt_implPark, IsoActiveFlag_t rt_implWork)
    {Base_c::forceMaintainPower(rb_ecuPower, rb_actuatorPower, rt_implTransport, rt_implPark, rt_implWork);};

  #endif
  /**
    set engine speed
    @param ri16_val value to store as engine rpm value
  */
  void setEngine(int16_t ri16_val) { Base_c::setEngine( ri16_val );};

  /**
    set rear hitch
    @param rb_val uint8_t value to store as position of rear hitch
  */
  void setHitchRear(uint8_t rb_val) { Base_c::setHitchRear( rb_val );};
  /**
    set front hitch
    @param rb_val uint8_t value to store as position of front hitch
  */
  void setHitchFront(uint8_t rb_val) { Base_c::setHitchFront( rb_val );};

  /**
    set the calendar value
    @param ri16_year value to store as year
    @param rb_month value to store as month
    @param rb_day value to store as day
    @param rb_hour value to store as hour
    @param rb_minute value to store as minute
    @param rb_second value to store as second
  */
  void setCalendar(int16_t ri16_year, uint8_t rb_month, uint8_t rb_day, uint8_t rb_hour, uint8_t rb_minute, uint8_t rb_second)
  {Base_c::setCalendar(ri16_year, rb_month, rb_day, rb_hour, rb_minute, rb_second);};
  /*@}*/

  /* ****************************************************** */
  /** \name Retrieve Values which are sent from other ECUs  */
  /*@{*/

  /**
    get the value of real speed (measured by radar)
    @return actual radar measured speed value
  */
  int16_t speedReal() const { return Base_c::speedReal();};
  /**
    get the value of theoretical speed (calculated from gear)
    @return theoretical gear calculated speed value
  */
  int16_t speedTheor() const { return Base_c::speedTheor();};

  /**
    get the real driven distance with int16_t val
    @return actual radar measured driven distance value
  */
  int32_t distReal() const { return Base_c::distReal();};
  /**
    get the real driven distance with int16_t val
    @return actual gear calculated driven distance value
  */
  int32_t distTheor() const { return Base_c::distTheor();};

  /**
    get rear pto
    @return actual rpm speed of rear PTO
  */
  int16_t ptoRear() const { return Base_c::ptoRear();};
  /**
    get front pto
    @return actual rpm speed of front PTO
  */
  int16_t ptoFront() const { return Base_c::ptoFront();};

  /**
    get engine speed
    @return actual engine rpm speed value
  */
  int16_t engine() const { return Base_c::engine();};

  /**
    get rear hitch
    @return actual position of rear hitch
  */
  uint8_t hitchRear() const { return Base_c::hitchRear();};
  /**
    get front hitch
    @return actual position of front hitch
  */
  uint8_t hitchFront() const { return Base_c::hitchFront();};

  /** check if a calendar information was received since init */
  bool isCalendarReceived() const { return Base_c::isCalendarReceived();};
  /**
    get the calendar year value
    @return actual calendar year value
  */
  int16_t year() const { return Base_c::year();};
  /**
    set the calendar year value
    @param rui16_year actual calendar year value
  */
  void setYear(uint16_t rui16_year) { Base_c::setYear( rui16_year );};
  /**
    get the calendar month value
    @return actual calendar month value
  */
  uint8_t month() const { return Base_c::month();};
  /**
    set the calendar month value
    @param rb_month actual calendar month value
  */
  void setMonth(uint8_t rb_month) { Base_c::setMonth( rb_month );};
  /**
    get the calendar day value
    @return actual calendar day value
  */
  uint8_t day() const { return Base_c::day();};
  /**
    set the calendar day value
    @param rb_day actual calendar day value
  */
  void setDay(uint8_t rb_day) { Base_c::setDay( rb_day );};
  /**
    get the calendar hour value
    @return actual calendar hour value
  */
  uint8_t hour() const { return Base_c::hour();};
  /**
    set the calendar hour value
    @param rb_hour actual calendar hour value
  */
  void setHour(uint8_t rb_hour) { Base_c::setHour( rb_hour );};
  /**
    get the calendar minute value
    @return actual calendar minute value
  */
  uint8_t minute() const { return Base_c::minute();};
  /**
    set the calendar minute value
    @param rb_minute actual calendar minute value
  */
  void setMinute(uint8_t rb_minute) { Base_c::setMinute( rb_minute );};
  /**
    get the calendar second value
    @return actual calendar second value
  */
  uint8_t second() const { return Base_c::second();};
  /**
    set the calendar second value
    @param rb_second actual calendar second value
  */
  void setSecond(uint8_t rb_second) { Base_c::setSecond( rb_second );};

  /**
    deliver the gtp of the sender of the base data

    possible errors:
        * Err_c::range rui8_typeNr doesn't match valid base msg type number
    @param rt_typeGrp base msg type no of interest: BaseDataGroup1 | BaseDataGroup2 | BaseDataCalendar
    @return GETY_POS code of member who is sending the intereested base msg type
  */
  const iGetyPos_c& senderGtp(BaseDataGroup_t rt_typeGrp) { return static_cast<const iGetyPos_c&>(Base_c::senderGtp( rt_typeGrp ));};

  #ifdef USE_DIN_9684
  /** deliver rear left draft */
  int rearLeftDraft() const { return Base_c::rearLeftDraft();};
  /** deliver rear right draft */
  int rearRightDraft() const { return Base_c::rearRightDraft();};
  /** deliver rear total draft Newton */
  int rearDraftNewton() const { return Base_c::rearDraftNewton();};
  /** deliver rear total draft Percent */
  int rearDraftNominal() const { return Base_c::rearDraftNominal();};
  /** deliver fuel consumption L/h */
  int fuelRate() const { return Base_c::fuelRate();};
  /** deliver fuel temperature �C */
  int fuelTemperature() const { return Base_c::fuelTemperature();};
  #endif
  #ifdef USE_ISO_11783
  /** deliver front hitch draft
    * @return front hitch draft [-320.000N;322.550N]; 1N/bit
    */
  int32_t hitchFrontDraft() const { return Base_c::hitchFrontDraft();};
  /** deliver rear hitch draft
    * @return rear hitch draft [-320.000N;322.550N]; 1N/bit
    */
  int32_t hitchRearDraft() const { return Base_c::hitchRearDraft();};
  /** deliver front hitch nominal link force
    * @return front nominal link force [-100%;100%]; 1 promille per bit
    */
  int16_t hitchFrontLowerLinkForce() const { return Base_c::hitchFrontLowerLinkForce();};
  /** deliver rear hitch nominal link force
    * @return rear nominal link force [-100%;100%]; 1 promille per bit
    */
  int16_t hitchRearLowerLinkForce() const { return Base_c::hitchRearLowerLinkForce();};
  /** deliver explicit information whether front PTO is engaged
    * @return IsoActive -> PTO is engaged
    */
  IsoActiveFlag_t ptoFrontEngaged() const { return Base_c::ptoFrontEngaged();};
  /** deliver explicit information whether rear PTO is engaged
    * @return IsoActive -> PTO is engaged
    */
  IsoActiveFlag_t ptoRearEngaged() const { return Base_c::ptoRearEngaged();};
  /** deliver std RPM of front PTO (1000 n/min; 540 n/min)
    * @return IsoActive == 1000 n/min; false == 540 n/min
    */
  IsoActiveFlag_t ptoFront1000() const { return Base_c::ptoFront1000();};
  /** deliver std RPM of rear PTO (1000 n/min; 540 n/min)
    * @return IsoActive == 1000 n/min; false == 540 n/min
    */
  IsoActiveFlag_t ptoRear1000() const { return Base_c::ptoRear1000();};
  /** deliver economy mode of front PTO
    * @return IsoActive -> front PTO is in economy mode
    */
  IsoActiveFlag_t ptoFrontEconomy() const { return Base_c::ptoFrontEconomy();};
  /** deliver economy mode of rear PTO
    * @return IsoActive -> rear PTO is in economy mode
    */
  IsoActiveFlag_t ptoRearEconomy() const { return Base_c::ptoRearEconomy();};

  /**
    * deliver the ISO key switch state of the tractor
    * @return IsoActive -> key switch ON
    */
  IsoActiveFlag_t keySwitch() const { return Base_c::keySwitch();};
  /** deliver the maximum power time of the tractor in [min]
    * @return maximum power time of the tractor in [min]
    */
  uint8_t maxPowerTime() const { return Base_c::maxPowerTime();};

  /** deliver last receive time of maintain power request
    * @return time in [ms] since system start -> comparable to system time
    */
  int32_t lastMaintainPowerRequest() const { return Base_c::lastMaintainPowerRequest();};

  /** check whether maintenance of ECU power was requested */
  bool maintainEcuPower() const { return Base_c::maintainEcuPower();};

  /** check whether maintenance of actuator power was requested */
  bool maintainActuatorPower() const { return Base_c::maintainActuatorPower();};
  /** check whether maintenance of power
    * for implement in transport state was requested */
  bool maintainPowerForImplInTransport() const { return Base_c::maintainPowerForImplInTransport();};
  /** check whether maintenance of power
    * for implement in park state was requested */
  bool maintainPowerForImplInPark() const { return Base_c::maintainPowerForImplInPark();};
  /** check whether maintenance of power
    * for implement in work state was requested */
  bool maintainPowerForImplInWork() const { return Base_c::maintainPowerForImplInWork();};

  /** deliver raw GPS Latitude [degree] with scaling 10.0e-7 */
  int32_t getGpsLatitudeDegree10Minus7( void ) const { return Base_c::getGpsLatitudeDegree10Minus7(); };
  /** deliver raw GPS Longitude [degree] with scaling 10.0e-7 */
  int32_t getGpsLongitudeDegree10Minus710Minus7( void ) const { return Base_c::getGpsLongitudeDegree10Minus710Minus7(); };
  #if defined(USE_FLOAT_DATA_TYPE) || defined(USE_DIN_GPS)
  /** deliver Minute GPS Latitude */
  float getGpsLatitudeMinute( void ) const { return Base_c::getGpsLatitudeMinute(); };
  /** deliver Minute GPS Longitude */
  float getGpsLongitudeMinute( void ) const { return Base_c::getGpsLongitudeMinute(); };
  #endif
  /** deliver GPS altitude - [cm] */
  uint32_t getGpsAltitudeCm( void ) const { return Base_c::getGpsAltitudeCm(); };
  /** deliver GPS receive qualitiy */
  IsoAgLib::IsoGpsRecMode_t getGpsMode( void ) const { return Base_c::getGpsMode();};
  /** deliver GPS speed as [cm/s] */
  uint16_t getGpsSpeedCmSec( void ) const { return Base_c::getGpsSpeedCmSec();};
  /** deliver GPS Heading [1x10E-4rad] */
  uint16_t getGpsHeadingRad10Minus4( void ) const { return Base_c::getGpsHeadingRad10Minus4(); };
  /*@}*/

  #endif
 private:
  /** allow getIBaseInstance() access to shielded base class.
      otherwise __IsoAgLib::getBaseInstance() wouldn't be accepted by compiler
    */
  #if defined( PRT_INSTANCE_CNT ) && ( PRT_INSTANCE_CNT > 1 )
  friend iBase_c& getIBaseInstance( uint8_t rui8_instance );
  #else
  friend iBase_c& getIBaseInstance( void );
  #endif
};

#if defined( PRT_INSTANCE_CNT ) && ( PRT_INSTANCE_CNT > 1 )
  /** C-style function, to get access to the unique Base_c singleton instance
    * if more than one CAN BUS is used for IsoAgLib, an index must be given to select the wanted BUS
    */
  inline iBase_c& getIBaseInstance( uint8_t rui8_instance = 0 )
  { return static_cast<iBase_c&>(__IsoAgLib::getBaseInstance(rui8_instance));};
#else
  /** C-style function, to get access to the unique Base_c singleton instance */
  inline iBase_c& getIBaseInstance( void )
  { return static_cast<iBase_c&>(__IsoAgLib::getBaseInstance());};
#endif

}
#endif
