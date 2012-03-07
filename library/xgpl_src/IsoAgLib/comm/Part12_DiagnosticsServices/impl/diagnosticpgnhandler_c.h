/*
  diagnosticpgnhandler_c.h

  (C) Copyright 2009 - 2012 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/
#ifndef DIAGNOSTIC_PGN_HANDLER_C_H
#define DIAGNOSTIC_PGN_HANDLER_C_H

#include <IsoAgLib/hal/hal_typedef.h>
#include <IsoAgLib/comm/Part5_NetworkManagement/impl/isorequestpgnhandler_c.h>
#include <IsoAgLib/util/impl/bitfieldwrapper_c.h>

#include "../idiagnosticstypes.h"

// Begin Namespace __IsoAgLib
namespace __IsoAgLib {

// forward declarations
class IdentItem_c;
class DiagnosticProtocol_c;

/**
  This class implements the handling of Diagnostic
  requests for a given IdentItem_c instance

  @author Nizar Souissi
  @author Martin Wodok
*/
class DiagnosticPgnHandler_c : public IsoRequestPgnHandler_c
{

public:
  DiagnosticPgnHandler_c (IdentItem_c&);
  virtual ~DiagnosticPgnHandler_c();

  void init();
  void close();

  virtual bool processMsgRequestPGN (uint32_t /*aui32_pgn*/, IsoItem_c* /*apc_isoItemSender*/, IsoItem_c* /*apc_isoItemReceiver*/, int32_t );


  bool setEcuIdentification( const char *acstr_partNr, const char *acstr_serialNr, const char *acstr_location, const char *acstr_type, const char *acstr_manufacturerName );

  bool setSwIdentification( const char *acstr_swIdentification );

  //! Setter for the different certification message fields
  //! Parameter:
  //! @param ui16_year Certification year as in ISO 11783-7 A.29.1, must be between 2000 and 2061 
  //! @param a_revision Certification revision as in ISO 11783-7 A.29.2
  //! @param a_laboratoryType Certification laboratory type as in ISO 11783-7 A.29.3
  //! @param aui16_laboratoryId Certification laboratory ID (11 bits wide) as in ISO 11783-7 A.29.4
  //! @param acrc_certificationBitMask Compliance certification type bitfield ( as in ISO 11783-7 A.29.5 till A.29.17 )
  //! @param aui16_referenceNumber Compliance certification reference number ( as in ISO 11783-7 A.29.18 )
  bool setCertificationData(
    uint16_t ui16_year, IsoAgLib::CertificationRevision_t a_revision,
    IsoAgLib::CertificationLabType_t a_laboratoryType, uint16_t aui16_laboratoryId,
    const IsoAgLib::CertificationBitMask_t& acrc_certificationBitMask, uint16_t aui16_referenceNumber );

  DiagnosticProtocol_c& getDiagnosticProtocol();

private:
  void sendSinglePacket (const HUGE_MEM uint8_t* rhpb_data,int32_t ai32_pgn);

private: // attributes
  IdentItem_c& mrc_identItem;
  DiagnosticProtocol_c* mpc_diagnosticProtocol;

  char *mcstr_EcuIdentification;
  char *mcstr_SwIdentification;
  bool mb_certificationIsSet;
  uint8_t m_certification[8];

private:
  /** not copyable : copy constructor is only declared, never defined */
  DiagnosticPgnHandler_c(const DiagnosticPgnHandler_c&);
  /** not copyable : copy operator is only declared, never defined */
  DiagnosticPgnHandler_c& operator=(const DiagnosticPgnHandler_c&); 
};

} // namespace __IsoAgLib
#endif
