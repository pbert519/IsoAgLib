/***************************************************************************
                          vt2iso-globals.h
                             -------------------
    begin                : Mon Mar 31 2003
    copyright            : (C) 2003 - 2004 by Dipl.-Inf.(FH)
    email                : eva.erdmannk@osb-ag:de
    type                 : Header
    $LastChangedDate: 2004-10-01 13:51:42 +0200 (Fri, 01 Oct 2004) $
    $LastChangedRevision: 426 $
 ***************************************************************************/

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
#ifndef VT2ISGLOBALS_H
#define VT2ISGLOBALS_H

#include "vt2iso-defines.hpp"
#include <iostream>
#include <string>



extern bool itocolour(unsigned int ui_index, std::string& c_outputText);
extern bool itofontsize(unsigned int ui_index, std::string& c_outputText);
extern bool itofonttype(unsigned int ui_index, std::string& c_outputText);
extern bool itoformat(unsigned int ui_index, std::string& c_outputText);
extern bool itohorizontaljustification(unsigned int ui_index, std::string& c_outputText);
extern bool itoverticaljustification(unsigned int ui_index, std::string& c_outputText);
extern bool itopolygontype(unsigned int ui_index, std::string& c_outputText);
extern bool itoellipsetype(unsigned int ui_index, std::string& c_outputText);
extern bool itoacousticsignal(unsigned int ui_index, std::string& c_outputText);
extern bool itopriority(unsigned int ui_index, std::string& c_outputText);
extern bool itoauxfunctiontype(unsigned int ui_index, std::string& c_outputText);
extern bool itofilltype(unsigned int ui_index, std::string& c_outputText);
extern bool itoevent(unsigned int ui_index, std::string& c_outputText);
extern bool itovalidationtype(unsigned int ui_index, std::string& c_outputText);
extern bool itolineardirection(unsigned int ui_index, std::string& c_outputText);
extern bool itometeroptions(uint8_t ui8_options, std::string& c_outputText);
extern bool itoarchedbargraphoptions(uint8_t ui8_options, std::string& c_outputText);
extern bool itostringoptions(uint8_t ui8_options, std::string& c_outputText);
extern bool itoinputnumberoptions(uint8_t ui8_options, std::string& c_outputText);
extern bool itonumberoptions(uint8_t ui8_options, std::string& c_outputText);
extern bool itopicturegraphicoptions(uint8_t ui8_options, std::string& c_outputText);
extern bool itopicturegraphicrle(uint8_t ui8_options, std::string& c_outputText);
extern bool itolinearbargraphoptions(uint8_t ui8_options, std::string& c_outputText);
extern bool itogcoptions(uint8_t ui8_options, std::string& c_outputText);
extern bool itoinputobjectoptions(uint8_t ui8_options, std::string& c_outputText);
extern bool itobuttonoptions(uint8_t ui8_options, std::string& c_outputText);
extern bool itolinesuppression(uint8_t ui8_options, std::string& c_outputText);
extern bool itofontstyle(uint8_t ui8_options, std::string& c_outputText);
extern bool itolineart(int i_lineart, std::string& c_outputText);
extern bool itocolourdepth(uint8_t ui8_options, std::string& c_outputText);
extern bool itomacrocommand(uint8_t ui8_command, std::string& c_outputText);


extern int colourtoi (const char* text_colour);
extern int masktypetoi (const char* masktype);
extern int colourdepthtoi (const char* text_colourdepth);
extern signed int fonttypetoi (const char* text_fonttype);
extern signed int booltoi (const char *text_bool);
extern signed int fontsizetoi (const char *text_fontsize);
extern signed int formattoi (const char *text_format);
extern signed int horizontaljustificationtoi (const char *text_horiz);
extern signed int verticaljustificationtoi (const char *text_vert);
extern unsigned int stringoptionstoi (const char *text_options);
extern unsigned int inputnumberoptionstoi (const char *text_options);
extern unsigned int numberoptionstoi (const char *text_options);
extern unsigned int picturegraphicoptionstoi (const char *text_options);
extern unsigned int picturegraphicrletoi (const char *text_options);
extern unsigned int meteroptionstoi (const char *text_options);
extern unsigned int linearbargraphoptionstoi (const char *text_options);
extern unsigned int archedbargraphoptionstoi (const char *text_options);
extern signed int prioritytoi (const char *text_priority);
extern signed int acousticsignaltoi (const char *text_acousticsignal);
extern unsigned int fontstyletoi (const char *text_fontstyle);
extern unsigned int linedirectiontoi (const char *text_linedirection);
extern unsigned int linearttoi (const char *text_lineart);
extern unsigned int linesuppressiontoi (const char *text_linesuppression);
extern unsigned int ellipsetypetoi (const char *text_ellipsetype);
extern unsigned int polygontypetoi (const char *text_polygontype);
extern unsigned int validationtypetoi (const char *text_validationtype);
extern unsigned int filltypetoi (const char *text_filltype);
extern unsigned int eventtoi (const char *text_eventName);
extern unsigned int auxfunctiontypetoi(const char *text_auxFunctionType);
extern unsigned int gcoptionstoi (const char *text_options);
extern unsigned int inputobjectoptiontoi (const char *text_inputobjectoptions);
extern unsigned int buttonoptiontoi (const char *text_buttonoptions);

#endif // #endif VT2ISGLOBALS_H
