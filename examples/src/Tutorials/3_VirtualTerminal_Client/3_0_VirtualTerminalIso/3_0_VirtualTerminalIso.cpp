/* *************************************************************************
                          3_0_VirtualTerminalIso.cpp
                             -------------------
    begin                : Sun Jul 18 17:00:00 CEST 2004
    copyright            : (C) 2000 - 2006 by Dipl.-Inf. Martin Wodok
    email                : m.wodok@osb-ag:de
 ***************************************************************************/
/* *************************************************************************
 * This example main application is published NOT as GPL`ed Open Source,   *
 * so that you can include this source file in propietary closed projects. *
 * Everybody is encouraged to use the examples in the examples    *
 * directory for a quick and easy start of development for IsoAgLib        *
 * applications.                                                           *
 *                                                                         *
 * So nobody is bound to publish changes of the example source files.      *
 * But every changed example, which could help learning the use of the     *
 * GPLed "IsoAgLib", is invited to be sent to the original author, so      *
 * that it can be published with the other examples.                       *
 ***************************************************************************/
/* *************************************************************************
 *                                                                         *
 * This is an example of "IsoAgLib", an object oriented program library    *
 * to serve as a software layer between application specific program and   *
 * communication protocol details. By providing simple function calls for  *
 * jobs like starting a measuring program for a process data value on a    *
 * remote ECU, the main program has not to deal with single CAN telegram   *
 * formatting. This way communication problems between ECU's which use     *
 * this library should be prevented.                                       *
 * Everybody and every company is invited to use this library to make a    *
 * working plug and play standard out of the printed protocol standard.    *
 *                                                                         *
 * Copyright (C) 1999 - 2004 Dipl.-Inform. Achim Spangler                  *
 *                                                                         *
 * The "IsoAgLib" is free software; you can redistribute it and/or         *
 * modify it under the terms of the GNU Lesser General Public License as   *
 * published by the Free Software Foundation; either version 2 of the      *
 * License, or (at your option) any later version.                         *
 *                                                                         *
 * The "IsoAgLib" is distributed in the hope that it will be useful, but   *
 * WITHOUT ANY WARRANTY; without even the implied warranty of              *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU       *
 * General Public License for more details.                                *
 *                                                                         *
 * You should have received a copy of the GNU General Public License       *
 * along with IsoAgLib; if not, write to the Free Software Foundation,     *
 * Inc., 59 Temple Place, Suite 330, Boston, MA  02111_1307  USA           *
 ***************************************************************************/
