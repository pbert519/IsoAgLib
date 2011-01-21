/*
  isofilterbox_c.cpp

  (C) Copyright 2009 - 2010 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/
#include "isofilterbox_c.h"
#include <IsoAgLib/comm/Part5_NetworkManagement/impl/isomonitor_c.h>
#include <IsoAgLib/comm/impl/isobus_c.h>

namespace __IsoAgLib {


// dlcForce == -1: don't check dlc. value of 0..8: force to be exactly this dlc!
IsoFilter_s::IsoFilter_s (CanCustomer_c& arc_canCustomer, const IsoAgLib::iMaskFilter_c& arc_maskFilter, const IsoName_c* apc_isoNameDa, const IsoName_c* apc_isoNameSa, int8_t ai8_dlcForce ) :
  mc_maskFilter( arc_maskFilter )
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
IsoFilter_s::equalMaskAndFilter (const IsoFilter_s& acrc_isoFilter) const
{
  return( ( mc_maskFilter == acrc_isoFilter.mc_maskFilter )
       && (mc_isoNameSa   == acrc_isoFilter.mc_isoNameSa)
       && (mc_isoNameDa   == acrc_isoFilter.mc_isoNameDa));
}





IsoFilterBox_c::IsoFilterBox_c (const IsoFilter_s& arcs_isoFilter MULTITON_INST_PARAMETER_DEF_WITH_COMMA)
: MULTITON_PARENT_CONSTRUCTOR
  ms_isoFilter (arcs_isoFilter)
, mpc_filterBox (NULL)
, mc_adaptedMaskFilter( 0, 0 )
{}



bool
IsoFilterBox_c::updateOnAdd()
{
  if (mpc_filterBox == NULL)
  { // There's no filter for this IsoFilter yet, let's see if we could create it!
    IsoAgLib::iMaskFilter_c c_maskFilter = ms_isoFilter.mc_maskFilter;

    if (ms_isoFilter.mc_isoNameSa.isSpecified())
    { // see if it's in the monitorlist!
      if (getIsoMonitorInstance4Comm().existIsoMemberISOName (ms_isoFilter.mc_isoNameSa, true))
      { // retrieve current address
        const uint8_t cui8_adr = getIsoMonitorInstance4Comm().isoMemberISOName (ms_isoFilter.mc_isoNameSa).nr();

        isoaglib_assert( ( c_maskFilter.getFilter() & 0xff ) == 0 );

        c_maskFilter.setFilter( c_maskFilter.getFilter() | cui8_adr );
        c_maskFilter.setMask( c_maskFilter.getMask() | 0xff );

      }
      else return false; // can't create the filter - IsoName not claimed on the bus!
    }
    if (ms_isoFilter.mc_isoNameDa.isSpecified())
    { // see if it's in the monitorlist!
      if (getIsoMonitorInstance4Comm().existIsoMemberISOName (ms_isoFilter.mc_isoNameDa, true))
      { // retrieve current address
        const uint8_t cui8_adr = getIsoMonitorInstance4Comm().isoMemberISOName (ms_isoFilter.mc_isoNameDa).nr();

        isoaglib_assert( ( c_maskFilter.getFilter() & 0xff00UL ) == 0 );

        c_maskFilter.setFilter( c_maskFilter.getFilter() | ( static_cast<uint32_t>( cui8_adr ) << 8 ) );
        c_maskFilter.setMask( c_maskFilter.getMask() | 0xff00UL );
      }
      else return false; // can't create the filter - IsoName not claimed on the bus!
    }

    mpc_filterBox = getIsoBusInstance4Comm().insertFilter (*ms_isoFilter.mpc_canCustomer,
                                                           c_maskFilter,
                                                           false, ms_isoFilter.mi8_dlcForce);
    mc_adaptedMaskFilter = c_maskFilter;
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
      getIsoBusInstance4Comm().deleteFilter( *ms_isoFilter.mpc_canCustomer, mc_adaptedMaskFilter );
      mpc_filterBox = NULL;
    }
  }
}


} // end of namespace __IsoAgLib
