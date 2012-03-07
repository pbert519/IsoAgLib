/***************************************************************************
                          setpointlocal_c.h - object for managing setpoints

  (C) Copyright 2009 - 2012 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/
#ifndef SETPOINT_LOCAL_H
#define SETPOINT_LOCAL_H

#include <IsoAgLib/comm/Part10_TaskController_Client/StdSetpointElements/impl/setpointbase_c.h>
#include <IsoAgLib/comm/Part10_TaskController_Client/StdSetpointElements/impl/setpointregister_c.h>
#include <IsoAgLib/comm/Part10_TaskController_Client/impl/proc_c.h>

#include <list>

// Begin Namespace __IsoAgLib
namespace __IsoAgLib {
class ProcDataLocalBase_c;

/**
  object for managing setpoints commanded from remote to local process data objects;
  handling of received setpoints;
  managing the master setpoint, which is used for controlling;
  help for sending correct notify;
  semi automatic and manual handling of received values
  @author Dipl.-Inform. Achim Spangler
*/
class SetpointLocal_c : public SetpointBase_c  {
private:
  typedef STL_NAMESPACE::list<SetpointRegister_c> Vec_SetpointRegister;
  typedef STL_NAMESPACE::list<SetpointRegister_c>::iterator Vec_SetpointRegisterIterator;
  typedef STL_NAMESPACE::list<SetpointRegister_c>::const_iterator Vec_SetpointRegisterConstIterator;

public:
  /**
    default constructor which can set needed pointers to containing objects
    @param apc_processData pointer to containing ProcessData instance
  */
  SetpointLocal_c(
    ProcDataBase_c *const apc_processData = NULL ) : SetpointBase_c( apc_processData )
    {init( apc_processData );};

  /**
    initialise this SetpointLocal_c to a well defined starting condition
    @param apc_processData pointer to containing ProcessData instance
  */
  void init( ProcDataBase_c *const apc_processData );
  /**
    assginment from another object
    @param acrc_src source SetpointLocal_c instance
    @return reference to source for cmd like "setp1 = setp2 = setp3;"
  */
  const SetpointLocal_c& operator=( const SetpointLocal_c& acrc_src );
  /**
    copy constructor for SetpointLocal
    @param acrc_src source SetpointLocal_c instance
  */
   SetpointLocal_c( const SetpointLocal_c& acrc_src );
  /** default destructor which has nothing to do */
  ~SetpointLocal_c();

  /**
    retreive simple master setpoint
    @return actual received setpoint value (calculated with setpoint )
  */
  int32_t setpointMasterVal() const {return mpc_master->exact();};
  /**
    set the setpoint value
    @param ai32_val new setpoint value
  */
  void setSetpointMasterVal( int32_t ai32_val )
    { if (!existMaster()) setStaticMaster();
      setMasterMeasurementVal( ai32_val );
    };

  /**
    set if master setpoint should be preserved even if caller
    isoName is no more active (default false )
    @param ab_static choose if master setpoint should be preserved
          if caller isn't acitve
  */
  void setStaticMaster( bool ab_static = true )
    {mb_staticMaster = ab_static;};
  /**
    check if actual a master setpoint item is defined
    @see master
    @return true -> an active master setpoint exist
  */
  bool existMaster() const {
    // Keep this argument order for operator !=, otherwise tasking 7.5
    // won't compile:
    return mvec_register.end() != mpc_master;
  }
  /**
    deliver the actual master entry
    @see existMaster
    @return actual valid master setpoint
  */
  SetpointRegister_c& master(){return *mpc_master;};
  /**
    deliver the actual master entry
    @see existMaster
    @return actual valid master setpoint
  */
  const SetpointRegister_c& masterConst() const {return *mpc_master;};
  /**
    check if unhandled new setpoint from actual master was received
    -> check the actual master first before answering the other unhandled
    @see unhandledMaster
    @see acceptNewMaster
    @return true -> master setpoint comanding member sent new setpoint
  */
  bool existUnhandledMaster();
  /**
    deliver the new sent setpoint values from the actual master;
    if no unhandled setpoint of the master exist, return the actual accepted value

    possible errors:
        * Err_c::elNonexistent no master setpoint found
    @see existUnhandledMaster
    @see acceptNewMaster
    @return reference to master setpoint register
  */
  SetpointRegister_c& unhandledMaster();
  /**
    if there exist a new unhandled setpoint from the master setpoint sender;
    accept the new value with this command
    otherwise the old value is still used and answered

    possible errors:
        * Err_c::elNonexistent no master setpoint found
    @see existUnhandledMaster
    @see unhandledMaster
    @param ab_accept true ( default ) -> new setpoint of master is accepted -> send positive notify
  */
  void acceptNewMaster( bool ab_accept = true );
  /**
    set the master setpoint manually
    (in some cases remote systems request informations
     through process data setpoints )
    @param ai32_val wanted setpoint value
  */
  void setMasterMeasurementVal( int32_t ai32_val );

