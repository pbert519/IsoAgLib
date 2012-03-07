/***************************************************************************
                          measureproglocal_c.h - object for managing local

  (C) Copyright 2009 - 2012 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/
#ifndef MEASUREPROG_LOCAL_H
#define MEASUREPROG_LOCAL_H

#include <IsoAgLib/comm/Part10_TaskController_Client/StdMeasureElements/impl/measureprogbase_c.h>

// Begin Namespace __IsoAgLib
namespace __IsoAgLib {
class ProcDataLocalBase_c;


/**
  managing of local measure progs and values;
  sending values for running programs,
  answering value requests,
  starting and stopping measure programs by remote
  @see MeasureProgRemote
  @author Dipl.-Inform. Achim Spangler
*/
class MeasureProgLocal_c : public MeasureProgBase_c  {
private:

  struct ThresholdInfo_s
  {
    Proc_c::type_t en_type;
    Proc_c::doSend_t en_doSend;
    int32_t i32_threshold;
    bool b_isMax;
  };
  #ifdef OPTIMIZE_HEAPSIZE_IN_FAVOR_OF_SPEED
  typedef STL_NAMESPACE::list<ThresholdInfo_s,MALLOC_TEMPLATE(ThresholdInfo_s) > List_ThresholdInfo;
  typedef STL_NAMESPACE::list<ThresholdInfo_s,MALLOC_TEMPLATE(ThresholdInfo_s) >::iterator List_ThresholdInfoIterator;
  typedef STL_NAMESPACE::list<ThresholdInfo_s,MALLOC_TEMPLATE(ThresholdInfo_s)>::const_iterator List_ThresholdInfoConstIterator;
  #else
  typedef STL_NAMESPACE::list<ThresholdInfo_s> List_ThresholdInfo;
  typedef STL_NAMESPACE::list<ThresholdInfo_s>::iterator List_ThresholdInfoIterator;
  typedef STL_NAMESPACE::list<ThresholdInfo_s>::const_iterator List_ThresholdInfoConstIterator;
  #endif

public:
  /**
    constructor which can optionally set most element vars of MeasureProgLocal
    @param apc_processData optional pointer to containing ProcDataLocal_c instance (def NULL)
    @param ai32_masterVal optional actual central local measured value used as masterVal (def 0)
    @param ai32_initialVal optional initial value (e.g which was stored in EEPROM) (default 0)
    @param ac_callerISOName optional ISOName of remote member, which caused creation of this instance (default 0xFF == no member)
  */
  MeasureProgLocal_c(
    ProcDataBase_c *const apc_processData = NULL,
    int32_t ai32_masterVal = 0,
    int32_t ai32_initialVal = 0,
    const IsoName_c& ac_callerISOName = IsoName_c::IsoNameUnspecified() );
  /**
    initialise this MeasureProgLocal_c instance to a well defined initial state
    @param apc_processData optional pointer to containing ProcDataLocal_c instance (def NULL)
    @param ai32_masterVal optional actual central local measured value used as masterVal (def 0)
    @param ai32_initialVal optional initial value (e.g which was stored in EEPROM) (default 0)
    @param ac_callerISOName optional ISOName of remote member, which caused creation of this instance (default 0xFF == no member)
  */
  void init(
    ProcDataBase_c *const apc_processData,
    int32_t ai32_masterVal = 0,
    int32_t ai32_initialVal = 0,
    const IsoName_c& ac_callerISOName = IsoName_c::IsoNameUnspecified() );

  /**
    assignment of MeasureProgLocal_c objects
    @param acrc_src source MeasureProgLocal_c instance
    @return reference to source instance for assignment like "prog1 = prog2 = prog3;"
  */
  const MeasureProgLocal_c& operator=(const MeasureProgLocal_c& acrc_src);
  /**
    copy constructor for MeasureProgLocal
    @param acrc_src source MeasureProgLocal_c instance
  */
   MeasureProgLocal_c(const MeasureProgLocal_c& acrc_src);
  /** default destructor which has nothing to do */
  virtual ~MeasureProgLocal_c();
  /**
    start a measuring programm with new master measurement value

    possible errors:
      * dependant error in ProcDataLocal_c if EMPF or SEND not valid
      * dependant error in CanIo_c on send problems

    @param ren_type used increment types: Proc_c::TimeProp, Proc_c::DistProp, ...
    @param ren_doSend value types to send on trigger of subprog: Proc_c::DoNone, Proc_c::DoVal, Proc_c::DoValForExactSetpoint...
    @param ai32_masterVal actual master value to start with
    @return true -> starting values sent with success
  */
  bool start(Proc_c::type_t ren_type,
             Proc_c::doSend_t ren_doSend, int32_t ai32_masterVal);

