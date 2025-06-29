#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "CRC32_Generate.h"

/* We define the states for our secure boot concept */
typedef enum {
    STATE_INIT,
    STATE_CALC_CRC,
    STATE_LOAD_SIG,
    STATE_VERIFY_SIG,
    STATE_SUCCESS,
    STATE_FAIL,
    NUM_STATES
} SecureBootState;

typedef struct {
    SecureBootState state;
    uint8_t crc32[4];
    uint8_t signature[4];
    int error_code;
    const char *bin_path;
    const char *sig_path;
} SecureBootContext;

int load_signature(const char *sig_path, uint8_t *sig, size_t sig_len) {
    FILE *f = fopen(sig_path, "rb");
    if (!f) return -1;
    size_t read_bytes = fread(sig, 1, sig_len, f);
    fclose(f);
    return (read_bytes == sig_len) ? 0 : -1;
}

int simple_verify_signature(const uint8_t *sig, const uint8_t *hash) {
    return (memcmp(sig, hash, 4) == 0) ? 0 : -1;
}

/* Function pointer which points to the state handler */
typedef void (*StateHandler)(SecureBootContext *ctx);

/*  State handlers */
void handle_init(SecureBootContext *ctx) {
    generate_crc32_table(crc32_lookup_table);
    ctx->state = STATE_CALC_CRC;
}

void handle_calc_crc(SecureBootContext *ctx) {
    if (crc32_file(ctx->bin_path, ctx->crc32) == 0) {
        ctx->state = STATE_LOAD_SIG;
    } else {
        ctx->error_code = -1;
        ctx->state = STATE_FAIL;
    }
}

void handle_load_sig(SecureBootContext *ctx) {
    if (load_signature(ctx->sig_path, ctx->signature, sizeof(ctx->signature)) == 0) {
        ctx->state = STATE_VERIFY_SIG;
    } else {
        ctx->error_code = -2;
        ctx->state = STATE_FAIL;
    }
}

void handle_verify_sig(SecureBootContext *ctx) {
    if (simple_verify_signature(ctx->signature, ctx->crc32) == 0) {
        ctx->state = STATE_SUCCESS;
    } else {
        ctx->error_code = -3;
        ctx->state = STATE_FAIL;
    }
}

void handle_success(SecureBootContext *ctx) {
    printf("[+] Secure boot passed!\n");
}

void handle_fail(SecureBootContext *ctx) {
    printf("[!] Secure boot failed with error %d\n", ctx->error_code);
}

/* Array of state handlers defined above */
StateHandler state_handlers[NUM_STATES] = {
    handle_init,
    handle_calc_crc,
    handle_load_sig,
    handle_verify_sig,
    handle_success,
    handle_fail
};

int secure_boot(const char *bin_path, const char *sig_path) {
    SecureBootContext ctx = {
        .state = STATE_INIT,
        .error_code = 0,
        .bin_path = bin_path,
        .sig_path = sig_path
    };

    while (ctx.state != STATE_SUCCESS && ctx.state != STATE_FAIL) {
        state_handlers[ctx.state](&ctx);
    }

    state_handlers[ctx.state](&ctx);

    return (ctx.state == STATE_SUCCESS) ? 0 : -1;
}

int main(int argc, char **argv) {
    if (argc != 3) {
        printf("Usage: %s <binary_file> <signature_file>\n", argv[0]);
        return 1;
    }
    return secure_boot(argv[1], argv[2]);
}
