/***************************************************************************
                          simplemanagesetpointlocal_c.h - managing of simple
                                                       local process data object
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
#ifndef SIMPLE_MANAGE_SETPOINT_LOCAL_H
#define SIMPLE_MANAGE_SETPOINT_LOCAL_H

/* *************************************** */
/* ********** include headers ************ */
/* *************************************** */
#include "../../../impl/processelementbase_c.h"
#include "../../../impl/proc_c.h"

// Begin Namespace IsoAgLib
namespace __IsoAgLib {

// **************************************************************************************
// *************************** SimpleManageSetpointLocal_c ***************************
// **************************************************************************************


/**
  simple management of a Setpoint value
  without flexible setpoints.
  */
class SimpleManageSetpointLocal_c : public ProcessElementBase_c
{
 public:
  /**
    constructor which initialse both pointers if given
    @param rpc_processData optional pointer to containing ProcessData instance
  */
  SimpleManageSetpointLocal_c( ProcDataBase_c *const rpc_processData = NULL )
  : ProcessElementBase_c( rpc_processData )
  { init( rpc_processData );};
  /**
    initialise this SimpleManageSetpointLocal_c instance to a well defined initial state
    @param rpc_processData optional pointer to containing ProcessData instance
  */
  void init( ProcDataBase_c *const rpc_processData = NULL );
  /** copy constructor */
  SimpleManageSetpointLocal_c( const SimpleManageSetpointLocal_c& rrefc_src );
  /** assignment operator */
  const SimpleManageSetpointLocal_c& operator=( const SimpleManageSetpointLocal_c& rrefc_src );
  /** processing of a setpoint message */
  void processSetpoint();
  /**
    send a exact-setpoint to a specified target (selected by GPT)
    @param rc_targetGtp GetyPos of target
    @param ren_type optional PRI specifier of the message (default Proc_c::Target )
    @return true -> successful sent
  */
  bool sendSetpoint( GetyPos_c rc_targetGtp, Proc_c::progType_t ren_progType = Proc_c::Target ) const
   { return sendSetpointMod( 0, rc_targetGtp, ren_progType );};
  /**
    send a sub-setpoint (selected by MOD) to a specified target (selected by GPT)
    @param rui8_mod select sub-type of setpoint
    @param rc_targetGtp GetyPos of target
    @param ren_type optional PRI specifier of the message (default Proc_c::Target )
    @return true -> successful sent
  */
  bool sendSetpointMod( uint8_t rui8_mod, GetyPos_c rc_targetGtp, Proc_c::progType_t ren_progType = Proc_c::Target ) const;
  #ifdef SIMPLE_SETPOINT_WITH_PERCENT
  /**
    send a percentual-setpoint to a specified target (selected by GPT)
    @param rc_targetGtp GetyPos of target
    @param ren_type optional PRI specifier of the message (default Proc_c::Target )
    @return true -> successful sent
  */
  bool sendSetpointPercent( GetyPos_c rc_targetGtp, Proc_c::progType_t ren_progType = Proc_c::Target ) const
   { return sendSetpointMod( 1, rc_targetGtp, ren_progType );};
  /**
    retreive simple master setpoint
    @param rb_percented optional calculate with percent value (default false)
    @return actual received setpoint value (calculated with setpoint)
  */
  int32_t setpointMasterVal(bool rb_percented = false ) const;
  /**
    retrieve percent setpoint value
    @return percent value (set to initial 100)
  */
  uint8_t setpointPercentVal()const {return b_setpointPercentVal;};
  /**
    set the setpoint percent value
    @param rb_percent new percent setpoint value
  */
  void setSetpointPercentVal(uint8_t rb_percent){b_setpointPercentVal = rb_percent;};
    #ifdef USE_FLOAT_DATA_TYPE
  /**
    retreive simple master setpoint
    @param rb_percented optional calculate with percent value (default false)
    @return actual received setpoint value (calculated with setpoint)
  */
  float setpointMasterValFloat(bool rb_percented = false ) const;
    #endif
  #else
  /**
    retreive simple master setpoint
    @return actual received setpoint value
  */
  int32_t setpointMasterVal() const;
    #ifdef USE_FLOAT_DATA_TYPE
  /**
    retreive simple master setpoint
    @return actual received setpoint value
  */
  float setpointMasterValFloat() const;
    #endif
  #endif
  /**
    set the setpoint value
    @param ri32_val new setpoint value
  */
  void setSetpointMasterVal(int32_t ri32_val);
  #ifdef USE_FLOAT_DATA_TYPE
  /**
    set the setpoint value as float value
    @param rf_val new setpoint value
  */
  void setSetpointMasterVal(float rf_val);
  #endif
 protected:
  #if !defined(HANDLE_SETPOINT_MEASURE_EQUIVALENT)
    #ifdef USE_FLOAT_DATA_TYPE
    /** define simple setpoint master value */
    union {
      int32_t i32_setpointMasterVal;
      float f_setpointMasterVal;
    };
    #else // USE_FLOAT_DATA_TYPE
    /** define simple setpoint master value */
    int32_t i32_setpointMasterVal;
    #endif // USE_FLOAT_DATA_TYPE
  #endif // !defined(HANDLE_SETPOINT_MEASURE_EQUIVALENT)

  #if defined(SIMPLE_SETPOINT_WITH_PERCENT)
  /** percent setpoint value */
  uint8_t b_setpointPercentVal;
  #endif
};

}
#endif
