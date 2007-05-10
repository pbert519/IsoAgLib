#include "rs232_target_extensions.h"

#include <IsoAgLib/hal/pc/errcodes.h>

#include <fcntl.h>      /* for open()/read()/write() */
#include <termios.h>    /* for tcgetattr()/tcsetattr() */
#include <unistd.h>     /* for close() */



#include <cstdio>
#include <cstdlib>
#include <cctype>
#include <ctime>
#include <cstring>
#include <deque>

namespace __HAL {
struct T_BAUD { uint32_t rate; uint32_t flag; } t_baud[] = {
  {    600L, B600    }, {   1200L, B1200   }, {   2400L, B2400   },
  {   4800L, B4800   }, {   9600L, B9600   }, {  19200L, B19200  },
  {  38400L, B38400  }, {  57600L, B57600  }, { 115200L, B115200 } };



struct termios t_com[RS232_INSTANCE_CNT];
int32_t f_com[RS232_INSTANCE_CNT];
std::deque<int8_t> deq_readPuff[RS232_INSTANCE_CNT];

static bool arr_usedPort[RS232_INSTANCE_CNT] = {
#if RS232_INSTANCE_CNT > 0
false
#endif
#if RS232_INSTANCE_CNT > 1
, false
#endif
#if RS232_INSTANCE_CNT > 2
, false
#endif
#if RS232_INSTANCE_CNT > 3
, false
#endif
#if RS232_INSTANCE_CNT > 4
, false
#endif
};

#define false 0
#define true 1

int8_t c_read;

/** close the RS232 interface. */
int16_t close_rs232(uint8_t comport)
{
  if ( comport >= RS232_INSTANCE_CNT ) return HAL_RANGE_ERR;
  if ( arr_usedPort[comport] )
  {
    tcsetattr(f_com[comport], TCSANOW, &(t_com[comport]));
    close(f_com[comport]);
    arr_usedPort[comport] = false;
		return HAL_NO_ERR;
  }
	else
	{
		return HAL_NOACT_ERR;
	}
}

void close_rs232()
{
  for ( int ind = 0; ind < RS232_INSTANCE_CNT; ind++)
  {
    if ( arr_usedPort[ind] )
    {
      tcsetattr(f_com[ind], TCSANOW, &(t_com[ind]));
      close(f_com[ind]);
      arr_usedPort[ind] = false;
    }
  }
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

#ifdef SYSTEM_A1
// Don't allow gps communication on com 1 of the A1 - it is used for
if(rui8_channel == 0)
  return HAL_NO_ERR;
#endif
  static char com[] = "/dev/ttySx";
  struct termios tty_options;
  struct T_BAUD *b;
  uint32_t  baudflag;

  // first close if already configured
  close_rs232(rui8_channel);

  b = t_baud;
  do {
    baudflag = b->flag;
    if (b->rate >= wBaudrate) break;
  } while (++b < t_baud + sizeof t_baud/sizeof *t_baud);

  com[9] = rui8_channel+'0';
  if ((f_com[rui8_channel] = open(com, O_RDWR|O_NOCTTY|O_NDELAY)) < 0) return HAL_CONFIG_ERR;
  if (tcgetattr(f_com[rui8_channel], &(t_com[rui8_channel]))) return HAL_CONFIG_ERR;

  arr_usedPort[rui8_channel] = true;
  atexit(close_rs232);

  /* Configure port reading */
  fcntl(f_com[rui8_channel], F_SETFL, FNDELAY);

  /* Get the current options for the port */
  tcgetattr(f_com[rui8_channel], &tty_options);
  cfsetispeed(&tty_options, baudflag);
  cfsetospeed(&tty_options, baudflag);

  if ( bitSoftwarehandshake )
  { // use XON/XOFF flow control
    tty_options.c_iflag |= (IXON | IXOFF);
  }
  else
  { // don't use XON/XOFF
    tty_options.c_iflag &= ~(IXON | IXOFF);
  }

  // ignore BRK condition and parity error ( maybe also activate CR ignore - but that might be wanted )
  tty_options.c_iflag |= IGNBRK | IGNPAR /*| IGNCR*/;

  // no implementation defined output processing
  tty_options.c_oflag &= ~(OPOST);
  tty_options.c_oflag |= ONLCR;
  /* Enable the receiver and set local mode */
  tty_options.c_cflag |= (CLOCAL | CREAD);

  switch ( bMode )
  {
    case __HAL::DATA_8_BITS_NOPARITY: // no parity
      tty_options.c_cflag &= ~PARENB;
      break;
    case __HAL::DATA_7_BITS_EVENPARITY:
    case __HAL::DATA_8_BITS_EVENPARITY:
      // even parity
      tty_options.c_cflag |= PARENB;
      tty_options.c_cflag &= ~PARODD;
      break;
    default: // odd parity
      tty_options.c_cflag |= (PARENB | PARODD);
      break;
  }

  if ( bStoppbits == 2 ) tty_options.c_cflag |= CSTOPB;
  else tty_options.c_cflag &= ~CSTOPB;

  // prepare setting of amount of databits
  tty_options.c_cflag &= ~CSIZE;

  switch ( bMode )
  {
    case __HAL::DATA_7_BITS_EVENPARITY:
    case __HAL::DATA_7_BITS_ODDPARITY:
      /* Select 8 data bits */
      tty_options.c_cflag |=  CS7;
      break;
    default:
      /* Select 8 data bits */
      tty_options.c_cflag |=  CS8;
      break;
  }

  if ( bitSoftwarehandshake )
  { // enable flow control
    tty_options.c_cflag |= CRTSCTS;
  }
  else
  { // no flow control
    tty_options.c_cflag &= ~CRTSCTS;
  }

  /* Enable data to be processed as raw input */
  tty_options.c_lflag &= ~(ICANON | ECHO | ISIG);

  /* Set the new options for the port */
  tcsetattr(f_com[rui8_channel], TCSANOW, &tty_options);

  // wait some time to avoid buffer error
  usleep( 500000 ); // sleep for 500msec

  // reset read puffer
  deq_readPuff[rui8_channel].clear();

  return HAL_NO_ERR;
}

/**
  set the RS232 Baudrate
  @param wBaudrate wanted baudrate
  @return HAL_NO_ERR -> o.k. else baudrate setting incorrect
 */
int16_t setRs232Baudrate(uint16_t wBaudrate, uint8_t rui8_channel)
{
  if ( rui8_channel >= RS232_INSTANCE_CNT ) return HAL_RANGE_ERR;
  struct termios tty_options;
  struct T_BAUD *b;
  uint32_t  baudflag;

  // first close if already configured
  close_rs232(rui8_channel);

  b = t_baud;
  do {
    baudflag = b->flag;
    if (b->rate >= wBaudrate) break;
  } while (++b < t_baud + sizeof t_baud/sizeof *t_baud);

  if (tcgetattr(f_com[rui8_channel], &tty_options)) return 0;
  cfsetispeed(&tty_options, baudflag);
  cfsetospeed(&tty_options, baudflag);

  /* Set the new options for the port */
  tcsetattr(f_com[rui8_channel], TCSANOW, &tty_options);

  // wait some time to avoid buffer error
  usleep( 500000 ); // sleep for 500msec
  return 1;
}

int16_t SioPutBuffer(uint32_t comport, const uint8_t *p, int16_t n)
{
  if ( comport >= RS232_INSTANCE_CNT ) return 0;
  n = write(f_com[comport], p, n) == n;
  tcdrain(f_com[comport]);
  return n;
}
/**
  send single uint8_t on RS232
  @param bByte data uint8_t to send
  @return HAL_NO_ERR -> o.k. else send puffer overflow
 */
int16_t put_rs232Char(uint8_t bByte, uint8_t rui8_channel)
{
  if ( rui8_channel >= RS232_INSTANCE_CNT ) return HAL_RANGE_ERR;
  // printf("RS232:\n %c\n", bByte);
  uint8_t b_data = bByte;
  SioPutBuffer(rui8_channel, &b_data, 1);
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
  SioPutBuffer(rui8_channel, bpWrite, wNumber);
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
  // printf("RS232:: %s", pbString);
  SioPutBuffer(rui8_channel, pbString, strlen((const char*)pbString));
  return HAL_NO_ERR;
}

/**
  get the amount of data [uint8_t] in receive puffer
  @return receive puffer data byte
 */
int16_t getRs232RxBufCount(uint8_t rui8_channel)
{
  if ( rui8_channel >= RS232_INSTANCE_CNT ) return HAL_RANGE_ERR;
  int8_t c_temp[300];
  int16_t tempLen = read(f_com[rui8_channel], c_temp, (299));

  for ( uint16_t ind = 0; ind < tempLen; ind++ ) deq_readPuff[rui8_channel].push_back( c_temp[ind] );

  return deq_readPuff[rui8_channel].size();
}

/**
  read single int8_t from receive puffer
  @param pbRead pointer to target data
  @return HAL_NO_ERR -> o.k. else puffer underflow
 */
int16_t getRs232Char(uint8_t *pbRead, uint8_t rui8_channel)
{
  if ( rui8_channel >= RS232_INSTANCE_CNT ) return HAL_RANGE_ERR;
  getRs232RxBufCount(rui8_channel);
  if (! deq_readPuff[rui8_channel].empty())
  {
    pbRead[0] = deq_readPuff[rui8_channel].front();
    deq_readPuff[rui8_channel].pop_front();
    return HAL_NO_ERR;
  }
  else
  {
    return HAL_NOACT_ERR;
  }
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
  getRs232RxBufCount(rui8_channel);
  if (! deq_readPuff[rui8_channel].empty())
  {
    for ( std::deque<int8_t>::iterator iter = deq_readPuff[rui8_channel].begin(); iter != deq_readPuff[rui8_channel].end(); iter++ )
    { // check if terminating char is found
      if ( *iter == bLastChar )
      { // found -> copy area from begin to iterator
        uint16_t ind = 0;
        for ( ; ( deq_readPuff[rui8_channel].front() != bLastChar); ind++ )
        {
          pbRead[ind] = deq_readPuff[rui8_channel].front();
          deq_readPuff[rui8_channel].pop_front();
        }
        // lastly pop the termination char and terminate the result string
        deq_readPuff[rui8_channel].pop_front();
        pbRead[ind] = '\0';
        return HAL_NO_ERR;
      }
    }
  }
  return HAL_NOACT_ERR;
}

#if 0
int8_t *KeyGetString(char *buffer, int16_t len)
{
  struct termios t;
  int8_t *p;
  tcgetattr(0, &t);
  tcsetattr(0, TCSAFLUSH, &t_0);
  p = (int8_t*)fgets(buffer, len, stdin);
  tcsetattr(0, TCSAFLUSH, &t);
  return p;
}
#endif


/**
  get the amount of data [uint8_t] in send puffer
  @return send puffer data byte
 */
int16_t getRs232TxBufCount(uint8_t rui8_channel)
{
  if ( rui8_channel >= RS232_INSTANCE_CNT ) return HAL_RANGE_ERR;
  /** @todo decide if RS232 TX buffer from OS should be asked */
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
  /** @todo should this be implemented? */
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
  /** @todo should this be implemented? */
  return HAL_NO_ERR;
}
/**
  get errr code of BIOS
  @return 0=parity, 1=stopbit framing error, 2=overflow
 */
int16_t getRs232Error(uint8_t *Errorcode, uint8_t rui8_channel)
{
  if ( rui8_channel >= RS232_INSTANCE_CNT ) return HAL_RANGE_ERR;
  /** @todo should this be implemented? */
  return HAL_NO_ERR;
}



/**
  clear receive puffer
 */
void clearRs232RxBuffer(uint8_t rui8_channel)
{
  if ( rui8_channel >= RS232_INSTANCE_CNT ) return;
  /** @todo should this be implemented? */
};

/**
  clear send puffer
 */
void clearRs232TxBuffer(uint8_t rui8_channel)
{
  if ( rui8_channel >= RS232_INSTANCE_CNT ) return;
  /** @todo should this be implemented? */
}

} // end of namespace __HAL
