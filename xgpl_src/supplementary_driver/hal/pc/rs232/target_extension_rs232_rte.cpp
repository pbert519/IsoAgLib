/***************************************************************************
                          target_extensions_rs232_rte.cpp - source for PC specific
                                                extensions for the HAL
                                                for RS232
                             -------------------
    begin                : Sat Jan 01 2003
    copyright            : (C) 2003 by Dipl.-Inform. Achim Spangler
    email                : a.spangler@osb-ag:de
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
 * Copyright (C) 1999 - 2004 Dipl.-Inform. Achim Spangler                 *
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
#define  STRICT

#define CVS_REV "$Revision$"
#define RTE_CLIENT
#include <rte_client.h>		// be an RTE client
#include <rte_serial.h>

#include "rs232_target_extensions.h"
#include <IsoAgLib/util/compiler_adoption.h>
#include <IsoAgLib/hal/pc/system/system.h>
#include <cstdio>
#include <cstring>
#include <vector>

namespace __HAL {

/* **************************************** */
/* ****** RS232 I/O BIOS functions  ******* */
/* **************************************** */
#if RS232_INSTANCE_CNT == 1
serial_c *pc_serial[RS232_INSTANCE_CNT] = NULL;
#elif RS232_INSTANCE_CNT == 2
serial_c *pc_serial[RS232_INSTANCE_CNT] = {NULL,NULL};
#elif RS232_INSTANCE_CNT == 3
serial_c *pc_serial[RS232_INSTANCE_CNT] = {NULL,NULL,NULL};
#else
serial_c *pc_serial[RS232_INSTANCE_CNT];
#endif

STL_NAMESPACE::vector<uint8_t> c_buffer[RS232_INSTANCE_CNT];

/** send handler which is called by RTE on each new received data -> store current fertilizer amount */
int rs232_send_handler(rtd_handler_para_t* para, uint8_t size, const uint8_t *data ) {
  // ignore events from other channel than arr_serialChannels[cui32_fertilizerSerialIndex]
	for ( int testInd = 0; testInd < RS232_INSTANCE_CNT; testInd++ )
	{
  	if ( para->rtd_msg->channel != testInd ) continue;
		// append received string in puffer
		#if 0
		std::cerr << hex << "RS232 Receive Handler: #";
		#endif
		for ( uint16_t ind = 0; ind < size; ind++ ) {
			c_buffer[testInd].push_back( data[ind] );
			#if 0
			std::cerr << "0x" << int ( data[ind] ) << ", ";
			#endif
		}
		#if 0
		std::cerr << "#" << dec << std::endl;
		#endif
		return 0;
	}
	// if reaching here -> no channel matched
	return -1;
}


/**
  init the RS232 interface
  @param wBaudrate wnated Baudrate {75, 600, 1200, 2400, 4800, 9600, 19200}
        as configured in <Application_Config/isoaglib_config.h>
  @param bMode one of (DATA_7_BITS_EVENPARITY = 1, DATA_8_BITS_EVENPARITY = 2,
          DATA_7_BITS_ODDPARITY = 3, DATA_8_BITS_ODDPARITY = 4, DATA_8_BITS_NOPARITY = 5)
  @param bStoppbits amount of stop bits (1,2)
  @param bitSoftwarehandshake true -> use xon/xoff software handshake
  @return HAL_NO_ERR -> o.k. else one of settings incorrect
*/
int16_t init_rs232(uint16_t wBaudrate,uint8_t bMode,uint8_t bStoppbits,bool bitSoftwarehandshake, uint8_t rui8_channel)
{
	if ( rui8_channel >= RS232_INSTANCE_CNT ) return HAL_RANGE_ERR;
  char param[30];
  uint16_t ui16_dataBit = 8;
  if ( ( bMode == 1 ) || ( bMode == 3 ) ) ui16_dataBit = 7;
  char par = 'n';
  if      ( ( bMode == 1 ) || ( bMode == 2 ) ) par = 'e';
  else if ( ( bMode == 3 ) || ( bMode == 4 ) ) par = 'o';
  sprintf( param, "C%hd,R,E0,B%d,L%d%c%hd", rui8_channel,wBaudrate, ui16_dataBit, par, bStoppbits );

	if ( ! rte_is_init() ) {
    if (rte_connect( "rte1" ) < 0) {
      cerr << "Unable to connect RTE1 server." << endl;
      exit(1);
    } else {
      cout << "Connected to RTE1" << endl;
    }
  }
  if ( pc_serial[rui8_channel] == NULL )pc_serial[rui8_channel] = new serial_c;
	pc_serial[rui8_channel]->set_channel( 0 );
  pc_serial[rui8_channel]->set_send_handler( rs232_send_handler, 0 );
  pc_serial[rui8_channel]->set_line_parameters( param );
  pc_serial[rui8_channel]->set_echo( false );
  return HAL_NO_ERR;
} // soll "C0,R,E0,B4800,L8n1"
/**
  set the RS232 Baudrate
  @param wBaudrate wanted baudrate
  @return HAL_NO_ERR -> o.k. else baudrate setting incorrect
*/
int16_t setRs232Baudrate(uint16_t wBaudrate, uint8_t rui8_channel)
{
	if ( rui8_channel >= RS232_INSTANCE_CNT ) return HAL_RANGE_ERR;
  pc_serial[rui8_channel]->set_baud( wBaudrate );
  return HAL_NO_ERR;
}
/**
  get the amount of data [uint8_t] in receive puffer
  @return receive puffer data byte
*/
int16_t getRs232RxBufCount(uint8_t rui8_channel)
{
	if ( rui8_channel >= RS232_INSTANCE_CNT ) return HAL_RANGE_ERR;
  return c_buffer[rui8_channel].size();
}
/**
  get the amount of data [uint8_t] in send puffer
  @return send puffer data byte
*/
int16_t getRs232TxBufCount(uint8_t rui8_channel)
{
	if ( rui8_channel >= RS232_INSTANCE_CNT ) return HAL_RANGE_ERR;
  return 0;
}
/**
  configure a receive puffer and set optional irq function pointer for receive
  @param wBuffersize wanted puffer size
  @param pFunction pointer to irq function or NULL if not wanted
*/
int16_t configRs232RxObj(uint16_t wBuffersize,void (*pFunction)(uint8_t *bByte), uint8_t rui8_channel)
{
	if ( rui8_channel >= RS232_INSTANCE_CNT ) return HAL_RANGE_ERR;
  return HAL_NO_ERR;
}
/**
  configure a send puffer and set optional irq function pointer for send
  @param wBuffersize wanted puffer size
  @param funktionAfterTransmit pointer to irq function or NULL if not wanted
  @param funktionBeforTransmit pointer to irq function or NULL if not wanted
*/
int16_t configRs232TxObj(uint16_t wBuffersize,void (*funktionAfterTransmit)(uint8_t *bByte),
                                void (*funktionBeforTransmit)(uint8_t *bByte), uint8_t rui8_channel)
{
	if ( rui8_channel >= RS232_INSTANCE_CNT ) return HAL_RANGE_ERR;
  return HAL_NO_ERR;
}
/**
  get errr code of BIOS
  @return 0=parity, 1=stopbit framing error, 2=overflow
*/
int16_t getRs232Error(uint8_t *Errorcode, uint8_t rui8_channel)
{
	if ( rui8_channel >= RS232_INSTANCE_CNT ) return HAL_RANGE_ERR;
  *Errorcode = 0;
  return HAL_NO_ERR;
}