  /**
    start a measuring program without new master measurement value

    possible errors:
      * dependant error in ProcDataLocal_c if EMPF or SEND not valid
      * dependant error in CanIo_c on send problems

    @param ren_type used increment types: Proc_c::TimeProp, Proc_c::DistProp, ...
    @param ren_doSend value types to send on trigger of subprog: Proc_c::DoNone, Proc_c::DoVal, Proc_c::DoValForExactSetpoint...
    @return true -> starting values sent with success
  */
  virtual bool start(Proc_c::type_t ren_type, Proc_c::doSend_t ren_doSend);
  /**
    stop local measuring programs -> send actual values

    possible errors:
      * dependant error in ProcDataLocal_c if EMPF or SEND not valid
      * dependant error in CanIo_c on send problems
    @param b_deleteSubProgs is only needed for remote ISO case (but is needed due to overloading here also)
    @param ren_type used increment types: Proc_c::TimeProp, Proc_c::DistProp, ...
    @param ren_doSend value types to send on trigger of subprog: Proc_c::DoNone, Proc_c::DoVal, Proc_c::DoValForExactSetpoint...
    @return true -> stop values sent with success
  */
  virtual bool stop(bool b_deleteSubProgs = true, Proc_c::type_t ren_type = Proc_c::NullType,
                    Proc_c::doSend_t ren_doSend = Proc_c::DoVal);
  /**
    deliver med val
    @return actual medium value
  */
  int32_t med() const {return ( (mi32_medCnt > 0) ? (mi32_medSum/mi32_medCnt) : 0 );};

  /**
    deliver integ val
    @return integral val for this measure prog
  */
  int32_t integ() const { return mi32_integ; };

  /**
    send a sub-information (selected by en_valueGroup) to a specified target (selected by GPT)
    @param en_valueGroup value group to send
    @param ac_targetISOName ISOName of target
    @return true -> successful sent
  */
  bool sendValForGroup( ProcessCmd_c::ValueGroup_t en_valueGroup, const IsoName_c& ac_targetISOName) const;

  /**
    send a sub-information from the corresponding setpoint master to a specified target (selected by GPT)
    @param en_valueGroup value group to send
    @param ac_targetISOName ISOName of target
    @return true -> successful sent
  */
  bool sendSetpointValForGroup( ProcessCmd_c::ValueGroup_t en_valueGroup, const IsoName_c& ac_targetISOName) const;
  /**
    deliver to en_valueGroup according setpoint from a master setpoint
    @param en_valueGroup of wanted subtype
    @return value of specified subtype
  */
  int32_t setpointValForGroup(ProcessCmd_c::ValueGroup_t en_valueGroup) const;

  /**
    process a message: reset command or value requests

    possible errors:
      * dependant error in ProcDataLocal_c if EMPF or SEND not valid
      * dependant error in CanIo_c on send problems
    @return true -> received msg processed by this instance
  */
  virtual bool processMsg( const ProcessPkg_c& arc_data );
  /**
    set the measure prog value and send values if triggered to do

    possible errors:
      * dependant error in ProcDataLocal_c if EMPF or SEND not valid
      * dependant error in CanIo_c on send problems
    @param ai32_val new measure value
  */
  virtual void setVal(int32_t ai32_val);

  /**
    send the values which are registered by a running mesuring program

    possible errors:
      * dependant error in ProcDataLocal_c if EMPF or SEND not valid
      * dependant error in CanIo_c on send problems
    @param ren_doSend value types to send on trigger of subprog: Proc_c::DoNone, Proc_c::DoVal, Proc_c::DoValForExactSetpoint...
    @return true -> value send triggered and performed with success
  */
  bool sendRegisteredVals(Proc_c::doSend_t ren_doSend = Proc_c::DoVal);

