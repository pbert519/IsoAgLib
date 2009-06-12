#!/bin/sh
# ####################################################################### #
#                         update_makefile.sh  -
#                          Script to create file list, configuration
#                          headers and project files for applications
#                          based on a feature set specifying config file
#                            -------------------
#   begin                 Mon Aug 02 2004
#   copyright            : (C) 2004 - 2006 by Dipl.-Inform. Achim Spangler
#   email                : a.spangler@osb-ag:de
# ####################################################################### #

# ####################################################################### #
#                                                                         #
# This file is part of the "IsoAgLib", an object oriented program library #
# to serve as a software layer between application specific program and   #
# communication protocol details. By providing simple function calls for  #
# jobs like starting a measuring program for a process data value on a    #
# remote ECU, the main program has not to deal with single CAN telegram   #
# formatting. This way communication problems between ECU's which use     #
# this library should be prevented.                                       #
# Everybody and every company is invited to use this library to make a    #
# working plug and play standard out of the printed protocol standard.    #
#                                                                         #
# Copyright (C) 2000 - 2004 Dipl.-Inform. Achim Spangler                  #
#                                                                         #
# The IsoAgLib is free software; you can redistribute it and/or modify it #
# under the terms of the GNU General Public License as published          #
# by the Free Software Foundation; either version 2 of the License, or    #
# (at your option) any later version.                                     #
#                                                                         #
# This library is distributed in the hope that it will be useful, but     #
# WITHOUT ANY WARRANTY; without even the implied warranty of              #
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU       #
# General Public License for more details.                                #
#                                                                         #
# You should have received a copy of the GNU General Public License       #
# along with IsoAgLib; if not, write to the Free Software Foundation,     #
# Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA           #
#                                                                         #
# As a special exception, if other files instantiate templates or use     #
# macros or inline functions from this file, or you compile this file and #
# link it with other works to produce a work based on this file, this file#
# does not by itself cause the resulting work to be covered by the GNU    #
# General Public License. However the source code for this file must still#
# be made available in accordance with section (3) of the                 #
# GNU General Public License.                                             #
#                                                                         #
# This exception does not invalidate any other reasons why a work based on#
# this file might be covered by the GNU General Public License.           #
#                                                                         #
# Alternative licenses for IsoAgLib may be arranged by contacting         #
# the main author Achim Spangler by a.spangler@osb-ag:de                  #
# ####################################################################### #

# ####################################################################### #
# This file contains several helper functions to create
# project specific Makefiles based on several config variable
# settings. The vars are used to defined the features which are
# needed by the application
# ####################################################################### #

# Prefer these functions to echo due to incompatible versions (Bourne
# shell builtin, Bash builtin, command):
echo_() { printf '%s\n' "$*"; }
echo_e() { printf '%b\n' "$*"; }
echo_e_n() { printf '%b' "$*"; }
echo_n() { printf '%s' "$*"; }
# Compatible but slow variant as fallback:
alias echo=$(which echo)

PARAMETER_TARGET_SYSTEM="UseConfigFile"
PARAMETER_CAN_DRIVER="UseConfigFile"
PARAMETER_CAN_DEVICE_FOR_SERVER="UseConfigFile"
PARAMETER_RS232_DRIVER="UseConfigFile"


USE_EMBED_LIB_DIRECTORY="library/commercial_BIOS/bios_esx"
USE_EMBED_HEADER_DIRECTORY="library/commercial_BIOS/bios_esx"
#USE_EMBED_LIBS="C756/Xos20l.lib Module/Xma20l.lib"
#USE_EMBED_BIOS_SRC="Xos20go.asm Xos20err.c xos20esx.h XOS20EEC.H XOS20EEC.OBJ"
#USE_EMBED_ILO="Xos20lcs.ilo"
USE_EMBED_COMPILER_DIR="c:/programme/tasking/c166"

USE_STLPORT_HEADER_DIRECTORY="C:/STLport/stlport"
# USE_STLPORT_LIBRARY_DIRECTORY=""

# if spec file defines this, the setting is simply overwritten
APP_SEARCH_SRC_CONDITION="*.C *.cc *.c *.cpp *.cxx"
APP_SEARCH_HDR_CONDITION="*.h *.hpp"

APP_PATH_EXCLUDE=""
APP_SRC_EXCLUDE=""

ISOAGLIB_INSTALL_PATH="/usr/local"

USE_SYSTEM_DEFINE=""

GENERATE_FILES_ROOT_DIR="$PWD"

# this function is used to verify and
# correct the several project configuration
# variables
# if one of the following variables isn't set
# the corresponding default values are used
# + USE_LITTLE_ENDIAN_CPU=1 --> most CPU types have little endian number variable representation -> number variable can be converted directly from int variable memory representation into CAN little endian string
# + USE_CAN_DRIVER="simulating"|"sys"|"msq_server"|"socket_server"|"socket_server_hal_simulator" -> select wanted driver connection for CAN
# + USE_CAN_DEVICE_FOR_SERVER="no_card"|"pcan"|"A1"|"rte" -> use this device for building the can_server
# + USE_RS232_DRIVER="simulating"|"sys"|"rte"|"hal_simulator" -> select wanted driver connection for RS232
# + CAN_BUS_CNT ( specify amount of available CAN channels at ECU; default 1 )
# + CAN_INSTANCE_CNT ( specify amount of CAN channels; default 1 )
# + PRT_INSTANCE_CNT ( specify amount of CAN channels to use for protocol; must be <= CAN_INSTANCE_CNT; default 1 )
# + RS232_INSTANCE_CNT ( specify amount of RS232 channels; default 1 )
# + PROJECT ( subdirectory name and executable filename defined by created Makefile )
# + REL_APP_PATH ( specify path to application of this project - error message if not given; use relative path!! )
# + APP_NAME ( optionally select single CC file for the main app - otherwise the whole given path is interpreted as part of this project )
#
# + PRJ_ISO11783=1 ( select ISO 11783, default not even ISO for internal CAN comm only )
# + PRJ_ISO_TERMINAL ( specify if ISO virtual terminal is wanted; default 0; only possible if PRJ_ISO11783=1 -> error message if not )
# + PRJ_BASE ( specify if Base data - main tractor information as PGN or LBS Base message - is wanted; default 0 )
# + PRJ_TRACTOR_GENERAL (only incorporate parts from BASE that provide tractor hitch and RPM information)
# + PRJ_TRACTOR_MOVE (only incorporate parts from BASE that provide tractor distance and speed information)
# + PRJ_TRACTOR_PTO (only incorporate parts from BASE that provide tractor PTO information)
# + PRJ_TRACTOR_LIGHT (only incorporate parts from BASE that provide tractor lighting information)
# + PRJ_TRACTOR_FACILITIES (only incorporate parts from BASE that provide tractor facilities information)
# + PRJ_TRACTOR_AUX (only incorporate parts from BASE that provide tractor auxiliary valve information)
# + PRJ_TRACTOR_GUIDANCE (only incorporate parts from BASE that provide tractor guidance information)
# + PRJ_TRACTOR_CERTIFICATION (only incorporate parts from BASE that provide tractor certification information)
# + PRJ_TIME_GPS (only incorporate parts from BASE that provide time and GPS information)
# + PRJ_PROCESS ( specify if process data should be used ; default 0 )
#   - PROC_LOCAL ( specify if local process data shall be used; must be activated for all types of local process data; default 0 )
#     o PROC_LOCAL_STD ( specify if full featured local process data shall be used; default 0 )
#     o PROC_LOCAL_SIMPLE_MEASURE ( specify if local process data with restricted measurement feature set shall be used; default 0 )
#     o PROC_LOCAL_SIMPLE_SETPOINT ( specify if local process data with restricted setpoint feature set shall be used; default 0 )
#     o PROC_LOCAL_SIMPLE_MEASURE_SETPOINT ( specify if process daza with maximum restricted feature set shall be used; default 0 )
#   - PROC_REMOTE ( specify if remote process data shall be used; must be activated for all types of remote process data; default 0 )
#     o PROC_REMOTE_STD ( specify if full featured remote process data shall be used; default 0 )
#     o PROC_REMOTE_SIMPLE_MEASURE ( specify if remote process data with restricted measurement feature set shall be used; default 0 )
#     o PROC_REMOTE_SIMPLE_SETPOINT ( specify if remote process data with restricted setpoint feature set shall be used; default 0 )
#     o PROC_REMOTE_SIMPLE_MEASURE_SETPOINT ( specify if process daza with maximum restricted feature set shall be used; default 0 )
#     o PROC_REMOTE_SIMPLE_MEASURE_SETPOINT_COMBINED ( specify if process daza with maximum restricted feature without any distinction between measurement and setpoint set shall be used; default 0 )
# + PRJ_DATASTREAMS ( specify if the input and output filestream should be accessed by IsoAgLib; provides target HAL for filestream handling; default 0 )
# + PRJ_EEPROM ( specify if the EEPROM should be accessed by IsoAgLib; provides important functions for local process data and local ISO items; default 0 )
# + PRJ_ACTOR ( specify if IsoAgLib extension for PWM access should be used; provides several utility and diagnostic functions; default 0 )
# + PRJ_SENSOR_DIGITAL ( specify if IsoAgLib extension for digital sensor data read should be used; provides several utility and diagnostic functions; default 0 )
# + PRJ_SENSOR_COUNTER ( specify if IsoAgLib extension for counting sensor data read should be used; provides several utility and diagnostic functions; default 0 )
# + PRJ_SENSOR_ANALOG ( specify if IsoAgLib extension for analog sensor data read should be used; provides several utility and diagnostic functions; default 0 )
# + PRJ_RS232 ( specify if IsoAgLib extension for C++ stream oriented I/O should be used; default 0 )

set_default_values()
{
    PRJ_SEND_DEBUG=0
    USE_FLOAT_DATA_TYPE=0
    OPTIMIZE_HEAPSIZE_IN_FAVOR_OF_SPEED=0
    USE_LITTLE_ENDIAN_CPU=1
    # set default USE_LITTLE_ENDIAN_CPU, so that quick number variable
    # to CAN string conversions are possible:
    USE_LITTLE_ENDIAN_CPU=1
    # set default USE_PCAN_LIB, so that quick number variable to CAN
    # string conversions are possible:
    USE_PCAN_LIB=0
    # set default USE_BIG_ENDIAN_CPU, so that quick number variable to
    # CAN string conversions are possible:
    USE_BIG_ENDIAN_CPU=0
    # option to not switch the system relais to ON, on startup
    # (default: set relais to ON on startup):
    PRJ_DO_NOT_START_RELAIS_ON_STARTUP=0
    USE_VT_UNICODE_SUPPORT=0
    PRJ_SYSTEM_WITH_ENHANCED_CAN_HAL=0
    CAN_BUS_CNT=1
    CAN_BUS_USED=0
    CAN_INSTANCE_CNT=1
    PRT_INSTANCE_CNT=1
    RS232_INSTANCE_CNT=1
    APP_NAME=''
    PRJ_PROPRIETARY_PGN_INTERFACE=0
    PRJ_ISO11783=0
    PRJ_ISO11783=0
    PRJ_ISO_TERMINAL=0
    PRJ_ISO_FILESERVER_CLIENT=0
    PRJ_ISO_TERMINAL=0
    PRJ_ISO_FILESERVER_CLIENT=0
    PRJ_ISO_TERMINAL_SERVER=0
    PRJ_ISO_TERMINAL_LAYOUT_MANAGER=0
    PRJ_RS232_OVER_CAN=0
    PRJ_MULTIPACKET_STREAM_CHUNK=1
    PRJ_BASE=0
    PRJ_TRACTOR_GENERAL=0
    PRJ_TRACTOR_MOVE=0
    PRJ_TRACTOR_PTO=0
    PRJ_TIME_GPS=0
    PRJ_TRACTOR_LIGHT=0
    PRJ_TRACTOR_FACILITIES=0
    PRJ_TRACTOR_AUX=0
    PRJ_TRACTOR_GUIDANCE=0
    PRJ_TRACTOR_CERTIFICATION=0
    PRJ_TRACTOR_MOVE_SETPOINT=0
    PRJ_TRACTOR_PTO_SETPOINT=0
    PRJ_PROCESS=0
    PROC_LOCAL=0
    PROC_LOCAL_STD=0
    PROC_LOCAL_SIMPLE_MEASURE=0
    PROC_LOCAL_SIMPLE_SETPOINT=0
    PROC_LOCAL_SIMPLE_MEASURE_SETPOINT=0
    PROC_REMOTE=0
    PROC_REMOTE_STD=0
    PROC_REMOTE_SIMPLE_MEASURE=0
    PROC_REMOTE_SIMPLE_SETPOINT=0
    PROC_REMOTE_SIMPLE_MEASURE_SETPOINT=0
    PROC_REMOTE_SIMPLE_MEASURE_SETPOINT_COMBINED=0
    PRJ_EEPROM=0
    PRJ_DATASTREAMS=0
    PRJ_ACTOR=0
    PRJ_SENSOR_DIGITAL=0
    PRJ_SENSOR_ANALOG=0
    PRJ_SENSOR_COUNTER=0
    PRJ_RS232=0
    PRJ_MULTIPACKET=0
    # preset some conditional vars
    INC_LOC_STD_MEASURE_ELEMENTS=0
    INC_LOC_SIMPLE_MEASURE_ELEMENTS=0
    INC_LOC_STD_SETPOINT_ELEMENTS=0
    INC_LOC_SIMPLE_SETPOINT_ELEMENTS=0
    INC_REM_STD_MEASURE_ELEMENTS=0
    INC_REM_SIMPLE_MEASURE_ELEMENTS=0
    INC_REM_STD_SETPOINT_ELEMENTS=0
    INC_REM_SIMPLE_SETPOINT_ELEMENTS=0
    # has to be overridden by configuration:
    USE_TARGET_SYSTEM='void'
    HAL_PATH='void'
}

check_set_correct_variables()
{
    if [ "$CAN_BUS_CNT" -lt "$CAN_INSTANCE_CNT" ]; then
        echo_ "ERROR! The amount of available CAN BUS channels at ECU must be at least as high as wanted of used CAN instances"
        exit 2
    fi

    : ${ISO_AG_LIB_PATH:?"ERROR! Please specify the path to the root directory of IsoAgLib - i.e. where xgpl_src and IsoAgLibExamples are located"}
    ISO_AG_LIB_INSIDE="../../$ISO_AG_LIB_PATH"

    # check if ISO_AG_LIB_PATH valid
    if [ ! -d "$ISO_AG_LIB_PATH" ]; then
        echo_ "ERROR! ISO_AG_LIB_PATH is not a directory"
    else
        if [ ! -d "$ISO_AG_LIB_PATH/library" ]; then
            echo_ "ERROR! No library found within ISO_AG_LIB ($ISO_AG_LIB_PATH) - maybe your directory is not updated?"
        fi
    fi

    if [ "$PRT_INSTANCE_CNT" -gt "$CAN_INSTANCE_CNT" ]; then
        echo_ "ERROR! The Amount of protocol instances must be smaller or equal to amount of CAN instances"
        echo_ "Current false setting: PRT_INSTANCE_CNT=$PRT_INSTANCE_CNT \t CAN_INSTANCE_CNT=$CAN_INSTANCE_CNT"
        exit 2
    elif [ "$PRT_INSTANCE_CNT" -lt 1 ]; then
        echo_ "ERROR! There must be at least one protocol instance"
        exit 2
    fi

    : ${PROJECT:?"ERROR! Please set the variable PROJECT to the build sub-directory and executable filename"}
    : ${REL_APP_PATH:?"ERROR! Please set the variable REL_APP_PATH to the directory of the application sources"}

    if [ "$PRJ_ISO11783" -eq 0 ]; then
        PRJ_ISO11783=0
        PRJ_ISO_TERMINAL=0
        PRJ_ISO_FILESERVER_CLIENT=0
    fi

    if [ "$PRJ_ISO11783" -lt 1 -a "$PRJ_ISO_TERMINAL" -gt 0 ]; then
        echo_ "Warning overwrite setting of PRJ_ISO_TERMINAL == $PRJ_ISO_TERMINAL as ISO11783 is not activated"
        echo_ "Set PRJ_ISO11783 to 1 if you want ISO 11783 virtual terminal"
        PRJ_ISO_TERMINAL=0
    fi

    if test "$PRJ_ISO11783" -lt 1 -a "$PRJ_ISO_FILESERVER_CLIENT" -gt 0 ; then
        echo_ "Warning overwrite setting of PRJ_ISO_FILESERVER_CLIENT == $PRJ_ISO_FILESERVER_CLIENT as ISO11783 is not activated"
        echo_ "Set PRJ_ISO11783 to 1 if you want ISO 11783 fileserver"
        PRJ_ISO_FILESERVER_CLIENT=0
    fi

    if [ "$PRJ_RS232_OVER_CAN" -gt 0 ]; then
        PRJ_RS232=1 # force activation of RS232 driver class when this special RS232 HAL is activated
    fi

    if [ "$PRJ_BASE" -gt 0 ]; then
        # activate all base data sub-features, when PRJ_BASE is activated
        PRJ_TRACTOR_GENERAL=1
        PRJ_TRACTOR_MOVE=1
        PRJ_TRACTOR_PTO=1
        PRJ_TIME_GPS=1
        PRJ_TRACTOR_LIGHT=1
        PRJ_TRACTOR_FACILITIES=1
        PRJ_TRACTOR_AUX=1
        PRJ_TRACTOR_GUIDANCE=1
        PRJ_TRACTOR_CERTIFICATION=1
    fi

    if [ "$PRJ_TRACTOR_MOVE_SETPOINT" -ne 0 ]; then
        PRJ_TRACTOR_MOVE=1 # force basic trac move to compile in
    fi
    if [ "$PRJ_TRACTOR_PTO_SETPOINT" -gt 0 ]; then
        PRJ_TRACTOR_PTO=1 # force basic trac move to compile in
    fi

    # and also deactivate the features that are only available with the extension directories
    EXT_BASE_SRC="$ISO_AG_LIB_PATH/library/xgpl_src/IsoAgLib/comm/Part7_ApplicationLayer/ext"

    if [ ! -d "$EXT_BASE_SRC" ]; then
        # ISO is not active -> deactivate all ISO only features
        echo_ "Information:"; echo_
        echo_ "The directory $EXT_BASE_SRC with (optional) extension modules does not exist."
        echo_ "Deactivating features:"; echo_
        echo_ "PRJ_TRACTOR_LIGHT=0"
        echo_ "PRJ_TRACTOR_AUX=0"
        echo_ "PRJ_TRACTOR_GUIDANCE=0"
        echo_ "PRJ_TRACTOR_CERTIFICATION=0"; echo_
        echo_ "Please contact m.rothmund@osb-ag.de to gain access to this restriced area."
        PRJ_TRACTOR_LIGHT=0
        PRJ_TRACTOR_AUX=0
        PRJ_TRACTOR_GUIDANCE=0
        PRJ_TRACTOR_CERTIFICATION=0
        echo_
    fi

    # overwrite settings from config file with command line parameter settings
    if [ $PARAMETER_TARGET_SYSTEM != "UseConfigFile" ] ; then
        USE_TARGET_SYSTEM=$PARAMETER_TARGET_SYSTEM
        # echo_ "Use Parameter value for target system: $USE_TARGET_SYSTEM"
        # else
        # echo_ "Use Target System from config file: $USE_TARGET_SYSTEM"
    fi
    if [ $PARAMETER_CAN_DRIVER != "UseConfigFile" ] ; then
        USE_CAN_DRIVER=$PARAMETER_CAN_DRIVER
        #echo_ "Use Parameter value for can driver: $PARAMETER_CAN_DRIVER"
    fi
    if [ $PARAMETER_CAN_DEVICE_FOR_SERVER != "UseConfigFile" ] ; then
        USE_CAN_DEVICE_FOR_SERVER=$PARAMETER_CAN_DEVICE_FOR_SERVER
    fi
    if [ $PARAMETER_RS232_DRIVER != "UseConfigFile" ] ; then
        USE_RS232_DRIVER=$PARAMETER_RS232_DRIVER
        #echo_ "Use Parameter value for rs232 driver: $PARAMETER_RS232_DRIVER"
    fi

    case "$USE_TARGET_SYSTEM" in
        pc_win32)
            USE_SYSTEM_DEFINE="SYSTEM_PC"
            GENERATE_FILES_ROOT_DIR="$1/Dev-C++/"
            IDE_NAME="Visual Studio, Dev-C++"
            ;;        
        pc_linux)
            USE_SYSTEM_DEFINE="SYSTEM_PC"
            GENERATE_FILES_ROOT_DIR="$1/kdevelop_make/"
            IDE_NAME="KDevelop, make"
            ;;        
        esx)
            USE_SYSTEM_DEFINE="SYSTEM_ESX"
            GENERATE_FILES_ROOT_DIR="$1/EDE/"
            IDE_NAME="Tasking EDE"
            ;;        
        esxu)
            USE_SYSTEM_DEFINE="SYSTEM_ESXu"
            GENERATE_FILES_ROOT_DIR="$1/EDE/"
            IDE_NAME="Tasking EDE"
            ;;
        c2c)
            USE_SYSTEM_DEFINE="SYSTEM_C2C"
            GENERATE_FILES_ROOT_DIR="$1/EDE/"
            IDE_NAME="Tasking EDE"
            ;;
        imi)
            USE_SYSTEM_DEFINE="SYSTEM_IMI"
            GENERATE_FILES_ROOT_DIR="$1/EDE/"
            IDE_NAME="Tasking EDE"
            ;;
        pm167)
            USE_SYSTEM_DEFINE="SYSTEM_PM167"
            GENERATE_FILES_ROOT_DIR="$1/EDE/"
            IDE_NAME="Tasking EDE"
            ;;
        Dj1)
            USE_SYSTEM_DEFINE="SYSTEM_DJ1"
            GENERATE_FILES_ROOT_DIR="$1/EDE/"
            IDE_NAME="Tasking EDE"
            ;;
        p1mc)
            USE_SYSTEM_DEFINE="SYSTEM_P1MC"
            GENERATE_FILES_ROOT_DIR="$1/CCS/"
            IDE_NAME="Code Composer Studio"
            ;;
    esac
}

