// ---------------------------------------------------------------------------------------------
/** \file      system_target_extensions.cpp
    \brief     AMS5 specific extensions for the HAL for central system
    \version   1.01
    \date      24.07.2007
    \author    Stefan Klueh (stefan.klueh@gesas:de)
    \author    copyright (c) 2007 GESAS GmbH
*/
// ---------------------------------------------------------------------------------------------
/*   History: 
              18.06.2007 V1.00  - first release

              20.02.2008 V1.01  - revision of NMI interrupt handler

*/

#include "system_target_extensions.h"
#include <../commercial_BIOS/bios_ams5/ams_bios.h>

namespace __HAL /** \brief Sublayer of HAL */
{
   static uint16_t  SerialNumber = 1234;

   /* *********************************** */
   /** \name Global System BIOS functions */
   /*@{*/

   /**
      open the system with system specific function call
      @return error state (HAL_NO_ERR == o.k.)
   */
   int16_t open_system(void)
   {
      /**
         initialize the system
         - set all BIOS variables
         - start system timer
      */
      AMSBIOS::sys_init();
      AMSBIOS::sys_setSystemTimer(1);
      return HAL_NO_ERR;
   };

   /**
      close the system with system specific function call
      @return error state (HAL_NO_ERR == o.k.)
   */
   int16_t closeSystem(void)
   {
      return HAL_NO_ERR;
   };

   /**
      check if open_System() has already been called
   */
   bool isSystemOpened(void)
   {
      /** check "systemState" set by BIOS::systemInit()  */
      return (AMSBIOS::sysSystemState == 0x0001)?true:false;
   };

   /**
      trigger the watchdog
   */
   void wdTriggern(void)
   {
      /**
         toggle the Watchdog Input Pin (MAX705) to restart Timer
         Timeout Period: 1.6 sec
      */
      AMSBIOS::sys_resetWDT();
   };

   /**
      get the system time in [ms]
      @return [ms] since start of the system
   */
   int32_t getTime(void)
   {
      return AMSBIOS::sys_getSystemTimer();
   };

   /**
      hand over the serial number to a pointer
      @return HAL_NO_ERR
   */
   int16_t getSnr(uint8_t *snrDat)
   {
      *snrDat = SerialNumber;
      return HAL_NO_ERR;
   };

   /**
      serial number as long interger
      @return serial number
   */
   int32_t getSerialNr(int16_t* pi16_errCode)
   {
      return ((int32_t)SerialNumber);
   };

   uint32_t getSerialNr(void)
   {
     return((int32_t)SerialNumber);   
   }
   /**
      software output enable/disable
      @param bitState true  -> outputs enable
                      false -> outputs disable
   */
   void setOutputMask(bool bitState)
   {
      AMSBIOS::io_setOutputMask(bitState);
   };

   /*@}*/
}  // end namespace __HAL

extern "C"
{
   /**
      NMI interrupt handler

      Interrupt caused by power down detection
   */
   __interrupt void __NMI_handler(void)
   {
      uint16_t waitTime;
   
      // wait about 1ms
      waitTime = 1500;
      while (waitTime)
      {
         waitTime--;
         AMSBIOS::sys_resetWDT();
      }
   
      if (AMSBIOS::sys_isPowerDown())
      {
         // power supply is below 8V  (NMI-Input == 0)
         // wait about 1ms
         waitTime = 1500;
         while (waitTime)
         {
            waitTime--;
            AMSBIOS::sys_resetWDT();
         }

         // check again
         if (AMSBIOS::sys_isPowerDown())
         {
            // power supply is still below 8V  (NMI-Input == 0)
            // wait for WDT-Reset
           AMSBIOS::io_setOutput(doX9_1, 1) ;
           while (1);
         }      
      }
   }
}
