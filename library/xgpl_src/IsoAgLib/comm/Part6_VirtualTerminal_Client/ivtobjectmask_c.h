/*
  ivtobjectmask_c.h

  (C) Copyright 2009 - 2011 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/
#ifndef IVTOBJECTMASK_C_H
#define IVTOBJECTMASK_C_H

#include "impl/vtobject_c.h"

// Begin Namespace __IsoAgLib
namespace IsoAgLib {

class iVtObjectMask_c : public __IsoAgLib::vtObject_c
{
public:
  //  Operation: setSoftKeyMask
  //! @param newSoftKeyMask:
  //! @param b_updateObject:
  virtual void setSoftKeyMask(iVtObjectSoftKeyMask_c* newSoftKeyMask,
                              bool b_updateObject= false, bool b_enableReplaceOfCmd=false) = 0;

  virtual ~iVtObjectMask_c() {}
};

} // end of namespace __IsoAgLib

#endif
