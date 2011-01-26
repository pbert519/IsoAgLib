/*
  isoaglib_config.h: application specific configuration settings

  (C) Copyright 2009 - 2010 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/
#ifndef _ISOAGLIB_CONFIG_H_
#define _ISOAGLIB_CONFIG_H_


/** \page Hardwareadaptation Howto adapt IsoAgLib to new platform
  This page describes the needed steps to adapt the IsoAgLib to an new
  platform type.
  NOTE: THIS IS UNDER DEVELOPMENT, AS THE HAL STRUCTURE CHANGED!
  <ul>
      <li>\<FOO\>.h -> adapt calls to wanted target BIOS/OS
      <li>\<FOO\>_target_extensions.h/cc -> implement functions which aren't included in standard library of target ( i.e. which can't be adapted by simple name mapping and
      parameter reordering )
      <li>hal_can_interface.h/cc -> standardized CAN implementation ( simple name mapping is not suitable for CAN )
  </ul>
*/

/** allow configuration by parameter value "YES" */
#define YES 1
/** allow configuration by parameter value "NO" */
#define NO 0

/** test if the autogenerated config header must be included */
#ifdef PRJ_USE_AUTOGEN_CONFIG
#  define _autogen_config_header_ <PRJ_USE_AUTOGEN_CONFIG>
#  include _autogen_config_header_
#endif
#include <IsoAgLib/util/compiler_adaptation.h>
#include <IsoAgLib/hal/hal_config.h>
// Note: "hal_config.h" needs the path-defines from PRJ_USE_AUTOGEN_CONFIG


#define Stringize(x) #x
#define ExpandAndStringize(x) Stringize(x)


/**************************************************************/
/**
 * \name Handle fast data type
 */
#ifndef HANDLE_FAST_DATATYPE_AS_STRICT
#  define HANDLE_FAST_DATATYPE_AS_STRICT NO
#endif

/* ******************************************************** */
/**
 * \name CONFIG_DEFAULT_POWERDOWN_STRATEGY Default strategy which is used by
 * IsoAgLib to decide on powerdown. As an automatic poweroff due to
 * detection of CanEn loss could cause unwanted system stopp on short
 * voltage low bursts of the power supply, it is appropriate to
 * stop the automatic powerdown in favour of an explicit call of
 * power off.
 * But this explicit handling could cause problems, if the application
 * reaches a state, where the watchdog is triggered, so that no reset
 * occurs, but the application doesn't either handle a loss of CanEn.
 * This could causes a never ending run of the ECU, if the application
 * can reach such a state.
 * Possible Settings: IsoAgLib::PowerdownByExplcitCall
 *                    or  IsoAgLib::PowerdownOnCanEnLoss
 */
/*@{*/
#ifndef CONFIG_DEFAULT_POWERDOWN_STRATEGY
  /** select Power Down trigger source:
      - IsoAgLib::PowerdownByExplcitCall for staying alive during CAN_EN loss,
          and allowing controlled powerdown after detection of CAN_EN loss
      - IsoAgLib::PowerdownOnCanEnLoss for automatic stop of ECU on detection of
          CAN_EN loss
  */
  /// select reaction on powerdown detection - can be manually overridden in project config file
#  define CONFIG_DEFAULT_POWERDOWN_STRATEGY IsoAgLib::PowerdownByExplcitCall
#endif

#ifndef CONFIG_BUFFER_SHORT_CAN_EN_LOSS_MSEC
  /** select time intervall in MSEC during which the loss of CAN_EN is ignored.
      IMPORTANT: This slection has only influence on system if IsoAgLib::PowerdownByExplcitCall
      is selected, as this settings causes iSystem_c::canEn() to return _TRUE_ in shorter power loss
      phases than the time intervall specified here.
      The automatic power down of the ECU ( OS or BIOS ) can't be manipulated this way.
      IF iSystem_c::canEn() shall immediately answer false on CAN_EN loss, set this defined as
      UNDEFINED!!! - the handling of this buffer time is then compiled away
    */
  /// define consolidation time for CAN_EN loss detection - can be manually overridden in project config file
#  define CONFIG_BUFFER_SHORT_CAN_EN_LOSS_MSEC 1000
#endif
/*@}*/


/* ******************************************************** */
/**
 * \name Set buffer sizes for CAN handling
 */
/*@{*/
#ifndef CONFIG_CAN_SEND_BUFFER_SIZE
  /// set buffer size for CAN send
#  define CONFIG_CAN_SEND_BUFFER_SIZE 20
#endif

/** configure the MULTI-PACKET parameters (TP/ETP/FP/Chunks)
    for RECEPTION, i.e. MultiReceive-related classes
*/
#ifndef CONFIG_MULTI_RECEIVE_CHUNK_SIZE_IN_PACKETS
  /// Multiply this SizeInPackets by 7 to get the size of ONE CHUNK (ONLY OF INTEREST IF STREAM IS CHUNKED)
#  define CONFIG_MULTI_RECEIVE_CHUNK_SIZE_IN_PACKETS 16
#endif

