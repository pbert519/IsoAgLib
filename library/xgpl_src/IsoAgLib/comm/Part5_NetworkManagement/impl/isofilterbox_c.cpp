/***************************************************************************
                              isofilterbox_c.cpp
                             -------------------
    begin                : Mon Oct 09 2006
    copyright            : (C) 2000 - 2007 by Dipl.-Inf. Martin Wodok
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

#include "isofilterbox_c.h"
#include <IsoAgLib/comm/Part5_NetworkManagement/impl/isomonitor_c.h>
#include <IsoAgLib/driver/can/impl/canio_c.h>

namespace __IsoAgLib {


// dlcForce == -1: don't check dlc. value of 0..8: force to be exactly this dlc!
IsoFilter_s::IsoFilter_s (CanCustomer_c& arc_canCustomer, uint32_t aui32_mask, uint32_t aui32_filter, const IsoName_c* apc_isoNameDa, const IsoName_c* apc_isoNameSa, int8_t ai8_dlcForce, Ident_c::identType_t at_identType)
  : mc_identMask (aui32_mask, at_identType)
  , mc_identFilter (aui32_filter, at_identType)
  , mpc_canCustomer (&arc_canCustomer)
  , mi8_dlcForce (ai8_dlcForce)
{
  if (apc_isoNameDa) mc_isoNameDa = *apc_isoNameDa;                  // operator =
  else /* no name */ mc_isoNameDa.setUnspecified();
  if (apc_isoNameSa) mc_isoNameSa = *apc_isoNameSa;                  // operator =
  else /* no name */ mc_isoNameSa.setUnspecified();
}

IsoFilter_s::~IsoFilter_s() {}


bool
IsoFilter_s::equalMaskAndFilter (const IsoFilter_s& arcc_isoFilter) const
{
  return ((mc_identMask   == arcc_isoFilter.mc_identMask)
       && (mc_identFilter == arcc_isoFilter.mc_identFilter)
       && (mc_isoNameSa   == arcc_isoFilter.mc_isoNameSa)
       && (mc_isoNameDa   == arcc_isoFilter.mc_isoNameDa));
}





IsoFilterBox_c::IsoFilterBox_c (const IsoFilter_s& arcs_isoFilter SINGLETON_VEC_KEY_PARAMETER_DEF_WITH_COMMA)
: SINGLETON_PARENT_CONSTRUCTOR
  ms_isoFilter (arcs_isoFilter)
, mpc_filterBox (NULL)
{}



bool
IsoFilterBox_c::updateOnAdd()
{
  if (mpc_filterBox == NULL)
  { // There's no filter for this IsoFilter yet, let's see if we could create it!
    Ident_c c_mask   = ms_isoFilter.mc_identMask;
    Ident_c c_filter = ms_isoFilter.mc_identFilter;

    if (ms_isoFilter.mc_isoNameSa.isSpecified())
    { // see if it's in the monitorlist!
      if (getIsoMonitorInstance4Comm().existIsoMemberISOName (ms_isoFilter.mc_isoNameSa, true))
      { // retrieve current address
        const uint8_t cui8_adr = getIsoMonitorInstance4Comm().isoMemberISOName (ms_isoFilter.mc_isoNameSa).nr();
        c_filter.set (cui8_adr, 0, Ident_c::ExtendedIdent);
        c_mask.set   (0xFF,     0, Ident_c::ExtendedIdent); // open filter for address-byte
      }
      else return false; // can't create the filter - IsoName not claimed on the bus!
    }
    if (ms_isoFilter.mc_isoNameDa.isSpecified())
    { // see if it's in the monitorlist!
      if (getIsoMonitorInstance4Comm().existIsoMemberISOName (ms_isoFilter.mc_isoNameDa, true))
      { // retrieve current address
        const uint8_t cui8_adr = getIsoMonitorInstance4Comm().isoMemberISOName (ms_isoFilter.mc_isoNameDa).nr();
        c_filter.set (cui8_adr, 1, Ident_c::ExtendedIdent);
        c_mask.set   (0xFF,     1, Ident_c::ExtendedIdent); // open filter for address-byte
      }
      else return false; // can't create the filter - IsoName not claimed on the bus!
    }

    mpc_filterBox = getCanInstance4Comm().insertFilter (*ms_isoFilter.mpc_canCustomer,
                                                        c_mask.ident(), c_filter.ident(),
                                                        false, c_filter.identType(),
                                                        ms_isoFilter.mi8_dlcForce);
    mc_adaptedIdentMask = c_mask;
    mc_adaptedIdentFilter = c_filter;
    return true;
  }
  return false;
}


void
IsoFilterBox_c::updateOnRemove (const IsoName_c* apc_isoName)
{
  if (mpc_filterBox)
  { // let's see what we have merged into this filter
    if ( (apc_isoName == NULL)
         ||
         (ms_isoFilter.mc_isoNameSa == *apc_isoName)
         ||
         (ms_isoFilter.mc_isoNameDa == *apc_isoName) )
    {
      getCanInstance4Comm().deleteFilter (*ms_isoFilter.mpc_canCustomer,
                                           mc_adaptedIdentMask.ident(),
                                           mc_adaptedIdentFilter.ident(),
                                           ms_isoFilter.mc_identMask.identType());
      mpc_filterBox = NULL;
    }
  }
}


} // end of namespace __IsoAgLib