/* *********************************************************************** */
/** \example 3_0_VirtualTerminalIso.cpp
 * This tutorial shall provide the base program, which uploads a ISO 11783
 * virtual terminal mask definition ( ObjectPool ) to the terminal.
 * Additionally some event handlers are implemented, to show their use for
 * real programs.
 *
 * <H1>What is demonstrated</H1>
 * <ol>
 * <li>Interaction with virtual ISO 11783 Terminal
 * <ul>
 *  <li>Register Mask Pool at IsoAgLib::iIsoTerminal_c with IsoAgLib::iIsoTerminal_c::registerIsoObjectPool()
 *  <li>Use event handler which are derived from IsoAgLib::iIsoTerminalObjectPool_c
 *  <li>Implementation of abstract IsoAgLib::iIsoTerminalObjectPool_c::eventNumericValue
 *    to react on received numeric value which was selected at terminal
 *  <li>Implementation of abstract IsoAgLib::iIsoTerminalEventHandler_c::eventKeyCode()
 *    to react on pressed terminal keys
 * </ul>
 * <li>Trigger periodic activities of ISO<i><sub>AgLib</sub></i>
 *  <ul>
 *  <li>Core class IsoAgLib::iScheduler_c for scheduling of all periodic activities
 *  <li>Method IsoAgLib::iScheduler_c::timeEvent() which can<ul>
 *    <li>Perform activities until defined al_endTime is reached, which is important
 *      for scheduling purposes of whole system - call by IsoAgLib::iScheduler_c::timeEvent( al_endTime )
 *    <li>Process all received CAN messages until all receive buffers are empty
 *      -> simple call, but can lead to deadlock on to high CAN load
 *    </ul>
 *  </ul>
 * <li>Create local identity, for which the ISO<i><sub>AgLib</sub></i> performs an address claim,
 *    so that the ECU can access the <b><i>ISO11783</i></b> BUS with IsoAgLib::IsoAgLib::iIdentItem_c
 *    and its constructor IsoAgLib::iIdentItem_c::iIdentItem_c
 * </ol>
 * <H1>Where to look for further information</H1>
 * <ol>
 * <li>Description of used XML syntax to specify ISO 11783 virtual terminal mask pools at \ref XMLspec
 * <li>Overview information on Scheduling at \ref SchedulerPage
 * <li>Overview on global communication functions at \ref CommOverPage
 * </ol>
 *
 * <H1>Howto compile this example</H1>
 * <ol>
 * <li>Call the script update_makefile.sh with the spec file
 *     conf_3_0_VirtualTerminalIso from the directory where the spec file
 *     and the script are located<br>
 *     <tt>\#> update_makefile.sh conf_3_0_VirtualTerminalIso</tt>
 * <li><b>Only Linux:</b><ul>
 *     <li><tt>\#>cd 3_0_VirtualTerminalIso</tt> to go to the subdirectory 3_0_VirtualTerminalIso
 *     <li><tt>\#> make</tt><br>
 *     <li>GNU make will then use the fresh created Makefile to create the executable 3_0_VirtualTerminalIso in this directory
 *     </ul>
 * <li><b>Other Development Environments:</b><ul>
 *     <li>import the file list in filelist__3_0_VirtualTerminalIso.txt into the IDE of your choice
 *     <li>set <i>-DPRJ_USE_AUTOGEN_CONFIG=config_3_0_VirtualTerminalIso</i> as global project parameter
 *     <li>add target specific headers, sources or libraries to the project
 *     <li>let your IDE build the project
 *   </ul>
 * </ol>
 *
 * <H1>Needed Project Specification</H1>
 * The project specification, which is needed as input for
 * the generation script update_makefile.sh is described in
 * \ref PrjSpec3_0_VirtualTerminalIso__pc_linux__simulating__simulating .
 *
 * The adapted project specifications for different setups:
 * <ul>
 * <li>Configuration Setups for Linux on PC:
 * <ul>
 *    <li> \ref PrjSpec3_0_VirtualTerminalIso__pc_linux__msq_server_no_card__rte
 *		<li> \ref PrjSpec3_0_VirtualTerminalIso__pc_linux__msq_server_rte__rte
 *		<li> \ref PrjSpec3_0_VirtualTerminalIso__pc_linux__msq_server_A1__rte
 *		<li> \ref PrjSpec3_0_VirtualTerminalIso__pc_linux__msq_server_pcan__rte
 *    <li> \ref PrjSpec3_0_VirtualTerminalIso__pc_linux__msq_server_no_card__simulating
 *		<li> \ref PrjSpec3_0_VirtualTerminalIso__pc_linux__msq_server_rte__simulating
 *		<li> \ref PrjSpec3_0_VirtualTerminalIso__pc_linux__msq_server_A1__simulating
 *		<li> \ref PrjSpec3_0_VirtualTerminalIso__pc_linux__msq_server_pcan__simulating
 *    <li> \ref PrjSpec3_0_VirtualTerminalIso__pc_linux__msq_server_no_card__sys
 *		<li> \ref PrjSpec3_0_VirtualTerminalIso__pc_linux__msq_server_rte__sys
 *		<li> \ref PrjSpec3_0_VirtualTerminalIso__pc_linux__msq_server_A1__sys
 *		<li> \ref PrjSpec3_0_VirtualTerminalIso__pc_linux__msq_server_pcan__sys
 *    <li> \ref PrjSpec3_0_VirtualTerminalIso__pc_linux__simulating__rte
 *    <li> \ref PrjSpec3_0_VirtualTerminalIso__pc_linux__simulating__simulating
 *    <li> \ref PrjSpec3_0_VirtualTerminalIso__pc_linux__simulating__sys
 *  </ul>
 * <li>Configuration Setups for Win32 on PC:
 * <ul>
 *    <li> \ref PrjSpec3_0_VirtualTerminalIso__pc_win32__can_server_sock__simulating
 *    <li> \ref PrjSpec3_0_VirtualTerminalIso__pc_win32__can_server_sock__sys
 *  </ul>
 * <li>Configuration Setupts for some embedded targets:
 * <ul>
 *    <li> \ref PrjSpec3_0_VirtualTerminalIso__esx__sys__sys
 *    <li> \ref PrjSpec3_0_VirtualTerminalIso__imi__sys__sys
 *    <li> \ref PrjSpec3_0_VirtualTerminalIso__pm167__sys__sys
 *  </ul>
 * </ul>
 *
 * <H1>Resulting Project File List</H1>
 * See \ref FileLists3_0_VirtualTerminalIso__pc_linux__simulating__simulating for needed files
 * ( filelist__3_0_VirtualTerminalIso.txt ),
 *  with \ref SrcList3_0_VirtualTerminalIso__pc_linux__simulating__simulating containing the needed sources
 *  and with \ref HdrList3_0_VirtualTerminalIso__pc_linux__simulating__simulating containing the needed headers.
 *
 * The resulting file lists for different setups:
 * <ul>
 * <li>Configuration Setups for Linux on PC:
 * <ul>
 *    <li> \ref FileLists3_0_VirtualTerminalIso__pc_linux__msq_server_no_card__rte
 *		<li> \ref FileLists3_0_VirtualTerminalIso__pc_linux__msq_server_rte__rte
 *		<li> \ref FileLists3_0_VirtualTerminalIso__pc_linux__msq_server_A1__rte
 *		<li> \ref FileLists3_0_VirtualTerminalIso__pc_linux__msq_server_pcan__rte
 *    <li> \ref FileLists3_0_VirtualTerminalIso__pc_linux__msq_server_no_card__simulating
 *		<li> \ref FileLists3_0_VirtualTerminalIso__pc_linux__msq_server_rte__simulating
 *		<li> \ref FileLists3_0_VirtualTerminalIso__pc_linux__msq_server_A1__simulating
 *		<li> \ref FileLists3_0_VirtualTerminalIso__pc_linux__msq_server_pcan__simulating
 *    <li> \ref FileLists3_0_VirtualTerminalIso__pc_linux__msq_server_no_card__sys
 *		<li> \ref FileLists3_0_VirtualTerminalIso__pc_linux__msq_server_rte__sys
 *		<li> \ref FileLists3_0_VirtualTerminalIso__pc_linux__msq_server_A1__sys
 *		<li> \ref FileLists3_0_VirtualTerminalIso__pc_linux__msq_server_pcan__sys
 *    <li> \ref FileLists3_0_VirtualTerminalIso__pc_linux__simulating__rte
 *    <li> \ref FileLists3_0_VirtualTerminalIso__pc_linux__simulating__simulating
 *    <li> \ref FileLists3_0_VirtualTerminalIso__pc_linux__simulating__sys
 *  </ul>
 * <li>Configuration Setups for Win32 on PC:
 * <ul>
 *    <li> \ref FileLists3_0_VirtualTerminalIso__pc_win32__can_server_sock__simulating
 *    <li> \ref FileLists3_0_VirtualTerminalIso__pc_win32__can_server_sock__sys
 *  </ul>
 * <li>Configuration Setupts for some embedded targets:
 * <ul>
 *    <li> \ref FileLists3_0_VirtualTerminalIso__esx__sys__sys
 *    <li> \ref FileLists3_0_VirtualTerminalIso__imi__sys__sys
 *    <li> \ref FileLists3_0_VirtualTerminalIso__pm167__sys__sys
 *  </ul>
 * </ul>
 *
 * <H1>Resulting Project Configuration Header</H1>
 * This header is automatically included by xgpl_src/IsoAgLib/isoaglib_config.h
 * if the \#define PRJ_USE_AUTOGEN_CONFIG is set to
 * config_3_0_VirtualTerminalIso ( see also at \ref PrjConfig3_0_VirtualTerminalIso ).
 *                                                                         */
