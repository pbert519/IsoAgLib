/***************************************************************************
                          irs232io_c.h  -  header for RS232IO_c object
                                         for serial communication (Rs232)
                             -------------------
    begin                : Mon Oct 25 1999
    copyright            : (C) 1999 - 2004 by Dipl.-Inform. Achim Spangler
    email                : a.spangler@osb-ag:de
    type                 : Header
    $LastChangedDate$
    $LastChangedRevision$
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
#ifndef IRS232_IO_H
#define IRS232_IO_H

/* *************************************** */
/* ********** include headers ************ */
/* *************************************** */
#include "impl/rs232io_c.h"

namespace IsoAgLib {

/**
  object for serial communication via RS232 device;
  the interface is initialized during constructor call;
  and is closed during destructor -> automatic;
  stream input and output operators ease the data communication
  @short object for serial communication via RS232 device.
  @author Dipl.-Inform. Achim Spangler
*/
class iRS232IO_c : private __IsoAgLib::RS232IO_c {
public:

  /**
    enum type for setting data bit, stop bit and parity within one parameter:
    databitParityStopbit {7,8}_{N,O,E}_{1,2}
  */
  enum t_dataMode {_7_E_1 = 0x11, _7_E_2 = 0x12, _7_O_1 = 0x31, _7_O_2 = 0x32, _8_E_1 = 0x21, _8_E_2 = 0x22,
        _8_O_1 = 0x41, _8_O_2 = 0x42, _8_N_1 = 0x51, _8_N_2 = 0x52};

  // ++++++++++++++++++++++++++++++++++++
  // ++++ RS232 managing operations ++++
  // ++++++++++++++++++++++++++++++++++++

  /**
    init function which initialises the BIOS RS232
    @param rui16_baudrate baudrate {75, 600, 1200, 2400, 4800, 9600, 19200}
    @param ren_dataMode data mode setting of {7,8}_{N,O,E}_{1,2}
    @param rb_xonXoff use XON/XOFF sw handshake (true, false)
    @param rui16_sndPuf sending puffer size
    @param rui16_recPuf recieving puffer size

    possible errors:
        * Err_c::badAlloc not enough memory for allocating the puffers
        * Err_c::range one of the configuration vals is not in allowed ranges
  */
  bool init(uint16_t rui16_baudrate = DEFAULT_BAUDRATE,
          t_dataMode ren_dataMode = iRS232IO_c::t_dataMode(DEFAULT_DATA_MODE),
          bool rb_xonXoff = DEFAULT_XON_XOFF,
          uint16_t rui16_sndPuf = DEFAULT_SND_PUF_SIZE, uint16_t rui16_recPuf = DEFAULT_REC_PUF_SIZE)
  {return RS232IO_c::init(rui16_baudrate, RS232IO_c::t_dataMode(ren_dataMode), rb_xonXoff, rui16_sndPuf, rui16_recPuf);};
  /**
    set the baudrate to a new value
    @param rui16_baudrate baudrate {75, 600, 1200, 2400, 4800, 9600, 19200}

    possible errors:
        * Err_c::range the wanted baudrate is not allowed
    @return true -> setting successful
  */
  bool setBaudrate(uint16_t rui16_baudrate) {return RS232IO_c::setBaudrate(rui16_baudrate);};

  /**
    deliver the actual baudrate
    @return RS232 baudrate
  */
  uint16_t baudrate() const {return RS232IO_c::baudrate();};

  /**
    set send puffer size
    @param rui16_sndPuf sending puffer size

    possible errors:
        * Err_c::badAlloc not enough memory for allocating the puffer
    @return true -> allocating of puffer successful
  */
  bool setSndPufferSize(uint16_t rui16_pufferSize)
    {return RS232IO_c::setSndPufferSize(rui16_pufferSize);};

  /**
    deliver the actual send puffer size
    @return send puffer size
  */
  uint16_t sndPufferSize()const{return RS232IO_c::sndPufferSize();};
  /**
    clear the send puffer without send of actual data in puffer
  */
  void clearSndPuffer()const{RS232IO_c::clearSndPuffer();};
  /**
    set receive puffer size
    @param rui16_recPuf receiving puffer size

    possible errors:
        * Err_c::badAlloc not enough memory for allocating the puffer
    @return true -> allocating of puffer successful
  */
  bool setRecPufferSize(uint16_t rui16_pufferSize);
  /**
    deliver the actual receive puffer size
    @return receive puffer size
  */
  uint16_t rec_pufferSize()const{return RS232IO_c::rec_pufferSize();};
  /**
    clear the receive puffer without reading of actual data in puffer
  */
  void clearRecPuffer()const{RS232IO_c::clearRecPuffer();};
  /**
    check if the receive puffer is empty
    @return true -> receive puffer is empty
  */
  bool eof()const{return RS232IO_c::eof();};
  /**
    deliver the count of data uint8_t in receive puffer
    @return amount of data bytes in receive puffer
  */
  uint16_t rec_pufferCnt()const{return RS232IO_c::rec_pufferCnt();};

