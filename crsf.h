/*
 * crsf_enhanced.h
 *
 *  Created on: Dec 6, 2025
 *      Author: pappa
 */

#ifndef CRSF_H
#define CRSF_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

// ==============================
// CRSF Protocol Constants
// ==============================

// Sync bytes
#define CRSF_SYNC_BYTE           0xC8
#define CRSF_SYNC_BYTE_ELRS      0xEE

// Frame sizes
#define CRSF_MAX_PAYLOAD_LEN     62
#define CRSF_FRAME_SIZE_MAX      64
#define CRSF_HEADER_LEN          2      // Device addr + Frame size
#define CRSF_FRAME_LEN_OFFSET    1
#define CRSF_FRAME_TYPE_OFFSET   2
#define CRSF_FRAME_PAYLOAD_OFFSET 3

// Device addresses
#define CRSF_ADDRESS_BROADCAST   0x00
#define CRSF_ADDRESS_USB         0x10
#define CRSF_ADDRESS_TBS_CORE_PNP 0x80
#define CRSF_ADDRESS_RESERVED1   0x8A
#define CRSF_ADDRESS_CURRENT_SENSOR 0xC0
#define CRSF_ADDRESS_GPS         0xC2
#define CRSF_ADDRESS_TBS_BLACKBOX 0xC4
#define CRSF_ADDRESS_FLIGHT_CONTROLLER 0xC8
#define CRSF_ADDRESS_RESERVED2   0xCA
#define CRSF_ADDRESS_RACE_TAG    0xCC
#define CRSF_ADDRESS_RADIO_TRANSMITTER 0xEA
#define CRSF_ADDRESS_CRSF_RECEIVER 0xEC
#define CRSF_ADDRESS_CRSF_TRANSMITTER 0xEE

#define CRSF_CHANNELS_DEFAULT \
{                             \
    .ch1 = 1500,              \
    .ch2 = 1500,              \
    .ch3 = 1500,              \
    .ch4 = 1500,              \
    .ch5 = 1500,              \
    .ch6 = 1500,              \
    .ch7 = 1500,              \
    .ch8 = 1500,              \
    .ch9 = 1500,              \
    .ch10 = 1500,             \
    .ch11 = 1500,             \
    .ch12 = 1500,             \
    .ch13 = 1500,             \
    .ch14 = 1500,             \
    .ch15 = 1500,             \
    .ch16 = 1500              \
}

// ==============================
// CRSF Frame Types
// ==============================

typedef enum {
    // Telemetry frames (Receiver -> Transmitter)
    CRSF_FRAMETYPE_GPS = 0x02,
    CRSF_FRAMETYPE_VARIO = 0x03,
    CRSF_FRAMETYPE_BATTERY_SENSOR = 0x08,
    CRSF_FRAMETYPE_BARO_ALTITUDE = 0x09,
    CRSF_FRAMETYPE_HEARTBEAT = 0x0B,
    CRSF_FRAMETYPE_LINK_STATISTICS = 0x14,
    CRSF_FRAMETYPE_OPENTX_SYNC = 0x10,
    CRSF_FRAMETYPE_RADIO_ID = 0x3A,

    // Attitude and position
    CRSF_FRAMETYPE_ATTITUDE = 0x1E,
    CRSF_FRAMETYPE_FLIGHT_MODE = 0x21,

    // RC Channels (Transmitter -> Receiver)
    CRSF_FRAMETYPE_RC_CHANNELS_PACKED = 0x16,
    CRSF_FRAMETYPE_SUBSET_RC_CHANNELS_PACKED = 0x17,
    CRSF_FRAMETYPE_LINK_STATISTICS_RX = 0x1C,
    CRSF_FRAMETYPE_LINK_STATISTICS_TX = 0x1D,

    // Device communication
    CRSF_FRAMETYPE_DEVICE_PING = 0x28,
    CRSF_FRAMETYPE_DEVICE_INFO = 0x29,
    CRSF_FRAMETYPE_PARAMETER_SETTINGS = 0x2C,
    CRSF_FRAMETYPE_PARAMETER_READ = 0x2D,
    CRSF_FRAMETYPE_COMMAND = 0x32,

    // ELRS specific
    CRSF_FRAMETYPE_ELRS_STATUS = 0x2A,
    CRSF_FRAMETYPE_ELRS_BOOTLOADER = 0x30,

    // MSP over CRSF (Betaflight/iNav)
    CRSF_FRAMETYPE_MSP_REQ = 0x7A,     // MSP request
    CRSF_FRAMETYPE_MSP_RESP = 0x7B,    // MSP response
    CRSF_FRAMETYPE_MSP_WRITE = 0x7C,   // MSP write

    // Custom / vendor specific
    CRSF_FRAMETYPE_ARDUINO = 0x80
} crsf_frame_type_e;