/* *************************************************************************/
/** the define PRJ_USE_AUTOGEN_CONFIG is used by xgpl_src/IsoAgLib/isoaglib_config.h
    to include project specific configuration settings.
    Set this define in the project file or Makefile of the whole
    project, so that each source file is compiled with this setting
  */
#ifndef PRJ_USE_AUTOGEN_CONFIG
  #define PRJ_USE_AUTOGEN_CONFIG config_3_0_VirtualTerminalIso.h
#endif
// include the central interface header for the hardware adaptation layer part
// of the "IsoAgLib"
/* include headers for the needed drivers */
#include <IsoAgLib/driver/system/isystem_c.h>
#include <IsoAgLib/driver/can/icanio_c.h>
/* include the central interface header for the communication layer part
   of the "IsoAgLib" */
#include <IsoAgLib/scheduler/ischeduler_c.h>
#include <IsoAgLib/comm/Part5_NetworkManagement/iidentitem_c.h>
#include <IsoAgLib/comm/Part5_NetworkManagement/iisomonitor_c.h>
#include <IsoAgLib/comm/Part3_DataLink/imultisend_c.h>
#include <IsoAgLib/comm/Part6_VirtualTerminal_Client/ivtclientservercommunication_c.h>
#include <supplementary_driver/driver/datastreams/streaminput_c.h>
#include <cstdlib>
#include <string>
#ifdef DEBUG
  #ifdef SYSTEM_PC
    #include <iostream>
  #else
    #include <supplementary_driver/driver/rs232/irs232io_c.h>
  #endif
#endif
#ifdef SYSTEM_PC
  #ifdef WIN32
    #include <windows.h>
  #endif
#endif

/* the following include direction includes
   all generated ISO Terminal Object Pool Definitions */
