/***************************************************************************
 *                                                                         *
 * This file is part of the "IsoAgLib", an object oriented program library *
 * to serve as a software layer between application specific program and   *
 * communication protocol details. By providing simple function calls for  *
 * jobs like starting a measuring program for a process data value on a    *
 * remote ECU, the main program has not to deal with single CAN telegram   *
 * formatting. This way communication problems between ECU's which use     *
 * this library should be prevented.                                       *
 * Everybody and every company is invited to use this library to make a    *
 * working plug and play standard out of the printed protocol standard.    *
 *                                                                         *
 * Copyright (C) 2000 - 2004 Dipl.-Inform. Achim Spangler                  *
 *                                                                         *
 * The IsoAgLib is free software; you can redistribute it and/or modify it *
 * under the terms of the GNU General Public License as published          *
 * by the Free Software Foundation; either version 2 of the License, or    *
 * (at your option) any later version.                                     *
 *                                                                         *
 * This library is distributed in the hope that it will be useful, but     *
 * WITHOUT ANY WARRANTY; without even the implied warranty of              *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU       *
 * General Public License for more details.                                *
 *                                                                         *
 * You should have received a copy of the GNU General Public License       *
 * along with IsoAgLib; if not, write to the Free Software Foundation,     *
 * Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA           *
 ***************************************************************************/
#ifndef SYSTEM_PC_VC
	#include <stdint.h>
#else
	typedef unsigned char uint8_t;
	typedef signed char int8_t;
	typedef unsigned short uint16_t;
	typedef short int16_t;
	typedef unsigned int uint32_t;
	typedef int int32_t;
	typedef unsigned long long int uint64_t;
	typedef long int long int64_t;
#endif

#define stringLength 256


// Object Types
#define otDevice                       (0)
#define otDeviceElement                (1)
#define otDeviceProcessData            (2)
#define otDeviceProperty               (3)
#define otDeviceProcessDataCombination (4)
#define otDeviceValuePresentation      (5)
#define maxObjectTypes                 (6) // +++ MAX +++ //object will not be inserted if (objType >= maxObjectTypes)


// Object May Be
#define ombDevice                       (uint64_t(1)<<0)
#define ombDeviceElement                (uint64_t(1)<<1)
#define ombDeviceProcessData            (uint64_t(1)<<2)
#define ombDeviceProperty               (uint64_t(1)<<3)
#define ombDeviceProcessDataCombination (uint64_t(1)<<4)
#define ombDeviceValuePresentation      (uint64_t(1)<<5)

// Attributes
#define attrDesignator (0)
#define attrSoftware_version (1)
#define attrWorkingset_mastername (2)
#define attrSerial_number (3)
#define attrStructure_label (4)
#define attrLocalization_label (5)
#define attrElement_type (6)
#define attrElement_number (7)
#define attrParent_name (8)
#define attrDdi (9)
#define attrProperties (10)
#define attrTrigger_methods (11)
#define attrDevice_value_presentation_name (12)
#define attrProperty_value (13)
#define attrOffset (14)
#define attrScale (15)
#define attrNumber_of_decimals (16)
#define attrUnitdesignator (17)
#define attrSelf_conf (18)
#define attrIndustry_group (19)
#define attrFunction (20)
#define attrWanted_SA (21)
#define attrFunc_Inst (22)
#define attrECU_Inst (23)
#define attrStore_SA_at_EEPROM_address (24)
#define attrManufacturer_code (25)
#define attrDevice_class (26)
#define attrDevice_class_instance (27)
#define attrWS_identity_number (28)
#define attrFeature_set (29)
#define attrPriority (30)
#define attrDevProgVarName (31)
#define attrProcProgVarName (32)
#define attrCumulative_value (33)
#define attrCommand_type (34)
#define attrSetpoint (35)

#define maxAttributeNames (36)


#define maxObjectTypesToCompare (maxObjectTypes)
char otCompTable [maxObjectTypesToCompare] [stringLength+1] = {
    "device",
    "deviceelement",
    "deviceprocessdata",
    "deviceproperty",
    "deviceprocessdatacombination",
    "devicevaluepresentation"
};

uint64_t omcTypeTable [maxObjectTypesToCompare] = {
    /* "device", */                       ombDeviceElement | ombDeviceValuePresentation,
    /* "deviceelement" */                 ombDeviceProcessData | ombDeviceProperty,
    /* "deviceprocessdata" */             ombDeviceProcessDataCombination,
    /* "deviceproperty" */                0,
    /* "deviceprocessdatacombination" */  0,
    /* "devicevaluepresentation" */       0
};

char otClassnameTable [maxObjectTypes] [stringLength+1] = {
    "Device",
    "DeviceElement",
    "DeviceProcessData",
    "DeviceProperty",
    "DeviceProcessDataCombination",
    "DeviceValuePresentation"
};


char attrNameTable [maxAttributeNames] [stringLength+1] = {
      "designator",
      "software_version",
      "workingset_mastername",
      "serialnumber",
      "structure_label",
      "localization_label",
      "element_type",
      "element_number",
      "parent_name",
      "ddi",
      "properties",
      "trigger_methods",
      "device_value_presentation_name",
      "property_value",
      "offset",
      "scale",
      "number_of_decimals",
      "unit_designator",
      "self_configurable_address",
      "industry_group",
      "function",
      "wanted_sa",
      "function_instance",
      "ecu_instance",
      "store_sa_at_eeprom_address",
      "manufacturer_code",
      "device_class",
      "device_class_instance",
      "ws_identity_number",
      "feature_set",
      "priority",
      "device_program_name",
      "proc_program_name",
      "cumulative_value",
      "command_type",
      "is_setpoint"
};

#define maxTableID 7
char tableIDTable [maxTableID] [stringLength+1] = {
    "DVC",
    "DET",
    "DPD",
    "DPT",
    "DPC",
    "DVP"
};

#define maxDeviceElementType 7
char deviceElementTypeTable [maxDeviceElementType] [stringLength+1] = {
    "device",
    "function",
    "bin",
    "section",
    "unit",
    "connector",
    "navigationreference"
};

#define maxProperty 2
char propertyTable [maxProperty] [stringLength+1] = {
    "defaultset",
    "setable"
};

#define maxTriggerMethods 5
char triggerMethodTable [maxTriggerMethods] [stringLength+1] = {
    "timeinterval",
    "distanceinterval",
    "thresholdlimits",
    "onchange",
    "counter"
};

#define maxTruthTable 2
char truthTable [maxTruthTable] [stringLength+1] = {
    "yes",
    "true"
};

#define maxFalseTable 2
char falseTable [maxFalseTable] [stringLength+1] = {
    "no",
    "false"
};

#define maxFeatureSet 4
char featureSetTable [maxFeatureSet] [stringLength+1] = {
    "Standard",
    "SimpleMeasure",
    "SimpleSetpoint",
    "SimpleSetpointSimpleMeasure"
};

#define maxCommandTypes 4
char commandTypeTable [maxCommandTypes] [stringLength+1] = {
    "exact",
    "default",
    "min",
    "max"
};