/**
  read single int8_t from receive puffer
  @param pbRead pointer to target data
  @return HAL_NO_ERR -> o.k. else puffer underflow
*/
int16_t getRs232Char(uint8_t *pbRead, uint8_t rui8_channel)
{  // retrieve first char
	if ( rui8_channel >= RS232_INSTANCE_CNT ) return HAL_RANGE_ERR;
  if ( c_buffer[rui8_channel].empty() ) {
    // no char in receive buffer
    *pbRead = '\0';
    return HAL_RANGE_ERR;
  }
  pbRead[0] = c_buffer[rui8_channel][0];
//  pbRead[1] = '\0';
  // move rest of string one to front
  c_buffer[rui8_channel].erase( c_buffer[rui8_channel].begin() );
  return HAL_NO_ERR;
}
/**
  read bLastChar terminated string from receive puffer
  @param pbRead pointer to target data
  @param bLastChar terminating char
  @return HAL_NO_ERR -> o.k. else puffer underflow
*/
int16_t getRs232String(uint8_t *pbRead,uint8_t bLastChar, uint8_t rui8_channel)
{
	if ( rui8_channel >= RS232_INSTANCE_CNT ) return HAL_RANGE_ERR;
  vector<uint8_t>::iterator pc_iter = find( c_buffer[rui8_channel].begin(), c_buffer[rui8_channel].end(), bLastChar );
  int16_t ind = -1;
  for ( vector<uint8_t>::iterator pc_reader = c_buffer[rui8_channel].begin(); pc_reader != pc_iter;  pc_reader++ ) {
    // increment ind
    ind++;
    // copy element
    pbRead[ind] = *pc_reader;
  }
  // clear buffer
  c_buffer[rui8_channel].erase( c_buffer[rui8_channel].begin(), pc_iter );
  // terminate result string if '\0' isn't last element
  if ( pbRead[ind] != '\0' ) pbRead[ind] = '\0';
  return HAL_NO_ERR;
}

/**
  send single uint8_t on RS232
  @param bByte data uint8_t to send
  @return HAL_NO_ERR -> o.k. else send puffer overflow
*/
int16_t put_rs232Char(uint8_t bByte, uint8_t rui8_channel)
{
	if ( rui8_channel >= RS232_INSTANCE_CNT ) return HAL_RANGE_ERR;
  pc_serial[rui8_channel]->send( 1, &bByte );
  return HAL_NO_ERR;
}
/**
  send string of n uint8_t on RS232
  @param bpWrite pointer to source data string
  @param wNumber number of data uint8_t to send
  @return HAL_NO_ERR -> o.k. else send puffer overflow
*/
int16_t put_rs232NChar(const uint8_t *bpWrite,uint16_t wNumber, uint8_t rui8_channel)
{
	if ( rui8_channel >= RS232_INSTANCE_CNT ) return HAL_RANGE_ERR;
  pc_serial[rui8_channel]->send( wNumber, bpWrite );
  return HAL_NO_ERR;
}
/**
  send '\0' terminated string on RS232
  @param pbString pointer to '\0' terminated (!) source data string
  @return HAL_NO_ERR -> o.k. else send puffer overflow
*/
int16_t put_rs232String(const uint8_t *pbString, uint8_t rui8_channel)
{
	if ( rui8_channel >= RS232_INSTANCE_CNT ) return HAL_RANGE_ERR;
  pc_serial[rui8_channel]->send( (const char*)pbString );
  return HAL_NO_ERR;
}

/**
  clear receive puffer
*/
void clearRs232RxBuffer(uint8_t rui8_channel)
{
	if ( rui8_channel >= RS232_INSTANCE_CNT ) return;
  c_buffer[rui8_channel].clear();
};

/**
  clear send puffer
*/
void clearRs232TxBuffer(uint8_t rui8_channel)
{
}

} // End of name space __HAL
