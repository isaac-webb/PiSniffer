#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <pigpio.h>
#include <unistd.h>
#include "ENCX24J600.h"

using namespace std;

// Opens the SPI handle for later use
ENCX24J600::ENCX24J600(unsigned channel, unsigned ce) {
	// Open the SPI bus
	handle = spiOpen(channel, 10000000, 0);
	
	// Make sure we succeeded in opening the SPI channel
	if (handle < 0) {
		perror("Failed to initialize SPI\n");
	}
}

// Closes the SPI handle
void ENCX24J600::close() {
	// Close the SPI bus
	if (spiClose(handle) < 0) {
		perror("Failed to close SPI\n");
	}
}

// Issues a reset to the Ethernet controller
void ENCX24J600::reset_ethernet() {
	spiWrite(handle, (char *) &SETETHRST, 1);
}

// Selects the register bank
int ENCX24J600::select_bank(unsigned bank) {
	// Select the correct bank
	char tx;
	switch (bank) {
	case 0:
		tx = B0SEL;
		break;
	case 1:
		tx = B1SEL;
		break;
	case 2:
		tx = B2SEL;
		break;
	case 3:
		tx = B3SEL;
		break;
	default:
		return -1;
	}
	
	return spiWrite(handle, &tx, 1);
}

// Writes 1 byte to the provided banked control register
int ENCX24J600::write_control_register_banked(uint8_t regno, char val) {
	char tx_buf[2];
	
	// Write the value to the register
	tx_buf[0] = WCR | regno;
	tx_buf[1] = val;
	
	return spiWrite(handle, tx_buf, 2);
}

// Reads 1 byte from the provided banked control register
int ENCX24J600::read_control_register_banked(uint8_t regno, char *val) {
	char tx_buf[2];
	char rx_buf[2];
	
	// Write the read command
	tx_buf[0] = RCR | regno;
	tx_buf[1] = 0;
	
	int ret;
	if ((ret = spiXfer(handle, tx_buf, rx_buf, 2)) < 0) {
		return ret;
	}
	
	*val = rx_buf[1];
	
	return 0;
}

// Writes data into the chip's SRAM
int ENCX24J600::write_data(char *data, uint16_t len,
						   uint16_t dst_addr) {
	// Write the destination address to the GP write pointer
	char command[len + 1];
	command[0] = WGPWRPT;
	command[1] = dst_addr & 0xFF;
	command[2] = (dst_addr >> 8) & 0xFF;
	if (spiWrite(handle, command, 3) != 3) {
		return -1;
	}
	
	command[0] = RGPWRPT;
	command[1] = 0;
	command[2] = 0;
	if (spiWrite(handle, command, 3) != 3) {
		return -1;
	}
	
	// Write the actual data
	command[0] = WGPDATA;
	memcpy(command + 1, data, len);
	spiWrite(handle, command, len + 1);
	return 1;
}

// Reads data from the chip's SRAM
int ENCX24J600::read_data(char *data, uint16_t len, uint16_t src_addr) {
	// Write the source address to the GP read pointer
	char command[len + 1];
	command[0] = WGPRDPT;
	command[1] = src_addr & 0xFF;
	command[2] = (src_addr >> 8) & 0xFF;
	if (spiWrite(handle, command, 3) != 3) {
		return -1;
	}
	
	// Read the actual data
	char rx_buf[len + 1];
	command[0] = RGPDATA;
	command[1] = 0;
	command[2] = 0;
	int ret;
	if ((ret = spiXfer(handle, command, rx_buf, len + 1)) < 0) {
		return -1;
	}
	memcpy(data, rx_buf + 1, len);
	
	return ret;
}