# Take the first argument as variable name and append the remaining
# arguments to the variable's value.
append()
{
    local VARIABLE="$1"
    local FUNCTION="append_$VARIABLE"
    # If the following statement causes a visible error message "...
    # not found", then the running shell is incompatible to the Bourne
    # shell. This is problematic, not for the following statement, but
    # elsewhere in this script! E.g. Bash is not compatible enough,
    # but common Linux systems let you configure that /bin/dash plays
    # the role of /bin/sh, and you should do so. (On Debian-like
    # systems: dpkg-reconfigure dash)
    type "$FUNCTION" >/dev/null ||
        eval "$FUNCTION() { $VARIABLE=\"\$$VARIABLE\$*\"; }"
    shift
    "$FUNCTION" "$@"
}

# this function uses the "find" cmd
# to derive based on the selected features the
# corresponding file list into filelist_$PROJECT_PURE.txt
create_filelist( )
{
    COMM_PROC_FEATURES=""

    if [ "$PRJ_PROCESS" -gt 0 ]; then
        if [ -n "$COMM_PROC_FEATURES" ]; then
            append COMM_PROC_FEATURES " -o "
        fi
        append COMM_PROC_FEATURES " -name 'processdatachangehandler_c.*' -o -name 'iprocess_c.*' -o -name 'elementddi_s.h' -o -name 'proc_c.h' -o -path '*/Part7_ProcessData/impl/proc*' -o -path '*/Part7_ProcessData/iprocesscmd*' -o -path '*/Part7_ProcessData/impl/processcmd*' -o -path '*/Part7_ProcessData/*procdata*base_c.h'"

        if [ "$PRJ_ISO11783" -gt 0 -a "$PROC_LOCAL" -gt 0 ]; then
            # allow DevPropertyHandler
            append COMM_PROC_FEATURES " -o -path '*/Part10_TaskController_Client/i*devproperty*'"
        else
            append COMM_PROC_FEATURES " -o \( -path '*/Part10_TaskController_Client/i*devproperty*' -a -not -name 'devproperty*' \)"
        fi

        if [ "$PROC_LOCAL" -gt 0 ]; then
            append COMM_PROC_FEATURES " -o -path '*/Part7_ProcessData/Local/impl/*'"

            if [ "$PROC_LOCAL_STD" -gt 0 ]; then
                append COMM_PROC_FEATURES " -o -path '*/Part7_ProcessData/Local/Std/*'"
                INC_LOC_STD_MEASURE_ELEMENTS=1
                INC_LOC_STD_SETPOINT_ELEMENTS=1
            fi
            if [ "$PROC_LOCAL_SIMPLE_MEASURE" -gt 0 ]; then
                append COMM_PROC_FEATURES " -o -path '*/Part7_ProcessData/Local/SimpleMeasure/*'"
                INC_LOC_STD_SETPOINT_ELEMENTS=1
            fi
            if [ "$PROC_LOCAL_SIMPLE_SETPOINT" -gt 0 ]; then
                append COMM_PROC_FEATURES " -o -path '*/Part7_ProcessData/Local/SimpleSetpoint/*'"
                INC_LOC_STD_MEASURE_ELEMENTS=1
                INC_LOC_SIMPLE_SETPOINT_ELEMENTS=1
            fi
            if [ "$PROC_LOCAL_SIMPLE_MEASURE_SETPOINT" -gt 0 ]; then
                append COMM_PROC_FEATURES " -o -path '*/Part7_ProcessData/Local/SimpleMeasureSetpoint/*'"
                INC_LOC_SIMPLE_SETPOINT_ELEMENTS=1
            fi

            if [ "$INC_LOC_STD_MEASURE_ELEMENTS" -gt 0 ]; then
                append COMM_PROC_FEATURES " -o -path '*/Part7_ProcessData/Local/StdMeasureElements/*'"
            fi
            if [ "$INC_LOC_STD_SETPOINT_ELEMENTS" -gt 0 ]; then
                append COMM_PROC_FEATURES " -o -path '*/Part7_ProcessData/Local/StdSetpointElements/*'"
            fi
            if [ "$INC_LOC_SIMPLE_SETPOINT_ELEMENTS" -gt 0 ]; then
                append COMM_PROC_FEATURES " -o -path '*/Part7_ProcessData/Local/SimpleSetpointElements/*'"
            fi
        fi
        if [ "$PROC_REMOTE" -gt 0 ]; then
            append COMM_PROC_FEATURES " -o -path '*/Part7_ProcessData/Remote/impl/*'"

            if [ "$PROC_REMOTE_STD" -gt 0 ]; then
                append COMM_PROC_FEATURES " -o -path '*/Part7_ProcessData/Remote/Std/*'"
                INC_REM_STD_MEASURE_ELEMENTS=1
                INC_REM_STD_SETPOINT_ELEMENTS=1
            fi
            if [ "$PROC_REMOTE_SIMPLE_MEASURE" -gt 0 ]; then
                append COMM_PROC_FEATURES " -o -path '*/Part7_ProcessData/Remote/SimpleMeasure/*'"
                INC_REM_SIMPLE_MEASURE_ELEMENTS=1
                INC_REM_STD_SETPOINT_ELEMENTS=1
            fi
            if [ "$PROC_REMOTE_SIMPLE_SETPOINT" -gt 0 ]; then
                append COMM_PROC_FEATURES " -o -path '*/Part7_ProcessData/Remote/SimpleSetpoint/*'"
                INC_REM_STD_MEASURE_ELEMENTS=1
                INC_REM_SIMPLE_SETPOINT_ELEMENTS=1
            fi
            if test "$PROC_REMOTE_SIMPLE_MEASURE_SETPOINT" -gt 0 -a "$PROC_REMOTE_SIMPLE_MEASURE_SETPOINT_COMBINED" -gt 0   ; then
                append COMM_PROC_FEATURES " -o -path '*/Part7_ProcessData/Remote/SimpleMeasureSetpoint/*'"
                INC_REM_SIMPLE_MEASURE_ELEMENTS=1
                INC_REM_SIMPLE_SETPOINT_ELEMENTS=1
            elif [ "$PROC_REMOTE_SIMPLE_MEASURE_SETPOINT" -gt 0 ]; then
                append COMM_PROC_FEATURES " -o -name '*procdataremotesimplesetpointsimplemeasure_c.*'"
                INC_REM_SIMPLE_MEASURE_ELEMENTS=1
                INC_REM_SIMPLE_SETPOINT_ELEMENTS=1
            elif [ "$PROC_REMOTE_SIMPLE_MEASURE_SETPOINT_COMBINED" -gt 0 ]; then
                append COMM_PROC_FEATURES " -o -name '*procdataremotesimplesetpointsimplemeasurecombined_c.*'"
            fi
            if [ "$INC_REM_STD_MEASURE_ELEMENTS" -gt 0 ]; then
                append COMM_PROC_FEATURES " -o -path '*/Part7_ProcessData/Remote/StdMeasureElements/*'"
            fi
            if [ "$INC_REM_SIMPLE_MEASURE_ELEMENTS" -gt 0 ]; then
                append COMM_PROC_FEATURES " -o -path '*/Part7_ProcessData/Remote/SimpleMeasureElements/*'"
            fi
            if [ "$INC_REM_STD_SETPOINT_ELEMENTS" -gt 0 ]; then
                append COMM_PROC_FEATURES " -o -path '*/Part7_ProcessData/Remote/StdSetpointElements/*'"
            fi
            if [ "$INC_REM_SIMPLE_SETPOINT_ELEMENTS" -gt 0 ]; then
                append COMM_PROC_FEATURES " -o -path '*/Part7_ProcessData/Remote/SimpleSetpointElements/*'"
            fi
        fi

        if test "$INC_LOC_STD_MEASURE_ELEMENTS" -gt 0 -o "$INC_REM_STD_MEASURE_ELEMENTS" -gt 0 ; then
            append COMM_PROC_FEATURES " -o -path '*/Part7_ProcessData/StdMeasureElements/*'"
        fi
        if test "$INC_LOC_STD_SETPOINT_ELEMENTS" -gt 0 -o "$INC_REM_STD_SETPOINT_ELEMENTS" -gt 0 ; then
            append COMM_PROC_FEATURES " -o -path '*/Part7_ProcessData/StdSetpointElements/*'"
        fi

    fi

    #USE_TARGET_SYSTEM="pc"
    case "$USE_TARGET_SYSTEM" in
        pc*)
            HAL_PATH="pc"
            ;;
        *)
            HAL_PATH=$USE_TARGET_SYSTEM
            ;;
    esac

    COMM_FEATURES=" -path '*/IsoAgLib/typedef.h' -o -path '*/hal/"$HAL_PATH"/typedef.h' -o -name 'isoaglib_config.h' -o -path '*/hal/config.h'"
    if [ "$PRJ_BASE" -gt 0 ]; then
        append COMM_FEATURES " -o -path '*/Part7_ApplicationLayer/*'"
    else
        if test "$PRJ_TRACTOR_GENERAL" -gt 0 -o "$PRJ_TRACTOR_MOVE" -gt 0 -o "$PRJ_TRACTOR_FACILITIES" -gt 0 -o "$PRJ_TRACTOR_PTO" -gt 0 -o "$PRJ_TRACTOR_LIGHT" -gt 0 -o "$PRJ_TRACTOR_AUX" -gt 0 -o "$PRJ_TIME_GPS" -gt 0 -o "$PRJ_TRACTOR_GUIDANCE" -gt 0 -o "$PRJ_TRACTOR_CERTIFICATION" -gt 0; then
            append COMM_FEATURES " -o -name 'ibasetypes.h' -o -name 'basecommon_c*'"
        fi
        if [ "$PRJ_TRACTOR_GENERAL" -gt 0 ]; then
            append COMM_FEATURES " -o \( -path '*/Part7_ApplicationLayer/*' -a -name '*tracgeneral_c*' \)"
        fi
        if test "$PRJ_TRACTOR_MOVE" -gt 0 -a "$PRJ_TRACTOR_MOVE_SETPOINT" -gt 0 ; then
            append COMM_FEATURES " -o \( -path '*/Part7_ApplicationLayer/*' -a -name '*tracmove*' \)"
        elif test "$PRJ_TRACTOR_MOVE" -gt 0 -a "$PRJ_TRACTOR_MOVE_SETPOINT" -lt 1 ; then
            append COMM_FEATURES " -o \( -path '*/Part7_ApplicationLayer/*' -a -name '*tracmove_c.*' \)"
        fi
        if test "$PRJ_TRACTOR_PTO" -gt 0 -a "$PRJ_TRACTOR_PTO_SETPOINT" -gt 0 ; then
            append COMM_FEATURES " -o \( -path '*/Part7_ApplicationLayer/*' -a -name '*tracpto*' \)"
        elif test "$PRJ_TRACTOR_PTO" -gt 0 -a "$PRJ_TRACTOR_PTO_SETPOINT" -lt 1 ; then
            append COMM_FEATURES " -o \( -path '*/Part7_ApplicationLayer/*' -a -name '*tracpto_c.*' \)"
        fi
        if test "$PRJ_TRACTOR_LIGHT" -gt 0 -a "$PRJ_ISO11783" -gt 0 ; then
            append COMM_FEATURES " -o \( -path '*/Part7_ApplicationLayer/*' -a -name '*traclight*' \)"
        else
            PRJ_TRACTOR_LIGHT=0
        fi
        if test "$PRJ_TRACTOR_FACILITIES" -gt 0 -a "$PRJ_ISO11783" -gt 0 ; then
            # tracfacilities is only defined for ISO 11783
            append COMM_FEATURES " -o \( -path '*/Part7_ApplicationLayer/*' -a -name '*tracfacilities_c*' \)"
        else
            PRJ_TRACTOR_FACILITIES=0
        fi
        if test "$PRJ_TRACTOR_AUX" -gt 0 -a "$PRJ_ISO11783" -gt 0 ; then
            # tracaux is only defined for ISO 11783
            append COMM_FEATURES " -o \( -path '*/Part7_ApplicationLayer/*' -a -name '*tracaux*' \)"
        else
            PRJ_TRACTOR_AUX=0
        fi
        if test "$PRJ_TRACTOR_GUIDANCE" -gt 0 -a "$PRJ_ISO11783" -gt 0 ; then
            # tracguidance is only defined for ISO 11783
            append COMM_FEATURES " -o \( -path '*/Part7_ApplicationLayer/*' -a -name '*tracguidance*' \)"
        else
            PRJ_TRACTOR_GUIDANCE=0
        fi
        if test "$PRJ_TRACTOR_CERTIFICATION" -gt 0 -a "$PRJ_ISO11783" -gt 0 ; then
            # tracguidance is only defined for ISO 11783
            append COMM_FEATURES " -o \( -path '*/Part7_ApplicationLayer/*' -a -name '*traccert*' \)"
        else
            PRJ_TRACTOR_CERTIFICATION=0
        fi
        if [ "$PRJ_TIME_GPS" -gt 0 ]; then
            append COMM_FEATURES " -o \( -path '*/Part7_ApplicationLayer/*' -a -name '*timeposgps*' \)"
        fi
    fi

    if [ "$PRJ_PROPRIETARY_PGN_INTERFACE" -gt 0 ]; then
        append COMM_FEATURES " -o -path '*/ProprietaryCan/*'"
    fi

    if [ "$PRJ_ISO_FILESERVER_CLIENT" -gt 0 ]  ; then
        if [ "$PRJ_ISO11783" -gt 0 ]; then
            append COMM_FEATURES " -o -path '*/Part13_FileServer_Client/*'"
        fi
    fi

    if [ "$PRJ_ISO_FILESERVER_CLIENT" -gt 0 ]  ; then
        PRJ_MULTIPACKET=1
    fi

    if [ "$PRJ_ISO_TERMINAL_SERVER" -gt 0 ]; then
        PRJ_MULTIPACKET=1
    fi
    if [ "$PRJ_ISO_TERMINAL" -gt 0 ]; then
        if [ "$USE_ISO_TERMINAL_GRAPHICCONTEXT" = "" ]; then
            # exclude graphicscontext_c
            if [ "$PRJ_ISO_TERMINAL_LAYOUT_MANAGER" -gt 0 ]; then
                append COMM_FEATURES " -o \( -path '*/Part6_VirtualTerminal_Client/*' -a -not -name '*graphicscontext_c*' \)"
            else
                append COMM_FEATURES " -o \( -path '*/Part6_VirtualTerminal_Client/i*' -a -not -name '*graphicscontext_c*' \)"
            fi
        else
            if [ "$PRJ_ISO_TERMINAL_LAYOUT_MANAGER" -gt 0 ]; then
                append COMM_FEATURES " -o -path '*/Part6_VirtualTerminal_Client/*'"
            else
                append COMM_FEATURES " -o -path '*/Part6_VirtualTerminal_Client/i*'"
            fi
            USE_ISO_TERMINAL_GRAPHICCONTEXT=1
        fi
        if [ "$USE_ISO_TERMINAL_GETATTRIBUTES" != "" ]; then
            USE_ISO_TERMINAL_GETATTRIBUTES=1
        fi
        PRJ_MULTIPACKET=1
    fi
    if [ "$PRJ_PROPRIETARY_PGN_INTERFACE" -gt 0 ]; then
        PRJ_MULTIPACKET=1
    fi
    if [ "$PRJ_DATASTREAMS" -lt 1 ]; then
        if test "$PRJ_ISO_TERMINAL" -gt 0 -o "$PRJ_TIME_GPS" -gt 0 ; then
            append COMM_FEATURES " -o -path '*/driver/datastreams/volatilememory_c.*'"
        fi
    fi
    if [ "$PRJ_ISO11783" -gt 0 ]; then
        append COMM_FEATURES " -o -path '*/Part3_DataLink/i*multi*' -o -path '*/Part3_DataLink/impl/stream_c.*' -o -path '*/Part3_DataLink/istream_c.*' -o -path '*/supplementary_driver/driver/datastreams/streaminput_c.h'  -o -path '*/IsoAgLib/convert.h'"
        if [ "$PRJ_MULTIPACKET_STREAM_CHUNK" -gt 0 ]; then
            append COMM_FEATURES " -o -path '*/Part3_DataLink/impl/streamchunk_c.*' -o -path '*/Part3_DataLink/impl/chunk_c.*'"
        else
            append COMM_FEATURES " -o -path '*/Part3_DataLink/impl/streamlinear_c.*'"
        fi
    fi

    if [ "$PRJ_ISO_FILESERVER_CLIENT" -gt 0 ]  ; then
        if [ "$PRJ_ISO11783" -gt 0 ]; then
            append COMM_FEATURES " -o -path '*/Part13_FileServer_Client/*'"
        fi
    fi

    DRIVER_FEATURES=" -path '*/hal/"$HAL_PATH"/can/can*.h'  -o  -path '*/hal/"$HAL_PATH"/can/hal_can*' -o -path '*/hal/can.h' -o -path '*/driver/system*'  -o \( -path '*/hal/"$HAL_PATH"/system*' -not -path '*hal_simulator*' \) -o -path '*/hal/system.h' -o -path '*/hal/"$HAL_PATH"/errcodes.h' -o -path '*/hal/"$HAL_PATH"/config.h'"


    echo_ "IsoAgLib's Project-Generator running..."
    echo_
    echo_ "CAN driver:    $USE_CAN_DRIVER"
    if [ "$USE_CAN_DRIVER" = "simulating" ]; then
        append DRIVER_FEATURES " -o -path '*/hal/"$HAL_PATH"/can/target_extension_can_simulating*'"
        if [ "$PRJ_SYSTEM_WITH_ENHANCED_CAN_HAL" -gt 0 ]; then
            echo_ 'The selected CAN driver "simulating" does NOT provide the enhanced CAN processing.'
            echo_ 'Thus the project files will be generated without enhanced CAN processing'
            PRJ_SYSTEM_WITH_ENHANCED_CAN_HAL=0
        fi
    elif [ "$USE_CAN_DRIVER" = "msq_server" ]; then
        append DRIVER_FEATURES " -o -path '*/hal/"$HAL_PATH"/can/target_extension_can_client_msq.*' -o -path '*/hal/"$HAL_PATH"/can/msq_helper*'"
    elif [ "$USE_CAN_DRIVER" = "socket_server" ]; then
        append DRIVER_FEATURES " -o -path '*/hal/"$HAL_PATH"/can/target_extension_can_client_sock.*'"
    elif [ "$USE_CAN_DRIVER" = "socket_server_hal_simulator" ]; then
        append DRIVER_FEATURES " -o -path '*/hal/"$HAL_PATH"/can/target_extension_can_client_sock_hal_simulator.*'"
    elif [ "$USE_CAN_DRIVER" = "sys" ]; then
        if [ "$PRJ_SYSTEM_WITH_ENHANCED_CAN_HAL" -gt 0 ]; then
            echo_ 'The selected CAN driver "sys" on embedded targets does NOT provide the enhanced CAN processing.'
            echo_ 'Thus the project files will be generated without enhanced CAN processing'
            PRJ_SYSTEM_WITH_ENHANCED_CAN_HAL=0
        fi
        #-- Currently there's no direct access to Linux/Windows CAN. You have to use the can_server.
        #-- The following is only kept for informational reasons - the files do not exist with these names currently!
        #   if [ $USE_TARGET_SYSTEM = "pc_linux" ]; then
        #     append DRIVER_FEATURES " -o -path '*/hal/"$HAL_PATH"/can/target_extension_can_linux_sys*'"
        #   elif [ $USE_TARGET_SYSTEM = "pc_win32" ]; then
        #     append DRIVER_FEATURES " -o -path '*/hal/"$HAL_PATH"/can/target_extension_can_w32_sys*'"
        #   fi
    else
        echo_ 'ERROR! Please set the config variable "USE_CAN_DRIVER" to one of "simulating"|"sys"|"msq_server"|"socket_server"|"socket_server_hal_simulator"'
        echo_ 'Current Setting is $USE_CAN_DRIVER'
        exit 3
    fi

    if [ "$PRJ_SYSTEM_WITH_ENHANCED_CAN_HAL" -lt 1 ]; then
        #in NOT enhanced can hal -> we need to integrate the sources for the central CAN FIFO
        append DRIVER_FEATURES " -o -path '*/hal/generic_utils/can/*'"
    fi

    if [ "$PRJ_EEPROM" -gt 0 ]; then
        append DRIVER_FEATURES " -o -path '*/driver/eeprom/*' -o -path '*/hal/"$HAL_PATH"/eeprom/*' -o -path '*/hal/eeprom.h' -o -name 'eeprom_adr.h'"
    fi
    if [ "$PRJ_DATASTREAMS" -gt 0 ]; then
        append DRIVER_FEATURES " -o -path '*/driver/datastreams/*' -o -path '*/hal/"$HAL_PATH"/datastreams/*' -o -path '*/hal/datastreams.h'"
    fi
    if [ "$PRJ_ACTOR" -gt 0 ]; then
        append DRIVER_FEATURES " -o -path '*/driver/actor*' -o -path '*/hal/"$HAL_PATH"/actor/actor.h' -o -path '*/hal/"$HAL_PATH"/actor/actor_target_extensions.*' -o -path '*/hal/actor.h'"
    fi
    if [ "$PRJ_SENSOR_DIGITAL" -gt 0 ]; then
        append DRIVER_FEATURES " -o -name '*digitali_c.*'"
    fi
    if [ "$PRJ_SENSOR_ANALOG" -gt 0 ]; then
        append DRIVER_FEATURES " -o -name '*analogi*'"
    fi
    if [ "$PRJ_SENSOR_COUNTER" -gt 0 ]; then
        append DRIVER_FEATURES " -o -name '*counteri*'"
    fi
    if test "$PRJ_SENSOR_DIGITAL" -gt 0 -o "$PRJ_SENSOR_ANALOG" -gt 0 -o "$PRJ_SENSOR_COUNTER" -gt 0 ; then
        append DRIVER_FEATURES " -o -path '*/hal/"$HAL_PATH"/sensor/sensor.h' -path '*/hal/"$HAL_PATH"/sensor/sensor_target_extensions.*' -o -name '*sensorbase_c.*' -o -name '*sensor_c.*' -o -name '*sensori_c.*' -o -path '*/hal/sensor.h'"
    fi
    if [ "$PRJ_RS232" -gt 0 ]; then
        append DRIVER_FEATURES " -o -path '*/driver/rs232/*' -o -path '*/hal/rs232.h' -o -path '*/hal/"$HAL_PATH"/rs232/rs232.h'"
        if [ "$PRJ_RS232_OVER_CAN" -gt 0 ]; then
            append DRIVER_FEATURES " -o -path '*/hal/virtualDrivers/rs232/*'"
        else
            #-- The following line is wrong and never had any effect.
            #-- Just leaving in for informational reasons.
            #     append DRIVER_FEATURES " -o -path '*/hal/"$HAL_PATH"/rs232/rs232/*'"
            echo_ "RS232 driver:  $USE_RS232_DRIVER"
            if [ "$USE_RS232_DRIVER" = "simulating" ]; then
                append DRIVER_FEATURES " -o -path '*/hal/"$HAL_PATH"/rs232/target_extension_rs232_simulating*'"
            elif [ "$USE_RS232_DRIVER" = "hal_simulator" ]; then
                append DRIVER_FEATURES " -o -path '*/hal/"$HAL_PATH"/rs232/target_extension_rs232_hal_simulator*'"
            elif [ "$USE_RS232_DRIVER" = "rte" ]; then
                append DRIVER_FEATURES " -o -path '*/hal/"$HAL_PATH"/rs232/target_extension_rs232_rte*'"
            elif [ "$USE_RS232_DRIVER" = "sys" ]; then
                if [ "$USE_TARGET_SYSTEM" = "pc_win32" ]; then
                    append DRIVER_FEATURES " -o -path '*/hal/"$HAL_PATH"/rs232/target_extension_rs232_w32_sys*'"
                else
                    append DRIVER_FEATURES " -o -path '*/hal/"$HAL_PATH"/rs232/target_extension_rs232_linux_sys*'"
                fi
                #     PRJ_DEFINES="$PRJ_DEFINES USE_REAL_RS232"
                #     append DRIVER_FEATURES " -o -path '*/hal/"$HAL_PATH"/rs232/target_extension_rs232_simulating*'"
            else
                echo_ 'ERROR! Please set the config variable "USE_RS232_DRIVER" to one of "simulating"|"sys"|"rte"|"hal_simulator"'
                echo_ 'Current Setting is $USE_RS232_DRIVER'
                exit 3
            fi
        fi
        append DRIVER_FEATURES " -o -path '*/hal/"$HAL_PATH"/rs232/rs232_target_extensions.h'"
    fi

    # add the standard driver directory sources for CAN
    if [ "$PRJ_SYSTEM_WITH_ENHANCED_CAN_HAL" -gt 0 ]; then
        append DRIVER_FEATURES " -o \( -path '*/driver/can/*' -a -not -name 'msgobj*' \)"
    else
        append DRIVER_FEATURES " -o -path '*/driver/can/*'"
    fi

    LIB_ROOT="$ISO_AG_LIB_INSIDE/library/xgpl_src"
    SRC_EXT="\( -name '*.c' -o -name '*.cc' -o -name '*.cpp' \)"
    #  HDR_UTEST_EXT="\( -name '*-test.h' \)"
    #  HDR_UTEST_MOCK_EXT="\( -name '*-mock.h' \)"
    #  TESTRUNNER_EXT="\( -name 'testrunner.cpp' \)"
    #  UTEST_DIRS_EXT="\( -name 'utest' -type d \)"


    # go back to directory where config file resides
    mkdir -p $1
    cd $1
    # echo_ "create filelist with 1 $1 and GENERATE_FILES_ROOT_DIR $GENERATE_FILES_ROOT_DIR"
    # mkdir tmpdir
    mkdir -p $PROJECT
    cd $PROJECT
    FILELIST_LIBRARY_HDR="filelist"'__'"$PROJECT_hdr.library.txt"
    FILELIST_LIBRARY_PURE="filelist"'__'"$PROJECT.library.txt"

    FILELIST_APP_HDR="filelist"'__'"$PROJECT_hdr.app.txt"
    FILELIST_APP_PURE="filelist"'__'"$PROJECT.app.txt"

    FILELIST_COMBINED_HDR="filelist"'__'"$PROJECT_hdr.txt"
    FILELIST_COMBINED_PURE="filelist"'__'"$PROJECT.txt"

    if [ "A$DOXYGEN_EXPORT_DIR" = "A" ]; then
        FILELIST_DOXYGEN_READY="filelist"'__'"$PROJECT"'__'"$USE_TARGET_SYSTEM"'__'"$CAN_SERVER_FILENAME"'__'"$USE_RS232_DRIVER-doc.txt"
    else
        FILELIST_DOXYGEN_READY="$DOXYGEN_EXPORT_DIR/filelist"'__'"$PROJECT"'__'"$USE_TARGET_SYSTEM"'__'"$CAN_SERVER_FILENAME"'__'"$USE_RS232_DRIVER-doc.txt"
    fi


    rm -f "$FILELIST_LIBRARY_PURE" "$FILELIST_LIBRARY_HDR"
    rm -f "$FILELIST_APP_PURE" "$FILELIST_APP_HDR"
    rm -f "$FILELIST_COMBINED_PURE" "$FILELIST_COMBINED_HDR"

    touch "$FILELIST_LIBRARY_PURE" "$FILELIST_APP_PURE" "$FILELIST_COMBINED_PURE"

    FIND_TEMP_PATH="-path '*/scheduler/*' -o -path '*/Part5_NetworkManagement/*' -o -path '*/Part12_DiagnosticsServices/*' -o -path '*/util/*' -o -path '*/Part3_DataLink/i*can*' "

    # find wanted process data communication features
    if [ -n "$COMM_PROC_FEATURES" ]; then
        append FIND_TEMP_PATH " -o $COMM_PROC_FEATURES"
    fi
    # find wanted other communication features
    if [ -n "$COMM_FEATURES" ]; then
        append FIND_TEMP_PATH " -o $COMM_FEATURES"
    fi
    #find optional HW features
    if [ -n "$DRIVER_FEATURES" ]; then
        append FIND_TEMP_PATH " -o $DRIVER_FEATURES"
    fi

    EXCLUDE_PATH_PART="-a -not -path '*/xgpl_src/build/*'"
    if [ "A" != "A$APP_PATH_EXCLUDE"  ]; then
        EXCLUDE_PATH_PART="-and -not \( -path '*/xgpl_src/build/*'"
        for itemPathExclude in $APP_PATH_EXCLUDE ; do
            append EXCLUDE_PATH_PART " -or -path \"$itemPathExclude\""
        done
        append EXCLUDE_PATH_PART " \)"
    fi

    eval "find $LIB_ROOT -follow $SRC_EXT -a \( $FIND_TEMP_PATH \) $EXCLUDE_FROM_SYSTEM_MGMT $EXCLUDE_PATH_PART -printf '%h/%f\n' >> $FILELIST_LIBRARY_PURE"
    eval "find $LIB_ROOT -follow -name '*.h' -a \( $FIND_TEMP_PATH \) $EXCLUDE_FROM_SYSTEM_MGMT $EXCLUDE_PATH_PART -printf '%h/%f\n' >> $FILELIST_LIBRARY_HDR"

    ##############################
    # find application files
    ##############################
    FIRST_LOOP="YES"
    APP_SRC_PART=""
    if [ "A$APP_SRC_FILE" != "A" ]; then
        APP_SRC_PART="-and \( "
        for itemSrcInclude in $APP_SRC_FILE ; do
            if [ $FIRST_LOOP != "YES" ]; then
                append APP_SRC_PART " -or"
            else
                FIRST_LOOP="NO"
            fi
            append APP_SRC_PART " -name \"$itemSrcInclude\""
        done
        append APP_SRC_PART " \)"
    fi

    FIRST_LOOP="YES"
    EXCLUDE_PATH_PART=""
    if [ "A" != "A$APP_PATH_EXCLUDE"  ]; then
        EXCLUDE_PATH_PART="-and -not \( "
        for itemPathExclude in $APP_PATH_EXCLUDE ; do
            if [ $FIRST_LOOP != "YES" ]; then
                append EXCLUDE_PATH_PART " -or"
            else
                FIRST_LOOP="NO"
            fi
            append EXCLUDE_PATH_PART " -path \"$itemPathExclude\""
        done
        append EXCLUDE_PATH_PART " \)"
    fi

    FIRST_LOOP="YES"
    EXCLUDE_SRC_PART=""
    if [ "A" != "A$APP_SRC_EXCLUDE"  ]; then
        EXCLUDE_SRC_PART="-and -not \( "
        for itemSrcExclude in $APP_SRC_EXCLUDE ; do
            if [ $FIRST_LOOP != "YES" ]; then
                append EXCLUDE_SRC_PART " -or"
            else
                FIRST_LOOP="NO"
            fi
            append EXCLUDE_SRC_PART " -name \"$itemSrcExclude\""
        done
        append EXCLUDE_SRC_PART " \)"
    fi


    FIRST_LOOP="YES"
    APP_SEARCH_SRC_TYPE_PART="\( "

    # remove the joker '*' from the file type spec, as this causes only trouble
    APP_SEARCH_SRC_CONDITION=$(echo_ "$APP_SEARCH_SRC_CONDITION" |sed 's|\*||g')

    for itemSrcType in $APP_SEARCH_SRC_CONDITION ; do
        if [ $FIRST_LOOP != "YES" ]; then
            append APP_SEARCH_SRC_TYPE_PART " -or"
        else
            FIRST_LOOP="NO"
        fi
        append APP_SEARCH_SRC_TYPE_PART " -name '*$itemSrcType'"
    done
    append APP_SEARCH_SRC_TYPE_PART " \)"


    FIRST_LOOP="YES"
    APP_SEARCH_HDR_TYPE_PART="\( "
    for itemHdrType in $APP_SEARCH_HDR_CONDITION ; do
        if [ $FIRST_LOOP != "YES" ]; then
            append APP_SEARCH_HDR_TYPE_PART " -or"
        else
            FIRST_LOOP="NO"
        fi
        append APP_SEARCH_HDR_TYPE_PART " -name \"$itemHdrType\""
    done
    append APP_SEARCH_HDR_TYPE_PART " \)"

    for EACH_REL_APP_PATH in $REL_APP_PATH ; do
        eval "find $ISO_AG_LIB_INSIDE/$EACH_REL_APP_PATH/ -follow $APP_SEARCH_SRC_TYPE_PART $APP_SRC_PART $EXCLUDE_PATH_PART $EXCLUDE_SRC_PART -printf '%h/%f\n' >> $FILELIST_APP_PURE"
        eval "find $ISO_AG_LIB_INSIDE/$EACH_REL_APP_PATH/ -follow $APP_SEARCH_HDR_TYPE_PART $APP_SRC_PART $EXCLUDE_PATH_PART $EXCLUDE_SRC_PART -printf '%h/%f\n' >> $FILELIST_APP_HDR"
    done

    cat $FILELIST_LIBRARY_PURE $FILELIST_APP_PURE > $FILELIST_COMBINED_PURE
    cat $FILELIST_LIBRARY_HDR $FILELIST_APP_HDR > $FILELIST_COMBINED_HDR

    # create list with suitable block definition for doxygen import
    # start main block for all files for project
    #echo_ "/**" > $FILELIST_DOXYGEN_READY
    #echo_ "* \section FileLists$PROJECT"'__'"$USE_TARGET_SYSTEM"'__'"$CAN_SERVER_FILENAME"'__'"$USE_RS232_DRIVER List of Files for $PROJECT ." >> $FILELIST_DOXYGEN_READY
    #echo_ "*/" >> $FILELIST_DOXYGEN_READY
    #echo_ "/*@{*/" >> $FILELIST_DOXYGEN_READY
    echo_e "\n\n @section FileLists$PROJECT"'__'"$USE_TARGET_SYSTEM"'__'"$CAN_SERVER_FILENAME"'__'"$USE_RS232_DRIVER List of Files for $PROJECT with CAN Driver $USE_CAN_DRIVER and RS232 Driver $USE_RS232_DRIVER" > $FILELIST_DOXYGEN_READY

    # write block of source files
    #echo_ "/**" >> $FILELIST_DOXYGEN_READY
    #echo_ "* \section SrcList$PROJECT"'__'"$USE_TARGET_SYSTEM"'__'"$CAN_SERVER_FILENAME"'__'"$USE_RS232_DRIVER List of Sourcefiles for $PROJECT ." >> $FILELIST_DOXYGEN_READY
    #echo_ "*/" >> $FILELIST_DOXYGEN_READY
    #echo_ "/*@{*/" >> $FILELIST_DOXYGEN_READY
    echo_e "\n\n @section SrcList$PROJECT"'__'"$USE_TARGET_SYSTEM"'__'"$CAN_SERVER_FILENAME"'__'"$USE_RS232_DRIVER List of Sourcefiles for $PROJECT with CAN Driver $USE_CAN_DRIVER and RS232 Driver $USE_RS232_DRIVER" >> $FILELIST_DOXYGEN_READY
    echo_ "\code" >> $FILELIST_DOXYGEN_READY
    cat $FILELIST_COMBINED_PURE >> $FILELIST_DOXYGEN_READY
    echo_ "\endcode" >> $FILELIST_DOXYGEN_READY
    #echo_ "/*@}*/" >> $FILELIST_DOXYGEN_READY

    echo_ "" >> $FILELIST_DOXYGEN_READY

    # write block of header files
    #echo_ "/**" >> $FILELIST_DOXYGEN_READY
    #echo_ "* \section HdrList$PROJECT"'__'"$USE_TARGET_SYSTEM"'__'"$CAN_SERVER_FILENAME"'__'"$USE_RS232_DRIVER List of Sourcefiles for $PROJECT ." >> $FILELIST_DOXYGEN_READY
    #echo_ "*/" >> $FILELIST_DOXYGEN_READY
    #echo_ "/*@{*/" >> $FILELIST_DOXYGEN_READY
    echo_e "\n\n @section HdrList$PROJECT"'__'"$USE_TARGET_SYSTEM"'__'"$CAN_SERVER_FILENAME"'__'"$USE_RS232_DRIVER List of Headers for $PROJECT with CAN Driver $USE_CAN_DRIVER and RS232 Driver $USE_RS232_DRIVER" >> $FILELIST_DOXYGEN_READY
    echo_ "\code" >> $FILELIST_DOXYGEN_READY
    cat $FILELIST_COMBINED_HDR >> $FILELIST_DOXYGEN_READY
    echo_ "\endcode" >> $FILELIST_DOXYGEN_READY
    #echo_ "/*@}*/" >> $FILELIST_DOXYGEN_READY

    # write end of main block of all project files
    #echo_ "/*@}*/" >> $FILELIST_DOXYGEN_READY

    cat $FILELIST_COMBINED_HDR >> $FILELIST_COMBINED_PURE
    rm -f $FILELIST_COMBINED_HDR

    # go back to directory where config file resides
    cd $1

    return
}

