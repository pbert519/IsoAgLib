/***************************************************************************
                          measureSubprog.cc - Every increment type of a 
                                               measure prog is managed by a
                                               MeasureSubprog_c instance
                             -------------------
    begin                : Tue Sep 14 1999
    copyright            : (C) 1999 - 2004 by Dipl.-Inform. Achim Spangler
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

/* *************************************** */
/* ********** include headers ************ */
/* *************************************** */
#include "measuresubprog_c.h"
#include <cstdlib>

namespace __IsoAgLib {

/**
  default constructor which can optionally set increment type, increment and running state
  @param ren_type optional increment type (default time proportional)
  @param ri32_increment optional increment value of ren_type (default 0)
  @param rb_started optional running state (default off)
  @param ri32_lastVal optional value of last trigger event (default 0)
*/
MeasureSubprog_c::MeasureSubprog_c(Proc_c::type_t ren_type, int32_t ri32_increment, bool rb_started, int32_t ri32_lastVal){
  en_type = ren_type;
  i32_increment = ri32_increment;
  b_started = rb_started;
  i32_lastVal = ri32_lastVal;
}
#ifdef USE_FLOAT_DATA_TYPE
/**
  default constructor which can optionally set increment type, increment and running state
  @param ren_type optional increment type (default time proportional)
  @param rf_increment increment value of ren_type
  @param rb_started optional running state (default off)
  @param rf_lastVal optional value of last trigger event (default 0)
*/
MeasureSubprog_c::MeasureSubprog_c(Proc_c::type_t ren_type, float rf_increment, bool rb_started, float rf_lastVal){
  en_type = ren_type;
  f_increment = rf_increment;
  b_started = rb_started;
  f_lastVal = rf_lastVal;
}
#endif
/**
  operator= for Subprogs
  @param rrefc_src source instance
  @return reference to source instance for commands like "subp1 = subp2 = subp3;"
*/
const MeasureSubprog_c& MeasureSubprog_c::operator=(const MeasureSubprog_c& rrefc_src){
  b_started = rrefc_src.b_started;
  en_type = rrefc_src.en_type;
  i32_increment = rrefc_src.i32_increment;
  i32_lastVal = rrefc_src.i32_lastVal;
  return rrefc_src;
}

/**
  copy constructor for Subprogs
  @param rrefc_src source instance
*/
MeasureSubprog_c::MeasureSubprog_c(const MeasureSubprog_c& rrefc_src){
  b_started = rrefc_src.b_started;
  en_type = rrefc_src.en_type;
  i32_increment = rrefc_src.i32_increment;
  i32_lastVal = rrefc_src.i32_lastVal;
}

/** default destructor which has nothing to do */
MeasureSubprog_c::~MeasureSubprog_c(){
}

/**
  start a measuring subprogramm, potentially with increment and lastVal definition
  @param ri32_increment optional increment value (default old increment value used)
  @param ri32_lastVal last trigger value (default 0)
*/
void MeasureSubprog_c::start(int32_t ri32_lastVal, int32_t ri32_increment){
  // if wanted store given values (in both cases 0 is interpreted as not wanted)
  if (ri32_increment != 0) i32_increment = ri32_increment;
  if (ri32_lastVal != 0) i32_lastVal = ri32_lastVal;

  // register as started
  b_started = true;
}

