/***************************************************************************
                          procdatalocal_c.h - managing of local

  (C) Copyright 2009 - 2012 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/
#ifndef PROCDATA_LOCAL_H
#define PROCDATA_LOCAL_H

/* *************************************** */
/* ********** include headers ************ */
/* *************************************** */
#include <IsoAgLib/comm/Part10_TaskController_Client/impl/proc_c.h>
#include <IsoAgLib/comm/Part10_TaskController_Client/StdMeasureElements/impl/managemeasureproglocal_c.h>
#include <IsoAgLib/comm/Part10_TaskController_Client/StdSetpointElements/impl/setpointlocal_c.h>
#include <IsoAgLib/util/config.h>
#include <IsoAgLib/comm/Part5_NetworkManagement/impl/istate_c.h>
#include <IsoAgLib/comm/Part5_NetworkManagement/impl/isoname_c.h>
#include <IsoAgLib/util/impl/singleton.h>
#include "processpkg_c.h"
#include "processcmd_c.h"

namespace IsoAgLib {
  class ProcessDataChangeHandler_c;
  class EventSource_c;
}

// Begin Namespace IsoAgLib
namespace __IsoAgLib {
// ****************************************************************************************
// ********************************* ProcDataLocal_c *********************************
// ****************************************************************************************


/**
  @brief Internal implementation for managing of local process data object with standard
  ( %i.e. no restrictions ) feature set.

  Has __IsoAgLib::SetpointLocal_c / IsoAgLib::iSetpointLocal_c and __IsoAgLib::ManageMeasureProgLocal_c / IsoAgLib::iManageMeasureProgLocal_c
  as member for doing setpoint and measurement specific jobs.
  \n
  Projects which use this class, must include the files and succeding subdirectories
  of the following subdirectories of xgpl_src/IsoAgLib/Process:
  - \e Local/StdMeasureElements,
  - \e Local/StdSetpointElements,
  - \e StdMeasureElements,
  - \e StdSetpointElements and
  - \e impl
  @author Dipl.-Inform. Achim Spangler
*/
class ProcDataLocal_c : public ClientBase  {

public:
  /**
    constructor which can set all element vars
    @param ps_elementDDI optional pointer to array of structure IsoAgLib::ElementDdi_s which contains DDI, element, isSetpoint and ValueGroup
                         (array is terminated by ElementDdi_s.ui16_element == 0xFFFF)
    @param acrc_isoName optional ISOName code of Process-Data
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
  ProcDataLocal_c(uint16_t aui16_ddi,
                  uint16_t aui16_element,
                  const IsoName_c& acrc_isoName,
                  bool ab_isSetpoint,
                  uint8_t aui8_triggerMethod,
                  IsoAgLib::ProcessDataChangeHandler_c *apc_processDataChangeHandler = NULL,
                  int ai_multitonInst = 0
                  );

  /**
    initialise this ProcDataLocal_c instance to a well defined initial state
    @param ps_elementDDI optional pointer to array of structure IsoAgLib::ElementDdi_s which contains DDI, element, isSetpoint and ValueGroup
                         (array is terminated by ElementDdi_s.ui16_element == 0xFFFF)
    @param acrc_isoName optional ISOName code of Process-Data
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
  void init(uint16_t aui16_ddi,
            uint16_t aui16_element,
            const IsoName_c& acrc_isoName,
            bool ab_isSetpoint,
            uint8_t aui8_triggerMethod,
            IsoAgLib::ProcessDataChangeHandler_c *apc_processDataChangeHandler = NULL,
            int ai_multitonInst = 0
            );

  /** default destructor which has nothing to do */
  ~ProcDataLocal_c();

  /** set the pointer to the handler class
    * @param apc_processDataChangeHandler pointer to handler class of application
    */
  void setProcessDataChangeHandler( IsoAgLib::ProcessDataChangeHandler_c *apc_processDataChangeHandler )
   { mpc_processDataChangeHandler = apc_processDataChangeHandler; }

  /** deliver the poitner to the handler class
    * @return pointer to handler class of application (or NULL if not defined by application)
    */
  IsoAgLib::ProcessDataChangeHandler_c* getProcessDataChangeHandler( void ) const { return mpc_processDataChangeHandler; }

  /** deliver the master value (central measure value of this process data;
    can differ from measure vals of measure progs, as these can be reseted
    independent)
    @return actual master value
  */
  const int32_t& masterMeasurementVal() const {return mi32_masterVal;}

  /**
    set the masterMeasurementVal from main application independent from any measure progs
    @param ai32_val new measure value
  */
  virtual void setMasterMeasurementVal(int32_t ai32_val);

  /**
    increment the value -> update the local and the measuring programs values
    @param ai32_val size of increment of master value
  */
  virtual void incrMasterMeasurementVal(int32_t ai32_val);