# this function uses the "find" cmd
# to derive based on the selected features the
# corresponding file list into filelist_$FILELIST_UTEST_PURE.txt
# and filelist_$FILELIST_UTEST_MOCK_PURE.txt
create_utest_filelist()
{
    # UTESTs are disabled for now.
    return

    FILELIST_UTEST_PURE="filelist"'__'"$PROJECT.utest.txt"
    FILELIST_UTEST_MOCK_PURE="filelist"'__'"$PROJECT.utest.mock.txt"
    FILELIST_UTEST_MODSUT_PURE="filelist"'__'"$PROJECT.utest.modsut.txt"
    FILELIST_UTEST_RUNNER_PURE="filelist"'__'"$PROJECT.utest.runner.txt"

    DIRECTORYLIST_UTEST_PURE="directories"'__'"$PROJECT.utest.txt"

    # go to project directory
    cd $PROJECT
    # create new files containing the file lists
    rm -f "$FILELIST_UTEST_PURE" "$FILELIST_UTEST_MOCK_PURE" "$FILELIST_UTEST_MODSUT_PURE" "$FILELIST_UTEST_RUNNER_PURE" "$DIRECTORYLIST_UTEST_PURE"
    touch "$FILELIST_UTEST_PURE" "$FILELIST_UTEST_MOCK_PURE" "$FILELIST_UTEST_MODSUT_PURE" "$FILELIST_UTEST_RUNNER_PURE" "$DIRECTORYLIST_UTEST_PURE"

    # find unit test directories

    for EACH_REL_APP_PATH in $REL_APP_PATH ; do
        eval "find $ISO_AG_LIB_INSIDE/$EACH_REL_APP_PATH/ $UTEST_DIRS_EXT >> $DIRECTORYLIST_UTEST_PURE"
    done

    exec 0<$DIRECTORYLIST_UTEST_PURE

    while read line
    do
    # find unit test files (*-test.h)
        eval "find $line $HDR_UTEST_EXT -printf '%h/%f\n' >> $FILELIST_UTEST_PURE"

    # find mock object files (*-mock.h)
        eval "find $line $HDR_UTEST_MOCK_EXT -printf '%h/%f\n' >> $FILELIST_UTEST_MOCK_PURE"
    done
    # determine modified software under test (MOD-SUT) files
    sed -e 's!\(.*\)/utest/\(.*\)-test\.h!\1/\2_c.h!' < $FILELIST_UTEST_PURE >> $FILELIST_UTEST_MODSUT_PURE

    # find the testrunner (testrunner.cpp)
    for EACH_REL_APP_PATH in $REL_APP_PATH ; do
        eval "find $ISO_AG_LIB_INSIDE/$EACH_REL_APP_PATH/ $TESTRUNNER_EXT -printf '%h/%f\n' >> $FILELIST_UTEST_RUNNER_PURE"
    done
}