/** configures the maximum amount of packets to be allowed by all "CTS" messages.
    So you need to have at least a CAN-buffer for this many packets,
    because the clients will burst them and you cannot guarantee to handle
    them all immediately. Note that you of course need to have an even
    larger CAN-buffer, because other packets are also being received,
    like GPS data, Tractor data, network management, etc.etc.
    Be aware: This is only the part for MULTI_RECEIVE!
*/
#ifndef CONFIG_MULTI_RECEIVE_MAX_OVERALL_PACKETS_ADDED_FROM_ALL_BURSTS
  /// Maximum amount of packets to be allowed to be CTS'd in parallel (this amount will be distributed among all running streams)
#  define CONFIG_MULTI_RECEIVE_MAX_OVERALL_PACKETS_ADDED_FROM_ALL_BURSTS 16
#endif

/** configures the maximum amount of packets to be allowed by a "CTS" message.
    This is a maximum-cutoff value per client. MultiReceive will normally
    CTS for "CONFIG_MULTI_RECEIVE_MAX_BURST_IN_PACKETS_OVERALL / Number of clients" packets
    Use this value to limit if e.g. just one stream is running.
    Example: CONFIG_MULTI_RECEIVE_MAX_BURST_IN_PACKETS_OVERALL is set to 80 because you want 5 clients
    to be able to each send 16 packets, BUT you don't want if one single client
    is connected to allow 80 packets to be sent at once!
*/
#ifndef CONFIG_MULTI_RECEIVE_MAX_PER_CLIENT_BURST_IN_PACKETS
  /// Maximum amount of packets to be allowed by a "CTS" message.
#  define CONFIG_MULTI_RECEIVE_MAX_PER_CLIENT_BURST_IN_PACKETS CONFIG_MULTI_RECEIVE_MAX_OVERALL_PACKETS_ADDED_FROM_ALL_BURSTS
#endif

/** This delay is being used if EXACTLY ONE stream (1) is being received! */
#ifndef CONFIG_MULTI_RECEIVE_CTS_DELAY_AT_SINGLE_STREAM
  /// defines the amount of msec. to be wait until sending out the next CTS to the sender IF ONE STREAM ONLY BEING RECEIVED
#  define CONFIG_MULTI_RECEIVE_CTS_DELAY_AT_SINGLE_STREAM 0
#endif

/** This delay is being used if MORE THAN ONE stream (2, 3, 4, ...) is being received */
#ifndef CONFIG_MULTI_RECEIVE_CTS_DELAY_AT_MULTI_STREAMS
  /// defines the amount of msec. to be wait until sending out the next CTS to the sender IF MULTIPLE STREAMS BEING RECEIVED
#  define CONFIG_MULTI_RECEIVE_CTS_DELAY_AT_MULTI_STREAMS 50
#endif


/*@}*/


#define WORD_LO_HI 0
#define WORD_HI_LO 1
#define BYTE_HI_LO 2

/* ******************************************************** */
/**
 * \name Set configuration parameter for CAN FIFO handling. Valid only in case that not defined SYSTEM_WITH_ENHANCED_CAN_HAL
 */
/** Exponent of the 2^N operation,used  to determine the BufferSize of s_canFifoInstance
* NOTE : The CAN_FIFO_EXPONENT_BUFFER_SIZE must be less than  TARGET_WORDSIZE -1,
* otherwise the overflow of the UC and AC counter can lead to loss of CAN message. */

#ifndef CAN_FIFO_EXPONENT_BUFFER_SIZE
#  define  CAN_FIFO_EXPONENT_BUFFER_SIZE 8
#endif

/** When during the reconfiguration only a number of place less than CAN_FIFO_CRITICAL_FILLING_TOLERANCE_LEVEL
* are free in the CAN FIFO BUFFER, we are in  critical Filled level and also during the reconfiguration
* the Canio_c::processMsg it is called
*/

#ifndef CAN_FIFO_CRITICAL_FILLING_TOLERANCE_LEVEL
#  define  CAN_FIFO_CRITICAL_FILLING_TOLERANCE_LEVEL 9
#endif

/** The Variable is the extimated time occurring to the productor for writing one buffer element.
* NOTE: the measuring unit of this element is microsecond for the target ESX and millisend for the PC
* Please refers to the measurement unit returned by the function getCurrentTime()
*/

#ifndef CAN_FIFO_WRITING_TIME
#  define  CAN_FIFO_WRITING_TIME 100
#endif


/** In the CAN FIFO reading, if we detect that the buffer element,  where we want to read, has been writing
* right now, if the CAN_FIFO_READ_WAITING is set to YES, we wait a time= CAN_FIFO_WRITING_TIME and then read again.
*/

#ifdef CONFIG_CAN_FIFO_READ_WAITING
#   define  CAN_FIFO_READ_WAITING
#endif


/* ******************************************************** */

/**
 * \name Config access to RS232
 * Set the baudrate, buffer sizes and value coding.
 * These settings are used for initialisation on first access to RS232 ( pattern of singletons ).
 * All defines of this block can be overridden by project specific config file.
 */
