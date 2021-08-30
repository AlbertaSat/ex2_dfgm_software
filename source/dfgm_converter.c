#include "dfgm_converter.h"
#include "sci.h"
#include "sys_common.h"
#include <FreeRTOS.h>
#include <queue.h>
#include <stdlib.h>
#include <task.h>
#include "system.h"
#include "redposix.h"

#define BUFFER_SIZE 1248
#define QUEUE_DEPTH 32

#ifndef DFGM_RX_PRIO
#define DFGM_RX_PRIO configMAX_PRIORITIES - 1
#endif

#ifndef DFGM_SCI
#define DFGM_SCI scilinREG
#endif

#ifndef PRINTF_SCI
#define PRINTF_SCI scilinREG
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
    sciSend(PRINTF_SCI, sizeof(dfgm_packet_t), packet);
}

void dfgm_rx_task(void *pvParameters) {
    dfgm_packet_t rawDFGMData;
    FILE *
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
    switch (flags) {
    // add dfgmBuffer to a queue
    case SCI_RX_INT:
        portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;
        xQueueSendToBackFromISR(dfgmQueue, &dfgmBuffer, &xHigherPriorityTaskWoken);
        sciReceive(sci, BUFFER_SIZE, &dfgmBuffer);
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
        break;
    case SCI_TX_INT: break;
    default: break;
    }
}