#include "MaskDefinition/simpleVTIsoPool_derived-h.h"
#include "MaskDefinition/simpleVTIsoPool_derived-cpp.h"

class MainApplicationClass : public iObjectPool_simpleVTIsoPool_c {
public:
  virtual void eventKeyCode (uint8_t keyActivationCode, uint16_t objId, uint16_t objIdMask, uint8_t keyCode, bool wasButton);
  /* Uncomment the following function if you want to use command-response handling! */
  //virtual void eventPointingEvent (uint16_t aui16_xPosition, uint16_t aui16_yPosition);
  virtual void eventNumericValue (uint16_t objId, uint8_t ui8_value, uint32_t ui32_value);
  virtual void eventStringValue (uint16_t aui16_objId, uint8_t aui8_length, StreamInput_c &rc_streaminput, uint8_t aui8_unparsedBytes, bool b_isFirst, bool b_isLast);
  /* Uncomment the following function if you want to use input value string on-the-fly parsing/handling! */
  //virtual void eventStringValueAbort();
  virtual void eventObjectPoolUploadedSuccessfully (bool ab_wasLanguageUpdate, int8_t ai8_languageIndex, uint16_t aui16_languageCode);
  // virtual void eventVtSelectInputObject(uint16_t aui16_objectId, uint8_t aui8_hasFocus, uint8_t aui8_selected);
  //virtual void eventPrepareForLanguageChange (int8_t ai8_languageIndex, uint16_t aui16_languageCode);
  virtual void eventEnterSafeState ();
  /* Uncomment the following function if you want to use command-response handling! */
  //virtual void eventCommandResponse (uint8_t aui8_responseCommandError, const uint8_t apui8_responseDataBytes[8]);
  /* Uncomment the following function if you want to use a special colour-conversion! */
  virtual uint8_t convertColour (uint8_t colourValue, uint8_t colourDepth, IsoAgLib::iVtObject_c* obj, IsoAgLib::e_vtColour whichColour);
  /* Uncomment the following function if you want to react on any incoming LANGUAGE_PGN */
  virtual void eventLanguagePgn (const localSettings_s& ars_localSettings);
  /* Uncomment the following function if you want to react on any incoming VT Status messages */
  //virtual void eventVtStatusMsg();
  /* Uncomment the following function if you want to react on any incoming VT ESC */
  //virtual void eventVtESC(uint16_t /*aui16_ObjectId*/);
  /* Uncomment the following function if you want to react on any incoming Auxiliary Input Status messages */
  //virtual void eventAuxFunctionValue (uint16_t mui16_functionUid, uint16_t cui16_inputValueAnalog, uint16_t cui16_inputValueTransitions, uint8_t cui8_inputValueDigital);
  /* Uncomment the following function if you want to react on any incoming VT Get Attribute Value messages */
  //virtual void eventAttributeValue(IsoAgLib::iVtObject_c* obj, uint8_t ui8_attributeValue, uint8_t* pui8_value);
};


// the interface objects of the IsoAgLib are placed in the IsoAgLib namespace
// -> include all elements of this area for easy access
// with this command the text part "IsoAgLib::" can be avoided, which
// is needed for the documentation generator
using namespace IsoAgLib;

static MainApplicationClass Tutorial_3_0_Pool_c;
static iVtClientServerCommunication_c* spc_tut30csc;

/// Things needed for the Partial Pool Update following...
iVtObject_c* arrpc_vtObjectsToUpdate[] =
{
  &iVtObjectValSpeed,
  &iVtObjectValAccel,
  &iVtObjectBigLogo
};
static const unsigned int scui_newLogoWidth=16;
static const unsigned int scui_newLogoHeight=16;
static uint8_t newLogoBuffer [scui_newLogoWidth*scui_newLogoHeight];

// normal program variables (that are also used as source for the vt-display
static int16_t valSpeed=0,
               valMiles=0,
               valAccel=10,
               color=0,
               like=0;
// for changeAttribute when pressing on "Change Colour" button
static iVtObjectStringVariable_c *colTable [9] =
{ &iVtObjectStrNone
, &iVtObjectStrRed
, &iVtObjectStrGreen
, &iVtObjectStrBlue
, &iVtObjectStrYellow
, &iVtObjectStrCyan
, &iVtObjectStrMagenta
, &iVtObjectStrBlack
, &iVtObjectStrWhite
};
static uint8_t fgcolTable [9] = {0, 12, 2, 9, 14, 11, 13, 0, 1};
// some small helper functions
void updateMiles (uint32_t aui32_value) {
  valMiles = aui32_value;
  int16_t angle;
  angle = (180*valMiles)>>12;
  iVtObjectdEllipse.setEndAngle ((angle==0)?1:angle);
  iVtObjectValMiles.setValue (valMiles+10000);
}
void updateAccel(int32_t ai32_value) {
  valAccel = ai32_value;
  iVtObjectValAccel.setValue (valAccel+10000);
  iVtObjectAccelArchedBarGraph.setValue (valAccel +10000);
}