/*@{*/
#ifndef CONFIG_RS232_DEFAULT_XON_XOFF
  /// define default RS232 XON-XOFF usage - can be manually overridden in project config file
#  define CONFIG_RS232_DEFAULT_XON_XOFF false
#endif

#ifndef CONFIG_RS232_DEFAULT_SND_PUF_SIZE
  /// define default size of RS232 send buffer - can be manually overridden in project config file
#  define CONFIG_RS232_DEFAULT_SND_PUF_SIZE 200
#endif

#ifndef CONFIG_RS232_DEFAULT_REC_PUF_SIZE
  /// define size of RS232 receive buffer - can be manually overridden in project config file
#  define CONFIG_RS232_DEFAULT_REC_PUF_SIZE 10
#endif
/*@}*/

/* ******************************************************** */
/**
 * \name Basic config for sensor and actor
 * Set default PWM value for digital output and
 * calculation factors for analog input.
 * All defines of this block can be overridden by project specific config file.
 */
/*@{*/
#ifndef CONFIG_PWM_DEFAULT_FREQUENCY
  /// set default PWM frequency for digital output - can be manually overridden in project config file
#  define CONFIG_PWM_DEFAULT_FREQUENCY 100000
#endif
/*@}*/


/* ******************************************************** */
/**
 * \name Different time intervalls
 * Define execution time of timeEvent calls
 * and choose time for clearing of stale monitor list entries
 * All defines of this block can be overridden by project specific config file.
 */
/*@{*/
/** default maximum time to perform the function iScheduler_c::timeEvent
    if this function isn't called with a defined termination timestamp.
    This is needed to avoid a deadlock which could happen if processing
    and reaction on received CAN messages causes a never endling loop of
    new received messages - or in other words: if the CAN message queues
    don't ever get empty during the processing.
    If this time limit is reached, then something is probably quite broken
    on CAN BUS.
  */
#ifndef CONFIG_DEFAULT_MAX_SCHEDULER_TIME_EVENT_TIME
  /// default max execution time of iScheduler_c::timeEvent - can be manually overridden in project config file
#  define CONFIG_DEFAULT_MAX_SCHEDULER_TIME_EVENT_TIME 500
#endif

/** time to delete inactive ISO items in monitor list [msec]
    - if CONFIG_ISO_ITEM_MAX_AGE is not defined, no items are deleted
*/
#ifndef CONFIG_ISO_ITEM_MAX_AGE
  /// erase ISO 11783 items after time (>0), on missing address claim after request - can be manually overridden in project config file
#  define CONFIG_ISO_ITEM_MAX_AGE 3000
#endif

/** interval in sec to store actual process data value for process datas which are
    permanent in EEPROM
*/
#ifndef CONFIG_PROC_STORE_EEPROM_INTERVAL
  /// interval [ms] for value update in EEPROM for local process data - can be manually overridden in project config file
#  define CONFIG_PROC_STORE_EEPROM_INTERVAL 5000
#endif
/*@}*/


/* ******************************************************** */
/**
 * \name Basic BUS and MsgObj settings for CAN access
 */
/*@{*/
#ifndef CONFIG_CAN_DEFAULT_BUS_NUMBER
  /// CAN BUS number for IsoAgLib (0xFF forces explicit call of init, to open the CAN BUS )
#  define CONFIG_CAN_DEFAULT_BUS_NUMBER 0xFF
#endif

#ifndef CONFIG_CAN_DEFAULT_MIN_OBJ_NR
  /// define default min number for CAN Msg-Obj used for IsoAgLib
#  define CONFIG_CAN_DEFAULT_MIN_OBJ_NR 0
#endif

#ifndef CONFIG_CAN_DEFAULT_MAX_OBJ_NR
  /// define default max number for CAN Msg-Obj used for IsoAgLib
#  define CONFIG_CAN_DEFAULT_MAX_OBJ_NR 13
#endif
/*@}*/




/* ******************************************************** */
/*@{*/
#ifndef CONFIG_CAN_BLOCK_TIME
  /// BUS problem (not connected, blocked, off, ...)
  /// if CAN send lasts longer than defined
  /// --> discard send-queue and continue without sending.
#  define CONFIG_CAN_BLOCK_TIME 10
#endif

/** define interval for detection of incoming message loss.
    -> should normally NOT be changed by the user/app.
       keep it as is!
*/
#ifndef TIMEOUT_TRACTOR_DATA
  /// Message reception lost if no message received in this time
#  define TIMEOUT_TRACTOR_DATA 3000
#endif

#ifndef CONFIG_EEPROM_PADDING_BYTE
  /** set byte-value to retrieve when reading EEPROM out of bounds */
#  define CONFIG_EEPROM_PADDING_BYTE 0xFF
#endif
/*@}*/

/* ******************************************************** */
/**
 * \name Config basic properties of process data handling
 * Choose basic properties for process data handling
 */
