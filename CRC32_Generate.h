#ifndef CRC32_GENERATE_H
#define CRC32_GENERATE_H

#include <stdint.h>

#define POLY 0xEDB88320

extern uint32_t crc32_lookup_table[256];

void generate_crc32_table(uint32_t table[256]);
int crc32_file(const char *filename, uint8_t out[4]);

#endif // CRC32_GENERATE_H