 /**
   delivers if given value forces trigger of send of registered informations
   @param ri32_val actual increment relevant value (time for TimeProp, distance for DistProp, ...)
   @return true -> this subprog triggers (e.g. send actual value)
 */
bool MeasureSubprog_c::updateTrigger(int32_t ri32_val){
  if (CNAMESPACE::labs(ri32_val - i32_lastVal) >= i32_increment)
  {
    i32_lastVal = ri32_val;
    return true;
  }
  else
  {
    return false;
  }
}
#ifdef USE_FLOAT_DATA_TYPE
/**
  start a measuring subprogramm, potentially with increment and lastVal definition
  @param rf_increment increment value
  @param rf_lastVal last trigger value (default 0)
*/
void MeasureSubprog_c::start(float rf_increment, float rf_lastVal)
{ // if wanted store given values (in both cases 0 is interpreted as not wanted)
  if (rf_increment != 0) f_increment = rf_increment;
  if (rf_lastVal != 0) f_lastVal = rf_lastVal;

  // register as started
  b_started = true;
}
/**
  delivers if given value forces trigger of send of registered informations
  @param rf_val actual increment relevant value (time for TimeProp, distance for DistProp, ...)
  @return true -> this subprog triggers (e.g. send actual value)
*/
bool MeasureSubprog_c::updateTrigger(float rf_val)
{
  if ( ( ( rf_val > f_lastVal ) && ( ( rf_val - f_lastVal) >= f_increment ) )
    || ( ( rf_val < f_lastVal ) && ( ( f_lastVal - rf_val) >= f_increment ) ) )
  {
    f_lastVal = rf_val;
    return true;
  }
  else
  {
    return false;
  }
}
#endif

/**
  calculate a single identifying value for easier compare of Subprogs
  @return single value for comparison of Subprogs
*/
int32_t MeasureSubprog_c::calcCompVal()const{
  // for Subprog two items are considered equiv if type is identical
  return (en_type);
}

/**
  compare two Subprogs with ==
  @param rrefc_right compared MeasureSubprog_c instance
  @return true -> this instance is equal to the other
*/
bool MeasureSubprog_c::operator==(const MeasureSubprog_c& rrefc_right)const{
  return (calcCompVal() == rrefc_right.calcCompVal());
}

/**
  compare two Subprogs with !=
  @param rrefc_right compared MeasureSubprog_c instance
  @return true -> this instance is different to the other
*/
bool MeasureSubprog_c::operator!=(const MeasureSubprog_c& rrefc_right)const{
  return (calcCompVal() != rrefc_right.calcCompVal());
}

/**
  compare two Subprogs with <
  @param rrefc_right compared MeasureSubprog_c instance
  @return true -> this instance is < than the other
*/
bool MeasureSubprog_c::operator<(const MeasureSubprog_c& rrefc_right)const{
  return (calcCompVal() < rrefc_right.calcCompVal());
}
/**
  compare two Subprogs with <=
  @param rrefc_right compared MeasureSubprog_c instance
  @return true -> this instance is <= than the other
*/
bool MeasureSubprog_c::operator<=(const MeasureSubprog_c& rrefc_right)const{
  return (calcCompVal() <= rrefc_right.calcCompVal());
}
/**
  compare two Subprogs with >
  @param rrefc_right compared MeasureSubprog_c instance
  @return true -> this instance is > than the other
*/
bool MeasureSubprog_c::operator>(const MeasureSubprog_c& rrefc_right)const{
  return (calcCompVal() > rrefc_right.calcCompVal());
}
/**
  compare two Subprogs with >=
  @param rrefc_right compared MeasureSubprog_c instance
  @return true -> this instance is >= than the other
*/
bool MeasureSubprog_c::operator>=(const MeasureSubprog_c& rrefc_right)const{
  return (calcCompVal() >= rrefc_right.calcCompVal());
}

/**
  compare two Subprogs with ==
  @param ren_type compared increment type
  @return true -> this instance is equal to the compared increment type
*/
bool MeasureSubprog_c::operator==(Proc_c::type_t ren_type)const{
  return (en_type == ren_type);
}

/**
  compare two Subprogs with !=
  @param ren_type compared increment type
  @return true -> this instance is different to the compared increment type
*/
bool MeasureSubprog_c::operator!=(Proc_c::type_t ren_type)const{
  return (en_type != ren_type);
}

/**
  compare two Subprogs with <
  @param ren_type compared increment type
  @return true -> this instance is < than the compared increment type
*/
bool MeasureSubprog_c::operator<(Proc_c::type_t ren_type)const{
  return (en_type < ren_type);
}
/**
  compare two Subprogs with <=
  @param ren_type compared increment type
  @return true -> this instance is <= than the compared increment type
*/
bool MeasureSubprog_c::operator<=(Proc_c::type_t ren_type)const{
  return (en_type <= ren_type);
}
/**
  compare two Subprogs with >
  @param ren_type compared increment type
  @return true -> this instance is > than the compared increment type
*/
bool MeasureSubprog_c::operator>(Proc_c::type_t ren_type)const{
  return (en_type > ren_type);
}
/**
  compare two Subprogs with >=
  @param ren_type compared increment type
  @return true -> this instance is >= than the compared increment type
*/
bool MeasureSubprog_c::operator>=(Proc_c::type_t ren_type)const{
  return (en_type >= ren_type);
}

} // end of namespace __IsoAgLib
