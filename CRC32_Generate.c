#include <stdio.h>
#include "CRC32_Generate.h"

uint32_t crc32_lookup_table[256];

/* We have to generate the lookup table for our CRC32 algohithm.
This is needed so that when we calculate the CRC for the binary file using crc32_file
function, the values needed will not pe calculated for each byte of data, but directly
retrieved from crc32_lookup_table */

void generate_crc32_table(uint32_t table[256]) {
    for (uint32_t i = 0; i < 256; i++) {
        uint32_t crc = i;
        for (int j = 0; j < 8; j++) {
            if (crc & 1)
                crc = (crc >> 1) ^ POLY;
            else
                crc = crc >> 1;
        }
        table[i] = crc;
    }
}

/* We need to calculate the CRC for our binary file corresponding to this program*/
int crc32_file(const char *filename, uint8_t out[4]) {
    FILE *f = fopen(filename, "rb");
    if (!f) return -1;

    uint32_t crc = 0xFFFFFFFF;
    uint8_t buf[1024];
    size_t read_len;

    while ((read_len = fread(buf, 1, sizeof(buf), f)) > 0) {
        for (size_t i = 0; i < read_len; i++) {
            uint8_t idx = (crc ^ buf[i]) & 0xFF;
            crc = (crc >> 8) ^ crc32_lookup_table[idx];
        }
    }
    fclose(f);

    crc ^= 0xFFFFFFFF;
    out[0] = (crc >> 24) & 0xFF;
    out[1] = (crc >> 16) & 0xFF;
    out[2] = (crc >> 8) & 0xFF;
    out[3] = crc & 0xFF;

    return 0;
}