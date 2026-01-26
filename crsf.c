/*
 * crsf.c
 *
 *  Created on: Dec 12, 2025
 *      Author: pappa
 */

#include <stdio.h>
#include <string.h>

#include "crsf.h"

// ==============================
// CRC Implementation
// ===========

uint8_t crsf_crc8(const uint8_t *data, size_t len) {
    uint8_t crc = 0;
    const uint8_t poly = 0xD5;

    for (size_t i = 0; i < len; i++) {
        crc ^= data[i];
        for (int bit = 0; bit < 8; bit++) {
            if (crc & 0x80) {
                crc = (crc << 1) ^ poly;
            } else {
                crc <<= 1;
            }
        }
    }
    return crc;
}

// ==============================
// Parsing Functions
// ==============================


crsf_frame_type_e crsf_get_frame_type(const uint8_t *data) {
    if (data[0] != CRSF_SYNC_BYTE && data[0] != CRSF_SYNC_BYTE_ELRS) {
        return 0xFF; // Invalid
    }

    if (data[1] < 2 || data[1] > CRSF_MAX_PAYLOAD_LEN + 2) {
        return 0xFF; // Invalid length
    }

    return (crsf_frame_type_e)data[2];
}


bool crsf_is_valid_frame(const uint8_t *data, size_t len) {
    if (len < 4) return false; // Minimum: sync + len + type + crc

    crsf_frame_type_e type = crsf_get_frame_type(data);
    if (type == 0xFF) return false;

    uint8_t frame_len = data[1];
    if (len < frame_len + 2) return false; // +2 for sync and len

    // Validate CRC
    uint8_t calculated_crc = crsf_crc8(&data[2], frame_len - 1); // -1 for CRC itself
    return calculated_crc == data[frame_len + 1];
}

// ==============================
// RC Channel Handling
// ==============================

void crsf_create_rc_frame(crsf_frame_union_t *frame, const crsf_channels_t *channels) {
    frame->rc_channels.sync = CRSF_SYNC_BYTE;
    frame->rc_channels.type = CRSF_FRAMETYPE_RC_CHANNELS_PACKED;
    frame->rc_channels.len = 24; // 22 bytes payload + type + len

    // Convert channels struct to array
    uint16_t channel_array[16] = {
        channels->ch1, channels->ch2, channels->ch3, channels->ch4,
        channels->ch5, channels->ch6, channels->ch7, channels->ch8,
        channels->ch9, channels->ch10, channels->ch11, channels->ch12,
        channels->ch13, channels->ch14, channels->ch15, channels->ch16
    };

    // Pack channels
    crsf_pack_channels(frame->rc_channels.data, channel_array);

    // Calculate CRC
    frame->rc_channels.crc = crsf_crc8(&frame->rc_channels.type, frame->rc_channels.len - 1);
}

void crsf_pack_channels(uint8_t *buffer, const uint16_t *channels) {
    // Pack 16 channels of 11 bits each into 22 bytes
    uint32_t bit_buffer = 0;
    int bit_count = 0;
    int byte_index = 0;

    for (int i = 0; i < 16; i++) {
        // Ensure value is 11-bit
        uint16_t val = channels[i] & 0x07FF;

        // Add to bit buffer
        bit_buffer |= ((uint32_t)val << bit_count);
        bit_count += 11;

        // Write out full bytes
        while (bit_count >= 8) {
            buffer[byte_index++] = bit_buffer & 0xFF;
            bit_buffer >>= 8;
            bit_count -= 8;
        }
    }

    // Write remaining bits
    if (bit_count > 0) {
        buffer[byte_index] = bit_buffer & 0xFF;
    }
}

void crsf_unpack_channels(const uint8_t *buffer, uint16_t *channels) {
    uint32_t bit_buffer = 0;
    int bit_count = 0;
    int byte_index = 0;

    for (int i = 0; i < 16; i++) {
        // Read bits until we have 11
        while (bit_count < 11) {
            bit_buffer |= ((uint32_t)buffer[byte_index++] << bit_count);
            bit_count += 8;
        }

        // Extract 11-bit value
        channels[i] = bit_buffer & 0x07FF;

        // Remove used bits
        bit_buffer >>= 11;
        bit_count -= 11;
    }
}

void crsf_to_string(const uint8_t *frame, char *return_string, size_t return_string_size){
	crsf_channels_t channels;
	crsf_frame_type_e type = crsf_get_frame_type(frame);

	memset(return_string, 0, return_string_size);

    switch (type) {
        case CRSF_FRAMETYPE_LINK_STATISTICS:
            // Process link stats
            break;
        case CRSF_FRAMETYPE_GPS:
            // Process GPS data
            break;
        case CRSF_FRAMETYPE_RC_CHANNELS_PACKED: // 0X16
			{
				crsf_rc_channels_packed_t *channels_packed = (crsf_rc_channels_packed_t *)frame;
				uint16_t *pchannels = (uint16_t *)&channels;

				// Process flight data
				crsf_unpack_channels(channels_packed->data, pchannels);
				snprintf(return_string, return_string_size,"CRSF_FRAMETYPE_RC_CHANNELS_PACKED:\n");
				snprintf(return_string + strlen(return_string), return_string_size, "Roll: %4d, Pitch: %4d Throttle: %4d, Yaw: %4d, ", channels.ch1, channels.ch2, channels.ch3, channels.ch4);
				snprintf(return_string + strlen(return_string), return_string_size, "Disarm: %4d, Flight Mode: %4d, Buzzer: %4d, Blackbox log activation: %4d, ", channels.ch5, channels.ch6, channels.ch7, channels.ch8);
				snprintf(return_string + strlen(return_string), return_string_size, "VTX Control: %4d, Pan: %4d, OSD Menu Navigation: %4d, RTH activation: %4d, ", channels.ch9, channels.ch10, channels.ch11, channels.ch12);
				snprintf(return_string + strlen(return_string), return_string_size, "LED Strip Control: %4d, Script Control: %4d, Trainer Mode: %4d, Custom / Reserved: %4d\n", channels.ch13, channels.ch14, channels.ch15, channels.ch16);
			}
            break;
        default:
        	printf("Unknown type 0x%02X\n", type);
        	printf("Received frame type: 0x%02X\n", type);
    }
}