/*@{*/
/// define special command value for release of setpoint
#define SETPOINT_RELEASE_COMMAND  NO_VAL_32S
/// define special command value for signaling setpoint error
#define SETPOINT_ERROR_COMMAND  ERROR_VAL_32S

/**
  * @def RESET_MEASUREMENT_WITH_ZERO_EXACT_SETPOINT_YN
  * some systems send exact setpoint cmd with value 0 to reset a measurement value
  */
#  ifndef RESET_MEASUREMENT_WITH_ZERO_EXACT_SETPOINT_YN

#    define RESET_MEASUREMENT_WITH_ZERO_EXACT_SETPOINT_YN NO
#  endif
#  if RESET_MEASUREMENT_WITH_ZERO_EXACT_SETPOINT_YN == YES && !defined(RESET_MEASUREMENT_WITH_ZERO_EXACT_SETPOINT)
#    define RESET_MEASUREMENT_WITH_ZERO_EXACT_SETPOINT
#  endif


/**
  * @def LAV_COMMENT_MEASURE_PROG_CMD_YN
  * LAV_COMMENT_MEASURE_PROG_CMD allows usage of specific start/stop/reset cmd values
  */
#  ifndef LAV_COMMENT_MEASURE_PROG_CMD_YN
#    define LAV_COMMENT_MEASURE_PROG_CMD_YN NO
#  endif
#  if LAV_COMMENT_MEASURE_PROG_CMD_YN == YES && !defined(LAV_COMMENT_MEASURE_PROG_CMD)
#    define LAV_COMMENT_MEASURE_PROG_CMD
#  endif

/**
  * @def SIMPLE_SETPOINT_WITH_PERCENT_YN
  * choose if simple setpoint should support percent setpoints
  */

#  ifndef SIMPLE_SETPOINT_WITH_PERCENT_YN
#    define SIMPLE_SETPOINT_WITH_PERCENT_YN YES
#  endif
#  if SIMPLE_SETPOINT_WITH_PERCENT_YN == YES && !defined(SIMPLE_SETPOINT_WITH_PERCENT)
#    define SIMPLE_SETPOINT_WITH_PERCENT
#  endif

/**
  * @def SIMPLE_RESPOND_ON_SET_YN
  * choose if local process data should simply respond received setpoint
  * value after storing it (like Fieldstar (tm) )
  */
#  ifndef SIMPLE_RESPOND_ON_SET_YN
#    define SIMPLE_RESPOND_ON_SET_YN YES
#  endif
#  if SIMPLE_RESPOND_ON_SET_YN == YES && !defined(SIMPLE_RESPOND_ON_SET)
#    define SIMPLE_RESPOND_ON_SET
#  endif

/**
  * @def HANDLE_SETPOINT_MEASURE_EQUIVALENT_YN
  * choose if simple setpoint shouldn't distinguish between measurement and setpoint value
  */
#  ifndef HANDLE_SETPOINT_MEASURE_EQUIVALENT_YN
#    define HANDLE_SETPOINT_MEASURE_EQUIVALENT_YN NO
#  endif
#  if HANDLE_SETPOINT_MEASURE_EQUIVALENT_YN == YES && !defined(HANDLE_SETPOINT_MEASURE_EQUIVALENT)
#    define HANDLE_SETPOINT_MEASURE_EQUIVALENT
#  endif

/*@}*/

/* ******************************************************** */
/**
 * \name Define used optional modules
 * Define which of the configurable
 * services should be compiled and used.
 * Set the single ZZ_YN defines to YES / NO or
 * define the equivalent ZZ constant.
 */
/*@{*/
/**
  * @def USE_ISO_11783_YN
  * choose the use of ISO 11783
  */
#  ifndef USE_ISO_11783_YN
#    define USE_ISO_11783_YN NO
#  endif
#  if USE_ISO_11783_YN == YES && !defined(USE_ISO_11783)
#    define USE_ISO_11783
#  endif

/**
  * @def USE_ISO_TERMINAL_YN
  * choose the use of ISO TERMINAL
  */
#  ifndef USE_ISO_TERMINAL_YN
#    define USE_ISO_TERMINAL_YN NO
#  endif
#  if USE_ISO_TERMINAL_YN == YES && !defined(USE_ISO_TERMINAL) && defined( USE_ISO_11783 )
#    define USE_ISO_TERMINAL
#  endif

/**
  * @def USE_PROCESS_YN
  * activate module for base data ( periodic sent main tractor data like speed, PTO, Hitch )
  * ( even if IsoAgLib is mainly independent from this setting, the IsoItem_c::processMsg()
  *   requires this setting, to detect if a call to Base_c:processMsg() is defined -
  *   this call is performed if a REQUEST_PGN_MSG_PGN is processed by IsoItem_c and
  *   the requested PGN is TIME_DATE_PGN )
  */
#  ifndef USE_PROCESS_YN
#    define USE_PROCESS_YN NO
#  endif
#  if USE_PROCESS_YN == YES && !defined(USE_PROCESS)
#    define USE_PROCESS
#  endif