/******************************/
/* Example Code following.... */
/******************************/
/**
  hook function that gets called every time a color-value
  has to be adapted to VT's color-depth.
  --> Standard implementation will simply clip all color-values to
  BLACK (Color 0) besides the background/transparency colors to WHITE (Color 1)
  This overloaded function will set all clipped colors to BLACK (Color 0) instead!
  @param colorValue The color-value that was originally defined in the object
  @param colorDepth 0 for 1bit-color depth (2-colored VT, black/white)
                    1 for 4bit-color depth (16-colored VT)
                    2 for 8bit-color depth (256-colored VT)
  @param obj Reference to the object that's color's to be converted, use it for distinguishing a little more...
  @param whichColour Type of colour: BackgroundColour, LineColour, NeedleColour, etc. (See IsoAgLib::e_vtColour)
*/
uint8_t MainApplicationClass::convertColour(uint8_t colorValue, uint8_t colorDepth, IsoAgLib::iVtObject_c* /*obj*/, IsoAgLib::e_vtColour whichColour)
{
  if (colorDepth == 0 /* 2colored b/w */) {
    if ((whichColour == BackgroundColour) || (whichColour == TransparencyColour))
      return 1; /* white - std. background/transparency colour */
    else
      return 0; /* black - std. drawing colour */
  } else {
    // colorDepth == 1, as there's no color-violation in the case of colorDepth==2 !
    // this is just for the example. convert the colors that can be converted by 100% from 16..231 to color 0..15.
    // one also could convert using a best-match algorithm to colors 0..15 - this is just to show the principle.
    switch (colorValue) {
      case 16  /* 00,00,00 */: return 0;
      case 231 /* FF,FF,FF */: return 1;
      case 34  /* 00,99,00 */: return 2;
      case 37  /* 00,99,99 */: return 3;
      case 124 /* 99,00,00 */: return 4;
      case 127 /* 99,00,99 */: return 5;
      case 142 /* 99,99,00 */: return 6;
      case 188 /* CC,CC,CC */: return 7;
      case 145 /* 99,99,99 */: return 8;
      case 21  /* 00,00,FF */: return 9;
      case 46  /* 00,FF,00 */: return 10;
      case 51  /* 00,FF,FF */: return 11;
      case 196 /* FF,00,00 */: return 12;
      case 201 /* FF,00,FF */: return 13;
      case 226 /* FF,FF,00 */: return 14;
      case 19  /* 00,00,99 */: return 15;
      /* todo: best match the rest also to 0..15 !*/
      default: if ((whichColour == BackgroundColour) || (whichColour == TransparencyColour))
                 return 1; /* white - std. background/transparency colour */
               else
                 return 0; /* black - std. drawing colour */
    }
  }
}

// handle incoming number-inputs from vt
void MainApplicationClass::eventNumericValue ( uint16_t objId, uint8_t ui8_value, uint32_t ui32_value )
{
  switch (objId) {
    case iVtObjectIDInputMiles:
      updateMiles (ui32_value);
      break;
    case iVtObjectIDInputLikeage:
      like = ui8_value;
      if (like == 1) iVtObjectLikeageOP.setValue (&iVtObjectLikeIt);
                else iVtObjectLikeageOP.setValue (&iVtObjectLikeItNot);
      break;
    case iVtObjectIDInputListTypFakeMiles:
      /* +++ Showing what can be done in one statement: */
      updateMiles ((static_cast<iVtObjectOutputNumber_c*>(iVtObjectInputListTypFakeMiles.getListItem (ui8_value))->get_vtObjectOutputNumber_a()).value);
      /* +++ Showing the same in four lines, it may be easier to read/understand this way
      // get the selected object from the input list
      iVtObject_c* selectedObject = iVtObjectInputListTypFakeMiles.getListItem (ui8_value);
      // we know it's an iVtObjectOutputNumber, so it's save to cast
      iVtObjectOutputNumber_c* outputNumber = (iVtObjectOutputNumber_c *) selectedObject;
      // now get access to the data in the object
      int32_t value = outputNumber->get_vtObjectOutputNumber_a()->value;
      // now do whatever one wants to do with the value...
      updateMiles (value);
      */
      break;
  }
}