# function to create the project specific autogenerated
# config header
# autogen_project_config.h
create_autogen_project_config()
{
    # go to project dir - below config dir
    cd $1/$PROJECT
    PLATFORM=$(uname)
    if [ $PLATFORM = "Linux" ] ; then
        ENDLINE="\n"
    else
        ENDLINE="\r\n"
    fi

    for FIRST_REL_APP_PATH in $REL_APP_PATH ; do
        # CONFIG_NAME=$ISO_AG_LIB_INSIDE/library/xgpl_src/IsoAgLib/.config_$PROJECT.h
        CONFIG_NAME=$ISO_AG_LIB_INSIDE/$FIRST_REL_APP_PATH/config_$PROJECT.h
        VERSION_FILE_NAME=$ISO_AG_LIB_INSIDE/$FIRST_REL_APP_PATH/version.h
        if [ ! -f $VERSION_FILE_NAME ] ; then
            echo_    "#ifndef POOL_VERSION"            > $VERSION_FILE_NAME
            echo_e "\t#define POOL_VERSION 1.0"     >> $VERSION_FILE_NAME
            echo_    "#endif"                         >> $VERSION_FILE_NAME
            echo_    "#ifndef FIRMWARE_VERSION"       >> $VERSION_FILE_NAME
            echo_e "\t#define FIRMWARE_VERSION 1.0" >> $VERSION_FILE_NAME
            echo_    "#endif"                         >> $VERSION_FILE_NAME
        fi
        break;
    done

    if [ "A$DOXYGEN_EXPORT_DIR" = "A" ] ; then
        CONFIG_HEADER_DOXYGEN_READY="config_header"'__'"$PROJECT-doc.txt"
    else
        CONFIG_HEADER_DOXYGEN_READY="$DOXYGEN_EXPORT_DIR/config_header"'__'"$PROJECT-doc.txt"
    fi

  # first backup individual settings after line
  # START_INDIVIDUAL_PROJECT_CONFIG
    if [ -f $CONFIG_NAME ] ; then
        grep -A1000 "// START_INDIVIDUAL_PROJECT_CONFIG"  $CONFIG_NAME > $CONFIG_NAME.bak
    else
        touch $CONFIG_NAME.bak
    fi
    rm -f $CONFIG_NAME
  # write INST counts
    CONFIG_HEADER_FILENAME=$(basename $CONFIG_NAME)
    echo_ "// File: $CONFIG_HEADER_FILENAME" > $CONFIG_NAME
    echo_ "// IMPORTANT: Never change the first block of this header manually!!!" >> $CONFIG_NAME
    echo_ "//            All manual changes are overwritten by the next call of \"update_makefile.sh $CONF_FILE\" " >> $CONFIG_NAME
    echo_ "//            Perform changes direct in the feature and project setup file $CONF_FILE"  >> $CONFIG_NAME
    echo_ "//  ALLOWED ADAPTATION: Move the to be adapted defines from the middle block to the end after" >> $CONFIG_NAME
    echo_ "//                      the line START_INDIVIDUAL_PROJECT_CONFIG and remove the comment indication there."  >> $CONFIG_NAME
    echo_ "//                      All commented out defines in the middle block will be upated on next \"update_makefile.sh $CONF_FILE\" call,"  >> $CONFIG_NAME
    echo_ "//                      if the corresponding value in isoaglib_config.h changed" >> $CONFIG_NAME

    echo_e "\n\n// include an external file for definition of pool and firmware versions" >> $CONFIG_NAME
    echo_    "#include \"version.h\""  >> $CONFIG_NAME

    echo_e "$ENDLINE" >> $CONFIG_NAME
    echo_    "#define PRJ_USE_AUTOGEN_CONFIG config_$PROJECT.h" >> $CONFIG_NAME
    for SinglePrjDefine in $PRJ_DEFINES ; do
        SingleDefName="${SinglePrjDefine%%=*}"
        SingleDefRemainder="${SinglePrjDefine#$SingleDefName}"
        SingleDefValue="${SingleDefRemainder#=}"
        echo_ "#ifndef $SingleDefName" >> $CONFIG_NAME
        echo_ "#define $SingleDefName $SingleDefValue" >> $CONFIG_NAME
        echo_ "#endif // $SingleDefName" >> $CONFIG_NAME
    done
    echo_e "$ENDLINE" >> $CONFIG_NAME


    echo_e "#define CAN_BUS_CNT $CAN_BUS_CNT $ENDLINE" >> $CONFIG_NAME
    echo_e "#define CAN_BUS_USED $CAN_BUS_USED $ENDLINE" >> $CONFIG_NAME
    echo_e "#define CAN_INSTANCE_CNT $CAN_INSTANCE_CNT $ENDLINE" >> $CONFIG_NAME
    echo_e "#define PRT_INSTANCE_CNT $PRT_INSTANCE_CNT $ENDLINE" >> $CONFIG_NAME
    echo_e "#define RS232_INSTANCE_CNT $RS232_INSTANCE_CNT $ENDLINE" >> $CONFIG_NAME


    echo_ "// Decide if the CPU stores number variables in BIG or LITTLE endian byte order in memory." >> $CONFIG_NAME
    echo_ "// Most CPU will use LITTLE ENDIAN. Only some types of ARM, mostly 68k and PowerPC CPU types will use big endian." >> $CONFIG_NAME
    echo_ "// Please check the manual of your targret cpu. This setting is used to activate some quick number conversion algorithms," >> $CONFIG_NAME
    echo_ "// which provide quick conversion from number variable to CAN strings ( which are always little endian in ISO ) - and other way." >> $CONFIG_NAME
    if [ "$USE_LITTLE_ENDIAN_CPU" -gt 0 ] ; then
        echo_e "#define OPTIMIZE_NUMBER_CONVERSIONS_FOR_LITTLE_ENDIAN$ENDLINE" >> $CONFIG_NAME
    else
        echo_e "// #define OPTIMIZE_NUMBER_CONVERSIONS_FOR_LITTLE_ENDIAN$ENDLINE" >> $CONFIG_NAME
    fi
    if [ "$USE_BIG_ENDIAN_CPU" -gt 0 ] ; then
        echo_e "#define OPTIMIZE_NUMBER_CONVERSIONS_FOR_BIG_ENDIAN$ENDLINE" >> $CONFIG_NAME
    else
        echo_e "// #define OPTIMIZE_NUMBER_CONVERSIONS_FOR_BIG_ENDIAN$ENDLINE" >> $CONFIG_NAME
    fi

    if [ "$USE_PCAN_LIB" -gt 0 ] ; then
        echo_e "#define USE_PCAN_LIB$ENDLINE" >> $CONFIG_NAME
    else
        echo_e "// #define USE_PCAN_LIB$ENDLINE" >> $CONFIG_NAME
    fi

    if [ "$PRJ_DO_NOT_START_RELAIS_ON_STARTUP" -gt 0 ] ; then
        echo_e "#define CONFIG_DO_NOT_START_RELAIS_ON_STARTUP$ENDLINE" >> $CONFIG_NAME
    else
        echo_e "// #define CONFIG_DO_NOT_START_RELAIS_ON_STARTUP$ENDLINE" >> $CONFIG_NAME
    fi

    if [ "$PRJ_RS232_OVER_CAN" -gt 0 ] ; then
        echo_e "#define USE_RS232_OVER_CAN$ENDLINE" >> $CONFIG_NAME
    fi

    if [ "$USE_VT_UNICODE_SUPPORT" -gt 0 ] ; then
        echo_e "#define USE_VT_UNICODE_SUPPORT$ENDLINE" >> $CONFIG_NAME
    fi


    if [ "$PRJ_BASE" -gt 0 ] ; then
        echo_e "#ifndef USE_BASE $ENDLINE\t#define USE_BASE $ENDLINE#endif" >> $CONFIG_NAME
    fi
    if test "$PRJ_TRACTOR_GENERAL" -gt 0 ; then
        echo_e "#ifndef USE_TRACTOR_GENERAL $ENDLINE\t#define USE_TRACTOR_GENERAL $ENDLINE#endif" >> $CONFIG_NAME
    fi
    if test "$PRJ_TRACTOR_MOVE" -gt 0 ; then
        echo_e "#ifndef USE_TRACTOR_MOVE $ENDLINE\t#define USE_TRACTOR_MOVE $ENDLINE#endif" >> $CONFIG_NAME
    fi
    if test "$PRJ_TRACTOR_PTO" -gt 0 ; then
        echo_e "#ifndef USE_TRACTOR_PTO $ENDLINE\t#define USE_TRACTOR_PTO $ENDLINE#endif" >> $CONFIG_NAME
    fi
    if test "$PRJ_TRACTOR_LIGHT" -gt 0 ; then
        echo_e "#ifndef USE_TRACTOR_LIGHT $ENDLINE\t#define USE_TRACTOR_LIGHT $ENDLINE#endif" >> $CONFIG_NAME
    fi
    if test "$PRJ_TRACTOR_FACILITIES" -gt 0 ; then
        echo_e "#ifndef USE_TRACTOR_FACILITIES $ENDLINE\t#define USE_TRACTOR_FACILITIES $ENDLINE#endif" >> $CONFIG_NAME
    fi
    if test "$PRJ_TRACTOR_AUX" -gt 0 ; then
        echo_e "#ifndef USE_TRACTOR_AUX $ENDLINE\t#define USE_TRACTOR_AUX $ENDLINE#endif" >> $CONFIG_NAME
    fi
    if test "$PRJ_TRACTOR_GUIDANCE" -gt 0 ; then
        echo_e "#ifndef USE_TRACTOR_GUIDANCE $ENDLINE\t#define USE_TRACTOR_GUIDANCE $ENDLINE#endif" >> $CONFIG_NAME
    fi
    if test "$PRJ_TRACTOR_CERTIFICATION" -gt 0 ; then
        echo_e "#ifndef USE_TRACTOR_CERTIFICATION $ENDLINE\t#define USE_TRACTOR_CERTIFICATION $ENDLINE#endif" >> $CONFIG_NAME
    fi
    if test "$PRJ_TIME_GPS" -gt 0 ; then
        echo_e "#ifndef USE_TIME_GPS $ENDLINE\t#define USE_TIME_GPS $ENDLINE#endif" >> $CONFIG_NAME
    fi

    if [ "$PROC_LOCAL" -gt 0 ] ; then
        echo_e "#ifndef USE_PROC_DATA_DESCRIPTION_POOL $ENDLINE\t#define USE_PROC_DATA_DESCRIPTION_POOL $ENDLINE#endif" >> $CONFIG_NAME
    fi

    echo_ "// Decide if HEAP allocation strategy shall reduce size about 5K to 10K in favour of speed" >> $CONFIG_NAME
    echo_ "// Strong Advice: Don't activate this, as long your target has not too tight memory restrictions" >> $CONFIG_NAME
    echo_ "// Initialization of CAN filters and of local process data might get too slow under worst case conditions" >> $CONFIG_NAME
    if [ "$OPTIMIZE_HEAPSIZE_IN_FAVOR_OF_SPEED" -gt 0 ] ; then
        echo_e "#define OPTIMIZE_HEAPSIZE_IN_FAVOR_OF_SPEED$ENDLINE" >> $CONFIG_NAME
    else
        echo_e "// #define OPTIMIZE_HEAPSIZE_IN_FAVOR_OF_SPEED$ENDLINE" >> $CONFIG_NAME
    fi

    echo_ "/** allow configuration by parameter value YES */" >> $CONFIG_NAME
    echo_ "#ifndef YES" >> $CONFIG_NAME
    echo_ "  #define YES 1" >> $CONFIG_NAME
    echo_ "#endif" >> $CONFIG_NAME
    echo_ "/** allow configuration by parameter value NO */" >> $CONFIG_NAME
    echo_ "#ifndef NO" >> $CONFIG_NAME
    echo_ "  #define NO 0" >> $CONFIG_NAME
    echo_ "#endif" >> $CONFIG_NAME


    if [ "$USE_FLOAT_DATA_TYPE" -gt 0 ] ; then
        echo_ "// Decide if float shall be used for the project" >> $CONFIG_NAME
        echo_e "#define USE_FLOAT_DATA_TYPE$ENDLINE" >> $CONFIG_NAME
    fi


    if [ "$PRJ_PROCESS" -gt 0 ] ; then
        echo_e "#ifndef USE_PROCESS $ENDLINE  #define USE_PROCESS $ENDLINE#endif" >> $CONFIG_NAME
        if [ "$PROC_REMOTE" -gt 0 ] ; then
            if [ "$PROC_REMOTE_STD" -gt 0 ] ; then
                echo_e "#ifndef USE_PROC_REMOTE_STD $ENDLINE  #define USE_PROC_REMOTE_STD $ENDLINE#endif" >> $CONFIG_NAME
            fi
            if [ "$PROC_REMOTE_SIMPLE_MEASURE" -gt 0 ] ; then
                echo_e "#ifndef USE_PROC_REMOTE_SIMPLE_MEASURE $ENDLINE  #define USE_PROC_REMOTE_SIMPLE_MEASURE $ENDLINE#endif" >> $CONFIG_NAME
            fi
            if [ "$PROC_REMOTE_SIMPLE_SETPOINT" -gt 0 ] ; then
                echo_e "#ifndef USE_PROC_REMOTE_SIMPLE_SETPOINT $ENDLINE  #define USE_PROC_REMOTE_SIMPLE_SETPOINT $ENDLINE#endif" >> $CONFIG_NAME
            fi
            if [ "$PROC_REMOTE_SIMPLE_MEASURE_SETPOINT" -gt 0 ] ; then
                echo_e "#ifndef USE_PROC_REMOTE_SIMPLE_MEASURE_SETPOINT $ENDLINE  #define USE_PROC_REMOTE_SIMPLE_MEASURE_SETPOINT $ENDLINE#endif" >> $CONFIG_NAME
            fi
            if [ "$PROC_REMOTE_SIMPLE_MEASURE_SETPOINT_COMBINED" -gt 0 ] ; then
                echo_e "#ifndef USE_PROC_SIMPLE_REMOTE_MEASURE_SETPOINT_COMBINED $ENDLINE  #define USE_PROC_SIMPLE_REMOTE_MEASURE_SETPOINT_COMBINED $ENDLINE#endif" >> $CONFIG_NAME
            fi
        fi
    else
    # the default in isoaglib_config.h is to activate
    # PROCESS as long as USE_PROCESS_YN unset
        echo_e "#ifndef USE_PROCESS_YN $ENDLINE\t#define USE_PROCESS_YN NO $ENDLINE#endif" >> $CONFIG_NAME
    fi
    if [ "$PRJ_EEPROM" -gt 0 ] ; then
        echo_e "#ifndef USE_EEPROM_IO $ENDLINE\t#define USE_EEPROM_IO $ENDLINE#endif" >> $CONFIG_NAME
    else
    # the default in isoaglib_config.h is to activate
    # EEPROM as long as USE_EEPROM_IO_YN unset
        echo_e "#ifndef USE_EEPROM_IO_YN $ENDLINE\t#define USE_EEPROM_IO_YN NO $ENDLINE#endif" >> $CONFIG_NAME
    fi

    if test "$PRJ_DATASTREAMS" -gt 0 -o $PRJ_ISO_TERMINAL -gt 0 -o $PRJ_TIME_GPS -gt 0 ; then
        echo_e "#ifndef USE_DATASTREAMS_IO $ENDLINE\t#define USE_DATASTREAMS_IO $ENDLINE#endif" >> $CONFIG_NAME
    else
    # the default in isoaglib_config.h is to activate
    # DATASTREAMS as long as USE_DATASTREAMS_IO_YN unset
        echo_e "#ifndef USE_DATASTREAMS_IO_YN $ENDLINE\t#define USE_DATASTREAMS_IO_YN NO $ENDLINE#endif" >> $CONFIG_NAME
    fi

    if [ "$PRJ_ISO11783" -gt 0 ] ; then
        echo_e "#ifndef USE_ISO_11783 $ENDLINE\t#define USE_ISO_11783 $ENDLINE#endif" >> $CONFIG_NAME
        if [ "$PRJ_ISO_TERMINAL" -gt 0 ] ; then
            echo_e "#ifndef USE_ISO_TERMINAL $ENDLINE\t#define USE_ISO_TERMINAL $ENDLINE#endif" >> $CONFIG_NAME
            if [ "$USE_ISO_TERMINAL_ATTRIBUTES" != "" ] ; then
                echo_e "#ifndef USE_ISO_TERMINAL_ATTRIBUTES $ENDLINE\t#define USE_ISO_TERMINAL_ATTRIBUTES $ENDLINE#endif" >> $CONFIG_NAME
            fi
            if [ "$USE_ISO_TERMINAL_GRAPHICCONTEXT" != "" ] ; then
                echo_e "#ifndef USE_ISO_TERMINAL_GRAPHICCONTEXT $ENDLINE\t#define USE_ISO_TERMINAL_GRAPHICCONTEXT $ENDLINE#endif" >> $CONFIG_NAME
            fi
            if [ "$PRJ_ISO_TERMINAL_OBJECT_SELECTION1" != "" ] ; then
                echo_e "#ifndef PRJ_ISO_TERMINAL_OBJECT_SELECTION1 $ENDLINE\t#define PRJ_ISO_TERMINAL_OBJECT_SELECTION1 $PRJ_ISO_TERMINAL_OBJECT_SELECTION1 $ENDLINE#endif" >> $CONFIG_NAME
            fi
            if [ "$PRJ_ISO_TERMINAL_OBJECT_SELECTION2" != "" ] ; then
                echo_e "#ifndef PRJ_ISO_TERMINAL_OBJECT_SELECTION2 $ENDLINE\t#define PRJ_ISO_TERMINAL_OBJECT_SELECTION2 $PRJ_ISO_TERMINAL_OBJECT_SELECTION2 $ENDLINE#endif" >> $CONFIG_NAME
            fi
            if [ "$PRJ_ISO_TERMINAL_OBJECT_SELECTION3" != "" ] ; then
                echo_e "#ifndef PRJ_ISO_TERMINAL_OBJECT_SELECTION3 $ENDLINE\t#define PRJ_ISO_TERMINAL_OBJECT_SELECTION3 $PRJ_ISO_TERMINAL_OBJECT_SELECTION3 $ENDLINE#endif" >> $CONFIG_NAME
            fi
            if [ "$PRJ_ISO_TERMINAL_OBJECT_SELECTION4" != "" ] ; then
                echo_e "#ifndef PRJ_ISO_TERMINAL_OBJECT_SELECTION4 $ENDLINE\t#define PRJ_ISO_TERMINAL_OBJECT_SELECTION4 $PRJ_ISO_TERMINAL_OBJECT_SELECTION4 $ENDLINE#endif" >> $CONFIG_NAME
            fi
        fi
        if [ "$PRJ_MULTIPACKET" -gt 0 ] ; then
            if [ "$PRJ_MULTIPACKET_STREAM_CHUNK" -gt 0 ] ; then
                echo_e "#ifndef DEF_Stream_IMPL   $ENDLINE\t#define DEF_Stream_IMPL   StreamChunk   $ENDLINE#endif" >> $CONFIG_NAME
                echo_e "#ifndef DEF_Stream_c_IMPL $ENDLINE\t#define DEF_Stream_c_IMPL StreamChunk_c $ENDLINE#endif" >> $CONFIG_NAME
            else
                echo_e "#ifndef DEF_Stream_IMPL   $ENDLINE\t#define DEF_Stream_IMPL   StreamLinear   $ENDLINE#endif" >> $CONFIG_NAME
                echo_e "#ifndef DEF_Stream_c_IMPL $ENDLINE\t#define DEF_Stream_c_IMPL StreamLinear_c $ENDLINE#endif" >> $CONFIG_NAME
            fi
        fi
        if [ "$PRJ_ISO_TERMINAL_SERVER" -gt 0 ] ; then
            echo_e "#ifndef USE_ISO_TERMINAL_SERVER $ENDLINE\t#define USE_ISO_TERMINAL_SERVER $ENDLINE#endif" >> $CONFIG_NAME
            echo_ "with ISO_TERMINAL_SERVER!"
        fi
    fi

    if [ "$PRJ_SEND_DEBUG" -gt 0 ] ; then
        echo_e "#ifndef DEBUG $ENDLINE\t#define DEBUG $ENDLINE#endif" >> $CONFIG_NAME
    fi

  # write overwriteable parts of isoaglib_config.h
    echo_e "$ENDLINE// The following configuration values can be overwritten." >> $CONFIG_NAME
    echo_ "// These settings are initially defined in isoaglib_config.h ." >> $CONFIG_NAME
    echo_ "// These settings are in commented-out, so that you can activate and adapt them by" >> $CONFIG_NAME
    echo_e "// moving them below the line with START_INDIVIDUAL_PROJECT_CONFIG$ENDLINE"  >> $CONFIG_NAME
    for conf_line in $(grep "#define CONFIG_" $ISO_AG_LIB_INSIDE/library/xgpl_src/IsoAgLib/isoaglib_config.h | sed 's/#define \(CONFIG_[a-zA-Z0-9_]*\).*/\1/g') ; do
        conf_name=$(echo_ $conf_line | sed 's/#define \(CONFIG_[a-zA-Z0-9_]*\).*/\1/g')
        INDIV_CNT=$(grep -c $conf_name $CONFIG_NAME.bak)
        if [ "$INDIV_CNT" -lt 1 ] ; then
            grep -B1 "#define $conf_line" $ISO_AG_LIB_INSIDE/library/xgpl_src/IsoAgLib/isoaglib_config.h >> $CONFIG_NAME
            sed "s|#define $conf_name|// #define $conf_name|g" $CONFIG_NAME > $CONFIG_NAME.1
            #     CMDLINE=$(echo_ "sed -e 's|#define $conf_name|// #define $conf_name|g' $CONFIG_NAME > $CONFIG_NAME.1")
            #     echo_ $CMDLINE | sh
            mv $CONFIG_NAME.1 $CONFIG_NAME
            echo_e_n "$ENDLINE" >> $CONFIG_NAME
        fi
    done
    echo_e "$ENDLINE// DONT REMOVE THIS AND THE FOLLOWING LINE AS THEY ARE NEEDED TO DETECT YOUR PERSONAL PROJECT ADAPTATIONS!!!" >> $CONFIG_NAME
    FRESH=$(grep -c "// START_INDIVIDUAL_PROJECT_CONFIG" $CONFIG_NAME.bak)
    if [ "$FRESH" -lt 1 ] ; then
        echo_ "// START_INDIVIDUAL_PROJECT_CONFIG" >> $CONFIG_NAME
    fi
    cat $CONFIG_NAME.bak >> $CONFIG_NAME
    rm -f $CONFIG_NAME.bak

    sed -e 's|^[ \t]*//|//|g' $CONFIG_NAME > $CONFIG_NAME.1
    sed -e 's|^[ \t]*/\*|/\*|g' $CONFIG_NAME.1 > $CONFIG_NAME
    rm -f $CONFIG_NAME.1


    echo_e "$ENDLINE$ENDLINE \section PrjConfig$PROJECT List of configuration settings for $PROJECT" > $CONFIG_HEADER_DOXYGEN_READY
    echo_ " This is only a copy with doxygen ready comment blocks from the original file $CONFIG_NAME " >> $CONFIG_HEADER_DOXYGEN_READY
    echo_ " This header is included by <IsoAgLib/library/xgpl_src/IsoAgLib/isoaglib_config.h> based on the" >> $CONFIG_HEADER_DOXYGEN_READY
    echo_ " project define #define PRJ_USE_AUTOGEN_CONFIG config_$PROJECT.h ( Important: place the directory of the application source files in the include search path )" >> $CONFIG_HEADER_DOXYGEN_READY
    echo_ "" >> $CONFIG_HEADER_DOXYGEN_READY
    echo_ " Use the file $CONF_FILE in the directory $1 as input file for $0 to create the project generation files." >> $CONFIG_HEADER_DOXYGEN_READY
    echo_ "\code" >> $CONFIG_HEADER_DOXYGEN_READY
    cat $CONFIG_NAME >> $CONFIG_HEADER_DOXYGEN_READY
    echo_ "\endcode" >> $CONFIG_HEADER_DOXYGEN_READY

    # go back to directory where config file resides
    cd $1
}

