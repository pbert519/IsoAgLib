/*
  idiagnosticstypes.h: type for Failure Mode Indicators

  (C) Copyright 2009 - 2013 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/
#ifndef IDIAGNOSTICS_TYPES_H
#define IDIAGNOSTICS_TYPES_H

#include <IsoAgLib/isoaglib_config.h>
#include <IsoAgLib/util/impl/bitfieldwrapper_c.h>

namespace IsoAgLib {

/** use an enum for the different Failure Mode Indicators */
enum FailureModeIndicator_en {

  FMIDataValidButAboveNormalOperationRangeMostSevereLevel = 0,
  FMIDataValidButBelowNormalOperationalRangeMostSevereLevel,
  FMIDataErraticIntermittentOrIncorrect,
  FMIVoltageAboveNormalOrShortedToHighSource,
  FMIVoltageBelowNormalOrShortedToLowSource,
  FMICurrentBelowNormalOrOpenCircuit,
  FMICurrentAboveNormalOrGroundedCircuit,
  FMIMechanicalSystemNotRespondingOrImproperlyAdjusted,
  FMIAbnormalFrequencyOrPulseWidthOrPeriod,
  FMIAbnormalUpdateRate,
  FMIAbnormalRateOfChange,
  FMIRootCauseNotKnown,
  FMIBadIntelligentUnitOrComponent,
  FMIOutOfCalibration,
  FMISpecialInstructions,
  FMIDataValidButAboveNormalOperatingRangeLeastSevereLevel,
  FMIDataValidButAboveNormalOperatingRangeModeratelySevereLevel,
  FMIDataValidButBelowNormalOperatingRangeLeastSevereLevel,
  FMIDataValidButBelowNormalOperatingRangeModeratelySevereLevel,
  FMIReceivedNetworkDataInError,
  FMIConditionExists = 31

};

enum CertificationRevision_t
{
  CertificationRevisionFirstRevision = 0,
  CertificationRevisionSecondRevision = 1,
  CertificationRevisionThirdRevision = 2,
  CertificationRevisionFourthRevision = 3,
  CertificationRevisionNotAvailable = 7
};


enum CertificationLabType_t
{
  CertificationLabTypeNonCertifiedLab = 0,
  CertificationLabTypeAEFCertifiedLab = 1,
  CertificationLabTypeNotAvailable = 3
};

} // IsoAgLib


#endif