  /**
    deliver the count of unhandled setpoints
    @return count of unhandled received setpoints
  */
  uint8_t unhandledCnt()const;
  /**
    deliver the aui8_ind of the unhandled setpoints
    @param aui8_ind position of the wanted setpoint entry in list of unhandled setpoints
      (first entry: aui8_ind == 0!!)

    possible errors:
        * Err_c::range there are less than aui8_ind unhandled setpoints found
    @see unhandledCnt
    @return wanted unhandled setpoint
  */
  SetpointRegister_c& unhandledInd( uint8_t aui8_ind );
  /**
    deliver the first unhandled entry

    possible errors:
        * Err_c::elNonexistent no unhandled stepoint exist
    @see unhandledCnt
    @return reference to first unhandled new received setpoint entry
  */
  SetpointRegister_c& unhandledFirst();
  /**
    answer all unhandled setpoint enties;
    if new master setpoint was set, send the master
    the new accepted setpoint value;
    send all others NO_VAL_32S

    possible errors:
        * dependant error in ProcDataLocalBase_c commander of some setpoint isn't found in Monitor List
        * dependant error in CanIo_c on CAN send problems
  */
  void respondAckNack();
  /**
    set the allowed percentual deviation between actual measurement and
    existing master setpoint
    @param ab_percent allowed percantual deviation
  */
  void setAllowedDeltaPercent( uint8_t ab_percent){mb_allowedDeltaPercent = ab_percent;};
  /**
    get the allowed percentual deviation between actual measurement and
    existing master setpoint
    @return allowed percantual deviation
  */
  uint8_t allowedDeltaPercent()const{return mb_allowedDeltaPercent;};
  /**
    check if the given measuremet value is correct for the actual
    master setpoint;
    @param ai32_val measured value
    @param ab_sendIfError true -> if actual value exceeds setpoint limits
           the actual value is sent as notification (default true )
    @return 0 -> correct; (n<0) -> measurement is delta n to low;
            (n>0) -> measurement is delta n to high
  */
  int32_t checkMeasurement( int32_t ai32_val, bool ab_sendIfError = true );
  /**
    perform periodic actions
    (here: check if measure val is in limits; delete old handled not master setpoints and
            delete master entry if it's isoName isn't registered active any more )
    @return true -> all planned activities performed in allowed time
  */
  virtual bool timeEvent( void );


  /**
     send a sub-setpoint (selected by value group) to a specified target (selected by GPT)
     @param ac_targetISOName ISOName of target
     @param en_valueGroup: min/max/exact/default
     @param en_command
     @return true -> successful sent
  */
  bool sendSetpointForGroup(const IsoName_c& ac_targetISOName,
                            ProcessCmd_c::ValueGroup_t en_valueGroup = ProcessCmd_c::noValue,
                            ProcessCmd_c::CommandType_t en_command = ProcessCmd_c::noCommand) const;

  /**
    send a exact-setpoint to a specified target (selected by GPT)
    @param ac_targetISOName ISOName of target
    @return true -> successful sent
  */
  bool sendMasterSetpointVal( const IsoName_c& ac_targetISOName) const
   { return sendSetpointForGroup(ac_targetISOName, ProcessCmd_c::exactValue, ProcessCmd_c::setValue );};

private: // Private methods
  /** base function for assignment of element vars for copy constructor and operator= */
  void assignFromSource( const SetpointLocal_c& acrc_src );
  /**
    process a setpoint request for local process data
  */
  virtual void processRequest( const ProcessPkg_c& pkg ) const;
  /**
    process a setpoint set for local process data
  */
  virtual void processSet( const ProcessPkg_c& pkg );

  /**
    deliver a reference to ProcDataLocal_c
    (the base function only delivers ProcDataBase_c )
    @return reference to containing ProcDataLocalBase_c instance
  */
  ProcDataLocalBase_c& processData()
  {
    return *((ProcDataLocalBase_c*)((void*)ProcessElementBase_c::pprocessData()));
  };
  /**
    deliver a reference to ProcDataLocal_c
    (the base function only delivers ProcDataBase_c )
    @return reference to containing ProcDataLocalBase_c instance
  */
  const ProcDataLocalBase_c& processDataConst() const
  {
    return *((ProcDataLocalBase_c*)((void*)ProcessElementBase_c::pprocessData()));
  };
  /**
    deliver a pointer to ProcDataLocal_c
    (the base function only delivers ProcDataBase_c )
    @return pointer to containing ProcDataLocalBase_c instance
  */
  ProcDataLocalBase_c* pprocessData()
  {
    return ((ProcDataLocalBase_c*)((void*)ProcessElementBase_c::pprocessData()));
  };

private: // Private attributes
  /** container of registered setpoint values */
  Vec_SetpointRegister mvec_register;
  /** iterator cache to actual master setpoint */
  mutable Vec_SetpointRegisterIterator mpc_registerCache;
  /** SetpointRegister_c entry, which represents the actual used setpoint value */
  Vec_SetpointRegisterIterator mpc_master;
  /**
    maximal allowed setpoint (updated according to system state by main application )
    --> incoming setpoints can be handled automatic
  */
  int32_t mi32_setpointMaxAllowed;
  /**
    minimum allowed setpoint (updated according to system state by main application )
    --> incoming setpoints can be handled automatic
  */
  int32_t mi32_setpointMinAllowed;
  /* allowed variation between setpoint and measurement */
  uint8_t mb_allowedDeltaPercent;
  /**
    set if master setpoint should be preserved even if caller
    isoName is no more active (default false )
  */
  bool mb_staticMaster;

};

}
#endif
