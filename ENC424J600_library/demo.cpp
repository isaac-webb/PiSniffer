#include <cstdio>
#include <cstdlib>
#include <unistd.h>
#include <pigpio.h>
#include "ENCX24J600.h"

using namespace std;

static char data[64];
static uint8_t len;

void stop() {
    // Terminate the program
    gpioHardwareClock(4, 0);
    gpioTerminate();
}

int main() {
    // Read in the file we are going to hash
    FILE *f = fopen("input.txt", "r");
    fgets(data, 64, f);
    fclose(f);
    for (len = 0; data[len]; ++len);
    len--;
    printf("Data: %s\nLength: %d\n", data, len);
    
    // Initialize the GPIO library
    if (gpioInitialise() < 0) {
        perror("Initialization failed!\n");
        stop();
        exit(EXIT_FAILURE);
    }

    // Turn on the hardware clock
    gpioHardwareClock(4, 25000000);

    // Initialize the driver object
    ENCX24J600 enc(0, 0);

    // Wait for the SPI bus to stabilize
    printf("Writing 0x1234 to EUDAST\n");
    enc.select_bank(0);
    char high;
    char low;
    do {
        enc.write_control_register_banked(EUDASTL, 0x34);
        enc.write_control_register_banked(EUDASTH, 0x12);
        enc.read_control_register_banked(EUDASTL, &low);
        enc.read_control_register_banked(EUDASTH, &high);
    } while (high != 0x12 || low != 0x34);
    printf("SPI bus is active\n");
    
    // Reset the controller
    enc.reset_ethernet();
    sleep(1);
    
    // Verify that EUDAST is reset to 0
    enc.read_control_register_banked(EUDASTL, &low);
    enc.read_control_register_banked(EUDASTH, &high);
    if (high || low) {
        fprintf(stderr, "EUDAST was not reset to 0x%02x%02x!\n",
                high, low);
        stop();
        exit(EXIT_FAILURE);
    }
    printf("Reset successful!\n");

    // Write the data into the on-chip SRAM
    enc.write_data(data, len, 0x1000);
    char read_data[64];
    enc.read_data(read_data, len, 0x1000);
    printf("Data read from SRAM: %s\n", read_data);

    // Clean up and terminate
    enc.close();
    stop();
    return 0;
}
