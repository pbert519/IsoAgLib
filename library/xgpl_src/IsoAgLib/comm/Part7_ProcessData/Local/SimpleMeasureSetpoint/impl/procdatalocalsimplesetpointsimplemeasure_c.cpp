/***************************************************************************
                          procdatalocalsimplesetpointsimplemeasure_c.cpp - managing of simple
                                                       local process data object
                             -------------------
    begin                : Fri Apr 07 2000
    copyright            : (C) 2000 - 2009 by Dipl.-Inform. Achim Spangler
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
 * Copyright (C) 2000 - 2009 Dipl.-Inform. Achim Spangler                  *
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
#include "procdatalocalsimplesetpointsimplemeasure_c.h"

namespace __IsoAgLib {

// ***************************************************************************************
// ****************** ProcDataLocalSimpleSetpointSimpleMeasure_c ******************
// ***************************************************************************************

/**
  constructor which can set all element vars

  possible errors:
      * Err_c::badAlloc not enough memory to insert first  MeasureProgLocal
  ISO parameter
  @param ps_elementDDI optional pointer to array of structure IsoAgLib::ElementDdi_s which contains DDI, element, isSetpoint and ValueGroup
                       (array is terminated by ElementDdi_s.ui16_element == 0xFFFF)
  @param aui16_element
  @param acrc_isoName optional ISOName code of this instance
  @param apc_externalOverridingIsoName pointer to updated ISOName variable
  @param ab_cumulativeValue
           -# for process data like distance, time, area
               the value of the measure prog data sets is updated
               on master value update dependent on the value increment
               since the last master value update
               -> if a remote member resets his data set copy, datas of
                  other members aren't changed
               -> if this data is saved in EEPROM, the main application
                  needn't take into account the initial EEPROM value, as
                   setting of the master val is independent from EEPROM
           -#  for values like speed, state, rpm aren't updated by increment,
                -> the given master value is propagated equally to all
                    measure prog data sets
                -> if this data is saved in EEPROM, the stored value is loaded
                   as initial master value, and is initially propagated to all
                   measure prog data sets
  @param aui16_eepromAdr optional adress where value is stored in EEPROM
  @param apc_processDataChangeHandler optional pointer to handler class of application
  @param ai_multitonInst optional key for selection of IsoAgLib instance (default 0)
*/
ProcDataLocalSimpleSetpointSimpleMeasure_c::ProcDataLocalSimpleSetpointSimpleMeasure_c(
     const IsoAgLib::ElementDdi_s* ps_elementDDI,
     uint16_t aui16_element,
     const IsoName_c& acrc_isoName,
     const IsoName_c *apc_externalOverridingIsoName,
     bool ab_cumulativeValue,
#ifdef USE_EEPROM_IO
     uint16_t aui16_eepromAdr,
#endif
     IsoAgLib::ProcessDataChangeHandler_c *apc_processDataChangeHandler,
     int ai_multitonInst
     )
  : ProcDataLocalBase_c(ps_elementDDI, aui16_element,
                        acrc_isoName, apc_externalOverridingIsoName, ab_cumulativeValue,
#ifdef USE_EEPROM_IO
                        aui16_eepromAdr,
#endif
                        apc_processDataChangeHandler,
                        ai_multitonInst )
  , mc_setpoint( this )
{
}
/**
  initialise this ProcDataLocalSimpleSetpointSimpleMeasure_c
  instance to a well defined initial state

  possible errors:
      * Err_c::badAlloc not enough memory to insert first  MeasureProgLocal
  ISO parameter
  @param ps_elementDDI optional pointer to array of structure IsoAgLib::ElementDdi_s which contains DDI, element, isSetpoint and ValueGroup
                       (array is terminated by ElementDdi_s.ui16_element == 0xFFFF)
  @param aui16_element
  @param acrc_isoName optional ISOName code of this instance
  @param apc_externalOverridingIsoName pointer to updated ISOName variable
  @param ab_cumulativeValue
          -# for process data like distance, time, area
              the value of the measure prog data sets is updated
              on master value update dependent on the value increment
              since the last master value update
              -> if a remote member resets his data set copy, datas of
                  other members aren't changed
              -> if this data is saved in EEPROM, the main application
                  needn't take into account the initial EEPROM value, as
                  setting of the master val is independent from EEPROM
          -#  for values like speed, state, rpm aren't updated by increment,
                -> the given master value is propagated equally to all
                    measure prog data sets
                -> if this data is saved in EEPROM, the stored value is loaded
                  as initial master value, and is initially propagated to all
                  measure prog data sets
  @param aui16_eepromAdr optional adress where value is stored in EEPROM
  @param apc_processDataChangeHandler optional pointer to handler class of application
  @param ai_multitonInst optional key for selection of IsoAgLib instance (default 0)
*/
void ProcDataLocalSimpleSetpointSimpleMeasure_c::init(const IsoAgLib::ElementDdi_s* ps_elementDDI, uint16_t aui16_element,
                                                      const IsoName_c& acrc_isoName,
                                                      const IsoName_c *apc_externalOverridingIsoName,
                                                      bool ab_cumulativeValue,
#ifdef USE_EEPROM_IO
                                                      uint16_t aui16_eepromAdr,
#endif
                                                      IsoAgLib::ProcessDataChangeHandler_c *apc_processDataChangeHandler,
                                                      int ai_multitonInst
      )
{
  ProcDataLocalBase_c::init(ps_elementDDI, aui16_element,
                            acrc_isoName, apc_externalOverridingIsoName, ab_cumulativeValue,
#ifdef USE_EEPROM_IO
                            aui16_eepromAdr,
#endif
                            apc_processDataChangeHandler,
                            ai_multitonInst );
  mc_setpoint.init( this );
}
/** copy constructor */
ProcDataLocalSimpleSetpointSimpleMeasure_c::ProcDataLocalSimpleSetpointSimpleMeasure_c( const ProcDataLocalSimpleSetpointSimpleMeasure_c& acrc_src )
: ProcDataLocalBase_c( acrc_src ), mc_setpoint( acrc_src.mc_setpoint )
{
}
/** assignment operator */
const ProcDataLocalSimpleSetpointSimpleMeasure_c& ProcDataLocalSimpleSetpointSimpleMeasure_c::operator=( const ProcDataLocalSimpleSetpointSimpleMeasure_c& acrc_src )
{
  ProcDataLocalBase_c::operator=(acrc_src);
  mc_setpoint = acrc_src.mc_setpoint;
  return *this;
}

/** default destructor which has nothing to do */
ProcDataLocalSimpleSetpointSimpleMeasure_c::~ProcDataLocalSimpleSetpointSimpleMeasure_c(){
}


/** process a setpoint message */
void ProcDataLocalSimpleSetpointSimpleMeasure_c::processSetpoint()
{
  mc_setpoint.processSetpoint();
  // call base function to perform some processing steps for all kinds of
  // local process data
  ProcDataLocalBase_c::processSetpoint();
}

} // end of namespace __IsoAgLib