// ==============================
// ELRS Specific Constants
// ==============================

// ELRS Packet rates
typedef enum {
    ELRS_PACKET_RATE_50HZ = 0,
    ELRS_PACKET_RATE_150HZ = 1,
    ELRS_PACKET_RATE_250HZ = 2,
    ELRS_PACKET_RATE_500HZ = 3,
    ELRS_PACKET_RATE_1000HZ = 4
} elrs_packet_rate_e;

// ELRS Power levels
typedef enum {
    ELRS_POWER_10mW = 0,
    ELRS_POWER_25mW = 1,
    ELRS_POWER_50mW = 2,
    ELRS_POWER_100mW = 3,
    ELRS_POWER_250mW = 4,
    ELRS_POWER_500mW = 5,
    ELRS_POWER_1000mW = 6,
    ELRS_POWER_2000mW = 7
} elrs_power_level_e;

// ELRS RF modes
typedef enum {
    ELRS_RF_MODE_4CH = 0,
    ELRS_RF_MODE_DYNAMIC = 1,
    ELRS_RF_MODE_250HZ = 2,
    ELRS_RF_MODE_500HZ = 3
} elrs_rf_mode_e;

// ==============================
// Packed Structures (no padding)
// ==============================

#pragma pack(push, 1)

// Base frame structure
typedef struct {
    uint8_t device_addr;
    uint8_t frame_size;
    uint8_t type;
    uint8_t payload[CRSF_MAX_PAYLOAD_LEN];
    uint8_t crc;
} crsf_frame_t;

// RC Channels Packed (0x16) - 24 bytes total
typedef struct {
    uint8_t sync;
    uint8_t len;
    uint8_t type;
    uint8_t data[22];  // 16 channels * 11 bits = 176 bits = 22 bytes
    uint8_t crc;
} crsf_rc_channels_packed_t;

// Link Statistics (0x14) - 12 bytes total
typedef struct {
    uint8_t sync;
    uint8_t len;
    uint8_t type;
    uint8_t uplink_rssi_ant1;
    uint8_t uplink_rssi_ant2;
    uint8_t uplink_link_quality;
    uint8_t uplink_snr;
    uint8_t active_antenna;
    uint8_t rf_mode;
    uint8_t uplink_tx_power;
    uint8_t downlink_rssi;
    uint8_t downlink_link_quality;
    uint8_t downlink_snr;
    uint8_t crc;
} crsf_link_stats_t;

// GPS (0x02) - 18 bytes total
typedef struct {
    uint8_t sync;
    uint8_t len;
    uint8_t type;
    int32_t latitude;
    int32_t longitude;
    uint16_t ground_speed;
    uint16_t heading;
    int32_t altitude;
    uint8_t satellites;
    uint8_t crc;
} crsf_gps_t;

// Battery Sensor (0x08) - 11 bytes total
typedef struct {
    uint8_t sync;
    uint8_t len;
    uint8_t type;
    uint16_t voltage;
    uint16_t current;
    uint32_t capacity : 24;
    uint8_t remaining;
    uint8_t crc;
} crsf_battery_t;

// Attitude (0x1E) - 9 bytes total
typedef struct {
    uint8_t sync;
    uint8_t len;
    uint8_t type;
    int16_t pitch;
    int16_t roll;
    int16_t yaw;
    uint8_t crc;
} crsf_attitude_t;

// Device Info (0x29) - variable size
typedef struct {
    uint8_t sync;
    uint8_t len;
    uint8_t type;
    uint8_t dest_addr;
    uint8_t src_addr;
    uint8_t dev_type;
    uint8_t dev_id;
    uint8_t name_len;
    char name[16];
    uint8_t crc;
} crsf_device_info_t;

// ELRS Status (0x2A) - 11 bytes total
typedef struct {
    uint8_t sync;
    uint8_t len;
    uint8_t type;
    uint8_t packet_rate;
    uint8_t tx_power;
    uint8_t rx_sensitivity;
    uint8_t signal_quality;
    uint8_t snr;
    uint8_t antenna;
    uint8_t model_match;
    uint8_t ph_mode;
    uint8_t crc;
} crsf_elrs_status_t;