/*@}*/

/* ******************************************************** */
/**
 * \name Define usage of float data type for project.
 *  Relevant for process data, RS232 and some other places
 */
/*@{*/
/**
  * @def USE_FLOAT_DATA_TYPE_YN
  * define whether Process_c process data should provide
  * floating point
  */
#  ifndef USE_FLOAT_DATA_TYPE_YN
#    define USE_FLOAT_DATA_TYPE_YN NO
#  endif
#  if USE_FLOAT_DATA_TYPE_YN == YES && !defined(USE_FLOAT_DATA_TYPE)
#    define USE_FLOAT_DATA_TYPE 1

#  endif
/*@}*/



/* ******************************************************** */
/**
 * \name Definition of IsoAgLib managed CAN ports and dependent access macros
 * The several modules of the IsoAgLib have only one instance, as long
 * as only one CAN port is managed with IsoAgLib protocol.
 * Even in case of two or more CAN ports, the so called "singleton" pattern
 * can be used to define a secure way to handle these objects as standalone
 * instances, which access the other object types by global accessible
 * static functions. This eliminates the need of pointers between the
 * objects.
 */
/*@{*/
/** set the amount of CanIo_c instanes managed by System_c
  * normaly one, but if more CAN BUS'es should be managed
  * System_c can manage them seperate f.e. one for IsoAgLib and the other
  * for system/machine internal CAN communication
*/
#ifndef CAN_INSTANCE_CNT
#  define CAN_INSTANCE_CNT 1
#endif

/** count of CAN ports which are managed with IsoAgLib
    - in most cases just 1
    - but if you want to bridge some other network to IsoBus
      or have simply multiple ISOBUSses then select 2 (or more)
    ( all communication classes are designed to allow independent control of
      individual CAN instances per protocol instance set )
  */
#ifndef PRT_INSTANCE_CNT
#  define PRT_INSTANCE_CNT 1
#endif

#ifndef DEFAULT_BITRATE
  /// define DEFAULT BITRATE to default ISOBUS bitrate
#  define DEFAULT_BITRATE 250
#endif

/** set the amount of RS232IO_c channels available at the ECU
*/
#ifndef RS232_CHANNEL_CNT
#  define RS232_CHANNEL_CNT 1
#else
#  if RS232_CHANNEL_CNT < 1
#    error "RS232_CHANNEL_CNT < 1 is not allowed"
#  elif RS232_CHANNEL_CNT > 9
#    error "RS232_CHANNEL_CNT > 9 is not allowed"
#  endif
#endif

/** set the amount of RS232IO_c instances used by IsoAgLib
  * normaly one, but if more RS232 channels should be managed
  * IsoAgLib can manage them seperate
*/
#ifndef RS232_INSTANCE_CNT
#  define RS232_INSTANCE_CNT 1
#else
#  if RS232_INSTANCE_CNT < 1
#    error "RS232_INSTANCE_CNT < 1 is not allowed"
#  elif RS232_INSTANCE_CNT > 9
#    error "RS232_INSTANCE_CNT > 9 is not allowed"
#  endif
#endif


/*@}*/

/* ******************************************************** */
/**
 * \name Basic identifier settings for CAN access
 */
/*@{*/
/** set the type of mask (uint32_t, uint16_t)
  * -# for extended 29bit header CAN 2.0B
  *    (this can handle both extended AND standard
  *    but it reserves enough space for 29bit ident)

  * -# for standard 11bit header CAN 2.0A
*/
#ifndef MASK_TYPE
#  ifdef USE_ISO_11783
#    define MASK_TYPE uint32_t
#    define MASK_INVALID 0xC0000000L
#  else
#    define MASK_TYPE uint16_t
#    define MASK_INVALID 0xF800
#  endif
#endif

#ifndef USE_WORKING_SET
  /// auto-define USE_WORKING_SET dependend on the parts that need it!
#  if defined (USE_ISO_TERMINAL) || defined (USE_PROC_DATA_DESCRIPTION_POOL)
#    define USE_WORKING_SET
#  endif
#endif
#if defined USE_ISO_TERMINAL
#  if defined PRJ_ISO_TERMINAL_OBJECT_SELECTION1
#    define _autogen_config_header1_ <PRJ_ISO_TERMINAL_OBJECT_SELECTION1>
#    include _autogen_config_header1_
#  endif
#  if defined PRJ_ISO_TERMINAL_OBJECT_SELECTION2
#    define _autogen_config_header2_ <PRJ_ISO_TERMINAL_OBJECT_SELECTION2>
#    include _autogen_config_header2_
#  endif
#  if defined PRJ_ISO_TERMINAL_OBJECT_SELECTION3
#    define _autogen_config_header3_ <PRJ_ISO_TERMINAL_OBJECT_SELECTION3>
#    include _autogen_config_header3_
#  endif
#  if defined PRJ_ISO_TERMINAL_OBJECT_SELECTION4
#    define _autogen_config_header4_ <PRJ_ISO_TERMINAL_OBJECT_SELECTION4>
#    include _autogen_config_header4_
#  endif

