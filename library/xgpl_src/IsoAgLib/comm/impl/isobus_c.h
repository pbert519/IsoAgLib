/*
  isobus_c.h: Access class for the managed ISO11783-CAN-instances

  (C) Copyright 2009 - 2011 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/
#ifndef ISOBUS_C_H
#define ISOBUS_C_H

#include <IsoAgLib/util/impl/singleton.h>
#include <IsoAgLib/hal/hal_system.h>
#include <IsoAgLib/driver/can/imaskfilter_c.h>
#include <IsoAgLib/driver/can/impl/canio_c.h>


namespace __IsoAgLib {



/**
  Class managing one ISOBUS CAN instance.
  @author Martin Wodok
  @see CanIo_c
*/
class IsoBus_c {
#if 1 < PRT_INSTANCE_CNT
  MACRO_MULTITON_CONTRIBUTION();
#else
  MACRO_SINGLETON_CONTRIBUTION();
#endif
public:

  /** Initialize the CAN hardware and all related ISO-protocol modules.
      for possible error refer to @see CanIo_c
      @param aui8_busNumber number of the physical CAN bus (starting at 0)
      @return true if successful, false if bus-problem or already initialized
    */
  bool init (uint8_t aui8_busNumber);

  /** Close the CAN hardware and all related ISO-protocol modules.
      @return true if successful, false if already closed
    */
  bool close();

  IsoBus_c& operator<< (CanPkgExt_c& acrc_src);
  IsoBus_c& operator<< (CanPkg_c& acrc_src);

  uint8_t getBusNumber() const { return getCanInstance4Comm().getBusNumber(); }

  #ifdef USE_CAN_MEASURE_BUSLOAD
  uint16_t getBusLoad() const { return getCanInstance4Comm().getBusLoad(); }
  #endif

  uint8_t sendCanFreecnt() { return getCanInstance4Comm().sendCanFreecnt(); }

  bool stopSendRetryOnErr() { return getCanInstance4Comm().stopSendRetryOnErr(); }

  // @todo to be changed to return the FilterBox instead of a boolean.
  bool existFilter(const __IsoAgLib::CanCustomer_c& ar_customer, const IsoAgLib::iMaskFilter_c& arc_maskFilter ) {
    return getCanInstance4Comm().existFilter (ar_customer, IsoAgLib::iMaskFilterType_c( arc_maskFilter, IsoAgLib::iIdent_c::ExtendedIdent ), NULL);
  }

  FilterBox_c* insertFilter( CanCustomer_c& ar_customer,
                             const IsoAgLib::iMaskFilter_c& arc_maskFilter,
                             int ai_dlcForce,
                             bool ab_reconfigImmediate)
  {
    return getCanInstance4Comm().insertFilter(
      ar_customer,
      IsoAgLib::iMaskFilterType_c( arc_maskFilter, IsoAgLib::iIdent_c::ExtendedIdent ),
      ai_dlcForce,
      ab_reconfigImmediate);
  }

  bool insertStdFilter( CanCustomer_c& ar_customer,
                        const IsoAgLib::iMaskFilter_c& arc_maskFilter,
                        int ai_dlcForce,
                        bool ab_reconfigImmediate )
  {
    return ( getCanInstance4Comm().insertFilter(
        ar_customer,
        IsoAgLib::iMaskFilterType_c(  arc_maskFilter, IsoAgLib::iIdent_c::StandardIdent ),
        ai_dlcForce,
        ab_reconfigImmediate) != NULL );
  }


  bool deleteStdFilter( const CanCustomer_c& ar_customer, const IsoAgLib::iMaskFilter_c& arc_maskFilter )
  {
    return getCanInstance4Comm().deleteFilter (ar_customer, IsoAgLib::iMaskFilterType_c( arc_maskFilter, IsoAgLib::iIdent_c::StandardIdent) );
  }


  bool existStdFilter(const CanCustomer_c& ar_customer, const IsoAgLib::iMaskFilter_c& arc_maskFilter )
  {
    return getCanInstance4Comm().existFilter (ar_customer, IsoAgLib::iMaskFilterType_c( arc_maskFilter, IsoAgLib::iIdent_c::StandardIdent), NULL);
  }


 /** create a Standard Iso Filter Box
    possible errors:
        * Err_c::badAlloc on not enough memory for new FilterBox
        instance or for new configured MsgObj_c's
    @see __IsoAgLib::CANCustomer
    @param ar_customer reference to __IsoAgLib::CanCustomer_c  which needs
         filtered messages (-> on received msg call ar_customer.processMsg())
    @param aui32_pgn PGN
    @param ab_reconfigImmediate true -> all Filter objects are reconfigured
         to according CAN hardware MsgObj after creating this filter
    @return != true -> if inserting and wanted reconfiguration are performed without errors,
      a reference to the created FilterBox is returned
   @exception badAlloc
  */
  FilterBox_c* insertStandardIsoFilter(
    __IsoAgLib::CanCustomer_c& ar_customer,
    uint32_t aui32_pgn,
    bool ab_reconfigImmediate);

  bool reconfigureMsgObj() { return getCanInstance4Comm().reconfigureMsgObj(); }

  bool deleteFilter( const __IsoAgLib::CanCustomer_c& ar_customer, const IsoAgLib::iMaskFilter_c& arc_maskFilter )
  { return getCanInstance4Comm().deleteFilter (ar_customer, IsoAgLib::iMaskFilterType_c( arc_maskFilter, IsoAgLib::iIdent_c::ExtendedIdent ) ); }

  bool deleteAllFiltersForCustomer (const __IsoAgLib::CanCustomer_c& ar_customer)
  { return getCanInstance4Comm().deleteAllFiltersForCustomer (ar_customer); }
private:
  IsoBus_c() {}
  friend IsoBus_c &getIsoBusInstance(uint8_t aui8_instance);
};


/** C-style function, to get access to the unique singleton instance(s) */
IsoBus_c &getIsoBusInstance( uint8_t aui8_instance = 0 );

} // __IsoAgLib

#endif // ISOBUS_C_H
