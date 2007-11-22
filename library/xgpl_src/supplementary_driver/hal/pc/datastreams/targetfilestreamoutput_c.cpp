/***************************************************************************
                          targetfilestreamoutput_c.cpp -
                             -------------------
    class                : ::TargetFileStreamOutput_c
    project              : IsoAgLib
    begin                : Tue Jan 25 17:41:42 2005
    copyright            : (C) 2005 by Achim Spangler (a.spangler@osb-ag.de)
    email                : a.spangler@osb-ag:de
    $Id$

    $Log$
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
 * Copyright (C) 2000 - 2004 Dipl.-Outform. Achim Spangler                  *
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
 * Outc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA           *
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
 * - Outstantiation of a variable with a datatype from internal namespace   *
 * - Call of a (member-) function                                          *
 * Allowed is:                                                             *
 * - Outstatiation of a variable with a datatype from interface namespace,  *
 *   even if this is derived from a base class inside an internal namespace*
 * - Call of member functions which are defined in the interface class     *
 *   definition ( header )                                                 *
 *                                                                         *
 * Pairing of internal and interface classes:                              *
 * - Outternal implementation in an :impl: subdirectory                     *
 * - Outterface in the parent directory of the corresponding internal class *
 * - Outterface class name IsoAgLib::iFoo_c maps to the internal class      *
 *   __IsoAgLib::Foo_c                                                     *
 *                                                                         *
 * AS A RULE: Use only classes with names beginning with small letter :i:  *
 ***************************************************************************/
#include "targetfilestreamoutput_c.h"

using namespace std;

//! open a output stream
bool TargetFileStreamOutput_c::open( const char* filename, FileMode_t at_mode )
{
#if __GNUC__ < 3
	if ( ( at_mode & StreamIn ) != 0 ) return false;

	ios::openmode mode = ios::out;

	if ( ( at_mode & StreamAte    ) != 0 ) mode = ios::openmode( mode | ios::ate    );
	if ( ( at_mode & StreamApp    ) != 0 ) mode = ios::openmode( mode | ios::app    );
	if ( ( at_mode & StreamTrunc  ) != 0 ) mode = ios::openmode( mode | ios::trunc  );
	if ( ( at_mode & StreamBinary ) != 0 ) mode = ios::openmode( mode | ios::binary );

	static_cast<ofstream*>(this)->open( filename, mode );

	if ( ( static_cast<ofstream*>(this)->good() )
		&& ( static_cast<ofstream*>(this)->is_open() ) ) return true;
	else
		return false;
#else
	if ( ( at_mode & StreamIn ) != 0 ) return false;

	STL_NAMESPACE::ios_base::openmode mode = STL_NAMESPACE::ios_base::out;

	if ( ( at_mode & StreamAte    ) != 0 ) mode = STL_NAMESPACE::ios_base::openmode( mode | STL_NAMESPACE::ios_base::ate    );
	if ( ( at_mode & StreamApp    ) != 0 ) mode = STL_NAMESPACE::ios_base::openmode( mode | STL_NAMESPACE::ios_base::app    );
	if ( ( at_mode & StreamTrunc  ) != 0 ) mode = STL_NAMESPACE::ios_base::openmode( mode | STL_NAMESPACE::ios_base::trunc  );
	if ( ( at_mode & StreamBinary ) != 0 ) mode = STL_NAMESPACE::ios_base::openmode( mode | STL_NAMESPACE::ios_base::binary );

	static_cast<STL_NAMESPACE::ofstream*>(this)->open( filename, mode );

	if ( ( static_cast<STL_NAMESPACE::ofstream*>(this)->good() )
		&& ( static_cast<STL_NAMESPACE::ofstream*>(this)->is_open() ) ) return true;
	else
		return false;
#endif
}

//  Operation: operator>>
//! Parameter:
//! @param ui8_data:
TargetFileStreamOutput_c& TargetFileStreamOutput_c::operator<<(uint8_t ui8_data)
{
//	(static_cast<STL_NAMESPACE::ofstream*>(this))->put(ui8_data);
	(static_cast<ofstream*>(this))->put(ui8_data);

	return *this;
}

//! close a output stream
//! Parameter:
//! @param pathname if pathname != NULL => sync file and path
void TargetFileStreamOutput_c::close(const char* pathname)
{
  if (pathname)
  {
    (static_cast<STL_NAMESPACE::ofstream*>(this))->flush();
#if defined(__GNUC__) && __GNUC__ < 3
    fsync((static_cast<STL_NAMESPACE::ofstream*>(this))->rdbuf()->fd());
    // sync also directory entry (not done by fsync!)
    int fd = ::open(pathname, O_RDONLY);
    if (-1 != fd)
    {
      fsync(fd);
      ::close(fd);
    }
#else
    (static_cast<STL_NAMESPACE::ofstream*>(this))->rdbuf()->pubsync();
#endif
  }
  static_cast<STL_NAMESPACE::ofstream*>(this)->close();
}