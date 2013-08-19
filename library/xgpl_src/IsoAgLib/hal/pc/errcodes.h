/*
  errcodes.h: header for the PSEUDO BIOS error codes

  (C) Copyright 2009 - 2013 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/
#ifndef __ERRCODES_H__
#define __ERRCODES_H__

#include "typedef.h"

#define HAL_NO_ERR         ((int16_t) 0 )
#define HAL_UNKNOWN_ERR    ((int16_t)-1 )
#define HAL_WARN_ERR       ((int16_t)-2 )
#define HAL_BUSY_ERR       ((int16_t)-4 )
#define HAL_RANGE_ERR      ((int16_t)-5 )
#define HAL_OVERFLOW_ERR   ((int16_t)-6 )
#define HAL_NOACT_ERR      ((int16_t)-8 )
#define HAL_CONFIG_ERR     ((int16_t)-10)

#define HAL_DIGOUT_OPEN        ((int16_t)-20 )
#define HAL_DIGOUT_SHORTCUT    ((int16_t)-21 )
#define HAL_DIGOUT_OVERTEMP    ((int16_t)-22 )
#define HAL_DIGOUT_UNDERVOLT   ((int16_t)-23 )
#define HAL_DIGOUT_OVERVOLT    ((int16_t)-24 )

#endif
