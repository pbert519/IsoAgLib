/*
  filterbox_c.h: Filter_Box_c permits free definition of mask/filter
    for receiving CAN telegrams by a CANCustomer (or derived) object

  (C) Copyright 2009 - 2012 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/
#ifndef FILTER_BOX_H
#define FILTER_BOX_H

/* *************************************** */
/* ********** include headers ************ */
/* *************************************** */
#include <IsoAgLib/util/iliberr_c.h>
#include <IsoAgLib/hal/hal_can.h>

#include "ident_c.h"
#include <IsoAgLib/comm/Part3_DataLink/impl/canpkgext_c.h>
#include <IsoAgLib/driver/can/impl/cancustomer_c.h>
#include <IsoAgLib/driver/can/imaskfilter_c.h>

// Begin Namespace __IsoAgLib
namespace __IsoAgLib {

/* *************************************** */
/* ********* class definition ************ */
/* *************************************** */
/** Virtual Msg Box where special Filter and Mask can be defined.
  CanIo_c handles free amount of instances of this class.
  Every MsgObj_c has 1 to m appointed instances of this class.
  Each FilterBox_c has exact one CanCustomer_c , which is sored on creation
  of a FilterBox_c instance, who registered for processing of received msg.
  @short FilterBox_c can use individual filter and mask
    for selecting received CAN telegrams and controls their processing.

  @author Dipl.-Inform. Achim Spangler
*/

class FilterBox_c {
public:
  struct CustomerLen_s
  {
    CustomerLen_s (CanCustomer_c* apc_customer, int8_t ai8_dlcForce) : pc_customer(apc_customer), i8_dlcForce (ai8_dlcForce) {}
    CanCustomer_c * pc_customer;
    int8_t i8_dlcForce; // 0..8: DLC must exactly be 0..8.   < 0 (-1): DLC doesn't care! (default!)
  };

  /** default constructor without parameter values for creating an instance
    with default start state (init variables)

     @exception badAlloc
  */
  FilterBox_c();

  /** copy constructor which uses data of another FilterBox_c instance
    @param acrc_src reference to the source FilterBox_c instance for copying
     @exception badAlloc
  */
  FilterBox_c(const FilterBox_c& acrc_src);

  /** copy values of acrc_src FilterBox_c object to this instance
    possible errors:
        * Err_c::badAlloc on not enough memory for copying puffed CAN msg from source

    @param acrc_src FilterBox_c instance with data to assign to this instance
    @return reference to this instance for chains like "box_1 = box_2 = ... = box_n;"
  */
  FilterBox_c& operator=(const FilterBox_c& acrc_src);

  /** clear the data of this instance */
  void clearData();


  /** store new can customer with same filter and mask
      @param pc_cancustomer  new can customer */
   void insertCustomer(CanCustomer_c* pc_cancustomer, int8_t ai8_len) {mvec_customer.push_back(CustomerLen_s(pc_cancustomer, ai8_len));}

  /* *************************************** */
  /* ******* filter/mask managing ********** */
  /* *************************************** */

  /** set the mask (t_mask) and filter (t_filter) of this FilterBox
    @param arc_maskFilter filter mask pair combination
    @param apc_customer pointer to the CanCustomer_c instance, which creates this FilterBox_c instance
    @param ai8_dlcForce force the DLC to be exactly this long (0 to 8 bytes). use -1 for NO FORCING and accepting any length can-pkg
  */
   void set (const IsoAgLib::iMaskFilterType_c& arc_maskFilter, 
            CanCustomer_c *apc_customer = NULL,
            int8_t ai8_dlcForce = -1);

  /** checks, if FilterBox_c definition given by ac_mask and ac_filter is the same
    @param ac_mask mask to use for comparison
    @param ac_filter filter to use for comparison
    @return true -> given mask and filter are same as the local defs
  */
  bool equalFilterMask(const IsoAgLib::iMaskFilterType_c& arc_filterMaskPair ) const
    { return (mc_maskFilterPair == arc_filterMaskPair ); }

  /** checks, if Filter_Box_c has already stored given customer
      @param ar_customer  customer to compare
      @return               true -> customer already stored
    */
  bool equalCustomer( const __IsoAgLib::CanCustomer_c& ar_customer ) const;


  /** delete CanCustomer_c  instance from array
      @param  ar_customer  CANCustomer to delete
      @return                true -> no more cancustomers exist, whole filterbox can be deleted
    */
  bool deleteFilter(const __IsoAgLib::CanCustomer_c & ar_customer);

  /** deliver const reference to filter mask pair */
  const IsoAgLib::iMaskFilterType_c& maskFilterPair() const {
    return mc_maskFilterPair;
  }

  /* ************************************************** */
  /* ***** insert/get/process buffered CanPkg_c ******* */
  /* ************************************************** */

  /** control the processing of a received message
    (MsgObj_c::processMsg inserted data directly in CANCustomer
     -> FilterBox_c::processMsg() initiates conversion of CAN string
        to data flags and starts processing in CanCustomer_c )

    possible errors:
        * Err_c::precondition no valid CanCustomer_c  (or derived) is registered
    @return true -> FilterBox_c was able to inform registered CANCustomer
  */
  bool processMsg( CanPkg_c& pkg );

private:
// Private attributes
  IsoAgLib::iMaskFilterType_c mc_maskFilterPair;

  /**vector of pointer to pc_customer CanCustomer_c  which works with the received CAN data */
  STL_NAMESPACE::vector<CustomerLen_s> mvec_customer;

  /** Indicate if FilterBox is iterating through the customers in "processMsg" */
  static FilterBox_c* mspc_currentlyProcessedFilterBox;
  static int msi_processMsgLoopIndex; /// "< 0" indicates Loop ==> need to adapt at delete operations
  static int msi_processMsgLoopSize;  /// used if in Loop mode, need to be adapted at remove/add, too.

};
}
#endif