create_makefile()
{
    # go to project dir - below config dir
    DEV_PRJ_DIR="$1/$PROJECT"
    cd $DEV_PRJ_DIR
    mkdir -p "objects_library" "objects_app" "objects_testrunner" "objects_utest"
    MakefileFilelistLibrary="$1/$PROJECT/$FILELIST_LIBRARY_PURE"
    MakefileFilelistLibraryHdr="$1/$PROJECT/$FILELIST_LIBRARY_HDR"

    MakefileFilelistApp="$1/$PROJECT/$FILELIST_APP_PURE"
    MakefileFilelistAppHdr="$1/$PROJECT/$FILELIST_APP_HDR"

    #  MakefileFileListUTest="$1/$PROJECT/$FILELIST_UTEST_PURE"
    #  MakefileFileListUTestMock="$1/$PROJECT/$FILELIST_UTEST_MOCK_PURE"
    #  MakefileFileListUTestMODSUT="$1/$PROJECT/$FILELIST_UTEST_MODSUT_PURE"
    #  MakefileFileListUTestRunner="$1/$PROJECT/$FILELIST_UTEST_RUNNER_PURE"

    MakefileName="Makefile"
    MakefileNameLong="Makefile"'__'"$CAN_SERVER_FILENAME"'__'"$USE_RS232_DRIVER"

    if [ "A$MAKEFILE_SKELETON_FILE" = "A" ] ; then
        MAKEFILE_SKELETON_FILE="$DEV_PRJ_DIR/$ISO_AG_LIB_INSIDE/tools/project_generation/update_makefile_MakefileSkeleton.txt"
    fi
    # create Makefile Header
    echo_ "#############################################################################" > $MakefileNameLong
    echo_ "# Makefile for building: $PROJECT" >> $MakefileNameLong
    echo_ "# Project:               $PROJECT" >> $MakefileNameLong
    echo_ "#############################################################################" >> $MakefileNameLong
    echo_ ""  >> $MakefileNameLong
    echo_ "####### Project specific variables" >> $MakefileNameLong
    echo_ "TARGET = $PROJECT" >> $MakefileNameLong
    echo_ "ISOAGLIB_PATH = $ISO_AG_LIB_INSIDE" >> $MakefileNameLong
    echo_ "INSTALL_PATH = $ISOAGLIB_INSTALL_PATH" >> $MakefileNameLong
    echo_n "APP_INC = " >> $MakefileNameLong
    KDEVELOP_INCLUDE_PATH="$ISO_AG_LIB_INSIDE/library;$ISO_AG_LIB_INSIDE/library/xgpl_src;"
    for EACH_REL_APP_PATH in $REL_APP_PATH ; do
        echo_n "-I$ISO_AG_LIB_INSIDE/$EACH_REL_APP_PATH " >> $MakefileNameLong
        append KDEVELOP_INCLUDE_PATH " $ISO_AG_LIB_INSIDE/$EACH_REL_APP_PATH;"
    done
    for SingleInclPath in $PRJ_INCLUDE_PATH ; do
        echo_n " -I$ISO_AG_LIB_INSIDE/$SingleInclPath" >> $MakefileNameLong
        append KDEVELOP_INCLUDE_PATH " $ISO_AG_LIB_INSIDE/$SingleInclPath;"
    done
    for SingleInclPath in $USE_LINUX_EXTERNAL_INCLUDE_PATH ; do
        echo_n " -I$SingleInclPath" >> $MakefileNameLong
        append KDEVELOP_INCLUDE_PATH " $SingleInclPath;"
    done
    echo_ "" >> $MakefileNameLong
    echo_n "LIBPATH = " >> $MakefileNameLong
    for SingleLibPath in $USE_LINUX_EXTERNAL_LIBRARY_PATH ; do
        echo_n " -L$SingleLibPath" >> $MakefileNameLong
    done
    echo_ "" >> $MakefileNameLong

    echo_n "EXTERNAL_LIBS = " >> $MakefileNameLong
    for SingleLibItem in $USE_LINUX_EXTERNAL_LIBRARIES ; do
        echo_n " $SingleLibItem" >> $MakefileNameLong
    done
    echo_ "" >> $MakefileNameLong

    echo_e "\n####### Include a version definition file into the Makefile context - when this file exists"  >> $MakefileNameLong
    echo_    "-include versions.mk" >> $MakefileNameLong

    echo_ "" >> $MakefileNameLong
    if [ "$USE_RS232_DRIVER" = "rte" -o "$USE_CAN_DEVICE_FOR_SERVER" = "rte" ] ; then
        echo_ "BIOS_LIB = /usr/local/lib/librte_client.a /usr/local/lib/libfevent.a" >> $MakefileNameLong
        # the new RTE library places the headers in /usr/local/include --> no special include paths are needed any more
        echo_n "BIOS_INC =" >> $MakefileNameLong
    fi
    echo_e_n "\nPROJ_DEFINES = \$(VERSION_DEFINES) -D$USE_SYSTEM_DEFINE -DPRJ_USE_AUTOGEN_CONFIG=config_$PROJECT.h" >> $MakefileNameLong
    for SinglePrjDefine in $PRJ_DEFINES ; do
        echo_n " -D$SinglePrjDefine" >> $MakefileNameLong
    done
    if [ $PRJ_SYSTEM_WITH_ENHANCED_CAN_HAL -gt 0 ] ; then
        echo_n " -DSYSTEM_WITH_ENHANCED_CAN_HAL" >> $MakefileNameLong
    fi

    if [ $USE_CAN_DRIVER = "msq_server" ] ; then
        echo_n " -DCAN_DRIVER_MESSAGE_QUEUE" >> $MakefileNameLong
    fi
    if [ $USE_CAN_DRIVER = "socket_server" -o $USE_CAN_DRIVER = "socket_server_hal_simulator" ] ; then
        echo_n " -DCAN_DRIVER_SOCKET" >> $MakefileNameLong
    fi

    echo_e "\n\n####### Definition of compiler binary prefix corresponding to selected target" >> $MakefileNameLong
    if [ "A$PRJ_COMPILER_BINARY_PRE" != "A" ] ; then
        echo_ "COMPILER_BINARY_PRE = \"$PRJ_COMPILER_BINARY_PRE\"" >> $MakefileNameLong

    else
        case $PRJ_DEFINES in
            *SYSTEM_A1*)
                echo_ "COMPILER_BINARY_PRE = /opt/hardhat/devkit/arm/xscale_le/bin/xscale_le-" >> $MakefileNameLong
                echo_ "SYSTEM_A1"
                ;;
            *SYSTEM_MCC*)
                echo_ "COMPILER_BINARY_PRE = /opt/eldk/usr/bin/ppc_6xx-" >> $MakefileNameLong
                echo_ "SYSTEM_MCC"
                ;;
            *)
                echo_ "COMPILER_BINARY_PRE = " >> $MakefileNameLong
                ;;
        esac
    fi


    echo_e "\n\nfirst: all\n" >> $MakefileNameLong
    echo_ "####### Files" >> $MakefileNameLong
    echo_n "SOURCES_LIBRARY = " >> $MakefileNameLong
    FIRST_LOOP="YES"
    for CcFile in $(grep -E "\.cc|\.cpp|\.c" $MakefileFilelistLibrary) ; do
        if [ $FIRST_LOOP != "YES" ] ; then
            echo_e_n '\\' >> $MakefileNameLong
            echo_e_n "\n\t\t" >> $MakefileNameLong
        else
            FIRST_LOOP="NO"
        fi
        echo_e_n "$CcFile  " >> $MakefileNameLong
    done
    echo_e "\n" >> $MakefileNameLong

    echo_n "SOURCES_APP = " >> $MakefileNameLong
    FIRST_LOOP="YES"
    for CcFile in $(grep -E "\.cc|\.cpp|\.c" $MakefileFilelistApp) ; do
        if [ $FIRST_LOOP != "YES" ] ; then
            echo_e_n '\\' >> $MakefileNameLong
            echo_e_n "\n\t\t" >> $MakefileNameLong
        else
            FIRST_LOOP="NO"
        fi
        echo_e_n "$CcFile  " >> $MakefileNameLong
    done
    echo_e "\n" >> $MakefileNameLong

    echo_n "HEADERS_APP = " >> $MakefileNameLong
    FIRST_LOOP="YES"
    for CcFile in $(grep -E "\.h|\.hpp|\.hh" $MakefileFilelistAppHdr) ; do
        if [ $FIRST_LOOP != "YES" ] ; then
            echo_e_n '\\' >> $MakefileNameLong
            echo_e_n "\n\t\t" >> $MakefileNameLong
        else
            FIRST_LOOP="NO"
        fi
        echo_e_n "$CcFile  " >> $MakefileNameLong
    done
    echo_e "\n" >> $MakefileNameLong

    #  echo_n "HEADERS_UTEST = " >> $MakefileNameLong
    #  FIRST_LOOP="YES"
    #  for CcFile in $(grep -E "\.h" $MakefileFileListUTest) ; do
    #    if [ $FIRST_LOOP != "YES" ] ; then
    #      echo_e_n '\\' >> $MakefileNameLong
    #      echo_e_n "\n\t\t" >> $MakefileNameLong
    #    else
    #      FIRST_LOOP="NO"
    #    fi
    #    echo_e_n "$CcFile  " >> $MakefileNameLong
    #  done
    #  echo_e "\n" >> $MakefileNameLong

    #  echo_n "HEADERS_UTEST_MOCKS = " >> $MakefileNameLong
    #  FIRST_LOOP="YES"
    #  for CcFile in $(grep -E "\.h" $MakefileFileListUTestMock) ; do
    #    if [ $FIRST_LOOP != "YES" ] ; then
    #      echo_e_n '\\' >> $MakefileNameLong
    #      echo_e_n "\n\t\t" >> $MakefileNameLong
    #    else
    #      FIRST_LOOP="NO"
    #    fi
    #    echo_e_n "$CcFile  " >> $MakefileNameLong
    #  done
    #  echo_e "\n" >> $MakefileNameLong

    #  echo_n "HEADERS_UTEST_MOD_SUT = " >> $MakefileNameLong
    #  FIRST_LOOP="YES"
    #  for CcFile in $(grep -E "\.h" $MakefileFileListUTestMODSUT) ; do
    #    if [ $FIRST_LOOP != "YES" ] ; then
    #      echo_e_n '\\' >> $MakefileNameLong
    #      echo_e_n "\n\t\t" >> $MakefileNameLong
    #    else
    #      FIRST_LOOP="NO"
    #    fi
    #    echo_e_n "$CcFile  " >> $MakefileNameLong
    #  done
    #  echo_e "\n" >> $MakefileNameLong

    #  echo_n "TESTRUNNER_SOURCES = " >> $MakefileNameLong
    #  FIRST_LOOP="YES"
    #  for CcFile in $(grep -E "\.cpp" $MakefileFileListUTestRunner) ; do
    #    if [ $FIRST_LOOP != "YES" ] ; then
    #      echo_e_n '\\' >> $MakefileNameLong
    #      echo_e_n "\n\t\t" >> $MakefileNameLong
    #    else
    #      FIRST_LOOP="NO"
    #    fi
    #    echo_e_n "$CcFile  " >> $MakefileNameLong
    #  done
    #  echo_e "\n" >> $MakefileNameLong


    ##### Library install header file gathering BEGIN

    rm -f FileListInterfaceStart.txt FileListInterface.txt FileListInterface4Eval.txt FileListInternal.txt FileListInterface4EvalPre.txt FileListInterface4EvalPre.*.txt

    grep    "/impl/" <"$MakefileFilelistLibraryHdr" > FileListInternal.txt
    grep    "/hal/"  <"$MakefileFilelistLibraryHdr" >> FileListInternal.txt
    grep -v "/impl/" <"$MakefileFilelistLibraryHdr" | grep -v /hal/ | grep -v ".cpp" > FileListInterface.txt

# it's a good idea to get the several typedef.h headers to the install set
    grep typedef.h FileListInternal.txt >> FileListInterface.txt
    grep -E "driver/*/i*.h" FileListInternal.txt >> FileListInterface.txt

# special exception to enable ISO-Request-PGN implementation in app scope
    grep -E "isorequestpgn_c.h" FileListInternal.txt >> FileListInterface.txt


    cp -a FileListInterface.txt FileListInterfaceStart.txt
    cp -a FileListInterface.txt FileListInterface4Eval.txt

    DoRepeat="TRUE";
    Iteration=0
    while [ $DoRepeat = "TRUE" ] ; do
        DoRepeat="FALSE";
        for InterfaceFile in $(cat FileListInterface4Eval.txt); do
            for IncludeLine in $(grep "#include" $InterfaceFile | sed -e 's/.*#include[ \t\<\"]*\([^\>\"\t ]*\).*/\1/g'); do
                #BaseName=$(basename $IncludeLine);
                BaseName=$(echo_ $IncludeLine | sed -e 's#.*xgpl_src/IsoAgLib/\(.*\)#\1#g' | sed -e 's#\.\./##g')
                CntHeader=$(echo_ $BaseName | grep -c '\.h');
                #echo_ "CntHeader $CntHeader";
                if [ $CntHeader -gt 0 ] ; then
                    # echo_ "Include Line fuer $IncludeLine mit Datei $BaseName";
                    CntInterface=$(grep -c "/$BaseName" FileListInterface.txt)
                    CntInternal=$(grep -c "/$BaseName" FileListInternal.txt)

                    #echo_ "Include Datei $BaseName kommt $CntInterface mal im Interface und $CntInternal mal intern vor";
                    if test $CntInterface -lt 1 -a $CntInternal -gt 0 ; then
                        #echo_ "Header $BaseName existiert noch nicht in Interface --> hinzufgen"
                        grep $BaseName FileListInternal.txt >> FileListInterface.txt
                        grep $BaseName FileListInternal.txt >> FileListInterface4EvalPre.txt
                        DoRepeat="TRUE";
                    fi
                fi
            done
        done
        if [ $DoRepeat = "TRUE" ] ; then
            Iteration=$(expr $Iteration + 1)
            cp -a FileListInterface4EvalPre.txt FileListInterface4EvalPre.$Iteration.txt
            cp -a FileListInterface4EvalPre.txt FileListInterface4Eval.txt
            rm -f FileListInterface4EvalPre.txt
        fi
    done
    echo_n "INSTALL_FILES_LIBRARY = " >> $MakefileNameLong
    FIRST_LOOP="YES"
    for InterfaceFile in $(cat FileListInterface.txt) ; do
        if [ $FIRST_LOOP != "YES" ] ; then
            echo_e_n ' \\' >> $MakefileNameLong
            echo_e_n "\n\t\t" >> $MakefileNameLong
        else
            FIRST_LOOP="NO"
        fi
        echo_e_n "$InterfaceFile" >> $MakefileNameLong
    done
    echo_e "\n" >> $MakefileNameLong

    rm -f FileListInterfaceStart.txt FileListInterface.txt FileListInterface4Eval.txt FileListInternal.txt FileListInterface4EvalPre.txt FileListInterface4EvalPre.*.txt

    ##### Library install header file gathering END

    cat $MAKEFILE_SKELETON_FILE >> $MakefileNameLong

    # NO UTESTs, no need to remove any testrunners
    #  # remove testrunner for A1
    #   case $PRJ_DEFINES in
    #       *SYSTEM_A1*)
    #       sed -e 's#all: \(.*\) testrunner\(.*\)#all: \1\2#g'  $MakefileNameLong > $MakefileNameLong.1
    #       mv $MakefileNameLong.1 $MakefileNameLong
    #       ;;
    #       *SYSTEM_A5*)
    #       sed -e 's#all: \(.*\) testrunner\(.*\)#all: \1\2#g'  $MakefileNameLong > $MakefileNameLong.1
    #       mv $MakefileNameLong.1 $MakefileNameLong
    #       ;;
    #   esac

    # add can_server creation to target "all"
    if [ $USE_CAN_DRIVER = "msq_server" -o $USE_CAN_DRIVER = "socket_server" -o $USE_CAN_DRIVER = "socket_server_hal_simulator" ] ; then
        cp $MakefileNameLong $MakefileNameLong.1
        sed -e 's#LFLAGS   =#LFLAGS   = -pthread#g' $MakefileNameLong.1 > $MakefileNameLong
    fi

    rm -f $MakefileNameLong.1

    # replace the install rules for version.h and the app config file
    sed -e "s#_PROJECT_CONFIG_REPLACE_#$CONFIG_NAME#g"  $MakefileNameLong > $MakefileNameLong.1
    sed -e "s#_PROJECT_VERSION_REPLACE_#$VERSION_FILE_NAME#g" $MakefileNameLong.1 > $MakefileNameLong
    rm -f $MakefileNameLong.1

    # replace any path items like "Bla/foo/../Blu" --> "Bla/Blu"
    while [ $(grep -c -e '/[0-9a-zA-Z_+\-]\+/\.\./' $MakefileNameLong) -gt 0 ] ; do
        sed -e 's|/[0-9a-zA-Z_+\-]\+/\.\./|/|g' $MakefileNameLong > $MakefileNameLong.1
        mv $MakefileNameLong.1 $MakefileNameLong
    done

    # create a symbolic link to get this individual MakefileNameLong referred as "Makefile"
    rm -f "Makefile"
    ln -s $MakefileNameLong "Makefile"

    # now create pure application makefile which is based upon an installed library
    MakefileName="MakefileApp"
    MakefileNameLong="MakefileApp"'__'"$CAN_SERVER_FILENAME"'__'"$USE_RS232_DRIVER"

    if [ "A$MAKEFILE_APP_SKELETON_FILE" = "A" ] ; then
        MAKEFILE_APP_SKELETON_FILE="$DEV_PRJ_DIR/$ISO_AG_LIB_INSIDE/tools/project_generation/update_makefile_MakefileAppSkeleton.txt"
    fi


    # create Makefile Header
    echo_ "#############################################################################" > $MakefileNameLong
    echo_ "# Makefile for building: $PROJECT" >> $MakefileNameLong
    echo_ "# Project:               $PROJECT" >> $MakefileNameLong
    echo_ "#############################################################################" >> $MakefileNameLong
    echo_ ""  >> $MakefileNameLong
    echo_ "####### Project specific variables" >> $MakefileNameLong
    echo_ "TARGET = $PROJECT" >> $MakefileNameLong
    echo_ "ISOAGLIB_INSTALL_PATH = $ISOAGLIB_INSTALL_PATH" >> $MakefileNameLong
    echo_n "APP_INC = " >> $MakefileNameLong
    KDEVELOP_INCLUDE_PATH="$ISO_AG_LIB_INSIDE/library;$ISO_AG_LIB_INSIDE/library/xgpl_src;"
    for EACH_REL_APP_PATH in $REL_APP_PATH ; do
        echo_n "-I$ISO_AG_LIB_INSIDE/$EACH_REL_APP_PATH " >> $MakefileNameLong
        append KDEVELOP_INCLUDE_PATH " $ISO_AG_LIB_INSIDE/$EACH_REL_APP_PATH;"
    done
    for SingleInclPath in $PRJ_INCLUDE_PATH ; do
        echo_n " -I$ISO_AG_LIB_INSIDE/$SingleInclPath" >> $MakefileNameLong
        append KDEVELOP_INCLUDE_PATH " $ISO_AG_LIB_INSIDE/$SingleInclPath;"
    done
    for SingleInclPath in $USE_LINUX_EXTERNAL_INCLUDE_PATH ; do
        echo_n " -I$SingleInclPath" >> $MakefileNameLong
        append KDEVELOP_INCLUDE_PATH " $SingleInclPath;"
    done
    echo_ "" >> $MakefileNameLong
    echo_n "LIBPATH = " >> $MakefileNameLong
    for SingleLibPath in $USE_LINUX_EXTERNAL_LIBRARY_PATH ; do
        echo_n " -L$SingleLibPath" >> $MakefileNameLong
    done
    echo_ "" >> $MakefileNameLong

    echo_n "EXTERNAL_LIBS = " >> $MakefileNameLong
    for SingleLibItem in $USE_LINUX_EXTERNAL_LIBRARIES ; do
        echo_n " $SingleLibItem" >> $MakefileNameLong
    done
    echo_ "" >> $MakefileNameLong

    echo_e "\n####### Include a version definition file into the Makefile context - when this file exists"  >> $MakefileNameLong
    echo_    "-include versions.mk" >> $MakefileNameLong


    echo_ "" >> $MakefileNameLong

    echo_e_n "\nPROJ_DEFINES = \$(VERSION_DEFINES) -D$USE_SYSTEM_DEFINE -DPRJ_USE_AUTOGEN_CONFIG=config_$PROJECT.h" >> $MakefileNameLong
    for SinglePrjDefine in $PRJ_DEFINES ; do
        echo_n " -D$SinglePrjDefine" >> $MakefileNameLong
    done
    if [ $PRJ_SYSTEM_WITH_ENHANCED_CAN_HAL -gt 0 ] ; then
        echo_n " -DSYSTEM_WITH_ENHANCED_CAN_HAL" >> $MakefileNameLong
    fi

    echo_e "\n\n####### Definition of compiler binary prefix corresponding to selected target" >> $MakefileNameLong
    if [ "A$PRJ_COMPILER_BINARY_PRE" != "A" ] ; then
        echo_ "COMPILER_BINARY_PRE = \"$PRJ_COMPILER_BINARY_PRE\"" >> $MakefileNameLong

    else
        case $PRJ_DEFINES in
            *SYSTEM_A1*)
                echo_ "COMPILER_BINARY_PRE = /opt/hardhat/devkit/arm/xscale_le/bin/xscale_le-" >> $MakefileNameLong
                echo_ "SYSTEM_A1"
                ;;
            *SYSTEM_MCC*)
                echo_ "COMPILER_BINARY_PRE = /opt/eldk/usr/bin/ppc_6xx-" >> $MakefileNameLong
                echo_ "SYSTEM_MCC"
                ;;
            *)
                echo_ "COMPILER_BINARY_PRE = " >> $MakefileNameLong
                ;;
        esac
    fi

    echo_e "\n\nfirst: all\n" >> $MakefileNameLong
    echo_ "####### Files" >> $MakefileNameLong
    echo_n "SOURCES_APP = " >> $MakefileNameLong
    FIRST_LOOP="YES"
    for CcFile in $(grep -E "\.cc|\.cpp|\.c" $MakefileFilelistApp) ; do
        if [ $FIRST_LOOP != "YES" ] ; then
            echo_e_n '\\' >> $MakefileNameLong
            echo_e_n "\n\t\t" >> $MakefileNameLong
        else
            FIRST_LOOP="NO"
        fi
        echo_e_n "$CcFile  " >> $MakefileNameLong
    done
    echo_e "\n" >> $MakefileNameLong

    cat $MAKEFILE_APP_SKELETON_FILE >> $MakefileNameLong

    # add can_server creation to target "all"
    if [ $USE_CAN_DRIVER = "msq_server" ] ; then
        cp $MakefileNameLong $MakefileNameLong.1
        sed -e 's#LFLAGS   =#LFLAGS   = -pthread#g' $MakefileNameLong.1 > $MakefileNameLong
    fi
    rm -f $MakefileNameLong.1

    # replace any path items like "Bla/foo/../Blu" --> "Bla/Blu"
    while [ $(grep -c -e '/[0-9a-zA-Z_+\-]\+/\.\./' $MakefileNameLong) -gt 0 ] ; do
        sed -e 's|/[0-9a-zA-Z_+\-]\+/\.\./|/|g' $MakefileNameLong > $MakefileNameLong.1
        mv $MakefileNameLong.1 $MakefileNameLong
    done

    # create a symbolic link to get this individual MakefileNameLong referred as "Makefile"
    rm -f "MakefileApp"
    ln -s $MakefileNameLong "MakefileApp"


    # now create a Kdevelop3 project file
    cp -a $DEV_PRJ_DIR/$ISO_AG_LIB_INSIDE/tools/project_generation/update_makefile_kdevelop3Generic.kdevelop $PROJECT.kdevelop

    sed -e "s/REPLACE_AUTHOR/$PROJECT_AUTHOR/g" $PROJECT.kdevelop > $PROJECT.kdevelop.1
    sed -e "s/REPLACE_AUTHOR_EMAIL/$PROJECT_AUTHOR_EMAIL/g" $PROJECT.kdevelop.1 > $PROJECT.kdevelop
    sed -e "s/REPLACE_PROJECT/$PROJECT/g" $PROJECT.kdevelop > $PROJECT.kdevelop.1
    sed -e "s#REPLACE_INCLUDE#$KDEVELOP_INCLUDE_PATH#g" $PROJECT.kdevelop.1 > $PROJECT.kdevelop
    # mv $PROJECT.kdevelop.1 $PROJECT.kdevelop
    rm -f $PROJECT.kdevelop.1

    echo_ "# KDevelop Custom Project File List" > $PROJECT.kdevelop.filelist
    cat filelist__$PROJECT.txt >> $PROJECT.kdevelop.filelist


    cd $1

    return
}