  /**
    init the element vals
    @param ai32_val initial measure val
  */
  virtual void initVal(int32_t ai32_val);
  /**
    reset the local value

    possible errors:
      * dependant error in ProcDataLocal_c if EMPF or SEND not valid
      * dependant error in CanIo_c on send problems
    @param ai32_val reset measure value to this value
    @return true -> reseted measure val sent with success
  */
  virtual bool resetVal(int32_t ai32_val = 0);

  /**
    reset the local intgral value

    possible errors:
      * dependant error in ProcDataLocal_c if EMPF or SEND not valid
      * dependant error in CanIo_c on send problems
    @return true -> reseted integ val sent with success
  */
  virtual bool resetInteg();
  /**
    reset the local medium value

    possible errors:
      * dependant error in ProcDataLocal_c if EMPF or SEND not valid
      * dependant error in CanIo_c on send problems
    @return true -> reseted medium val sent with success
  */
  virtual bool resetMed();
  /**
    reset the local min value

    possible errors:
      * dependant error in ProcDataLocal_c if EMPF or SEND not valid
      * dependant error in CanIo_c on send problems
    @return true -> reseted minimum val sent with success
  */
  virtual bool resetMin();
  /**
    reset the local max value

    possible errors:
      * dependant error in ProcDataLocal_c if EMPF or SEND not valid
      * dependant error in CanIo_c on send problems
    @return true -> reseted maximum val sent with success
  */
  virtual bool resetMax();
  /**
    periodic events
    (e.g. send value for time proportional progs)

    possible errors:
      * dependant error in ProcDataLocal_c if EMPF or SEND not valid
      * dependant error in CanIo_c on send problems
    @param pui16_nextTimePeriod calculated new time period, based on current measure progs (only for local proc data)
    @return true -> all planned activities performed in available time
  */
  virtual bool timeEvent( uint16_t *pui16_nextTimePeriod = NULL );

private: // Private methods
  /** base function for assignment of element vars for copy constructor and operator= */
  void assignFromSource( const MeasureProgLocal_c& acrc_src );
  /**  update proportional dependent values */
  void updatePropDepVals();

  /**
    set medium val
    @param ai32_val initial medium value
  */
  void setMed(int32_t ai32_val){mi32_medSum = ai32_val; mi32_medCnt = 1;};
  /**
    increment medCnt
  */
  void incrMedCnt(){mi32_medCnt++;};
  /**
    increment the mediumSum
    @param ai32_val new measure val to use for calculating the medium value
  */
  void incrMedSum(int32_t ai32_val){mi32_medSum += ai32_val;};

  /**
    set integ val
    @param ai32_val new integral value
  */
  void setInteg(int32_t ai32_val){mi32_integ = ai32_val;}
  /**
    increment the integer value
    @param ai32_val increment for integral
  */
  void incrInteg(int32_t ai32_val){mi32_integ += ai32_val;}

  /**
    deliver a reference to ProcDataLocal_c
    (the base function only delivers ProcDataBase_c)
    @return reference to containing ProcDataLocal_c
  */
  ProcDataLocalBase_c& processData()
  {
    return *((ProcDataLocalBase_c*)((void*)ProcessElementBase_c::pprocessData()));
  };
  /**
    deliver a pointer to ProcDataLocal_c
      (the base function only delivers ProcDataBase_c)
    @return pointer to containing ProcDataLocal_c
  */
  ProcDataLocalBase_c * pprocessData()const
  {
    return ((ProcDataLocalBase_c*)((void*)ProcessElementBase_c::pprocessData()));
  };

  /**
    helper function to check val() against limits
    @return TRUE if value sending allowed
  */
  bool minMaxLimitsPassed(Proc_c::doSend_t ren_doSend) const;

private: // Private attributes
  /** last master (eg. main prog or sensor) val  */
  int32_t mi32_lastMasterVal;
  /**
    sum of values which are used to calculate the medium
    (only defined if one proportional prog is active)
  */
  int32_t mi32_medSum;
  /**
    integral value (only defined if one proportional prog is active)
  */
  int32_t mi32_integ;

  /** count of used songle values to calculate medium val  */
  int32_t mi32_medCnt;


  /**
    stores if one subprog triggered,
    if sent wasn't possible within one call, this could be retried
    in later calls; a true val is only reseted to false, if sending was possible
  */
  bool mb_triggeredIncrement;

  /** gathered information about currently running threshold sub progs */
  List_ThresholdInfo mlist_thresholdInfo;
};

}
#endif
