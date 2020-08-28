#ifndef DFGM_CONVERTER_H
#define DFGM_CONVERTER_H

// mag field constants
#define XDACScale 1.757421875
#define XADCScale -0.0353
#define XOffset 0
#define YDACScale 2.031835938
#define YADCScale -0.0267
#define YOffset 0
#define ZDACScale 1.934375
#define ZADCScale -0.0302
#define ZOffset 0

struct __attribute__((__packed__)) dfgm_packet_t{
    uint8  dle;
    uint8  stx;
    uint8  pid;
    uint8  packet_type;
    uint16 packet_length;
    uint16 fs;
    uint32 pps_offset;
    uint16 hk[12];
    uint32 X[100]; // [xdac, xadc]
    uint32 Y[100]; // [ydac, yadc]
    uint32 Z[100]; // [zdac, zadc]
    uint16 board_id;
    uint16 sensor_id;
    uint8  reservedA;
    uint8  reservedB;
    uint8  reservedC;
    uint8  reservedD;
    uint8  reservedE;
    uint8 etx;
    uint16 crc;
};

void receive_packet(uint8 *buffer, struct dfgm_packet_t *packet);
void dfgm_convert_mag(uint8 *buffer, struct dfgm_packet_t *data);
void send_packet(struct dfgm_packet_t *packet);

#endif /* DFGM_CONVERTER_H_ */