# Generate suitable project file for the Win32 IDE "Dev-C++"
# which is OpenSource and is based on MinGW - i.e. gcc for Win32
# URL http://www.bloodshed.net/dev/devcpp.html
create_DevCCPrj()
{
    DEV_PRJ_DIR="$1$PROJECT"

    # echo_ "Create Projekt file for Dev-C++ in $DEV_PRJ_DIR"


    mkdir -p $DEV_PRJ_DIR/objects
    cd $DEV_PRJ_DIR
    # remove some file lists, which are not used for Dev-C++
    rm -f $FILELIST_LIBRARY_PURE $FILELIST_APP_PURE

    # org test
    PROJECT_FILE_NAME="$PROJECT"'__'"$CAN_SERVER_FILENAME"'__'"$USE_RS232_DRIVER.dev"
    DevCcPrjFilelist="$1/$PROJECT/$FILELIST_COMBINED_PURE"

    # echo_ "Erzeuge $PROJECT_FILE_NAME"
    PROJECT_EXE_NAME="$PROJECT"'__'"$CAN_SERVER_FILENAME"'__'"$USE_RS232_DRIVER.exe"
    CONFIG_HDR_NAME="config_""$PROJECT.h"

    UNIT_CNT_CC=$(grep -c -E "\.cc|\.cpp|\.c" $DevCcPrjFilelist)
    UNIT_CNT_HDR=$(grep -c -E "\.h|\.hpp" $DevCcPrjFilelist)
    UNIT_CNT=$(expr $UNIT_CNT_CC + $UNIT_CNT_HDR)


    echo_ "[Project]" > $PROJECT_FILE_NAME
    echo_ "FileName=$PROJECT_FILE_NAME" >> $PROJECT_FILE_NAME
    echo_ "Name=$PROJECT" >> $PROJECT_FILE_NAME
    echo_ "UnitCount=$UNIT_CNT" >> $PROJECT_FILE_NAME


    cat <<-ENDOFHEADERA >> $PROJECT_FILE_NAME
    Type=1
    Ver=1
    ObjFiles=
ENDOFHEADERA

    ISO_AG_LIB_PATH_WIN=$(echo_ "$ISO_AG_LIB_INSIDE" | sed -e 's#\\#=_=_#g' -e 's#/#=_=_#g')

    DEFINE_LINE='-D'"$USE_SYSTEM_DEFINE"'_@@_-DPRJ_USE_AUTOGEN_CONFIG='"$CONFIG_HDR_NAME"'_@@_'
    INCLUDE_DIR_LINE="$ISO_AG_LIB_PATH_WIN=_=_library;$ISO_AG_LIB_PATH_WIN=_=_library=_=_xgpl_src"

    LIB_DIR_LINE=""
    LIB_FILE_LINE="-lwinmm_@@_-lws2_32_@@_"


    USE_WIN32_EXTERNAL_INCLUDE_PATH=$(echo_ "$USE_WIN32_EXTERNAL_INCLUDE_PATH" | sed -e 's#\\#=_=_#g' -e 's#/#=_=_#g')

    USE_WIN32_EXTERNAL_LIBRARY_PATH=$(echo_ "$USE_WIN32_EXTERNAL_LIBRARY_PATH" | sed -e 's#\\#=_=_#g' -e 's#/#=_=_#g')

    USE_DEVCPP_EXTERNAL_LIBRARIES=$(echo_ "$USE_DEVCPP_EXTERNAL_LIBRARIES" | sed -e 's#\\#=_=_#g' -e 's#/#=_=_#g')

    REL_APP_PATH_WIN=$(echo_ "$REL_APP_PATH" | sed -e 's#\\#=_=_#g' -e 's#/#=_=_#g')

    PRJ_INCLUDE_PATH_WIN=$(echo_ "$PRJ_INCLUDE_PATH" | sed -e 's#\\#=_=_#g' -e 's#/#=_=_#g')

    for EACH_REL_APP_PATH in $REL_APP_PATH_WIN ; do
        append INCLUDE_DIR_LINE ";$ISO_AG_LIB_PATH_WIN=_=_$EACH_REL_APP_PATH"
    done
    for SingleInclPath in $PRJ_INCLUDE_PATH_WIN ; do
        append INCLUDE_DIR_LINE ";$ISO_AG_LIB_PATH_WIN=_=_$SingleInclPath"
    done
    for SingleInclPath in $USE_WIN32_EXTERNAL_INCLUDE_PATH ; do
        append INCLUDE_DIR_LINE ";$SingleInclPath"
    done
    for SingleLibItem in $USE_DEVCPP_EXTERNAL_LIBRARIES ; do
        append LIB_FILE_LINE "$SingleLibItem"'_@@_'
    done
    for SingleLibPath in $USE_WIN32_EXTERNAL_LIBRARY_PATH ; do
        if [ "M$LIB_DIR_LINE" != "M" ] ; then
            append LIB_DIR_LINE ";$SingleLibPath"
        else
            LIB_DIR_LINE="$SingleLibPath"
        fi
    done


    if  [ $USE_CAN_DRIVER = "socket_server" -o $USE_CAN_DRIVER = "socket_server_hal_simulator" ] ; then
        append DEFINE_LINE ""'-D__GNUWIN32___@@_-W_@@_-DWIN32_@@_-D_CONSOLE_@@_-D_MBCS_@@_-D_Windows_@@_-DCAN_DRIVER_SOCKET_@@_-DSYSTEM_WITH_ENHANCED_CAN_HAL_@@_'
    fi

    echo_n "Includes=$INCLUDE_DIR_LINE" >> $PROJECT_FILE_NAME
    echo_ "" >> $PROJECT_FILE_NAME

    echo_ "Libs=$LIB_DIR_LINE" >> $PROJECT_FILE_NAME
    echo_ "Linker=$LIB_FILE_LINE" >> $PROJECT_FILE_NAME

    cat <<-ENDOFHEADERA >> $PROJECT_FILE_NAME
    PrivateResource=
    ResourceIncludes=
    MakeIncludes=
ENDOFHEADERA



    for SinglePrjDefine in $PRJ_DEFINES ; do
        append DEFINE_LINE ""'-D'"$SinglePrjDefine"'_@@_'
    done
    if [ $PRJ_SYSTEM_WITH_ENHANCED_CAN_HAL -gt 0 ] ; then
        append DEFINE_LINE ""'-DSYSTEM_WITH_ENHANCED_CAN_HAL_@@_'
    fi

    echo_ "Compiler=$DEFINE_LINE" >> $PROJECT_FILE_NAME
    echo_ "CppCompiler=$DEFINE_LINE" >> $PROJECT_FILE_NAME

    while [ $(grep -c -e '=_=_[0-9a-zA-Z_+\-]\+=_=_\.\.=_=_' $PROJECT_FILE_NAME) -gt 0 ] ; do
        sed -e 's|=_=_[0-9a-zA-Z_+\-]\+=_=_\.\.=_=_|=_=_|g' $PROJECT_FILE_NAME > $PROJECT_FILE_NAME.1
        mv $PROJECT_FILE_NAME.1 $PROJECT_FILE_NAME
    done

    sed -e 's#=_=_#\\#g'  $PROJECT_FILE_NAME > $PROJECT_FILE_NAME.1
    mv $PROJECT_FILE_NAME.1 $PROJECT_FILE_NAME


    cat <<-ENDOFHEADERB >> $PROJECT_FILE_NAME
    IsCpp=1
    Icon=
    ExeOutput=
    ObjectOutput=objects
    OverrideOutput=0
ENDOFHEADERB

    echo_ "OverrideOutputName=$PROJECT_EXE_NAME" >> $PROJECT_FILE_NAME


    cat <<-ENDOFHEADERC >> $PROJECT_FILE_NAME
    HostApplication=
    Folders=
    CommandLine=
    UseCustomMakefile=0
    CustomMakefile=
    IncludeVersionInfo=0
    SupportXPThemes=0
    CompilerSet=0
    CompilerSettings=0000000000000000000000

    [VersionInfo]
    Major=0
    Minor=1
    Release=1
    Build=1
    LanguageID=1033
    CharsetID=1252
    CompanyName=
    FileVersion=
    FileDescription=Developed using the Dev-C++ IDE
    InternalName=
    LegalCopyright=
    LegalTrademarks=
    OriginalFilename=
    ProductName=
    ProductVersion=
    AutoIncBuildNr=0

ENDOFHEADERC

    SOURCES=$(cat $DevCcPrjFilelist)
    unit_ind=0
    for i in $SOURCES
    do
        if [ $i = "" ] ; then
            continue
        fi
        unit_ind=$(expr $unit_ind + 1)
        echo_ "[Unit$unit_ind]" >> $PROJECT_FILE_NAME
        echo_ "FileName=$i" >> $PROJECT_FILE_NAME
        echo_ "CompileCpp=1" >> $PROJECT_FILE_NAME
        echo_ "Folder=$PROJECT" >> $PROJECT_FILE_NAME
        echo_ "Compile=1" >> $PROJECT_FILE_NAME
        echo_ "Link=1" >> $PROJECT_FILE_NAME
        echo_ "Priority=1000" >> $PROJECT_FILE_NAME
        echo_ "OverrideBuildCmd=0" >> $PROJECT_FILE_NAME
        echo_ "BuildCmd=" >> $PROJECT_FILE_NAME
        echo_ "" >> $PROJECT_FILE_NAME
    done

    cd $1
}

unix_lines_to_windows_lines()
{
    local FROM_FILE=$1
    local TO_FILE=$2
    # New versions of SED understand \r directly, but older ones (e.g.
    # the current one of MSYS) don't. With printf it's OK for both:
    sed -e "$(printf 's|\r||g;s|$|\r|')" <"$FROM_FILE" >"$TO_FILE"
}

create_EdePrj()
{
    DEV_PRJ_DIR="$1/$PROJECT"
    # echo_ "Create Projekt file for EDE in $DEV_PRJ_DIR"
    mkdir -p $DEV_PRJ_DIR/Template.dir
    PROJECT_FILE_NAME="$PROJECT"'_'"$USE_TARGET_SYSTEM.pjt"
    EdePrjFilelist="$1/$PROJECT/$FILELIST_COMBINED_PURE"
    CONFIG_HDR_NAME="config_""$PROJECT.h"


    if [ $USE_EMBED_LIB_DIRECTORY = "library/commercial_BIOS/bios_esx" ] ; then
        ## adopt the BIOS file, if $USE_EMBED_LIB_DIRECTORY and
        ## and $USE_EMBED_HEADER_DIRECTORY reflect default value which doesn't match to defined target
        case "$USE_TARGET_SYSTEM" in
            esx)
                ;;
            pc*)
                ;;
            *)
                USE_EMBED_LIB_DIRECTORY="library/commercial_BIOS/bios_$USE_TARGET_SYSTEM"
                USE_EMBED_HEADER_DIRECTORY="library/commercial_BIOS/bios_$USE_TARGET_SYSTEM"
                ;;
        esac
    fi

    if [ "M$USE_EMBED_LIBS" = "M" ] ; then
       # no setting in the config file -> derive based on target
        case "$USE_TARGET_SYSTEM" in
            c2c) USE_EMBED_LIBS="c2c10l.lib" ;;
            Dj1) USE_EMBED_LIBS="djbiosmvt.lib" ;;
            esx) USE_EMBED_LIBS="C756/Xos20l.lib Module/Xma20l.lib" ;;
            esxu) USE_EMBED_LIBS="Mos10l.lib" ;;
            imi) USE_EMBED_LIBS="adis10l.lib" ;;
            pm167) USE_EMBED_LIBS="mios1s.lib" ;;
        esac
    fi
    if [ "M$USE_EMBED_BIOS_SRC" = "M" ] ; then
        # no setting in the config file -> derive based on target
        case "$USE_TARGET_SYSTEM" in
            c2c) USE_EMBED_BIOS_SRC="c2c10cs.asm c2c10err.c  c2c10err.h c2c10osy.h" ;;
            Dj1) USE_EMBED_BIOS_SRC="DjBiosMVT.h" ;;
            esx) USE_EMBED_BIOS_SRC="Xos20go.asm Xos20err.c xos20esx.h XOS20EEC.H XOS20EEC.OBJ" ;;
            esxu) USE_EMBED_BIOS_SRC="MOS10ERR.C  MOS10ERR.H  MOS10GO.ASM MOS10OSY.H" ;;
            imi) USE_EMBED_BIOS_SRC="adis10go_cs.asm adis10.h Xos20eec.h XOS20EEC.OBJ" ;;
            pm167) USE_EMBED_BIOS_SRC="mios1.h mx1_0go.asm Xos20eec.h  XOS20EEC.OBJ Xos20err.c  Xos20err.h" ;;
        esac
    fi
    if [ "M$USE_EMBED_ILO" = "M" ] ; then
        # no setting in the config file -> derive based on target
        case "$USE_TARGET_SYSTEM" in
            c2c) USE_EMBED_ILO="c2c10osy.ilo" ;;
            Dj1) USE_EMBED_ILO="MiniVT.ilo" ;;
            esx) USE_EMBED_ILO="Xos20lcs.ilo" ;;
            esxu) USE_EMBED_ILO="MOS10L.ILO" ;;
            imi) USE_EMBED_ILO="adis10s_cs.ilo" ;;
            pm167) USE_EMBED_ILO="mx1_0s.ilo" ;;
        esac
    fi


    for EACH_REL_APP_PATH in $REL_APP_PATH ; do
        if [ "M$USE_APP_PATH" = "M" ] ; then
            USE_APP_PATH=$(echo_ "$ISO_AG_LIB_INSIDE/$EACH_REL_APP_PATH" | sed -e 's|/[0-9a-zA-Z_+\-]+/\.\.||g' -e 's|\\[0-9a-zA-Z_+\-]+\\\.\.||g')
        else
            append USE_APP_PATH ""$(echo_ ";$ISO_AG_LIB_INSIDE/$EACH_REL_APP_PATH" | sed -e 's|/[0-9a-zA-Z_+\-]+/\.\.||g' -e 's|\\[0-9a-zA-Z_+\-]+\\\.\.||g')
        fi
    done
    echo_ "USE_APP_PATH: $USE_APP_PATH"

    INCLUDE_APP_PATH_TASKING="$ISO_AG_LIB_INSIDE/$USE_EMBED_HEADER_DIRECTORY"
    for SingleInclPath in $PRJ_INCLUDE_PATH ; do
        append INCLUDE_APP_PATH_TASKING ";$SingleInclPath"
    done

    USE_EMBED_HEADER_DIRECTORY=$(echo_ "$ISO_AG_LIB_INSIDE/$USE_EMBED_HEADER_DIRECTORY" | sed -e 's/\/[0-9a-zA-Z_+\-]+\/\.\.//g' -e 's|\\[0-9a-zA-Z_+\-]+\\\.\.||g')
    USE_EMBED_LIB_DIRECTORY=$(echo_ "$ISO_AG_LIB_INSIDE/$USE_EMBED_LIB_DIRECTORY" | sed -e 's/\/[0-9a-zA-Z_+\-]+\/\.\.//g' -e 's|\\[0-9a-zA-Z_+\-]+\\\.\.||g')


    USE_EMBED_ILO=$(echo_ "$USE_EMBED_LIB_DIRECTORY/$USE_EMBED_ILO" | sed -e 's/\/[0-9a-zA-Z_+\-]+\/\.\.//g' -e 's|\\[0-9a-zA-Z_+\-]+\\\.\.||g')

    USE_DEFINES="__TSW_CPP_756__,$USE_SYSTEM_DEFINE,PRJ_USE_AUTOGEN_CONFIG=$CONFIG_HDR_NAME"

    for SinglePrjDefine in $PRJ_DEFINES ; do
        append USE_DEFINES ""','"$SinglePrjDefine"
    done


    USE_TARGET_LIB_LINE=""
    for LIBRARY in $USE_EMBED_LIBS ; do
        append USE_TARGET_LIB_LINE " -Wo $USE_EMBED_LIB_DIRECTORY/$LIBRARY"
    done
    # echo_ "Library line: $USE_TARGET_LIB_LINE"
    USE_TARGET_LIB_LINE=$(echo_ "$USE_TARGET_LIB_LINE" | sed -e 's/\/[0-9a-zA-Z_+\-]+\/\.\.//g' -e 's|\\[0-9a-zA-Z_+\-]+\\\.\.||g')

    # avoid UNIX style directory seperator "/" as it can disturb Tasking during the link process ( during compile, everything runs fine with UNIX style - WMK seems to have problems with it durign link and hex gen )
    ISO_AG_LIB_PATH_WIN=$(echo_ "$ISO_AG_LIB_INSIDE" | sed -e 's#\\#=_=_#g' -e 's#/#=_=_#g')
    USE_EMBED_LIB_DIRECTORY=$(echo_ "$USE_EMBED_LIB_DIRECTORY" | sed -e 's#\\#=_=_#g' -e 's#/#=_=_#g')
    USE_EMBED_HEADER_DIRECTORY=$(echo_ "$USE_EMBED_HEADER_DIRECTORY" | sed -e 's#\\#=_=_#g' -e 's#/#=_=_#g')
    USE_TARGET_LIB_LINE=$(echo_ "$USE_TARGET_LIB_LINE" | sed -e 's#\\#=_=_#g' -e 's#/#=_=_#g')
    USE_EMBED_ILO=$(echo_ "$USE_EMBED_ILO" | sed -e 's#\\#=_=_#g' -e 's#/#=_=_#g')
    USE_APP_PATH=$(echo_ "$USE_APP_PATH" | sed -e 's#\\#=_=_#g' -e 's#/#=_=_#g')
    DEV_PRJ_DIR_WIN=$(echo_ "$DEV_PRJ_DIR" | sed -e 's#\\#=_=_#g' -e 's#/#=_=_#g')
    USE_DEFINES=$(echo_ "$USE_DEFINES" | sed -e 's#\\#=_=_#g' -e 's#/#=_=_#g')
    USE_EMBED_COMPILER_DIR=$(echo_ "$USE_EMBED_COMPILER_DIR" | sed -e 's#\\#=_=_#g' -e 's#/#=_=_#g')

    # remove some file lists, which are not used for Dev-C++
    rm -f "$1/$PROJECT/$FILELIST_LIBRARY_PURE" "$1/$PROJECT/$FILELIST_APP_PURE"


    # Build Tasking Project File by: a) first stub part; b) file list c) second stub part
    cp -a $DEV_PRJ_DIR/$ISO_AG_LIB_INSIDE/tools/project_generation/update_makefile_EDE.part1.pjt $DEV_PRJ_DIR/$PROJECT_FILE_NAME

    sed -e 's|\\\\|=_=_|g' -e 's|/|=_=_|g' $EdePrjFilelist > $EdePrjFilelist.1

    mv $EdePrjFilelist.1 $EdePrjFilelist
    cat $EdePrjFilelist >> $DEV_PRJ_DIR/$PROJECT_FILE_NAME
    rm -f $EdePrjFilelist.1

    # insert specific BIOS/OS sources
    for BiosSrc in $USE_EMBED_BIOS_SRC ; do
        echo_ "$USE_EMBED_LIB_DIRECTORY/$BiosSrc" >> $DEV_PRJ_DIR/$PROJECT_FILE_NAME
    done


    cat $DEV_PRJ_DIR/$ISO_AG_LIB_INSIDE/tools/project_generation/update_makefile_EDE.part2.pjt >> $DEV_PRJ_DIR/$PROJECT_FILE_NAME
    cd $DEV_PRJ_DIR
    sed -e "s#INSERT_PROJECT#$PROJECT#g" -e "s#INSERT_TARGET_LIB_DIRECTORY#$USE_EMBED_LIB_DIRECTORY#g" $PROJECT_FILE_NAME > $PROJECT_FILE_NAME.1
    sed -e "s#INSERT_ISO_AG_LIB_PATH#$ISO_AG_LIB_PATH_WIN#g" -e "s#INSERT_TARGET_HEADER_DIRECTORY#$USE_EMBED_HEADER_DIRECTORY#g" $PROJECT_FILE_NAME.1 > $PROJECT_FILE_NAME

    sed -e "s#INSERT_TARGET_LIB#$USE_TARGET_LIB_LINE#g" -e "s#INSERT_TARGET_ILO#$USE_EMBED_ILO#g" $PROJECT_FILE_NAME > $PROJECT_FILE_NAME.1

    sed -e "s#INSERT_APP_PATH#$USE_APP_PATH#g" -e "s#INSERT_PRJ_PATH#$DEV_PRJ_DIR_WIN#g" -e "s#INSERT_DEFINES#$USE_DEFINES#g" -e "s#INSERT_EMBED_COMPILER_DIR#$USE_EMBED_COMPILER_DIR#g" $PROJECT_FILE_NAME.1 > $PROJECT_FILE_NAME

    rm -f $PROJECT_FILE_NAME.1

    while [ $(grep -c -e '=_=_[0-9a-zA-Z_+\-]\+=_=_\.\.=_=_' $PROJECT_FILE_NAME) -gt 0 ] ; do
        sed -e 's|=_=_[0-9a-zA-Z_+\-]\+=_=_\.\.=_=_|=_=_|g' $PROJECT_FILE_NAME > $PROJECT_FILE_NAME.1
        mv $PROJECT_FILE_NAME.1 $PROJECT_FILE_NAME
    done
    while [ $(grep -c -e '=_=_[0-9a-zA-Z_+\-]\+=_=_\.\.=_=_' $EdePrjFilelist) -gt 0 ] ; do
        sed -e 's|=_=_[0-9a-zA-Z_+\-]\+=_=_\.\.=_=_|=_=_|g' $EdePrjFilelist > $EdePrjFilelist.1
        mv $EdePrjFilelist.1 $EdePrjFilelist
    done

    sed -e 's#=_=_#\\#g'  $PROJECT_FILE_NAME > $PROJECT_FILE_NAME.1
    sed -e 's#=_=_#\\#g'  $EdePrjFilelist > $EdePrjFilelist.1
    mv $EdePrjFilelist.1 $EdePrjFilelist

    # now set the target CPU if this varies from default CpuC167CS
    ## also adopt the BIOS file, if $USE_EMBED_LIB_DIRECTORY and
    ## and $USE_EMBED_HEADER_DIRECTORY reflect default value which doesn't match to defined target
    case "$USE_TARGET_SYSTEM" in
        Dj1)
            sed -e 's#CpuC167CS#CpuC167CR#g' $PROJECT_FILE_NAME.1 > $PROJECT_FILE_NAME
            mv $PROJECT_FILE_NAME $PROJECT_FILE_NAME.1
            ;;
        esxu)
            sed -e 's#CpuC167CS#CpuF269#g' $PROJECT_FILE_NAME.1 > $PROJECT_FILE_NAME
            mv $PROJECT_FILE_NAME $PROJECT_FILE_NAME.1
            ;;
        esx)
            # do nothing here
            ;;
    esac


    #echo_ "Converted UNIX to Windows Linebreak in $PROJECT_FILE_NAME"
    unix_lines_to_windows_lines "$PROJECT_FILE_NAME.1" "$PROJECT_FILE_NAME"
}

