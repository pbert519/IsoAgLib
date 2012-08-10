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
#include <IsoAgLib/comm/Part10_TaskController_Client/impl/procdatalocalbase_c.h>
#include <IsoAgLib/comm/Part10_TaskController_Client/StdMeasureElements/impl/managemeasureproglocal_c.h>
#include <IsoAgLib/comm/Part10_TaskController_Client/StdSetpointElements/impl/setpointlocal_c.h>

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
  \n
  Example:
  \code
  // define local device type
  IsoAgLib::iIsoName c_myIsoName( 1, 0 );
  // creation of process data instance
  iProcDataLocal_c c_workState;
  // init for LIS=0, local device type/subtype=1/0, complete work width=0xFF,
  // target process data/PRI=2, pointer to my local device type ( to resolve dynamic SA at time of cmd send ),
  // load/store measurememnt data to/from EEPROM
  c_workState.init( 0, 0, myIsoName, c_myIsoName, &c_myIsoName, false, 0x1234 );

  // update current measurement value ( real value, which can differ from commanded value )
  c_workState.setMasterMeasurementVal( 100 );
  // measurement programs and value requests are handled in the backend of the IsoAgLibrary

  // check if unhandled setpoints are received
  if ( c_workState.setpoint().unhandledCnt() > 0 ) {
    if ( ! c_workState.setpoint().existMaster() ) {
      for ( uint8_t ind = 0; ind < c_workState.setpoint().unhandledCnt(); ind++ )
      { // now decide on all received setpoints
        if ( ( c_workState.setpoint().unhandledCnt() == 1 )
          && ( ( c_workState.setpoint().unhandledFirst().exact() == 100 ) // undhandledFirst() is shortcut for unhandledInd( 0 )
            || ( c_workState.setpoint().unhandledInd( 0 ).exact() == 0   ) ) )
        { // as no master exist, and only one setpoint received -> simply accept if value is 0 or 100
          c_workState.setpoint().unhandledFirst().setMaster( true );
        }
      }
    }
    else if ( ( c_workState.setpoint().unhandledMaster().exact() == 100 )
           || ( c_workState.setpoint().unhandledMaster().exact() == 0 ) )
    { // simply accept new master if 0 or 100
      c_workState.setpoint().acceptNewMaster();
    }
    // send answer for all received setpoints
    c_workState.setpoint().respondAckNack();
  }
  \endcode

  @author Dipl.-Inform. Achim Spangler
*/
class ProcDataLocal_c : public ProcDataLocalBase_c  {

public:
  /**
    constructor which can set all element vars

    possible errors:
        * Err_c::badAlloc not enough memory to insert first  MeasureProgLocal
    ISO parameter
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
                  const IsoName_c *apc_externalOverridingIsoName = NULL,
                  bool ab_cumulativeValue = false,
                  IsoAgLib::ProcessDataChangeHandler_c *apc_processDataChangeHandler = NULL,
                  int ai_multitonInst = 0
                  );

  /**
    initialise this ProcDataLocal_c instance to a well defined initial state

    possible errors:
        * Err_c::badAlloc not enough memory to insert first  MeasureProgLocal
    ISO parameter
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
            const IsoName_c *apc_externalOverridingIsoName = NULL,
            bool ab_cumulativeValue = false,
            IsoAgLib::ProcessDataChangeHandler_c *apc_processDataChangeHandler = NULL,
            int ai_multitonInst = 0
            );

  /** default destructor which has nothing to do */
  ~ProcDataLocal_c();

  /** deliver a reference to the setpoint management class */
  SetpointLocal_c& setpoint( void ) { return mc_setpoint; }

  const SetpointLocal_c& setpointConst( void ) const { return mc_setpoint; }

  /**
    check if specific measureprog exist

    @param acrc_isoName DEVCLASS code of searched measure program
    @return true -> found item
  */
  bool existProg(const IsoName_c& acrc_isoName)
      {return mc_measureprog.existProg(acrc_isoName);};

  /**
    search for suiting measureprog, if not found AND if ab_doCreate == true
    create copy from first element at end of vector

    possible errors:
        * Err_c::elNonexistent wanted measureprog doesn't exist and ab_doCreate == false

    @param acrc_isoName DEVCLASS code of searched measure program
    @param ab_doCreate true -> create suitable measure program if not found
  */
  MeasureProgLocal_c& prog(const IsoName_c& acrc_isoName, bool ab_doCreate)
    { return mc_measureprog.prog(acrc_isoName, ab_doCreate);};


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

  /**
    perform periodic actions
    delete all running measure programs of members which are >3sec inactive;
    deletion of outdated setpoints is managed by SetpointLocal_c::timeEvent
    @param pui16_nextTimePeriod calculated new time period, based on current measure progs (only for local proc data)
    @return true -> all planned executions performed
  */
  virtual bool timeEvent(  uint16_t *pui16_nextTimePeriod = NULL );

  /**
    check if a setpoint master exists
    (used for accessing setpoint values from measure progs)
    @return true -> setpoint master exists
  */
  virtual bool setpointExistMaster() const { return setpointConst().existMaster();}

  /**
    (used for accessing setpoint values from measure progs)
    @return exact value of master setpoint
  */
  virtual int32_t setpointExactValue() const { return setpointConst().masterConst().exact();}

  /**
    (used for accessing setpoint values from measure progs)
    @return default value of master setpoint
  */
  virtual int32_t setpointDefaultValue() const { return setpointConst().masterConst().getDefault();}

  /**
    (used for accessing setpoint values from measure progs)
    @return min value of master setpoint
  */
  virtual int32_t setpointMinValue() const { return setpointConst().masterConst().min();}

  /**
    (used for accessing setpoint values from measure progs)
    @return max value of master setpoint
  */
  virtual int32_t setpointMaxValue() const { return setpointConst().masterConst().max();}

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

private: // Private methods

  /** processing of a setpoint message */
  virtual void processSetpoint( const ProcessPkg_c& pkg );

  /** process a measure prog message for local process data */
  virtual void processProg( const ProcessPkg_c& pkg );

  /** deliver reference to ManageMeasureProgLocal_c */
  ManageMeasureProgLocal_c& getManageProg( void ) { return mc_measureprog;}

 private:

  /** flaxible management of measure progs */
  ManageMeasureProgLocal_c mc_measureprog;

  /** flaxible management of setpoint */
  SetpointLocal_c mc_setpoint;
};


}
#endif