// MSP over CRSF request (0x7A)
typedef struct {
    uint8_t sync;
    uint8_t len;
    uint8_t type;
    uint8_t dest_addr;
    uint8_t src_addr;
    uint8_t msp_version;
    uint8_t payload_size;
    uint8_t packet_id;
    uint8_t function;
    uint8_t payload[CRSF_MAX_PAYLOAD_LEN - 7];
    uint8_t crc;
} crsf_msp_request_t;

// MSP over CRSF response (0x7B)
typedef struct {
    uint8_t sync;
    uint8_t len;
    uint8_t type;
    uint8_t dest_addr;
    uint8_t src_addr;
    uint8_t msp_version;
    uint8_t payload_size;
    uint8_t packet_id;
    uint8_t function;
    uint8_t payload[CRSF_MAX_PAYLOAD_LEN - 7];
    uint8_t crc;
} crsf_msp_response_t;

// Union for accessing any frame type
typedef union {
    uint8_t raw[CRSF_FRAME_SIZE_MAX];
    crsf_frame_t frame;
    crsf_rc_channels_packed_t rc_channels;
    crsf_link_stats_t link_stats;
    crsf_gps_t gps;
    crsf_battery_t battery;
    crsf_attitude_t attitude;
    crsf_device_info_t device_info;
    crsf_elrs_status_t elrs_status;
    crsf_msp_request_t msp_request;
    crsf_msp_response_t msp_response;
} crsf_frame_union_t;

#pragma pack(pop)

// ==============================
// Channel Helper Structure
// ==============================

// For easier channel manipulation (not part of wire protocol)
typedef struct {
    uint16_t ch1;
    uint16_t ch2;
    uint16_t ch3;
    uint16_t ch4;
    uint16_t ch5;
    uint16_t ch6;
    uint16_t ch7;
    uint16_t ch8;
    uint16_t ch9;
    uint16_t ch10;
    uint16_t ch11;
    uint16_t ch12;
    uint16_t ch13;
    uint16_t ch14;
    uint16_t ch15;
    uint16_t ch16;
} crsf_channels_t;

// ==============================
// Function Prototypes
// ==============================

#ifdef __cplusplus
extern "C" {
#endif


// CRC functions
uint8_t crsf_crc8(const uint8_t *data, size_t len);

// RC Channel functions
void crsf_pack_channels(uint8_t *buffer, const uint16_t *channels);
void crsf_unpack_channels(const uint8_t *buffer, uint16_t *channels);
void crsf_create_rc_frame(crsf_frame_union_t *frame, const crsf_channels_t *channels);

/*
bool crsf_validate_crc(const crsf_frame_t *frame);

// Frame creation helpers
void crsf_init_frame(crsf_frame_t *frame, uint8_t type, uint8_t dest_addr);
size_t crsf_finalize_frame(crsf_frame_t *frame);

// RC Channel functions
void crsf_pack_channels(uint8_t *buffer, const uint16_t *channels);



// Telemetry creation functions
void crsf_create_link_stats(crsf_frame_union_t *frame,
                           uint8_t uplink_rssi1, uint8_t uplink_rssi2,
                           uint8_t uplink_lq, uint8_t uplink_snr,
                           uint8_t active_ant, uint8_t rf_mode,
                           uint8_t tx_power, uint8_t downlink_rssi,
                           uint8_t downlink_lq, uint8_t downlink_snr);

void crsf_create_gps_frame(crsf_frame_union_t *frame,
                          int32_t lat, int32_t lon,
                          uint16_t speed, uint16_t heading,
                          int32_t alt, uint8_t sats);

void crsf_create_battery_frame(crsf_frame_union_t *frame,
                              uint16_t voltage, uint16_t current,
                              uint32_t capacity, uint8_t remaining);
*/

// Parsing functions
crsf_frame_type_e crsf_get_frame_type(const uint8_t *data);
bool crsf_is_valid_frame(const uint8_t *data, size_t len);

/*
// Utility functions
uint16_t crsf_channel_to_us(uint16_t crsf_val);
uint16_t crsf_us_to_channel(uint16_t us_val);

// ELRS specific functions
void crsf_create_elrs_status(crsf_frame_union_t *frame,
                            uint8_t packet_rate, uint8_t tx_power,
                            uint8_t rx_sens, uint8_t signal_quality,
                            uint8_t snr, uint8_t antenna,
                            uint8_t model_match, uint8_t ph_mode);
                            */

#ifdef __cplusplus
}
#endif

#endif // CRSF_H