create_CcsPrj()
{
    cd "$1/$PROJECT"
    CCS_PROJECT_NAME="$PROJECT"
    CCS_PROJECT_DIR="$1/$PROJECT"
    CCS_PROJECT_FILE_NAME="$PROJECT"'_'"$USE_TARGET_SYSTEM.pjt"
    CCS_PROJECT_FILE_LIST="$1/$PROJECT/$FILELIST_COMBINED_PURE"
    CCS_CONFIG_HDR_NAME="config_""$PROJECT.h"
    CCS_PROJECT_TEMPLATE=$CCS_PROJECT_DIR/$ISO_AG_LIB_INSIDE/library/xgpl_src/build/projectGeneration/update_makefile_CCSSkeleton.pjt
    CCS_COMMERCIAL_BIOS_PATH="library/commercial_BIOS/bios_$USE_TARGET_SYSTEM"
    CCS_PROJECT_FILE_NAME_PATH="$CCS_PROJECT_DIR/$CCS_PROJECT_FILE_NAME"

    if [ "M$ISOAGLIB_INSTALL_PATH" = "M" ] ; then
        CCS_LIB_INSTALL_DIR="./install/lib"
    else
        CCS_LIB_INSTALL_DIR=$ISOAGLIB_INSTALL_PATH
    fi
    CCS_LIB_INSTALL_HEADER_DIR="$CCS_LIB_INSTALL_DIR/include"
    CCS_LIB_INSTALL_LIB_DIR="$CCS_LIB_INSTALL_DIR/lib"

    # check platform specific settings
    if test $PRJ_ACTOR -gt 0 -o $PRJ_SENSOR_DIGITAL -gt 0 -o $PRJ_SENSOR_ANALOG -gt 0 -o $PRJ_SENSOR_COUNTER -gt 0 ; then
        echo_
        echo_ "Misconfigured config file: P1MC has no sensor/actor capabilities!"
        exit 0
    fi

    if test $RS232_INSTANCE_CNT -gt 0 -a $PRJ_RS232_OVER_CAN -eq 0 ; then
        echo_
        echo_ "Misconfigured config file: P1MC has no native rs232. Please enable PRJ_RS232_OVER_CAN or disable RS232_INSTANCE_CNT"
        exit 0
    fi

    # copy project template
    cp $CCS_PROJECT_TEMPLATE $CCS_PROJECT_DIR/$CCS_PROJECT_FILE_NAME


    for EACH_REL_APP_PATH in $REL_APP_PATH; do
        local PART="$(echo_ "$ISO_AG_LIB_INSIDE/$EACH_REL_APP_PATH" | sed -e 's|/[0-9a-zA-Z_+\-]+/\.\.||g' -e 's|/[0-9a-zA-Z_+\-]+\\\.\.||g')"
        if [ -z "$USE_APP_PATH" ]; then
            USE_APP_PATH="$PART"
        else
            append USE_APP_PATH ";$PART"
        fi
    done

    # project defines
    CCS_PROJECT_DEFINES=

    #include pathes
    CCS_INCLUDE_PATH=""
    append CCS_INCLUDE_PATH " -i\"\$(Proj_dir)/$ISO_AG_LIB_INSIDE/$CCS_COMMERCIAL_BIOS_PATH\""
    append CCS_INCLUDE_PATH " -i\"\$(Proj_dir)/\""
    append CCS_INCLUDE_PATH " -i\"\$(Proj_dir)/$ISO_AG_LIB_INSIDE/library\""
    append CCS_INCLUDE_PATH " -i\"\$(Proj_dir)/$ISO_AG_LIB_INSIDE/library/xgpl_src\""

    for EACH_EMBED_HEADER_DIR in $USE_EMBED_HEADER_DIRECTORY; do
        append CCS_INCLUDE_PATH " -i\"$EACH_EMBED_HEADER_DIR\""
    done

    append CCS_INCLUDE_PATH " -i\"\$(Proj_dir)/$CCS_LIB_INSTALL_HEADER_DIR\""

    # source files
    for EACH_SOURCE_FILE in $(cat $1/$PROJECT/$FILELIST_COMBINED_PURE | grep .*cpp$); do
        append CCS_SOURCE_FILE_LIST "\nSource=\"$EACH_SOURCE_FILE\""
    done

    sed -e "s#INSERT_PROJECT_DIR#$CCS_PROJECT_DIR#g" $CCS_PROJECT_FILE_NAME_PATH > $CCS_PROJECT_FILE_NAME_PATH.1
    sed -e "s#INSERT_ISOAGLIB_DIR#$ISO_AG_LIB_INSIDE#g" $CCS_PROJECT_FILE_NAME_PATH.1 > $CCS_PROJECT_FILE_NAME_PATH
    sed -e "s#INSERT_SOURCE_FILE_LIST#$CCS_SOURCE_FILE_LIST#g" $CCS_PROJECT_FILE_NAME_PATH > $CCS_PROJECT_FILE_NAME_PATH.1
    sed -e "s#INSERT_HEADER_SEARCH_PATH#$CCS_INCLUDE_PATH#g" $CCS_PROJECT_FILE_NAME_PATH.1 > $CCS_PROJECT_FILE_NAME_PATH
    sed -e "s#INSERT_CONFIG_HEADER_NAME#$CCS_CONFIG_HDR_NAME#g" $CCS_PROJECT_FILE_NAME_PATH > $CCS_PROJECT_FILE_NAME_PATH.1
    sed -e "s#INSERT_PROJECT_DEFINES#$CCS_PROJECT_DEFINES#g" $CCS_PROJECT_FILE_NAME_PATH.1 > $CCS_PROJECT_FILE_NAME_PATH
    sed -e "s#INSERT_PROJECT_NAME#$CCS_PROJECT_NAME#g" $CCS_PROJECT_FILE_NAME_PATH > $CCS_PROJECT_FILE_NAME_PATH.1
    sed -e "s#INSERT_LIB_INSTALL_DIR#$CCS_LIB_INSTALL_LIB_DIR#g" $CCS_PROJECT_FILE_NAME_PATH.1 > $CCS_PROJECT_FILE_NAME_PATH

    #mv $CCS_PROJECT_FILE_NAME_PATH.1 $CCS_PROJECT_FILE_NAME_PATH

    rm -f $CCS_PROJECT_FILE_NAME_PATH.1

    # create output
    mkdir -p $CCS_LIB_INSTALL_DIR
    mkdir -p $CCS_LIB_INSTALL_HEADER_DIR
    mkdir -p $CCS_LIB_INSTALL_LIB_DIR

    for FIRST_REL_APP_PATH in $REL_APP_PATH; do
        mv $CCS_PROJECT_DIR/$ISO_AG_LIB_INSIDE/$FIRST_REL_APP_PATH/config_$PROJECT.h $CCS_LIB_INSTALL_HEADER_DIR
        mv $CCS_PROJECT_DIR/$ISO_AG_LIB_INSIDE/$FIRST_REL_APP_PATH/version.h $CCS_LIB_INSTALL_HEADER_DIR
        break;
    done

    for EACH_INSTALL_HEADER in $(cat "$1/$PROJECT/$FILELIST_LIBRARY_HDR" | grep -v ".cpp"); do
        install -D $EACH_INSTALL_HEADER $CCS_LIB_INSTALL_HEADER_DIR/$(echo_ $EACH_INSTALL_HEADER | sed -e 's|.*xgpl_src/||')
    done
}

create_VCPrj()
{

    DEV_PRJ_DIR=$(echo_ "$1/$PROJECT" | sed -e 's/Dev-C++/VC6/g')
    # echo_ "Create Projekt file for VC6 in $DEV_PRJ_DIR"
    mkdir -p $DEV_PRJ_DIR
    # Visual Studio will create the needed Debug and Release directories on its own.
    PROJECT_FILE_NAME="$PROJECT"'__'"$CAN_SERVER_FILENAME"'__'"$USE_RS232_DRIVER.dsp"

    DspPrjFilelist="$1/$PROJECT/$FILELIST_COMBINED_PURE"
    CONFIG_HDR_NAME="config_""$PROJECT.h"

    # remove some file lists, which are not used for Dev-C++
    rm -f "$1/$PROJECT/$FILELIST_LIBRARY_PURE" "$1/$PROJECT/$FILELIST_APP_PURE"

    USE_DEFINES=$(echo_ " /D "'"'"$USE_SYSTEM_DEFINE"'"' " /D "'"'"PRJ_USE_AUTOGEN_CONFIG=$CONFIG_HDR_NAME"'"' | sed -e 's/SYSTEM_PC/SYSTEM_PC_VC/g')
    USE_d_DEFINES=$(echo_ $USE_DEFINES | sed -e 's#/D#/d#g')


    LIB_DIR_LINE=""
    LIB_FILE_LINE=""

    ISO_AG_LIB_PATH_WIN=$(echo_ "$ISO_AG_LIB_INSIDE" | sed -e 's#\\#_=_=#g' -e 's#/#=_=_#g')

    USE_STLPORT_HEADER_DIRECTORY=$(echo_ "$USE_STLPORT_HEADER_DIRECTORY" | sed -e 's#\\#_=_=#g' -e 's#/#=_=_#g')

    USE_WIN32_EXTERNAL_INCLUDE_PATH=$(echo_ "$USE_WIN32_EXTERNAL_INCLUDE_PATH" | sed -e 's#\\#=_=_#g' -e 's#/#=_=_#g')

    USE_WIN32_EXTERNAL_LIBRARY_PATH=$(echo_ "$USE_WIN32_EXTERNAL_LIBRARY_PATH" | sed -e 's#\\#=_=_#g' -e 's#/#=_=_#g')

    USE_MSVC_EXTERNAL_LIBRARIES=$(echo_ "$USE_MSVC_EXTERNAL_LIBRARIES" | sed -e 's#\\#=_=_#g' -e 's#/#=_=_#g')

    REL_APP_PATH_WIN=$(echo_ "$REL_APP_PATH" | sed -e 's#\\#=_=_#g' -e 's#/#=_=_#g')

    PRJ_INCLUDE_PATH_WIN=$(echo_ "$PRJ_INCLUDE_PATH" | sed -e 's#\\#=_=_#g' -e 's#/#=_=_#g')

    # echo_ "USE_CAN_DRIVER $USE_CAN_DRIVER; USE_CAN_DEVICE_FOR_SERVER $USE_CAN_DEVICE_FOR_SERVER"

    if  [ $USE_CAN_DRIVER = "socket_server" -o $USE_CAN_DRIVER = "socket_server_hal_simulator" ] ; then
        USE_INCLUDE_PATHS='/I "'"$ISO_AG_LIB_PATH_WIN=_=_library"'" /I "'"$ISO_AG_LIB_PATH_WIN=_=_library=_=_xgpl_src"'"'
        append USE_DEFINES ""' /D "CAN_DRIVER_SOCKET" /D "SYSTEM_WITH_ENHANCED_CAN_HAL"'
        append USE_d_DEFINES ""' /d "CAN_DRIVER_SOCKET" /D "SYSTEM_WITH_ENHANCED_CAN_HAL"'
    fi

    for EACH_REL_APP_PATH in $REL_APP_PATH_WIN ; do
        append USE_INCLUDE_PATHS ""' /I "'"$ISO_AG_LIB_PATH_WIN=_=_$EACH_REL_APP_PATH"'"'
    done

    for SingleInclPath in $PRJ_INCLUDE_PATH_WIN ; do
        append USE_INCLUDE_PATHS ""' /I "'"$ISO_AG_LIB_PATH_WIN=_=_$SingleInclPath"'"'
    done

    for SingleInclPath in $USE_WIN32_EXTERNAL_INCLUDE_PATH ; do
        append USE_INCLUDE_PATHS ""' /I "'"$SingleInclPath"'"'
    done
    for SingleLibItem in $USE_MSVC_EXTERNAL_LIBRARIES ; do
        append LIB_DIR_LINE ""' '"$SingleLibItem"
    done
    for SingleLibPath in $USE_WIN32_EXTERNAL_LIBRARY_PATH ; do
        append LIB_DIR_LINE ""' /libpath:"'"$SingleLibPath"'"'
    done


    sed -e 's#=_=_#\\#g'  $DspPrjFilelist > $DspPrjFilelist.1
    mv $DspPrjFilelist.1 $DspPrjFilelist

    #echo_ "Libs=$LIB_DIR_LINE"
    #echo_ "Linker=$LIB_FILE_LINE"

    for SinglePrjDefine in $PRJ_DEFINES ; do
        append USE_DEFINES ""' /D '"$SinglePrjDefine"
        append USE_d_DEFINES ""' /D '"$SinglePrjDefine"
    done
    if [ $PRJ_SYSTEM_WITH_ENHANCED_CAN_HAL -gt 0 ] ; then
        append USE_DEFINES ""' /D "SYSTEM_WITH_ENHANCED_CAN_HAL"'
        append USE_d_DEFINES ""' /D "SYSTEM_WITH_ENHANCED_CAN_HAL"'
    fi



    cp -a $DEV_PRJ_DIR/$ISO_AG_LIB_INSIDE/tools/project_generation/update_makefile_vc6_prj_base.dsp $DEV_PRJ_DIR/$PROJECT_FILE_NAME

    sed -e "s#INSERT_PROJECT#$PROJECT#g"  $DEV_PRJ_DIR/$PROJECT_FILE_NAME > $DEV_PRJ_DIR/$PROJECT_FILE_NAME.1
    sed -e "s#INSERT_INCLUDE_PATHS#$USE_INCLUDE_PATHS#g"  $DEV_PRJ_DIR/$PROJECT_FILE_NAME.1 > $DEV_PRJ_DIR/$PROJECT_FILE_NAME
    sed -e "s#INSERT_DEFINES#$USE_DEFINES#g" $DEV_PRJ_DIR/$PROJECT_FILE_NAME > $DEV_PRJ_DIR/$PROJECT_FILE_NAME.1
    sed -e "s#INSERT_d_DEFINES#$USE_d_DEFINES#g" $DEV_PRJ_DIR/$PROJECT_FILE_NAME.1 > $DEV_PRJ_DIR/$PROJECT_FILE_NAME
    sed -e "s#INSERT_CAN_LIB_PATH#$LIB_DIR_LINE#g" $DEV_PRJ_DIR/$PROJECT_FILE_NAME > $DEV_PRJ_DIR/$PROJECT_FILE_NAME.1

    USE_STLPORT_LIB_DIRECTORY=$(echo_ "$USE_STLPORT_HEADER_DIRECTORY" |sed 's|stlport|lib|g')
    sed -e "s#INSERT_STLPORT_LIB_PATH#$USE_STLPORT_LIB_DIRECTORY#g" $DEV_PRJ_DIR/$PROJECT_FILE_NAME.1 > $DEV_PRJ_DIR/$PROJECT_FILE_NAME

    while [ $(grep -c -e '=_=_[0-9a-zA-Z_+\-]\+=_=_\.\.=_=_' $DEV_PRJ_DIR/$PROJECT_FILE_NAME) -gt 0 ] ; do
        sed -e 's|=_=_[0-9a-zA-Z_+\-]\+=_=_\.\.=_=_|=_=_|g' $DEV_PRJ_DIR/$PROJECT_FILE_NAME > $DEV_PRJ_DIR/$PROJECT_FILE_NAME.1
        mv $DEV_PRJ_DIR/$PROJECT_FILE_NAME.1 $DEV_PRJ_DIR/$PROJECT_FILE_NAME
    done

    sed -e 's#=_=_#\\#g'  $DEV_PRJ_DIR/$PROJECT_FILE_NAME > $DEV_PRJ_DIR/$PROJECT_FILE_NAME.1
    mv $DEV_PRJ_DIR/$PROJECT_FILE_NAME.1 $DEV_PRJ_DIR/$PROJECT_FILE_NAME


    SOURCES=$(grep -E "\.cc|\.cpp|\.c|\.lib" $DspPrjFilelist)
    HEADERS=$(grep -E "\.h|\.hpp" $DspPrjFilelist)

    cat <<-ENDOFHEADERB >> $DEV_PRJ_DIR/$PROJECT_FILE_NAME
# Begin Group "Quellcodedateien"

# PROP Default_Filter "cc;cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
ENDOFHEADERB

    for i in $SOURCES
    do
        if [ $i = "" ] ; then
            continue
        fi
        echo_ "# Begin Source File" >> $DEV_PRJ_DIR/$PROJECT_FILE_NAME
        filename=$(echo_ "$i" |sed 's|/|\\|g')
        echo_ "SOURCE=$filename" >> $DEV_PRJ_DIR/$PROJECT_FILE_NAME
        echo_ "# End Source File" >> $DEV_PRJ_DIR/$PROJECT_FILE_NAME
        echo_ "" >> $DEV_PRJ_DIR/$PROJECT_FILE_NAME
    done

    cat <<-ENDOFHEADERB >> $DEV_PRJ_DIR/$PROJECT_FILE_NAME
# End Group

# Begin Group "Header-Dateien"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
ENDOFHEADERB

    for i in $HEADERS
    do
        if [ $i = "" ] ; then
            continue
        fi
        echo_ "# Begin Source File" >> $DEV_PRJ_DIR/$PROJECT_FILE_NAME
        filename=$(echo_ "$i" |sed 's|/|\\|g')
        echo_ "SOURCE=$filename" >> $DEV_PRJ_DIR/$PROJECT_FILE_NAME
        echo_ "# End Source File" >> $DEV_PRJ_DIR/$PROJECT_FILE_NAME
        echo_ "" >> $DEV_PRJ_DIR/$PROJECT_FILE_NAME
    done

    echo_ "# End Group" >> $DEV_PRJ_DIR/$PROJECT_FILE_NAME
    echo_ "# End Target" >> $DEV_PRJ_DIR/$PROJECT_FILE_NAME
    echo_ "# End Project" >> $DEV_PRJ_DIR/$PROJECT_FILE_NAME

    #echo_ "Convert UNIX to Windows Linebreak in $DEV_PRJ_DIR/$PROJECT_FILE_NAME"
    mv "$DEV_PRJ_DIR/$PROJECT_FILE_NAME" "$DEV_PRJ_DIR/$PROJECT_FILE_NAME.1"
    unix_lines_to_windows_lines "$DEV_PRJ_DIR/$PROJECT_FILE_NAME.1" "$DEV_PRJ_DIR/$PROJECT_FILE_NAME"
    rm -f $DEV_PRJ_DIR/$PROJECT_FILE_NAME.1
    cd $DEV_PRJ_DIR
    # org test
}