  /* ******************************** */
  /* iostream related output operator */
  /* ********    sending     ******** */
  /* ******************************** */

  /**
    send data uint8_t string with given length on RS232

    possible errors:
        * Err_c::rs232_overflow send buffer puffer overflow during send
    @param rpb_data pointer to data string
    @param rui8_len length of data string
  */
  void send(const uint8_t* rpData, uint8_t rui8_len) {RS232IO_c::send(rpData, rui8_len);};
  /**
    send string on RS232

    possible errors:
        * Err_c::rs232_overflow send buffer puffer overflow during send
    @param rrefc_data sent data string
    @return refernce to RS232IO_c for cmd like "rs232 << data1 << data2;"
  */
  iRS232IO_c& operator<<( const std::basic_string<char>& rrefc_data )
    {return static_cast<iRS232IO_c&>(RS232IO_c::operator<<(rrefc_data));};
  /**
    send NULL terminated string on RS232 (terminating NULL isn't sent)

    possible errors:
        * Err_c::rs232_overflow send buffer puffer overflow during send
    @param rpc_data pointer to NULL terminated string to send
    @return refernce to RS232IO_c for cmd like "rs232 << data1 << data2;"
  */
  iRS232IO_c& operator<<(const char *const rpc_data)
    {return static_cast<iRS232IO_c&>(RS232IO_c::operator<<(rpc_data));};
  /**
    send single uint8_t on RS232

    possible errors:
        * Err_c::rs232_overflow send buffer puffer overflow during send
    @param rb_data sent data byte
    @return refernce to RS232IO_c for cmd like "rs232 << data1 << data2;"
  */
  iRS232IO_c& operator<<(uint8_t rb_data)
    {return static_cast<iRS232IO_c&>(RS232IO_c::operator<<(rb_data));};
  /**
    send single int8_t on RS232

    possible errors:
        * Err_c::rs232_overflow send buffer puffer overflow during send
    @param rc_data sent data char
    @return refernce to RS232IO_c for cmd like "rs232 << data1 << data2;"
  */
  iRS232IO_c& operator<<(int8_t rc_data)
    {return static_cast<iRS232IO_c&>(RS232IO_c::operator<<(rc_data));};

  /**
    send uint16_t (2byte unsigned) value as text on RS232

    possible errors:
        * Err_c::rs232_overflow send buffer puffer overflow during send
    @param rui16_data sent data byte
    @return refernce to RS232IO_c for cmd like "rs232 << data1 << data2;"
  */
  iRS232IO_c& operator<<(uint16_t rui16_data)
    {return static_cast<iRS232IO_c&>(RS232IO_c::operator<<(rui16_data));};

  /**
    send int16_t (2 uint8_t unsigned) value as text on RS232

    possible errors:
        * Err_c::rs232_overflow send buffer puffer overflow during send
    @param ri16_data sent data byte
    @return refernce to RS232IO_c for cmd like "rs232 << data1 << data2;"
  */
  iRS232IO_c& operator<<(int16_t ri16_data)
    {return static_cast<iRS232IO_c&>(RS232IO_c::operator<<(ri16_data));};
  /**
    send uint32_t (4 uint8_t unsigned) value as text on RS232

    possible errors:
        * Err_c::rs232_overflow send buffer puffer overflow during send
    @param rui32_data sent data byte
    @return refernce to RS232IO_c for cmd like "rs232 << data1 << data2;"
  */
  iRS232IO_c& operator<<(uint32_t rui32_data)
    {return static_cast<iRS232IO_c&>(RS232IO_c::operator<<(rui32_data));};
  /**
    send int32_t (4 uint8_t signed) value as text on RS232

    possible errors:
        * Err_c::rs232_overflow send buffer puffer overflow during send
    @param ri32_data sent data byte
    @return refernce to RS232IO_c for cmd like "rs232 << data1 << data2;"
  */
  iRS232IO_c& operator<<(int32_t ri32_data)
    {return static_cast<iRS232IO_c&>(RS232IO_c::operator<<(ri32_data));};
  #ifdef USE_FLOAT_DATA_TYPE
  /**
    send float value as text on RS232

    possible errors:
        * Err_c::rs232_overflow send buffer puffer overflow during send
    @param rf_data sent data byte
    @return refernce to RS232IO_c for cmd like "rs232 << data1 << data2;"
  */
  iRS232IO_c& operator<<(float rf_data)
    {return static_cast<iRS232IO_c&>(RS232IO_c::operator<<(rf_data));};
  #endif
  /* ******************************** */
  /* iostream related output operator */
  /* ********    receiving     ******** */
  /* ******************************** */