#  ifndef PRJ_ISO_TERMINAL_OBJECT_SELECTION1
#    define USE_VTOBJECT_alarmmask
#    define USE_VTOBJECT_archedbargraph
#    define USE_VTOBJECT_auxiliaryfunction
#    define USE_VTOBJECT_auxiliaryinput
#    define USE_VTOBJECT_button
#    define USE_VTOBJECT_container
#    define USE_VTOBJECT_ellipse
#    define USE_VTOBJECT_inputboolean
#    define USE_VTOBJECT_inputlist
#    define USE_VTOBJECT_inputnumber
#    define USE_VTOBJECT_inputstring
#    define USE_VTOBJECT_key
#    define USE_VTOBJECT_line
#    define USE_VTOBJECT_linearbargraph
#    define USE_VTOBJECT_meter
#    define USE_VTOBJECT_objectpointer
#    define USE_VTOBJECT_outputnumber
#    define USE_VTOBJECT_stringvariable
#    define USE_VTOBJECT_outputstring
#    define USE_VTOBJECT_outputlist
#    define USE_VTOBJECT_polygon
#    define USE_VTOBJECT_rectangle
#  endif

#  ifndef USE_VTOBJECT_numbervariable
#    ifdef USE_VTOBJECT_inputnumber
#      define USE_VTOBJECT_numbervariable
#    else
#      ifdef USE_VTOBJECT_outputnumber
#        define USE_VTOBJECT_numbervariable
#      endif
#    endif
#  endif
#  ifndef USE_VTOBJECT_stringvariable
#    ifdef USE_VTOBJECT_inputstring
#      define USE_VTOBJECT_stringvariable
#    else
#      ifdef USE_VTOBJECT_outputstring
#        define USE_VTOBJECT_stringvariable
#      endif
#    endif
#  endif

// e.g. #include <MaskDefinition/IsoTerminalObjectSelection.inc>
#endif


/* ******************************************************** */
/**
 * \name Define PGN codes of ISO 11783 messages
 */
/*@{*/

// <DESTINATION> PGNs
#define CLIENT_TO_FS_PGN        0x00AA00LU
#define FS_TO_GLOBAL_PGN        0x00ABFFLU
#define FS_TO_CLIENT_PGN        0x00AB00LU
#define GUIDANCE_MACHINE_STATUS 0x00AC00LU
#define GUIDANCE_SYSTEM_CMD     0x00AD00LU
#define ETP_DATA_TRANSFER_PGN   0x00C700LU
#define ETP_CONN_MANAGE_PGN     0x00C800LU
#define PROCESS_DATA_PGN        0x00CB00LU
#define VT_TO_GLOBAL_PGN        0x00E6FFLU
#define VT_TO_ECU_PGN           0x00E600LU
#define ECU_TO_VT_PGN           0x00E700LU
/* we're NOT using ACK/NACK to Global address, we're directing directly to the sender! */
#define ACKNOWLEDGEMENT_PGN     0x00E800LU
#define REQUEST_PGN_MSG_PGN     0x00EA00LU
#define TP_DATA_TRANSFER_PGN    0x00EB00LU
#define TP_CONN_MANAGE_PGN      0x00EC00LU
#define ADDRESS_CLAIM_PGN       0x00EE00LU
#define PROPRIETARY_A_PGN       0x00EF00LU
#define PROPRIETARY_A2_PGN      0x01EF00LU