  /** process a message, which is adressed for this process data item;
    ProcDataLocal_c::processMsg() is responsible to delegate the
    processing of setpoint and measurement messages to the appripriate
    functions processSetpoint and processProg;
    both functions are virtual, so that depending on loacl or remote
    process data the suitable reaction can be implemented
  */
  void processMsg( ProcessPkg_c& pkg );

  /**
    perform periodic actions
    delete all running measure programs of members which are >3sec inactive;
    deletion of outdated setpoints is managed by SetpointLocal_c::timeEvent
    @param pui16_nextTimePeriod calculated new time period, based on current measure progs (only for local proc data)
    @return true -> all planned executions performed
  */
  virtual bool timeEvent(  uint16_t *pui16_nextTimePeriod = NULL );

  /** send the value to a specified target (selected by ISOName)
    @param ac_targetISOName ISOName of target
    @return true -> successful sent
  */
  void sendMasterMeasurementVal( const IsoName_c& ac_targetISOName ) const;

  /**
    Get setpoint value as received from remote system
    @return exact value of master setpoint
  */
  virtual int32_t setpointValue() const { return mc_setpoint.setpointVal(); }

  /**
    allow local client to actively start a measurement program
    (to react on a incoming "start" command for default data logging)
    @param ren_type measurement type: Proc_c::TimeProp, Proc_c::DistProp, ...
    @param ai32_increment
    @param apc_receiverDevice commanding ISOName
    @return true -> measurement started
  */
  bool startDataLogging(Proc_c::type_t ren_type /* Proc_c::TimeProp, Proc_c::DistProp, ... */,
                        int32_t ai32_increment, const IsoName_c* apc_receiverDevice = NULL );

  /**
    stop all measurement progs in all local process instances, started with given isoName
    @param rc_isoName
  */
  virtual void stopRunningMeasurement(const IsoName_c& rc_isoName);

  /**
    deliver value DDI (only possible if only one elementDDI in list)
    @return DDI
  */
  uint16_t DDI() const{ return mui16_ddi; }

  /**
    deliver value element (only possible if only one elementDDI in list)
    @return element
  */
  uint16_t element() const{ return mui16_element; }

  /**
    return true if ProcessData is set as setpoint 
    @return setpoint information
  */
  bool isSetPoint() const { return procdataconfiguration.mb_isSetpoint; }

  /**
    deliver trigger method information
    @return setpoint information
  */
  uint8_t triggerMethod() const { return procdataconfiguration.mui8_triggerMethod; } 

  /**
    deliver the isoName (retrieved from pointed isoName value, if valid pointer)
    @return actual ISOName
  */
  const IsoName_c& isoName() const { return mc_isoName; }

  /**
    check if this item has the same identity as defined by the parameters,
    if aui8_devClassInst is 0xFF a lazy match disregarding pos is done
    (important for matching received process data msg);
    @param acrc_isoNameReceiver compared isoName value
    @param aui16_DDI compared DDI value
    @param aui16_element compared element value
    @return true -> this instance has same Process-Data identity
  */
  bool matchISO( const IsoName_c& acrc_isoNameReceiver,
                 uint16_t aui16_DDI,
                 uint16_t aui16_element
               ) const;

protected: // Protected methods
  /** send the given int32_t value with variable ISOName ac_varISOName;
      set the int32_t value with conversion (according to central data type) in message
      string and set data format flags corresponding to central data type of this process data
      set general command before sendValISOName !
      @param ac_varISOName variable ISOName
      @param ai32_val int32_t value to send
      @return true -> sendIntern set successful EMPF and SEND
  */
  void sendValISOName( ProcessPkg_c& pkg, const IsoName_c& ac_varISOName, int32_t ai32_val = 0) const;

private: // Private methods
  /** processing of a setpoint message */
  virtual void processSetpoint( const ProcessPkg_c& pkg );

  /** process a measure prog message for local process data */
  virtual void processProg( const ProcessPkg_c& pkg );

private: // Private attributes

  /** IsoName_c information for this instance */
  IsoName_c mc_isoName;

  uint16_t mui16_ddi;
  uint16_t mui16_element;

  struct {
    bool mb_isSetpoint : 1;
    uint8_t mui8_triggerMethod : 7;
  } procdataconfiguration;

  /** store the master value of the main programm */
  int32_t mi32_masterVal;

  /** pointer to applications handler class, with handler functions
      which shall be called on correltating change events.
      (e.g. new received setpoint for local process data
       or new received measurement value for remote process data)
    */
  IsoAgLib::ProcessDataChangeHandler_c* mpc_processDataChangeHandler;

private:

  /** flaxible management of measure progs */
  ManageMeasureProgLocal_c mc_measureprog;

  /** flaxible management of setpoint */
  SetpointLocal_c mc_setpoint;

private:
  friend class MeasureProgLocal_c;

private:
  /** not copyable : copy constructor is only declared, never defined */
  ProcDataLocal_c(const ProcDataLocal_c&);
  /** not copyable : copy operator is only declared, never defined */
  ProcDataLocal_c& operator=(const ProcDataLocal_c&); 

};


}
#endif
