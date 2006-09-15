/***************************************************************************
                          monitoritem_c.h  - base class for member lists;
                                    stores access time
                                    and pointer to root Scheduler_c object
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
#ifndef MONITOR_ITEM_H
#define MONITOR_ITEM_H

/* *************************************** */
/* ********** include headers ************ */
/* *************************************** */
#include <IsoAgLib/typedef.h>
#include <IsoAgLib/util/impl/singleton.h>
#include <IsoAgLib/util/impl/devkey_c.h>
//#include "baseitem_c.h"

// Begin Namespace __IsoAgLib
namespace __IsoAgLib {

/**
  base class for member lists; stores item state, access time, device class,
  device class instance, member number and name.
  @author Dipl.-Inform. Achim Spangler
*/
class MonitorItem_c  : public BaseItem_c {
private:

public:
  /**
    constructor which takes optional the pointer to the containing Scheduler_c instance
    and the actual time as parameter to initialise all own values
    @param ri32_time optional timestamp to store as last update
    @param rc_devKey DEV_KEY code of this item
    @param rui8_nr number of this item (for real services equal to devClass)
    @param rb_status state of this ident (off, claimed address, ...) (default: off)
    @param ri_singletonVecKey optional key for selection of IsoAgLib instance (default 0)
  */
  MonitorItem_c(int32_t ri32_time = 0, const DevKey_c& rc_devKey = DevKey_c::DevKeyUnspecified, uint8_t rui8_nr = 0xF,
              IState_c::itemState_t rb_status = IState_c::IstateNull, int ri_singletonVecKey = 0);

  /**
    copy constructor which takes it initial values from another MonitorItem_c instance
    @param rrefc_monitorItem reference to the source MonitorItem_c instance
  */
  MonitorItem_c(const MonitorItem_c& rrefc_monitorItem);

  /** destructor which sets the update timestamp to 0 */
  virtual ~MonitorItem_c();

  /**
    operator= which defines src as const to avoid
    compile warnings with the automatic generated version
    @param rrefc_src reference to source MonitorItem_c to copy values from
    @return reference to the source MonitorItem_c instance for assign chains like "base1 = base2 = base3;"
  */
  MonitorItem_c& operator=(const MonitorItem_c& src);

  /**
    set all element data with one call
    @param ri32_time creation time of this item instance
    @param rc_devKey DEV_KEY code of this item
    @param rui8_nr number of this item (for real services equal to devClass)
    @param ren_status status information of this ident (IState_c::Off, IState_c::Active, ...)
        (default: IState_c::Active)
    @param ri_singletonVecKey optional key for selection of IsoAgLib instance (default 0)
  */
  void set(int32_t ri32_time = 0, const DevKey_c& rc_devKey = DevKey_c::DevKeyUnspecified, uint8_t rui8_nr = 0xF,
           itemState_t ren_status = IState_c::Active, int ri_singletonVecKey = 0);

  /**
    set DEV_KEY code of this item
    @param rc_devKey DEV_KEY
  */
  void setDevKey(const DevKey_c& rc_devKey){c_devKey = rc_devKey;}

  /**
    set number of this item
    @param rc_devKey number
  */
  void setNr(uint8_t rui8_nr){ui8_nr = rui8_nr;}

  /**
    deliver the DEV_KEY code of this item
    @return DEV_KEY code
  */
  const DevKey_c& devKey()const{return c_devKey;}

  /**
    deliver the DEVCLASS code alone (derived from devKey)
    @return DEVCLASS code
  */
  uint8_t devClass()const{return c_devKey.getDevClass();}

  /**
    deliver the device class inst code alone (derived from devKey)
    @return device class inst code
  */
  uint8_t devClassInst()const{return c_devKey.getDevClassInst();}

  /**
    deliver the number/adress of this item
    @return number
  */
  uint8_t nr()const{return ui8_nr;}

  /**
    deliver name
    @return pointer to the name uint8_t string (7byte)
  */
  virtual const uint8_t* name() const = 0;

  /**
    check if the name field is empty (no name received)
    @return true -> no name received
  */
  virtual bool isEmptyName() const = 0;

  /**
    deliver name as pure ASCII string
    @param pc_name string where ASCII string is inserted
    @param rui8_maxLen max length for name
  */
  virtual void getPureAsciiName(int8_t *pc_asciiName, uint8_t rui8_maxLen) = 0;

  /**
    lower comparison with another MonitorItem_c on the rigth (compare the DEV_KEY)
    @param rrefc_right rigth parameter for lower compare
  */
  bool operator<(const MonitorItem_c& rrefc_right) const
    {return (devKey() < rrefc_right.devKey())?true:false;}

  /**
    lower comparison with DEV_KEY uint8_t on the rigth
    @param rrefc_right rigth parameter for lower compare
  */
  bool operator<(const DevKey_c& rc_devKey)const{return (devKey() < rc_devKey)?true:false;}

  /**
    lower comparison between left DEV_KEY uint8_t and right MonitorItem
    @param rb_left DEV_KEY uint8_t left parameter
    @param rrefc_right rigth ServiceItem_c parameter
  */
  friend bool operator<(const DevKey_c& rc_left, const MonitorItem_c& rrefc_right);

  /**
    lower comparison between left MonitorItem_c and right DEV_KEY uint8_t
    @param rrefc_left left ServiceItem_c parameter
    @param rb_right DEV_KEY uint8_t right parameter
  */
  friend bool lessThan(const MonitorItem_c& rrefc_left, const DevKey_c& rc_right);

  /**
    equality comparison with DEV_KEY uint8_t on the rigth
    @param rrefc_right rigth parameter for lower compare
  */
  bool operator==(const DevKey_c& rc_right)const { return (devKey() == rc_right)?true:false;}

  /**
    difference comparison with DEV_KEY uint8_t on the rigth
    @param rrefc_right rigth parameter for lower compare
  */
  bool operator!=(const DevKey_c& rc_right) const{ return (devKey() != rc_right)?true:false;}

  /**
    compare given number to nr of this item and return result
    @param rui8_nr compared number
    @return true -> given number equal to nr of this item
  */
  bool equalNr(const uint8_t rui8_nr)const{return (nr() == rui8_nr)?true:false;}

protected:
  /** DEV_KEY of element */
  DevKey_c c_devKey;
private:
// Private attributes
  /** number of element */
  uint8_t ui8_nr : 8;
};

/**
  lower comparison between left MonitorItem_c and right DEV_KEY uint8_t
  @param rrefc_left left ServiceItem_c parameter
  @param rb_right DEV_KEY uint8_t right parameter
*/
bool lessThan(const MonitorItem_c& rrefc_left, const DevKey_c& rc_right);

}
#endif