// <NO DESTINATION> PGNs
#define SELECTED_SPEED_MESSAGE      0x00F022LU
#define ELECTRONIC_ENGINE_CONTROLLER_1_MESSAGE 0x00F004LU
#define ISOBUS_CERTIFICATION_PGN    0x00FD42LU
#define SELECTED_SPEED_CMD          0x00FD43LU
#define TRACTOR_FACILITIES_PGN      0x00FE09LU
#define WORKING_SET_MEMBER_PGN      0x00FE0CLU
#define WORKING_SET_MASTER_PGN      0x00FE0DLU
#define LANGUAGE_PGN                0x00FE0FLU
#define AUX_VALVE_0_ESTIMATED_FLOW  0x00FE10LU
#define AUX_VALVE_1_ESTIMATED_FLOW  0x00FE11LU
#define AUX_VALVE_2_ESTIMATED_FLOW  0x00FE12LU
#define AUX_VALVE_3_ESTIMATED_FLOW  0x00FE13LU
#define AUX_VALVE_4_ESTIMATED_FLOW  0x00FE14LU
#define AUX_VALVE_5_ESTIMATED_FLOW  0x00FE15LU
#define AUX_VALVE_6_ESTIMATED_FLOW  0x00FE16LU
#define AUX_VALVE_7_ESTIMATED_FLOW  0x00FE17LU
#define AUX_VALVE_8_ESTIMATED_FLOW  0x00FE18LU
#define AUX_VALVE_9_ESTIMATED_FLOW  0x00FE19LU
#define AUX_VALVE_10_ESTIMATED_FLOW 0x00FE1ALU
#define AUX_VALVE_11_ESTIMATED_FLOW 0x00FE1BLU
#define AUX_VALVE_12_ESTIMATED_FLOW 0x00FE1CLU
#define AUX_VALVE_13_ESTIMATED_FLOW 0x00FE1DLU
#define AUX_VALVE_14_ESTIMATED_FLOW 0x00FE1ELU
#define AUX_VALVE_15_ESTIMATED_FLOW 0x00FE1FLU
#define AUX_VALVE_0_MEASURED_FLOW   0x00FE20LU
#define AUX_VALVE_1_MEASURED_FLOW   0x00FE21LU
#define AUX_VALVE_2_MEASURED_FLOW   0x00FE22LU
#define AUX_VALVE_3_MEASURED_FLOW   0x00FE23LU
#define AUX_VALVE_4_MEASURED_FLOW   0x00FE24LU
#define AUX_VALVE_5_MEASURED_FLOW   0x00FE25LU
#define AUX_VALVE_6_MEASURED_FLOW   0x00FE26LU
#define AUX_VALVE_7_MEASURED_FLOW   0x00FE27LU
#define AUX_VALVE_8_MEASURED_FLOW   0x00FE28LU
#define AUX_VALVE_9_MEASURED_FLOW   0x00FE29LU
#define AUX_VALVE_10_MEASURED_FLOW  0x00FE2ALU
#define AUX_VALVE_11_MEASURED_FLOW  0x00FE2BLU
#define AUX_VALVE_12_MEASURED_FLOW  0x00FE2CLU
#define AUX_VALVE_13_MEASURED_FLOW  0x00FE2DLU
#define AUX_VALVE_14_MEASURED_FLOW  0x00FE2ELU
#define AUX_VALVE_15_MEASURED_FLOW  0x00FE2FLU
#define AUX_VALVE_0_COMMAND         0x00FE30LU
#define AUX_VALVE_1_COMMAND         0x00FE31LU
#define AUX_VALVE_2_COMMAND         0x00FE32LU
#define AUX_VALVE_3_COMMAND         0x00FE33LU
#define AUX_VALVE_4_COMMAND         0x00FE34LU
#define AUX_VALVE_5_COMMAND         0x00FE35LU
#define AUX_VALVE_6_COMMAND         0x00FE36LU
#define AUX_VALVE_7_COMMAND         0x00FE37LU
#define AUX_VALVE_8_COMMAND         0x00FE38LU
#define AUX_VALVE_9_COMMAND         0x00FE39LU
#define AUX_VALVE_10_COMMAND        0x00FE3ALU
#define AUX_VALVE_11_COMMAND        0x00FE3BLU
#define AUX_VALVE_12_COMMAND        0x00FE3CLU
#define AUX_VALVE_13_COMMAND        0x00FE3DLU
#define AUX_VALVE_14_COMMAND        0x00FE3ELU
#define AUX_VALVE_15_COMMAND        0x00FE3FLU
#define LIGHTING_DATA_PGN           0x00FE40LU
#define LIGHTING_COMMAND_PGN        0x00FE41LU
#define HITCH_PTO_COMMANDS          0x00FE42LU
#define REAR_PTO_STATE_PGN          0x00FE43LU
#define FRONT_PTO_STATE_PGN         0x00FE44LU
#define REAR_HITCH_STATE_PGN        0x00FE45LU
#define FRONT_HITCH_STATE_PGN       0x00FE46LU
#define MAINTAIN_POWER_REQUEST_PGN  0x00FE47LU
#define WHEEL_BASED_SPEED_DIST_PGN  0x00FE48LU
#define GROUND_BASED_SPEED_DIST_PGN 0x00FE49LU
#define SOFTWARE_IDENTIFICATION_PGN 0x00FEDALU
#define ECU_IDENTIFICATION_INFORMATION_PGN 0x00FDC5LU
#define ECU_DIAGNOSTIC_PROTOCOL_PGN     0x00FD32LU

#define ACTIVE_DIAGNOSTIC_TROUBLE_CODES_PGN                0x00FECALU
#define PREVIOUSLY_ACTIVE_DIAGNOSTIC_TROUBLE_CODES_PGN     0x00FECBLU
#define DIAGNOSTIC_DATA_CLEAR_PGN                          0x00FECCLU

#define TIME_DATE_PGN               0x00FEE6LU
#define VEHICLE_DIRECTION_SPEED     0x00FEE8LU
#define VEHICLE_POSITION            0x00FEF3LU
// PROPRIETARY_B_PGN ranges from 0x00FF00 to 0x00FFFF !
#define PROPRIETARY_B_PGN           0x00FF00LU