  /**
    receive data uint8_t string with given length on RS232

    possible errors:
        * Err_c::rs232_underflow receive puffer underflow during receive
    @param pb_data pointer to data string
    @param rui8_len length of data string
  */
  void receive(uint8_t* pData, uint8_t rui8_len) {RS232IO_c::receive(pData, rui8_len);};
  /**
    receive whitespace (or puffer end) terminated string on RS232
    @param refc_data reference to data string for receive
    @return refernce to RS232IO_c for cmd like "rs232 >> data1 >> data2;"
  */
  iRS232IO_c& operator>>( std::basic_string<char>& refc_data)
    {return static_cast<iRS232IO_c&>(RS232IO_c::operator>>(refc_data));};
  /**
    receive '\n' (or puffer end) terminated string on RS232
    @param pb_data pointer to string to receive
    @return refernce to RS232IO_c for cmd like "rs232 >> data1 >> data2;"
  */
  iRS232IO_c& operator>>(uint8_t* pb_data)
    {return static_cast<iRS232IO_c&>(RS232IO_c::operator>>(pb_data));};
  /**
    receive single uint8_t on RS232 (ignore white spaces)

    possible errors:
        * Err_c::rs232_underflow receive puffer underflow during receive
    @param b_data received data byte
    @return refernce to RS232IO_c for cmd like "rs232 >> data1 >> data2;"
  */
  iRS232IO_c& operator>>(uint8_t& b_data)
    {return static_cast<iRS232IO_c&>(RS232IO_c::operator>>(b_data));};
  /**
    receive single int8_t on RS232 (ignore white spaces)

    possible errors:
        * Err_c::rs232_underflow receive puffer underflow during receive
    @param c_data received data char
    @return refernce to RS232IO_c for cmd like "rs232 >> data1 >> data2;"
  */
  iRS232IO_c& operator>>(int8_t& c_data)
    {return static_cast<iRS232IO_c&>(RS232IO_c::operator>>(c_data));};
  /**
    receive uint16_t (2byte unsigned) value as text on RS232
    (ignore white spaces)

    possible errors:
        * Err_c::rs232_underflow receive puffer underflow during receive
    @param ui16_data received data byte
    @return refernce to RS232IO_c for cmd like "rs232 >> data1 >> data2;"
  */
  iRS232IO_c& operator>>(uint16_t& ui16_data)
    {return static_cast<iRS232IO_c&>(RS232IO_c::operator>>(ui16_data));};
  /**
    receive int16_t (2 uint8_t unsigned) value as text on RS232
    (ignore white spaces)

    possible errors:
        * Err_c::rs232_underflow receive puffer underflow during receive
    @param i16_data received data byte
    @return refernce to RS232IO_c for cmd like "rs232 >> data1 >> data2;"
  */
  iRS232IO_c& operator>>(int16_t& i16_data)
    {return static_cast<iRS232IO_c&>(RS232IO_c::operator>>(i16_data));};
  /**
    receive uint32_t (4 uint8_t unsigned) value as text on RS232
    (ignore white spaces)

    possible errors:
        * Err_c::rs232_underflow receive puffer underflow during receive
    @param ui32_data received data byte
    @return refernce to RS232IO_c for cmd like "rs232 >> data1 >> data2;"
  */
  iRS232IO_c& operator>>(uint32_t& ui32_data)
    {return static_cast<iRS232IO_c&>(RS232IO_c::operator>>(ui32_data));};
  /**
    receive int32_t (4 uint8_t signed) value as text on RS232
    (ignore white spaces)

    possible errors:
        * Err_c::rs232_underflow receive puffer underflow during receive
    @param i32_data received data byte
    @return refernce to RS232IO_c for cmd like "rs232 >> data1 >> data2;"
  */
  iRS232IO_c& operator>>(int32_t& i32_data)
    {return static_cast<iRS232IO_c&>(RS232IO_c::operator>>(i32_data));};
  #ifdef USE_FLOAT_DATA_TYPE
  /**
    receive float value as text on RS232
    (ignore white spaces)

    possible errors:
        * Err_c::rs232_underflow receive puffer underflow during receive
    @param f_data received data byte
    @return refernce to RS232IO_c for cmd like "rs232 >> data1 >> data2;"
  */
  iRS232IO_c& operator>>(float& f_data)
    {return static_cast<iRS232IO_c&>(RS232IO_c::operator>>(f_data));};
  #endif
private: //Private methods
  /** allow getIrs232Instance() access to shielded base class.
      otherwise __IsoAgLib::getRs232Instance() wouldn't be accepted by compiler
    */
  friend iRS232IO_c& getIrs232Instance( void );
  /** private constructor which prevents direct instantiation in user application
    * NEVER define instance of iRS232IO_c within application
    */
  iRS232IO_c( void ) { init();};

};

/** C-style function, to get access to the unique iRS232IO_c singleton instance */
inline iRS232IO_c& getIrs232Instance( void ) { return static_cast<iRS232IO_c&>(__IsoAgLib::getRs232Instance());};
}

#endif
