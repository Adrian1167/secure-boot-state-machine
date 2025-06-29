#include <stdio.h>
#include <stdint.h>
#include "CRC32_Generate.h"

int main(int argc, char **argv) {
    if (argc < 3) {
        printf("Usage: %s input_file output_signature\n", argv[0]);
        return 1;
    }

    generate_crc32_table(crc32_lookup_table);

    uint8_t crc[4];
    if (crc32_file(argv[1], crc) != 0) {
        printf("Failed to read file.\n");
        return 1;
    }

    FILE *f = fopen(argv[2], "wb");
    if (!f) {
        printf("Failed to open signature file for writing.\n");
        return 1;
    }
    fwrite(crc, 1, 4, f);
    fclose(f);

    printf("CRC32 signature written to %s\n", argv[2]);
    return 0;
}