// incoming key-events
void MainApplicationClass::eventKeyCode ( uint8_t keyActivationCode, uint16_t /*objId*/, uint16_t /*objIdMask*/, uint8_t keyCode, bool /*wasButton*/ )
{
/* just for your information! - defines are to be found in the "ivttypes.h" include!
  #define BUTTON_HAS_BEEN_UNLATCHED 0
  #define BUTTON_HAS_BEEN_PRESSED 1
  #define BUTTON_HAS_BEEN_LATCHED 1
  #define BUTTON_IS_STILL_HELD 2
  #define KEY_HAS_BEEN_RELEASED 0
  #define KEY_HAS_BEEN_PRESSED 1
  #define KEY_IS_STILL_HELD 2
*/
  // if only interested in xxx_PRESSED you don't have to distinguish between keys/buttons
  // if (wasButton) { // was button
  // } else { // was key
  // }
  if (keyActivationCode == KEY_HAS_BEEN_PRESSED) {   // == BUTTON_HAS_BEEN_PRESSED
    switch (keyCode) {
      case vtKeyCodeKeyReset:
        valSpeed = 0;
        updateAccel (10);
        updateMiles (0);
        iVtObjectValSpeed.setValue (valSpeed+10000);
        iVtObjectcontainerInAllMasks.setChildPosition (&iVtObjectBigLogo, 0, 0); // reset logo position to left upper corner
        break;
      case vtKeyCodeKeyChangeFill:
        if (iVtObjectdPolygon.get_vtObjectPolygon_a().fillAttributes == NULL)
        { // object was set to NULL as fill-attributes, so toggle it to "iVtObjectFillAttributes"
          iVtObjectdPolygon.setFillAttributes(&iVtObjectFillAttributes, true); // true == update object (needed for the if-clause above!)
        }
        else
        { // object was already set to "iVtObjectFillAttributes", so toggle it to NULL
          iVtObjectdPolygon.setFillAttributes(NULL, true); // true == update object (needed for the if-clause above!)
        }
        break;
      case vtKeyCodeKeyUpdatePool:
        { /// @todo OBSOLETE_WITH_NEW_TUTORIAL - Needs to be done correctly in the new tutorial with the new event-handler
          /// We should wait until a previous partial update has finished,
          /// as else we would modify the buffer while it's being used for the pool update!
          // this actually only needs to be done once!!! but I don't care for now...
          iVtObjectBigLogo.setRawData2 (newLogoBuffer, (sizeof (newLogoBuffer) / sizeof (uint8_t)), false, scui_newLogoWidth, scui_newLogoHeight, 32);
          for (unsigned int y=0; y < scui_newLogoHeight; y++)
            for (unsigned int x=0; x < scui_newLogoWidth; x++)
              newLogoBuffer [x+y*scui_newLogoHeight] = valSpeed + (x+y*scui_newLogoHeight); // write some nice pattern (depending on SPEED) in there...
          spc_tut30csc->sendCommandUpdateObjectPool (arrpc_vtObjectsToUpdate, sizeof(arrpc_vtObjectsToUpdate)/sizeof(iVtObject_c*));
        }
        break;
      case vtKeyCodeKeyMove:
        valSpeed += valAccel;
        updateMiles(valMiles + valSpeed);
        iVtObjectValSpeed.setValue (valSpeed+10000);
        break;
      case vtKeyCodeKeyMoreAccel:
        updateAccel (valAccel + 1);
        break;
      case vtKeyCodeKeyLessAccel:
        updateAccel (valAccel - 1);
        break;
      case vtKeyCodeKeyChangeLogoPosition:
        iVtObjectcontainerInAllMasks.setChildPosition (&iVtObjectBigLogo, HAL::getTime()%100, ((HAL::getTime()+2712)%64)-32); // move to kinda random position
        break;
      // Use b_updateObject here to save and access the hidden state directly via the object!
      case vtKeyCodeKeyLogo:
        if (!(iVtObjectcontainerInAllMasks.get_vtObjectContainer_a().hidden)) iVtObjectcontainerInAllMasks.hide (true);
        else iVtObjectcontainerInAllMasks.show (true);
        break;
      case vtKeyCodeKeyChangeCol:
        color += 1; if (color > 8) color = 1;
        iVtObjectColOS.setVariableReference (colTable [color]);
        iVtObjectFontAttributesNormal6x8.setFontColour (fgcolTable [color]);
        break;
      case vtKeyCodeKeyTransCol:
        iVtObjectBigLogo.setTransparencyColour ((iVtObjectBigLogo.get_vtObjectPictureGraphic_a().transparencyColour == 1) ? 11 : 1, /* updateObject:*/ true);
        break;
      case vtKeyCodeKeyChangeLang:
        // Attention: We're assuimung the the variable reference is NOT being used - else we would access a NULL pointer in "value"
        /// ATTENTION: Strings are always stored with PADDED SPACES, so consider this in the case of the string-compare!
        if (CNAMESPACE::strcmp (iVtObjectColLabel.get_vtObjectOutputString_a().value, "Color: ") == 0)
        { // Change from AE->BE
          iVtObjectColLabel.setValueRef ("Colour:", true); // setValueRef makes most sense on static strings like here
        }
        else
        { // Change from BE->AE (remember: "Color:" will be padded to the string-length and will become "Color: "
          iVtObjectColLabel.setValueCopy ("Color:", true); // but just to demonstrate setValueCopy also does the job, although it has a different target use...
        }
        break;
      case vtKeyCodeKeyForbidden:
        iVtObjectimiIsoMaskupload.changeActiveMask (&iVtObjectForbiddenAlarmMask);
        break;
      case vtKeyCodeACK:
        iVtObjectimiIsoMaskupload.changeActiveMask (&iVtObjectMainMask);
        break;
    }
  }
}
// has to be implemented - remember that if the VT drops out and comes again, the values have to be up2date!!!
void MainApplicationClass::eventObjectPoolUploadedSuccessfully (bool ab_wasLanguageUpdate, int8_t ai8_languageIndex, uint16_t aui16_languageCode)
{
  //! The language-code/index given here indicates that the Objectpool Upload or Update has finished NOW with exactly THIS given language!
  //! --> There's a difference to "eventLanguagePgn", because "eventLanguagePgn" will be called IMMEDIATELY when the user changes the
  //! --> language in the VT setup - but it may take some time until the pool is updated and remember that you can probably select more
  //! --> languages than your working-set supports!
  if (ab_wasLanguageUpdate)
  {
    /// The update takes place very fast here, so we don't need to perform anything here. Normally one would switch back to normal operation mask
    /// when it was switched on update to some "Wait while updating language..:" screen!
    #ifdef DEBUG
    EXTERNAL_DEBUG_DEVICE << "-->eventObjectPoolUploadedSuccessfully: LANGUAGE UPDATE TO Index "<<int(ai8_languageIndex)<<". User tried to select ["<<uint8_t(aui16_languageCode>>8)<<uint8_t(aui16_languageCode&0xFF)<<"] <--" << EXTERNAL_DEBUG_DEVICE_ENDL;
    #else
    aui16_languageCode = aui16_languageCode;  // Just prevent from warning
    ai8_languageIndex = ai8_languageIndex;    // Just prevent from warning
    #endif
  }
  else
  {
    iVtObjectColLabel.setValueRef ("Color:"); // this is done so the initial state is up again if VT lost and reconnected!
    iVtObjectColOS.setVariableReference (colTable [color], true);
    iVtObjectFontAttributesNormal6x8.setFontColour (fgcolTable [color], true);
    if (iVtObjectcontainerInAllMasks.get_vtObjectContainer_a().hidden) iVtObjectcontainerInAllMasks.hide ();
    updateAccel (valAccel);
    updateMiles (valMiles);
    iVtObjectValSpeed.setValue (valSpeed+10000);
    #ifdef DEBUG
    EXTERNAL_DEBUG_DEVICE << "-->eventObjectPoolUploadedSuccessfully: INITIAL UPLOAD TO Index "<<int(ai8_languageIndex)<<". User tried to select ["<<uint8_t(aui16_languageCode>>8)<<uint8_t(aui16_languageCode&0xFF)<<"] <--" << EXTERNAL_DEBUG_DEVICE_ENDL;
    #endif
  }
}
void MainApplicationClass::eventEnterSafeState ()
{
  // Nothing done here for now. (Commands being sent out to the VT are ignored by IsoTerminalServer_c)
  // As it's a simple Tutorial example there's nothing in real danger!
  // But take care of this function if using for real!!!
  #ifdef DEBUG
  EXTERNAL_DEBUG_DEVICE << "-->eventEnterSafeState<--" << EXTERNAL_DEBUG_DEVICE_ENDL;
  #endif
}
void MainApplicationClass::eventStringValue (uint16_t aui16_objId, uint8_t aui8_length, StreamInput_c &rc_streaminput, uint8_t /*aui8_unparsedBytes*/, bool /*b_isFirst*/, bool b_isLast)
{
  if (b_isLast)
  {
    // buffer anlegen mit length + 1
    STL_NAMESPACE::string c_buffer;
    for (uint8_t ind = 0;ind < aui8_length;ind++)
    {
      c_buffer.push_back( rc_streaminput.get() );
    }
    switch (aui16_objId)
    {
      case iVtObjectIDISdriver:
      {
        iVtObjectOSresonible.setValueCopy (c_buffer.c_str());
      } break;
      default:
        break;
    }
  }
}
void
MainApplicationClass::eventLanguagePgn(const localSettings_s& ars_localSettings)
{
  /// THIS FUNCTION SHOULD ONLY BE USED FOR CHANGE IN UNITS, ETC.
  /// FOR LANGUAGE CHANGE, REFER TO --> "eventObjectPoolUploadedSuccessfully" <--
  /// Remeber: If the language changed, it may take some time until your object-pool is updated in this language
  ///          You'll get a notification on an updated object-pool in "eventObjectPoolUploadedSuccessfully"!
  char languageCode[2+1]; languageCode[2+1-1] = 0x00;
  languageCode[0] = ars_localSettings.languageCode >> 8;
  languageCode[1] = ars_localSettings.languageCode & 0xFF;
  iVtObjectOSlanguage.setValueCopy (languageCode);
}


