/** @file sys_main.c 
*   @brief Application main file
*   @date 11-Dec-2018
*   @version 04.07.01
*
*   This file contains an empty main function,
*   which can be used for the application.
*/

/* 
* Copyright (C) 2009-2018 Texas Instruments Incorporated - www.ti.com 
* 
* 
*  Redistribution and use in source and binary forms, with or without 
*  modification, are permitted provided that the following conditions 
*  are met:
*
*    Redistributions of source code must retain the above copyright 
*    notice, this list of conditions and the following disclaimer.
*
*    Redistributions in binary form must reproduce the above copyright
*    notice, this list of conditions and the following disclaimer in the 
*    documentation and/or other materials provided with the   
*    distribution.
*
*    Neither the name of Texas Instruments Incorporated nor the names of
*    its contributors may be used to endorse or promote products derived
*    from this software without specific prior written permission.
*
*  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 
*  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT 
*  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
*  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT 
*  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
*  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT 
*  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
*  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
*  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
*  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
*  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*
*/


/* USER CODE BEGIN (0) */
#include "gio.h"
#include "sci.h"
#include "uartstdio.h"
#include "FreeRTOS.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "dfgm_converter.h"
#include "dfgm_filter.h"
/* USER CODE END */

/* Include Files */

#include "sys_common.h"

/* USER CODE BEGIN (1) */
#define BUFFER_SIZE 1248
#define BUFFER_2 300
uint8 dfgmBuffer[BUFFER_SIZE];
double dataBuffer[BUFFER_2];

/* USER CODE END */

/** @fn void main(void)
*   @brief Application main function
*   @note This function is empty by default.
*
*   This function is called after startup.
*   The user can use this function to implement the application.
*/

/* USER CODE BEGIN (2) */
extern void main_blinky_clone( void );
//extern void dfgm_convert_mag(uint8 *buffer);
/* USER CODE END */

int main(void)
{
/* USER CODE BEGIN (3) */

    _enable_IRQ();
    sciInit();

//    sciReceive(scilinREG, BUFFER_2, (uint8 *)&dataBuffer);
    sciReceive(scilinREG, BUFFER_SIZE, (uint8 *)&dfgmBuffer);
//    main_filter();

    while(1);

//    gioSetDirection(gioPORTB, 0xFFFFFFFF);
//    if (filterDummy()) {
//        main_blinky_clone();
//    }

/* USER CODE END */

    return 0;
}


/* USER CODE BEGIN (4) */
void sciNotification(sciBASE_t *sci, uint32 flags) {
    // create data packet
    struct dfgm_packet_t *data = (struct dfgm_packet_t *)pvPortMalloc(sizeof(struct dfgm_packet_t));
    if (data==NULL) {
        sciSend(scilinREG, 28, (unsigned char *)"Some kind of malloc() error\n");
        exit(1);
    }
    dfgm_convert_mag((uint8 *)&dfgmBuffer, data);
    send_packet(data);

    // free data packet
    vPortFree(data);

    // Receive next
    sciReceive(sci, BUFFER_SIZE, (uint8 *)&dfgmBuffer);
}

//void sciNotification(sciBASE_t *sci, uint32 flags) {
//    int sample;
//    char datestr[19];
//
//    // should be *g_ptr = {} to change buff
//    for (sample = 0; sample < 100; sample++) {
//        gptr->X[sample] = (double) dataBuffer[sample*3];
//        gptr->Y[sample] = (double) dataBuffer[1+sample*3];
//        gptr->Z[sample] = (double) dataBuffer[2+sample*3];
////        g_ptr->X[sample] = (((long)dataBuffer[  sample*24 ]) <<  56) | (((long)dataBuffer[ 1+sample*24]) <<  48) | (((long)dataBuffer[ 2+sample*24]) <<  40) | (((long)dataBuffer[ 3+sample*24]) <<  32) | (((long)dataBuffer[ 4+sample*24]) <<  24) | (((long)dataBuffer[ 5+sample*24]) <<  16) | (((long)dataBuffer[ 6+sample*24]) <<  8) | (long)dataBuffer[ 7+sample*24];
////        g_ptr->Y[sample] = (dataBuffer[ 8+sample*24] <<  56) | (dataBuffer[ 9+sample*24] <<  48) | (dataBuffer[10+sample*24] <<  40) | (dataBuffer[11+sample*24] <<  32) | (dataBuffer[12+sample*24] <<  24) | (dataBuffer[13+sample*24] <<  16) | (dataBuffer[14+sample*24] <<  8) | dataBuffer[15+sample*24];
////        g_ptr->Z[sample] = (dataBuffer[16+sample*24] <<  56) | (dataBuffer[17+sample*24] <<  48) | (dataBuffer[18+sample*24] <<  40) | (dataBuffer[19+sample*24] <<  32) | (dataBuffer[20+sample*24] <<  24) | (dataBuffer[21+sample*24] <<  16) | (dataBuffer[22+sample*24] <<  8) | dataBuffer[23+sample*24];
//        if (sample == 0)a
//        {
//            strcpy(gptr->datetime, datestr);
//        }
//    }
//    gptr->Xfilt = 99999.999; //Invalid data value
//    gptr->Yfilt = 99999.999; //Invalid data value
//    gptr->Zfilt = 99999.999; //Invalid data value
//
//    // Receive next
//    sciReceive(sci, BUFFER_2, (uint8 *)&dataBuffer);
//}

//void esmGroupNotification(int bit) {
//    return;
//}
//
//void esmGroup2Notification(int bit) {
//    return;
//}

/* USER CODE END */