// <NMEA 2000 GPS Messages>
#define NMEA_GPS_POSITION_RAPID_UPDATE_PGN        0x01F801LU
#define NMEA_GPS_COG_SOG_RAPID_UPDATE_PGN         0x01F802LU
#define NMEA_GPS_HIGH_PRECISION_RAPID_UPDATE_PGN  0x01F803LU
#define NMEA_GPS_POSITION_DATA_PGN                0x01F805LU
#define NMEA_GPS_TIME_DATE_PGN                    0x01F809LU
#define NMEA_GPS_DIRECTION_DATA_PGN               0x01FE11LU
#define NMEA_GNSS_PSEUDORANGE_NOISE_STATISTICS    0x01FA06LU



// NMEA_GPS_POSITION_RAPID_UPDATE_PGN 0x01F801LU   == 129025   IsoAgLib RECEIVES and SENDS this SINGLE-packet PGN
// NMEA_GPS_COG_SOG_RAPID_UPDATE_PGN  0x01F802LU   == 129026   IsoAgLib RECEIVES and SENDS this SINGLE-packet PGN
// NMEA_GPS_POSITION_DATA_PGN         0x01F805LU   == 129029   IsoAgLib RECEIVES and SENDS this MULTI- packet PGN
// NMEA_GPS_DIRECTION_DATA_PGN        0x01FE11LU   == 130577   IsoAgLib RECEIVES           this MULTI- packet PGN PGN (send with NMEA_GPS_COG_SOG_RAPID_UPDATE_PGN)

/*@}*/



/** define the debug device to use */
#ifndef INTERNAL_DEBUG_DEVICE
#  ifdef SYSTEM_PC

#    include <iostream>
#    include <fstream>
#    include "stdio.h"
#    ifndef UNICODE
#      define INTERNAL_DEBUG_DEVICE STL_NAMESPACE::cout
#      define INTERNAL_DEBUG_DEVICE_NEWLINE "\r\n"
#    else
#      define INTERNAL_DEBUG_DEVICE STL_NAMESPACE::wcout
#      define INTERNAL_DEBUG_DEVICE_NEWLINE L"\r\n"
#    endif
#    define INTERNAL_DEBUG_DEVICE_ENDL STL_NAMESPACE::endl
#    define INTERNAL_DEBUG_FLUSH fflush(0);
#  else
#    ifdef USE_RS232
#      define INTERNAL_DEBUG_DEVICE __IsoAgLib::getRs232Instance()
#    else
namespace __IsoAgLib {
struct NullOut_s {
  template <typename T> NullOut_s const &operator<<(T const &) const { return *this; }
  template <typename T> NullOut_s const &operator<<(T &) const { return *this; }
};
}
#      define INTERNAL_DEBUG_DEVICE (::__IsoAgLib::NullOut_s())
#    endif
#    ifndef UNICODE
#      define INTERNAL_DEBUG_DEVICE_ENDL "\r\n"
#      define INTERNAL_DEBUG_DEVICE_NEWLINE "\r\n"
#    else
#      define INTERNAL_DEBUG_DEVICE_ENDL L"\r\n"
#      define INTERNAL_DEBUG_DEVICE_NEWLINE L"\r\n"
#    endif
#    define INTERNAL_DEBUG_FLUSH
#  endif
#endif

#ifndef EXTERNAL_DEBUG_DEVICE
#  ifdef SYSTEM_PC
#    include <iostream>
#    include <fstream>
#    ifndef UNICODE
#      define EXTERNAL_DEBUG_DEVICE STL_NAMESPACE::cout
#      define EXTERNAL_DEBUG_DEVICE_NEWLINE "\r\n"
#    else
#      define EXTERNAL_DEBUG_DEVICE STL_NAMESPACE::wcout
#      define EXTERNAL_DEBUG_DEVICE_NEWLINE L"\r\n"
#    endif
#    define EXTERNAL_DEBUG_DEVICE_ENDL STL_NAMESPACE::endl
#    define EXTERNAL_DEBUG_FLUSH fflush(0);
#  else
#    define EXTERNAL_DEBUG_DEVICE IsoAgLib::getIrs232Instance()
#    ifndef UNICODE
#      define EXTERNAL_DEBUG_DEVICE_ENDL "\r\n"
#      define EXTERNAL_DEBUG_DEVICE_NEWLINE "\r\n"
#    else
#      define EXTERNAL_DEBUG_DEVICE_ENDL L"\r\n"
#      define EXTERNAL_DEBUG_DEVICE_NEWLINE L"\r\n"
#    endif
#    define EXTERNAL_DEBUG_FLUSH
#  endif
#endif
/*@}*/


#ifndef gluetoks
#  define gluetoks(a, b)  a ## b
#  define gluetoks3(a, b, c)  a ## b ## c
#  define gluetoks4(a, b, c, d)  a ## b ## c ## d
#  define gluetoks5(a, b, c, d, e)  a ## b ## c ## d ## e
#endif
#ifndef gluemacs
#  define gluemacs(a, b)  gluetoks(a, b)
#  define gluemacs3(a, b, c)  gluetoks3(a, b, c)
#  define gluemacs4(a, b, c, d)  gluetoks4(a, b, c, d)
#  define gluemacs5(a, b, c, d, e)  gluetoks5(a, b, c, d, e)
#endif
/*@}*/


#endif