perform_everything()
{
    # verify and correct the variables
    check_set_correct_variables $1 $2
    GENERATE_FILES_ROOT_DIR=$(echo_ "$GENERATE_FILES_ROOT_DIR" | sed -e 's|/[0-9a-zA-Z_+\-]+/\.\.||g' -e 's/\\[0-9a-zA-Z_+\-]+\\\.\.//g')
    # echo_ "Create project for $USE_TARGET_SYSTEM in $GENERATE_FILES_ROOT_DIR"

    # now call the function create_filelist() which build
    # the file list based on the varibles defined above
    create_filelist $GENERATE_FILES_ROOT_DIR $2

    # call function which build the file list for the unit
    # tests
    # UTESTs disabled right now! (even the create_utest_filelist simply returns at the beginning!)
    #  create_utest_filelist

    # call function to create project specific config file
    create_autogen_project_config $GENERATE_FILES_ROOT_DIR $2
    if [ $USE_TARGET_SYSTEM = "pc_linux" ] ; then
        # call function to create the Makefile in the project dir
        create_makefile $GENERATE_FILES_ROOT_DIR $2
        # check if a win32 project file whould be created
    elif [ $USE_TARGET_SYSTEM = "pc_win32" ] ; then
        create_DevCCPrj $GENERATE_FILES_ROOT_DIR $2
        create_VCPrj $GENERATE_FILES_ROOT_DIR $2
    elif [ $USE_TARGET_SYSTEM = "p1mc" ] ; then
        create_CcsPrj $GENERATE_FILES_ROOT_DIR $2
    else
        create_EdePrj $GENERATE_FILES_ROOT_DIR $2
    fi

    # cleanup temporary files
    rm -f $GENERATE_FILES_ROOT_DIR/$PROJECT/directories__*.txt
    find $GENERATE_FILES_ROOT_DIR/$PROJECT -name "filelist__*.[alu]*.txt" -exec rm {} \;
    find $GENERATE_FILES_ROOT_DIR/$PROJECT -name "filelist__*-doc.txt" -exec rm {} \;
    #$GENERATE_FILES_ROOT_DIR/$PROJECT/filelist__*.txt

    # jump to initial directory on start of script
    cd $3
}





# Usage: usage
# Print the usage.
usage ()
{
    cat <<EOF
Usage: $0 [OPTION] project_config_file
Create filelist, Makefile and configuration settings for an IsoAgLib project.

-h, --help                        print this message and exit.
--doxygen-export-directory=DIR    write the filelist and configuration files with doxygen documentation
blocks to the given directory instead of the default directory of all generated files.
--IsoAgLib-root=DIR               use the given root directory instead of the entry in the selected configuration file.
--target-system=TARGET            produce the project definition files for the selected TARGET instead of the
target which is specified in the configuration file
("pc_linux"|"pc_win32"|"esx"|"esxu"|"c2c"|"imi"|"pm167"|"Dj1"|"mitron167").
--pc-can-driver=CAN_DRIVER        produce the project definition files for the selected CAN_DRIVER if the project shall run on PC
("simulating"|"sys"|"msq_server"|"socket_server"|"socket_server_hal_simulator").
--pc-can-device-for-server=CAN_DEVICE_FOR_SERVER
use this device for building the can_server
("no_card"|"pcan"|"A1"|"rte").
--pc-rs232-driver=RS232_DRIVER    produce the project definition files for the selected RS232_DRIVER if the project shall run on PC
("simulating"|"sys"|"rte"|"hal_simulator").
--little-endian-cpu               select configuration for LITTLE ENDIAN CPU type
--big-endian-cpu                  select configuration for BIG ENDIAN CPU type
--with-makefile-skeleton=filename define project specific MakefileSkeleton text file which is used as base for
Makefiles (default: MakefileSkeleton.txt in the same directory as this script)

$0 parses the selected project configuration file and overwrites the default values for all contained settings.
It then collects the corresponding files which can then be imported to an individual IDE.
Additionally a project specific configuration header is created in the directory xgpl_src/Application_Config with the
name scheme ".config_<project_name>.h". If the #define PRJ_USE_AUTOGEN_CONFIG is set to ".config_<project_name>.h",
the central configuration header xgpl_src/IsoAgLib/isoaglib_config.h will include this header.
Please set additionally the SYSTEM_FOO for the wanted platform - $0 will output the correct define at the end of the
run.
Thus with these two DEFINE settings, the compiler can generate a clean running executable / HEX.

Report bugs to <m.wodok@osb-ag.de>.
EOF
}


# main

set_default_values

if [ $# -lt 1 ] ; then
    echo_ "ERROR! You must at least specify the configuration file for your project as parameter"
    echo_
    usage
    exit 1
fi

# Check the arguments.
for option in "$@"; do
    case "$option" in
        -h | --help)
            usage
            exit 0 ;;
        '--doxygen-export-directory='*)
            DOXYGEN_EXPORT_DIR_1=$(echo_ "$option" | sed -e 's/--doxygen-export-directory=//')
            mkdir -p "$DOXYGEN_EXPORT_DIR_1"
            CALL_DIR=$PWD
            cd "$DOXYGEN_EXPORT_DIR_1"
            DOXYGEN_EXPORT_DIR=$PWD
            cd $CALL_DIR ;;
        '--IsoAgLib-root='*)
            ISOAGLIB_ROOT=$(echo_ "$option" | sed 's/--IsoAgLib-root=//')
            if [ ! -f "$ISOAGLIB_ROOT/library/xgpl_src/IsoAgLib/isoaglib_config.h" ] ; then
                echo_ "Directory $ISOAGLIB_ROOT doesn't contain valid IsoAgLib directory root. The file xgpl_src/IsoAgLib/isoaglib_config.h can't be found." 1>&2
                usage
                exit 1
            else
                CALL_DIR=$PWD
                cd $ISOAGLIB_ROOT
                ISOAGLIB_ROOT=$PWD
                cd $CALL_DIR
            fi ;;
        '--target-system='*)
            PARAMETER_TARGET_SYSTEM=$(echo_ "$option" | sed 's/--target-system=//')
            ;;
        '--pc-can-driver='*)
            PARAMETER_CAN_DRIVER=$(echo_ "$option" | sed 's/--pc-can-driver=//')
            ;;
        '--pc-can-device-for-server='*)
            PARAMETER_CAN_DEVICE_FOR_SERVER=$(echo_ "$option" | sed 's/--pc-can-device-for-server=//')
            ;;
        '--pc-rs232-driver='*)
            PARAMETER_RS232_DRIVER=$(echo_ "$option" | sed 's/--pc-rs232-driver=//')
            ;;
        --little-endian-cpu)
            PARAMETER_LITTLE_ENDIAN_CPU=1
            ;;
        --big-endian-cpu)
            PARAMETER_LITTLE_ENDIAN_CPU=0
            USE_BIG_ENDIAN_CPU=1
            ;;
        '--with-makefile-skeleton='*)
            RootDir=$PWD
            MAKEFILE_SKELETON_FILE=$RootDir/$(echo_ "$option" | sed 's/--with-makefile-skeleton=//')
            ;;
        '--with-makefile-app-skeleton='*)
            RootDir=$PWD
            MAKEFILE_APP_SKELETON_FILE=$RootDir/$(echo_ "$option" | sed 's/--with-makefile-app-skeleton=//')
            ;;
        -*)
            echo_ "Unrecognized option $option'" 1>&2
            usage
            exit 1
            ;;
        *)
            CONF_FILE=$option
            ;;
    esac
done

# check if configuration file exists
if [ ! -f $CONF_FILE ] ; then
    echo_ "ERROR! configuration file $CONF_FILE does not exist!"
    exit 1
elif [ ! -r $CONF_FILE ] ; then
    echo_ "ERROR! configuration file $CONF_FILE is not readable!"
    exit 1
fi

abs_dir_of(){ cd "$(dirname "$1")" && pwd; }

SCRIPT_DIR="$(abs_dir_of "$0")"
CONF_DIR="$(abs_dir_of "$CONF_FILE")"

# echo_ "StartDir $START_DIR"
# echo_ "ScriptDir $SCRIPT_DIR"
# echo_ "ConfDir $CONF_DIR"

# now import the config setting file
. ./$CONF_FILE

# perform some checks based on user input
# check for correct target system setting
if [ $PARAMETER_TARGET_SYSTEM != "UseConfigFile" ] ; then
    USE_TARGET_SYSTEM=$PARAMETER_TARGET_SYSTEM
fi
case "$USE_TARGET_SYSTEM" in
    pc_linux | pc_win32 | esx | esxu | c2c | imi | p1mc | pm167 | Dj1 | mitron167)
        ;;
    *)
        echo_ "Unknown target system $USE_TARGET_SYSTEM" 1>&2
        usage
        exit 1 ;;
esac

# check for corrext CAN driver - and automatically adapt to embedded targets
if [ $PARAMETER_CAN_DRIVER != "UseConfigFile" ] ; then
    USE_CAN_DRIVER=$PARAMETER_CAN_DRIVER
    IS_CAN_SERVER=$(echo_ $PARAMETER_CAN_DRIVER | grep -c "msq_server")
    if [ $IS_CAN_SERVER -gt 0 ] ; then
        USE_CAN_DEVICE_FOR_SERVER=$(echo_ $PARAMETER_CAN_DRIVER | sed 's/msq_server_//g')
        USE_CAN_DRIVER="msq_server"
        PARAMETER_CAN_DRIVER="msq_server"
    fi
fi

if [ $PARAMETER_CAN_DEVICE_FOR_SERVER != "UseConfigFile" ] ; then
    USE_CAN_DEVICE_FOR_SERVER=$PARAMETER_CAN_DEVICE_FOR_SERVER
fi


#default for not-can_server
CAN_SERVER_FILENAME=$PARAMETER_CAN_DRIVER
case "$USE_CAN_DRIVER" in
    simulating)
        case "$USE_TARGET_SYSTEM" in
            pc_linux | pc_win32)
                # enhanced CAN HAL is not yet supported for simulating CAN HAL
                PRJ_SYSTEM_WITH_ENHANCED_CAN_HAL=0
                ;;
            *)
                echo_ "Override $USE_CAN_DRIVER CAN driver by system driver for embedded target $USE_TARGET_SYSTEM"
                USE_CAN_DRIVER="sys"
                PARAMETER_CAN_DRIVER="sys"
                # enhanced CAN HAL is not yet supported for the known embedded targets
                PRJ_SYSTEM_WITH_ENHANCED_CAN_HAL=0
                ;;
        esac
        ;;
    sys)
        case "$USE_TARGET_SYSTEM" in
            pc_linux | pc_win32)
                echo_ "A selection of sys CAN_DRIVER is only applicable for embedded targets." 1>&2
                usage
                exit 1
                ;;
            *)
                ;;
        esac
        # enhanced CAN HAL is not yet supported for the known embedded targets
        PRJ_SYSTEM_WITH_ENHANCED_CAN_HAL=0
        ;;
    msq_server)
        case "$USE_TARGET_SYSTEM" in
            pc_linux)
                # enhanced CAN HAL IS supported for the Linux can_server
                PRJ_SYSTEM_WITH_ENHANCED_CAN_HAL=1
                ;;
            pc_win32)
                echo_ "Server Client CAN driver can only used for target pc_linux. Overridden with socket_server" 1>&2
                USE_CAN_DRIVER="socket_server"
                USE_CAN_DEVICE_FOR_SERVER="no_card"
                CAN_SERVER_FILENAME="can_server_sock"
                ;;
            *)
                echo_ "Override $USE_CAN_DRIVER CAN driver by system driver for embedded target $USE_TARGET_SYSTEM"
                USE_CAN_DRIVER="sys"
                PARAMETER_CAN_DRIVER="sys"
                # enhanced CAN HAL is not yet supported for the known embedded targets
                PRJ_SYSTEM_WITH_ENHANCED_CAN_HAL=0
                ;;
        esac
        # make sure, that USE_CAN_DEVICE_FOR_SERVER is automatically set, when not yet defined
        if [ "A$USE_CAN_DEVICE_FOR_SERVER" = "A" ] ; then
            case $PRJ_DEFINES in
                *SYSTEM_A1*)
                    USE_CAN_DEVICE_FOR_SERVER="A1"
                    ;;
                *)
                    USE_CAN_DEVICE_FOR_SERVER="no_card"
                    ;;
            esac
        fi
        if [ $USE_TARGET_SYSTEM != "pc_win32" ] ; then
            CAN_SERVER_FILENAME=${USE_CAN_DRIVER}_${USE_CAN_DEVICE_FOR_SERVER}
        fi
        ;;
    socket_server | socket_server_hal_simulator)
        # enhanced CAN HAL IS supported for the socket based can_server
        PRJ_SYSTEM_WITH_ENHANCED_CAN_HAL=1
        if [ "A$USE_CAN_DEVICE_FOR_SERVER" = "A" ] ; then
            case $PRJ_DEFINES in
                *SYSTEM_A1*)
                    USE_CAN_DEVICE_FOR_SERVER="A1"
                    ;;
                *)
                    USE_CAN_DEVICE_FOR_SERVER="no_card"
                    ;;
            esac
        fi

        CAN_SERVER_FILENAME=can_server_sock_${USE_CAN_DEVICE_FOR_SERVER}
        if test $USE_TARGET_SYSTEM = "pc_win32" ; then
            if test $USE_CAN_DEVICE_FOR_SERVER = "no_card" ; then
                # skip extension "no_card"
                CAN_SERVER_FILENAME=can_server_sock
            fi
        fi
        ;;
    *)
        echo_ "Unknown CAN driver $USE_CAN_DRIVER" 1>&2
        usage
        exit 1
        ;;
esac

case "$USE_CAN_DEVICE_FOR_SERVER" in
    rte)
        case "$USE_TARGET_SYSTEM" in
            pc_linux)
                PRJ_SYSTEM_WITH_ENHANCED_CAN_HAL=1
                ;;
            pc_win32)
                echo_ "RTE CAN driver can only used for target pc_linux" 1>&2
                usage
                exit 1
                ;;
        esac
        ;;
esac



if [ $PARAMETER_RS232_DRIVER != "UseConfigFile" ] ; then
    USE_RS232_DRIVER=$PARAMETER_RS232_DRIVER
fi
case "$USE_RS232_DRIVER" in
    simulating)
        case "$USE_TARGET_SYSTEM" in
            pc_linux | pc_win32)
                ;;
            *)
                echo_ "Override $USE_RS232_DRIVER RS232 driver by system driver for embedded target $USE_TARGET_SYSTEM"
                USE_RS232_DRIVER="sys"
                PARAMETER_RS232_DRIVER="sys"
                ;;
        esac
        ;;
    sys)
        ;;
    rte)
        case "$USE_TARGET_SYSTEM" in
            pc_linux)
                ;;
            pc_win32)
                USE_RS232_DRIVER="sys"
                echo_ "RTE RS232 driver can only used for target pc_linux -> Override by sys"
                ;;
            *)
                echo_ "Override $USE_RS232_DRIVER RS232 driver by system driver for embedded target $USE_TARGET_SYSTEM"
                USE_RS232_DRIVER="sys"
                PARAMETER_RS232_DRIVER="sys"
                ;;
        esac
        ;;
    *)
        echo_ "Unknown RS232 driver $USE_RS232_DRIVER" 1>&2
        usage
        exit 1
        ;;
esac

# check for little/big endian setting
if [ "A$PARAMETER_LITTLE_ENDIAN_CPU" != "A" ] ; then
    USE_LITTLE_ENDIAN_CPU=$PARAMETER_LITTLE_ENDIAN_CPU
fi


# call the main function to create
# Makefile for defined config setting
perform_everything "$CONF_DIR" "$SCRIPT_DIR" "$START_DIR"


if [ "$USE_LITTLE_ENDIAN_CPU" -lt 1 ] ; then
    echo_  "Endianess:     Big Endian CPU"
else
    echo_  "Endianess:     Little Endian CPU"
fi
echo_    "Target:        $IDE_NAME - (The settings below are already set up therefore)"
echo_    "Defines:       $USE_SYSTEM_DEFINE PRJ_USE_AUTOGEN_CONFIG=config_$PROJECT.h $PRJ_DEFINES"
echo_n "Include Path:  "
for EACH_REL_APP_PATH in $REL_APP_PATH ; do
    echo_n "$ISO_AG_LIB_INSIDE/$EACH_REL_APP_PATH ";
done
echo_
echo_
echo_ "Generation successful."

if [ "A$DOXYGEN_EXPORT_DIR" != "A" ] ; then
    # doxygen export is specified -> copy config file there with suitable doc block
    CONF_BASE=$(basename $CONF_FILE)
    CONFIG_SPEC_DOXYGEN_READY="$DOXYGEN_EXPORT_DIR/spec"'__'"$CONF_BASE"'__'"$USE_TARGET_SYSTEM"'__'"$CAN_SERVER_FILENAME"'__'"$USE_RS232_DRIVER-doc.txt"
    #echo_ "/**" > $CONFIG_SPEC_DOXYGEN_READY
    #echo_ "* \section PrjSpec$PROJECT"'__'"$USE_TARGET_SYSTEM"'__'"$CAN_SERVER_FILENAME"'__'"$USE_RS232_DRIVER List of configuration settings for $PROJECT ." >> $CONFIG_SPEC_DOXYGEN_READY
    #echo_ "* This is only a copy with doxygen ready comment blocks from the original file in IsoAgLib/compiler_projeckdevelop_make/ " >> $CONFIG_SPEC_DOXYGEN_READY
    #echo_ "* Use the file $CONF_FILE in this directory as input file for $0 to create the project generation files." >> $CONFIG_SPEC_DOXYGEN_READY
    #echo_ "*/" >> $CONFIG_SPEC_DOXYGEN_READY
    #echo_ "/*@{*/" >> $CONFIG_SPEC_DOXYGEN_READY
#   cat $CONF_FILE >> $CONFIG_SPEC_DOXYGEN_READY
    #sed -e "s/USE_TARGET_SYSTEM=\".*/USE_TARGET_SYSTEM=\"$USE_TARGET_SYSTEM\"/g" -e "s/USE_CAN_DRIVER=\".*/USE_CAN_DRIVER=\"$USE_CAN_DRIVER\"/g" -e "s/USE_RS232_DRIVER=\".*/USE_RS232_DRIVER=\"$USE_RS232_DRIVER\"/g" $CONF_FILE > /tmp/$CONF_BASE
    #cat /tmp/$CONF_BASE >> $CONFIG_SPEC_DOXYGEN_READY
    #rm -f /tmp/$CONF_BASE
    #echo_ "/*@}*/" >> $CONFIG_SPEC_DOXYGEN_READY

    echo_e "$ENDLINE$ENDLINE @section PrjSpec$PROJECT"'__'"$USE_TARGET_SYSTEM"'__'"$CAN_SERVER_FILENAME"'__'"$USE_RS232_DRIVER List of configuration settings for $PROJECT with CAN Driver $USE_CAN_DRIVER and RS232 Driver $USE_RS232_DRIVER" > $CONFIG_SPEC_DOXYGEN_READY
    echo_ " This is only a copy with doxygen ready comment blocks from the original file in IsoAgLib/compiler_projeckdevelop_make/ " >> $CONFIG_SPEC_DOXYGEN_READY
    echo_ " Use the file $CONF_FILE in this directory as input file for $0 to create the project generation files." >> $CONFIG_SPEC_DOXYGEN_READY
    echo_ "\code" >> $CONFIG_SPEC_DOXYGEN_READY
    sed -e "s/USE_TARGET_SYSTEM=\".*/USE_TARGET_SYSTEM=\"$USE_TARGET_SYSTEM\"/g" -e "s/USE_CAN_DRIVER=\".*/USE_CAN_DRIVER=\"$USE_CAN_DRIVER\"/g" -e "s/USE_RS232_DRIVER=\".*/USE_RS232_DRIVER=\"$USE_RS232_DRIVER\"/g" $CONF_DIR/$CONF_FILE > /tmp/$CONF_BASE
    cat /tmp/$CONF_BASE >> $CONFIG_SPEC_DOXYGEN_READY
    echo_ "\endcode" >> $CONFIG_SPEC_DOXYGEN_READY
    rm -f /tmp/$CONF_BASE

fi