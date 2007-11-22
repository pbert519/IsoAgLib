/***************************************************************************
                          actoro_c.cpp  - implementation for the actor output
                                        management object
                             -------------------
    begin                : Mon Oct 25 1999
    copyright            : (C) 1999 - 2004 by Dipl.-Inform. Achim Spangler
    email                : a.spangler@osb-ag:de
    type                 : Source
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
 * Copyright (C) 1999 - 2004 Dipl.-Inform. Achim Spangler                  *
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
 *                                                                         *
 * As a special exception, if other files instantiate templates or use     *
 * macros or inline functions from this file, or you compile this file and *
 * link it with other works to produce a work based on this file, this file*
 * does not by itself cause the resulting work to be covered by the GNU    *
 * General Public License. However the source code for this file must still*
 * be made available in accordance with section (3) of the                 *
 * GNU General Public License.                                             *
 *                                                                         *
 * This exception does not invalidate any other reasons why a work based on*
 * this file might be covered by the GNU General Public License.           *
 *                                                                         *
 * Alternative licenses for IsoAgLib may be arranged by contacting         *
 * the main author Achim Spangler by a.spangler@osb-ag:de                  *
 ***************************************************************************/

 /**************************************************************************
 *                                                                         *
 *     ###    !!!    ---    ===    IMPORTANT    ===    ---    !!!    ###   *
 * Each software module, which accesses directly elements of this file,    *
 * is considered to be an extension of IsoAgLib and is thus covered by the *
 * GPL license. Applications must use only the interface definition out-   *
 * side :impl: subdirectories. Never access direct elements of __IsoAgLib  *
 * and __HAL namespaces from applications which shouldnt be affected by    *
 * the license. Only access their interface counterparts in the IsoAgLib   *
 * and HAL namespaces. Contact a.spangler@osb-ag:de in case your applicat- *
 * ion really needs access to a part of an internal namespace, so that the *
 * interface might be extended if your request is accepted.                *
 *                                                                         *
 * Definition of direct access:                                            *
 * - Instantiation of a variable with a datatype from internal namespace   *
 * - Call of a (member-) function                                          *
 * Allowed is:                                                             *
 * - Instatiation of a variable with a datatype from interface namespace,  *
 *   even if this is derived from a base class inside an internal namespace*
 * - Call of member functions which are defined in the interface class     *
 *   definition ( header )                                                 *
 *                                                                         *
 * Pairing of internal and interface classes:                              *
 * - Internal implementation in an :impl: subdirectory                     *
 * - Interface in the parent directory of the corresponding internal class *
 * - Interface class name IsoAgLib::iFoo_c maps to the internal class      *
 *   __IsoAgLib::Foo_c                                                     *
 *                                                                         *
 * AS A RULE: Use only classes with names beginning with small letter :i:  *
 ***************************************************************************/
#include "actoro_c.h"
#include <IsoAgLib/util/iliberr_c.h>
#include <IsoAgLib/util/config.h>
#include <IsoAgLib/driver/system/impl/system_c.h>

// Begin Namespace __IsoAgLib
namespace __IsoAgLib {
/** C-style function, to get access to the unique ActorO_c singleton instance */
ActorO_c& getActorInstance( void ) { return ActorO_c::instance();};

/**
  initialisation for the actor output management which sets the allowed number
  ranges for digital ouput channels.
  As the constructor is often not called for static instances, the init function
  is used by the Singleton base class, to set the unique instance in a well defined
  initial state

  possible errors:
      * Err_c::range given limits are not possible
  @param ab_digitalFirst smallest allowed digital output channel number (DIGITAL_OUTPUT_MIN)
  @param ab_digitalLast greatest allowed digital output channel number (DIGITAL_OUTPUT_MAX)
  @see masterHeader
*/
void ActorO_c::init(uint8_t ab_digitalFirst, uint8_t ab_digitalLast)
{ // store the channel limits for dig and analog
  // verify that System is int
  getSystemInstance().init();
  setDigitalLimits(ab_digitalFirst, ab_digitalLast);
}

/** destructor for the actor output manager object */
ActorO_c::~ActorO_c(){
}

/**
  set the limits for digital output channels (first setting can be done by constructor parameters)

  possible errors:
      * Err_c::range given limits are not possible
  @param ab_digitalFirst number of the smallest allowed digital output channel
  @param ab_digitalLast number of the greatest allowed digital output channel
*/
void ActorO_c::setDigitalLimits(uint8_t ab_digitalFirst, uint8_t ab_digitalLast){
  // check if output values are correct
  if (
       (ab_digitalFirst <= DIGITAL_OUTPUT_MAX)
#ifdef DIGITAL_OUTPUT_MIN_GREATER_ZERO
     &&(ab_digitalFirst >= DIGITAL_OUTPUT_MIN)
     &&(ab_digitalLast >= DIGITAL_OUTPUT_MIN)
#endif
     &&(ab_digitalLast <= DIGITAL_OUTPUT_MAX)
     )
  { // correct range
    setMinDigitalLimit(ab_digitalFirst);
    setMaxDigitalLimit(ab_digitalLast);
  }
  else
  { // wrong range
    getILibErrInstance().registerError( iLibErr_c::Range, iLibErr_c::Actor );
  }
}
/** handler function for access to undefined client.
  * the base Singleton calls this function, if it detects an error
  */
void ActorO_c::registerAccessFlt( void )
{
  getILibErrInstance().registerError( iLibErr_c::ElNonexistent, iLibErr_c::Actor );
  // throw exception if defined to do
  THROW_CONT_EL_NONEXIST
}

} // end of namespace __IsoAgLib