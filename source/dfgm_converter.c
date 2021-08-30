#include "dfgm_converter.h"
#include "sci.h"
#include "sys_common.h"
#include <FreeRTOS.h>
#include <queue.h>
#include <stdlib.h>
#include <task.h>
#include "system.h"

#define BUFFER_SIZE 1248
#define QUEUE_DEPTH 32

#ifndef DFGM_RX_PRIO
#define DFGM_RX_PRIO configMAX_PRIORITIES - 1
#endif

#ifndef DFGM_SCI
#define DFGM_SCI scilinREG
#endif

static uint8_t dfgmBuffer[BUFFER_SIZE];
static xQueueHandle dfgmQueue;

void dfgm_convert_mag(dfgm_packet_t *const data) {

    // convert raw data to magnetic field data
    int i;
    for (i = 0; i < 100; i++) {
        short xdac = (data->X[i]) >> 16;
        short xadc = ((data->X[i]) % (1 << 16));
        short ydac = (data->Y[i]) >> 16;
        short yadc = ((data->Y[i]) % (1 << 16));
        short zdac = (data->Z[i]) >> 16;
        short zadc = ((data->Z[i]) % (1 << 16));
        float X = (XDACScale * (float)xdac + XADCScale * (float)xadc + XOffset);
        float Y = (YDACScale * (float)ydac + YADCScale * (float)yadc + YOffset);
        float Z = (ZDACScale * (float)zdac + ZADCScale * (float)zadc + ZOffset);
        data->X[i] = *(uint32_t *)&X;
        data->Y[i] = *(uint32_t *)&Y;
        data->Z[i] = *(uint32_t *)&Z;
    }
}

void send_packet(dfgm_packet_t *packet) {
    sciSend(DFGM_SCI, 1, (uint8_t *)&packet->dle);
    sciSend(DFGM_SCI, 1, (uint8_t *)&packet->stx);
    sciSend(DFGM_SCI, 1, (uint8_t *)&packet->pid);
    sciSend(DFGM_SCI, 1, (uint8_t *)&packet->packet_type);
    sciSend(DFGM_SCI, 2, (uint8_t *)&packet->packet_length);
    sciSend(DFGM_SCI, 2, (uint8_t *)&packet->fs);
    sciSend(DFGM_SCI, 4, (uint8_t *)&packet->pps_offset);

    int i;
    for (i = 0; i < 12; i++) {
        sciSend(DFGM_SCI, 2, (uint8_t *)&packet->hk[i]);
    }
    for (i = 0; i < 100; i++) {
        sciSend(DFGM_SCI, 4, (uint8_t *)&packet->X[i]);
        sciSend(DFGM_SCI, 4, (uint8_t *)&packet->Y[i]);
        sciSend(DFGM_SCI, 4, (uint8_t *)&packet->Z[i]);
    }
    sciSend(DFGM_SCI, 2, (uint8_t *)&packet->board_id);
    sciSend(DFGM_SCI, 2, (uint8_t *)&packet->sensor_id);
    sciSend(DFGM_SCI, 1, (uint8_t *)&packet->reservedA);
    sciSend(DFGM_SCI, 1, (uint8_t *)&packet->reservedB);
    sciSend(DFGM_SCI, 1, (uint8_t *)&packet->reservedC);
    sciSend(DFGM_SCI, 1, (uint8_t *)&packet->reservedD);
    sciSend(DFGM_SCI, 1, (uint8_t *)&packet->reservedE);
    sciSend(DFGM_SCI, 1, (uint8_t *)&packet->etx);
    sciSend(DFGM_SCI, 2, (uint8_t *)&packet->crc);
}

void dfgm_rx_task(void *pvParameters) {
    dfgm_packet_t rawDFGMData;
    for (;;) {
        // receive packet from queue
        xQueueReceive(dfgmQueue, (void *)&rawDFGMData, portMAX_DELAY);

        // convert raw data to magnetic field data
        dfgm_convert_mag(&rawDFGMData);

        // TOO: Do something with the data!!!
    }
}

void dfgm_init(const sciBASE_t *sci) {
    dfgmQueue = xQueueCreate(QUEUE_DEPTH, BUFFER_SIZE);
    xTaskCreate(dfgm_rx_task, "DFGM RX", 256, NULL, DFGM_RX_PRIO,
                NULL); // TODO: keep prioriy in a header somewhere
    sciReceive(sci, BUFFER_SIZE, &dfgmBuffer);
    return;
}

void dfgm_sciNotification(sciBASE_t *sci, unsigned flags) {
    // add dfgmBuffer to a queue
    portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;
    xQueueSendToBackFromISR(dfgmQueue, &dfgmBuffer, &xHigherPriorityTaskWoken);
    sciReceive(sci, BUFFER_SIZE, &dfgmBuffer);
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    return;
}
