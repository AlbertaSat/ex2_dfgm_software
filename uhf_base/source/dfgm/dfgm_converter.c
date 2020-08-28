#include "sys_common.h"
#include "sci.h"
#include <stdlib.h>
#include "dfgm_converter.h"

void receive_packet(uint8 *buffer, struct dfgm_packet_t *packet) {
    packet->dle =           buffer[0];
    packet->stx =           buffer[1];
    packet->pid =           buffer[2];
    packet->packet_type =   buffer[3];
    packet->packet_length = (buffer[4] <<  8) | buffer[5];
    packet->fs =            (buffer[6] <<  8) | buffer[7];
    packet->pps_offset =    (buffer[8] <<  24) | (buffer[9] <<  16) | (buffer[10] <<  8) | buffer[11];
    int i;
    for (i = 0; i < 12; i++) {
        packet->hk[i] =     (buffer[12+(2*i)] <<  8) | buffer[13+(2*i)];
    }
    for (i = 0; i < 100; i++) {
        packet->X[i] =   (((int)buffer[36 + (12*i)]) <<  24) | (((int)buffer[37 + (12*i)]) << 16) | (((int)buffer[38 + (12*i)]) <<  8) | (int)buffer[39 + (12*i)];
        packet->Y[i] =   (((int)buffer[40 + (12*i)]) <<  24) | (((int)buffer[41 + (12*i)]) << 16) | (((int)buffer[42 + (12*i)]) <<  8) | (int)buffer[43 + (12*i)];
        packet->Z[i] =   (((int)buffer[44 + (12*i)]) <<  24) | (((int)buffer[45 + (12*i)]) << 16) | (((int)buffer[46 + (12*i)]) <<  8) | (int)buffer[47 + (12*i)];
    }
    packet->board_id =      (buffer[1236] <<  8) | buffer[1237];
    packet->sensor_id =     (buffer[1238] <<  8) | buffer[1239];
    packet->reservedA =     buffer[1240];
    packet->reservedB =     buffer[1241];
    packet->reservedC =     buffer[1242];
    packet->reservedD =     buffer[1243];
    packet->reservedE =     buffer[1244];
    packet->etx =           buffer[1245];
    packet->crc =           (buffer[1246] <<  8) | buffer[1247];
}

void dfgm_convert_mag(uint8 *buffer, struct dfgm_packet_t *data) {
    // load data from buffer to data packet
    receive_packet(buffer, data);

    // convert raw data to magnetic field data
    int i;
    for (i = 0; i < 100; i++) {
        short xdac = (data->X[i]) >> 16;
        short xadc = ((data->X[i])%(1<<16));
        short ydac = (data->Y[i]) >> 16;
        short yadc = ((data->Y[i])%(1<<16));
        short zdac = (data->Z[i]) >> 16;
        short zadc = ((data->Z[i])%(1<<16));
        float X = (XDACScale*(float)xdac + XADCScale*(float)xadc + XOffset);
        float Y = (YDACScale*(float)ydac + YADCScale*(float)yadc + YOffset);
        float Z = (ZDACScale*(float)zdac + ZADCScale*(float)zadc + ZOffset);
        data->X[i] = *(int *)&X;
        data->Y[i] = *(int *)&Y;
        data->Z[i] = *(int *)&Z;
    }
}

void send_packet(struct dfgm_packet_t *packet) {
    sciSend(scilinREG, 1, (uint8 *)&packet->dle);
    sciSend(scilinREG, 1, (uint8 *)&packet->stx);
    sciSend(scilinREG, 1, (uint8 *)&packet->pid);
    sciSend(scilinREG, 1, (uint8 *)&packet->packet_type);
    sciSend(scilinREG, 2, (uint8 *)&packet->packet_length);
    sciSend(scilinREG, 2, (uint8 *)&packet->fs);
    sciSend(scilinREG, 4, (uint8 *)&packet->pps_offset);

    int i;
    for (i = 0; i < 12; i++) {
        sciSend(scilinREG, 2, (uint8 *)&packet->hk[i]);
    }
    for (i = 0; i < 100; i++) {
        sciSend(scilinREG, 4, (uint8 *)&packet->X[i]);
        sciSend(scilinREG, 4, (uint8 *)&packet->Y[i]);
        sciSend(scilinREG, 4, (uint8 *)&packet->Z[i]);
    }
    sciSend(scilinREG, 2, (uint8 *)&packet->board_id);
    sciSend(scilinREG, 2, (uint8 *)&packet->sensor_id);
    sciSend(scilinREG, 1, (uint8 *)&packet->reservedA);
    sciSend(scilinREG, 1, (uint8 *)&packet->reservedB);
    sciSend(scilinREG, 1, (uint8 *)&packet->reservedC);
    sciSend(scilinREG, 1, (uint8 *)&packet->reservedD);
    sciSend(scilinREG, 1, (uint8 *)&packet->reservedE);
    sciSend(scilinREG, 1, (uint8 *)&packet->etx);
    sciSend(scilinREG, 2, (uint8 *)&packet->crc);
}


