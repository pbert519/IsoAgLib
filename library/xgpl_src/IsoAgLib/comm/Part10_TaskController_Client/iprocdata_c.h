/*
  iprocdata_c.h: managing of local process data

  (C) Copyright 2009 - 2013 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/
#ifndef IPROCDATA_C_H
#define IPROCDATA_C_H

#include "impl/procdata/procdata_c.h"
#include <IsoAgLib/comm/Part10_TaskController_Client/iprocdata.h>
#include <IsoAgLib/comm/Part10_TaskController_Client/itcclientconnection_c.h>
#include <IsoAgLib/comm/Part5_NetworkManagement/iisoname_c.h>

namespace __IsoAgLib {
  class Setpoint_c;
}

namespace IsoAgLib {
  
class iProcData_c : private __IsoAgLib::ProcData_c
{
public:
  iProcData_c() : ProcData_c() {}
  
  void init( iTcClientConnection_c& tcCC, const iDeviceObjectDpd_c& dpd, const iDeviceObjectDet_c& det, iProcDataHandler_c *apc_procDataHandler = NULL )
  {
    ProcData_c::init( tcCC, dpd, det, apc_procDataHandler );
  }

  void setProcDataHandler( iProcDataHandler_c *apc_procDataHandler )
  { ProcData_c::setProcDataHandler( apc_procDataHandler ); }

  iProcDataHandler_c* getProcDataHandler() const
  { return ProcData_c::getProcDataHandler(); }

  const iIsoName_c& isoName() const { return ProcData_c::isoName().toConstIisoName_c(); }

  uint16_t DDI() const { return ProcData_c::DDI(); }
  uint16_t element() const  { return ProcData_c::element(); }

  void sendMeasurementVal( const iIsoName_c& ac_targetISOName ) const
  { ProcData_c::sendMeasurementVal( ac_targetISOName ); }

  int32_t measurementVal() const { return ProcData_c::measurementVal(); }

  void setMeasurementVal( int32_t ai32_val )
  { ProcData_c::setMeasurementVal(ai32_val); }

  void incrMeasurementVal( int32_t ai32_val )
  { ProcData_c::incrMeasurementVal(ai32_val); }

  int32_t setpointVal() const
  { return ProcData_c::setpointVal(); }

  void startDataLogging(
    ProcData::MeasurementCommand_t ren_type /* ProcData::TimeProp, ProcData::DistProp, ... */,
    int32_t ai32_increment,
	IsoAgLib::ProcData::RemoteType_t a_ecuType )
  { ProcData_c::startDataLogging(ren_type, ai32_increment, a_ecuType ); }

  friend class __IsoAgLib::Setpoint_c;
};

}

#endif
