CC = gcc
CFLAGS = -Wall -Wextra -std=c99

# Executables
SIG_GEN = crc32_siggen
SECURE_BOOT = secure_boot

# Source files
SIG_GEN_SRCS = CRC32_Signature_Tool.c CRC32_Generate.c
SECURE_BOOT_SRCS = Secure_Boot.c CRC32_Generate.c

# Default target
all: $(SIG_GEN) $(SECURE_BOOT)

# Build the signature generator
$(SIG_GEN): $(SIG_GEN_SRCS)
	$(CC) $(CFLAGS) -o $@ $^

# Build the secure boot verifier
$(SECURE_BOOT): $(SECURE_BOOT_SRCS)
	$(CC) $(CFLAGS) -o $@ $^

sign:
ifeq ($(strip $(FILE)),)
	$(error Usage: make sign FILE=your_source.c)
endif
	./$(SIG_GEN) $(FILE) $(FILE).sig

verify:
ifeq ($(strip $(FILE)),)
	$(error Usage: make verify FILE=your_source.c)
endif
	./$(SECURE_BOOT) $(FILE) $(FILE).sig

# Clean build artifacts
clean:
	rm -f $(SIG_GEN) $(SECURE_BOOT) *.sig
