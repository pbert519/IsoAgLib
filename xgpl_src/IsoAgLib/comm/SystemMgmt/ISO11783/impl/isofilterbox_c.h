/***************************************************************************
                              isofilterbox_c.h
                             -------------------
    begin                : Mon Oct 09 2006
    copyright            : (C) 2000 - 2006 by Dipl.-Inf, Martin Wodok
    email                : m.wodok@osb-ag:de
    type                 : Header
    $LastChangedDate:  $
    $LastChangedRevision:  $
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
#ifndef ISO_FILTER_BOX_C_H
#define ISO_FILTER_BOX_C_H

/* *************************************** */
/* ********** include headers ************ */
/* *************************************** */
#include <IsoAgLib/typedef.h>
#include <IsoAgLib/driver/can/impl/ident_c.h>
#include <IsoAgLib/comm/SystemMgmt/ISO11783/impl/isoname_c.h>

// stl
#if defined(SYSTEM_PC) && !defined(SYSTEM_PC_VC) && __GNUC__ >= 3
  #include <ext/slist>
  namespace std { using __gnu_cxx::slist;}
#else
  #include <slist>
#endif


// Begin Namespace __IsoAgLib
namespace __IsoAgLib {

// forward declarations
class FilterBox_c;
class CANCustomer_c;

struct ISOFilter_s
{
  ISOFilter_s (CANCustomer_c& rrefc_canCustomer, uint32_t rui32_mask, uint32_t rui32_filter, const ISOName_c* rpc_isoNameDa = NULL, const ISOName_c* rpc_isoNameSa = NULL, Ident_c::identType_t rt_identType=Ident_c::ExtendedIdent)
    : c_identMask (rui32_mask, rt_identType)
    , c_identFilter (rui32_filter, rt_identType)
    , pc_canCustomer (&rrefc_canCustomer)
  {
    if (rpc_isoNameDa) c_isoNameDa = *rpc_isoNameDa;                // operator =
    else /* no name */ c_isoNameDa = ISOName_c::ISONameUnspecified; // operator =
    if (rpc_isoNameSa) c_isoNameSa = *rpc_isoNameSa;                // operator =
    else /* no name */ c_isoNameSa = ISOName_c::ISONameUnspecified; // operator =
  }

private:
  bool equalMaskAndFilter (const ISOFilter_s& rrefc_isoFilter);
  bool operator == (const ISOFilter_s& rrefc_isoFilter)
  { return equalMaskAndFilter (rrefc_isoFilter) && pc_canCustomer == rrefc_isoFilter.pc_canCustomer; }

private:
  Ident_c c_identMask;
  Ident_c c_identFilter;
  ISOName_c c_isoNameDa;
  ISOName_c c_isoNameSa;

  /** Pointer to a CANCustomer_c instance. Assume this like a reference to be always valid! */
  CANCustomer_c* pc_canCustomer;

  friend class ISOFilterBox_c;
};


class ISOFilterBox_c
{
public:
  enum RemoveAnswer_en
  {
    RemoveAnswerFailureNonExistent,
    RemoveAnswerSuccessBoxEmpty,
    RemoveAnswerSuccessBoxNotEmpty
  };

  struct ManagedISOFilter_s
  {
    ManagedISOFilter_s (const ISOFilter_s& rrefs_isoFilter) : s_isoFilter (rrefs_isoFilter), pc_filterBox (NULL) {}

    ISOFilter_s s_isoFilter;
    FilterBox_c* pc_filterBox;
  };

  typedef STL_NAMESPACE::slist<ManagedISOFilter_s> ManagedISOFilter_slist;
  typedef STL_NAMESPACE::slist<ManagedISOFilter_s>::iterator ManagedISOFilter_it;

  /** empty constructor - everything has to be "constructed" with the "init"-function! */
  ISOFilterBox_c();

  /** copy constructor, as implicit one was too large to be inlined! */
  ISOFilterBox_c(const ISOFilterBox_c&);

  /** initialization for one simple ISOFilterBox_c
    @param rrefc_customer reference to the CANCustomer_c instance, which creates this ISOFilterBox_c instance
   */
//  void init (CANCustomer_c& rrefc_canCustomer);

  /** add ISOFilter
    @return true on success
   */
  bool addIsoFilter (const ISOFilter_s& rrefcs_isoFilter);

  bool hasIsoFilterWithoutCustomer (const ISOFilter_s& rrefcs_isoFilter);
  bool hasIsoFilterWithCustomer    (const ISOFilter_s& rrefcs_isoFilter);

  RemoveAnswer_en removeIsoFilter (const ISOFilter_s& rrefcs_isoFilter);

  void updateOnAdd    (const ISOName_c& rpc_isoName);
  void updateOnRemove (const ISOName_c& rpc_isoName);

  void syncFiltersToCan();

  //! Here could come another constructor that takes a variable list of filters and
  //! keeps them all connected. Yet to be done, but not important right now...

private:
  ManagedISOFilter_slist slist_managedISOFilter;

  // management information about the state of this instance
  uint8_t ui8_filtersSetUp;
};

} // End Namespace __IsoAgLib
#endif
