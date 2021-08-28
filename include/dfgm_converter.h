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

typedef struct __attribute__((__packed__)) {
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
} dfgm_packet_t;

/**
 * @brief convert raw DFGM data to magnetic field data
 * 
 * @param data DFGM packet to process
 */
void dfgm_convert_mag(dfgm_packet_t * const data);

/**
 * @brief Send DFGM data to termincal via serial port
 * 
 * @param packet Data to be displayed
 */
void send_packet(dfgm_packet_t *packet);

/**
 * @brief Initialize the DFGM interface
 * 
 * @param sci Serial port connected to the DFGM
 */
void dfgm_init(const sciBASE_t *sci);

/**
 * @brief DFGM interrupt handling hook
 * 
 * @param sci 
 * @param flags 
 */
static void dfgm_sciNotification(sciBASE_t *sci, unsigned flags);

#endif /* DFGM_CONVERTER_H_ */