int main()
{
  getIcanInstance().init (0); // CAN-Bus 0 (with defaulting 250 kbit)
  // start address claim of the local identity/member
  IsoAgLib::iIdentItem_c c_myIdent (2,    // aui8_indGroup
                                    7,    // aui8_devClass
                                    0,    // aui8_devClassInst
                                    25,   // ab_func
                                    0x7FF,// aui16_manufCode
                                    27,   // aui32_serNo
                                    254,  // No preferred SA, auto-allocate in range 0x80 upwards...
                                    0x100,// EEPROM-Address to store the allocated SA for re-use after power-cycle
                                    0,    // ab_funcInst
                                    0,    // ab_ecuInst
                                    true, // ab_selfConf
                                    0);   // 0 means: We're WS-Master and have 0 WS-Slaves
                                    // further parameters use the default: NULL /* so no list given either */, 0 /* singletonVecKey */
  // the following line can be used to retrieve the reference to the used ISOName.
  // Attention: while not announced, some instance fields can change!
  // That's why you get a reference to the ISOName!
  /// IsoAgLib::iIsoName_c const &myISOName = c_myIdent.isoName();
  // Call to init iIsoTerminal instance and initialize object pool!
  spc_tut30csc = getIisoTerminalInstance().initAndRegisterIsoObjectPool (c_myIdent, Tutorial_3_0_Pool_c, "T30v2"); // PoolName: Tutorial 3.0 Version 2
  // only use 5 chars as the pool supports Multi-Language (the last 2 chars are used for the language-code then!
  if (spc_tut30csc == NULL)
  { // shouldln't happen normally!
    #if defined(DEBUG) && defined(SYSTEM_PC)
    EXTERNAL_DEBUG_DEVICE << "Could not registerIsoObjectPool()." << EXTERNAL_DEBUG_DEVICE_ENDL;
    MACRO_ISOAGLIB_ABORT();
    #else
    return 1; // Return from main() with error indicated
    #endif
  }
  /** IMPORTANT:
    - The following loop could be replaced of any repeating call of
      IsoAgLib::getISchedulerInstance().timeEvent();
      which is needed to perform all internal activities of the IsoAgLib.
    - Define the time intervall for IsoAgLib::getISchedulerInstance().timeEvent()
      in a way, that allows IsoAgLib to trigger all reactions on BUS
      in the ISO 11783 defined time resolution - especially the address
      claim process has some tight time restrictions, that suggest
      a trigger rate of at least 100msec ( you could call the function
      only during address claim, mask updload and other special
      circumstances in a high repetition rate )
    - The main loop is running until iSystem_c::canEn() is returning false.
      This function can be configured by the \#define CONFIG_BUFFER_SHORT_CAN_EN_LOSS_MSEC
      in isoaglib_config.h to ignore short CAN_EN loss.
    - This explicit control of power state without automatic powerdown on CanEn loss
      can be controled with the central config define
      \#define CONFIG_DEFAULT_POWERDOWN_STRATEGY IsoAgLib::PowerdownByExplcitCall
      or
      \#define CONFIG_DEFAULT_POWERDOWN_STRATEGY IsoAgLib::PowerdownOnCanEnLoss
      in the header xgpl_src/IsoAgLib/isoaglib_config.h
    - This can be also controlled during runtime with the function call:
      getIsystemInstance().setPowerdownStrategy( IsoAgLib::PowerdownByExplcitCall )
      or
      getIsystemInstance().setPowerdownStrategy( IsoAgLib::PowerdownOnCanEnLoss )
  */
  int32_t i32_idleTimeSpread = 0;
  while ( iSystem_c::canEn() )
  { // run main loop
    // IMPORTANT: call main timeEvent function for
    // all time controlled actions of IsoAgLib
    i32_idleTimeSpread = IsoAgLib::getISchedulerInstance().timeEvent();
    // The following sleep mechanism will be changed with the new scheduler reporting back the idle-time
    if ( i32_idleTimeSpread > 0 ) IsoAgLib::iCanIo_c::waitUntilCanReceiveOrTimeout( i32_idleTimeSpread );
  }
}

