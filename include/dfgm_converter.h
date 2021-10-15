#ifndef DFGM_CONVERTER_H
#define DFGM_CONVERTER_H

#include <stdint.h>
#include "HL_sci.h"
/**
 * File name convention
 * ccc-xxxxxxxxxx
 */

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

typedef struct __attribute__((packed)) {
    uint32_t X; // [xdac, xadc]
    uint32_t Y; // [ydac, yadc]
    uint32_t Z; // [zdac, zadc]
} dfgm_data_tuple_t;

typedef struct __attribute__((__packed__)) {
    uint8  dle;
    uint8  stx;
    uint8  pid;
    uint8  packet_type;
    uint16 packet_length;
    uint16 fs;
    uint32 pps_offset;
    uint16 hk[12];
    dfgm_data_tuple_t tup[100];
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
 * @brief Send DFGM data to terminal via serial port
 * 
 * @param packet Data to be displayed
 */
void send_packet(dfgm_packet_t *packet);

/**
 * @brief Initialize the DFGM interface
 */
void dfgm_init();

/**
 * @brief DFGM interrupt handling hook
 * 
 * @param sci 
 * @param flags 
 */
//static void dfgm_sciNotification(sciBASE_t *sci, unsigned flags);

#endif /* DFGM_CONVERTER_H_ */
