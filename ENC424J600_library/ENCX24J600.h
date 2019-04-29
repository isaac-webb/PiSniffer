#include <cstdint>

using namespace std;

// ENC424J600 Opcodes
const uint8_t RCR = 0x00;
const uint8_t RGPDATA = 0x28;
const uint8_t WGPDATA = 0x2A;
const uint8_t WCR = 0x40;
const uint8_t WGPRDPT = 0x60;
const uint8_t RGPRDPT = 0x62;
const uint8_t WGPWRPT = 0x6C;
const uint8_t RGPWRPT = 0x6E;
const uint8_t B0SEL = 0xC0;
const uint8_t B1SEL = 0xC2;
const uint8_t B2SEL = 0xC4;
const uint8_t B3SEL = 0xC6;
const uint8_t SETETHRST = 0xCA;

// ENC424J600 Registers
const uint8_t EUDASTL = 0x16;
const uint8_t EUDASTH = 0x17;
const uint8_t ESTATL = 0x1A;
const uint8_t ESTATH = 0x1B;

// Miscellaneous bitmasks
const uint8_t CLKRDY = 1 << 4;

class ENCX24J600 {
	unsigned handle;
public:
	// Constructors and destructors
	ENCX24J600(unsigned channel, unsigned ce);
	void close();
	
	// General reset/control methods
	void reset_ethernet();
	
	// Methods for reading and writing control registers
	int select_bank(unsigned bank);
	int write_control_register_banked(uint8_t regno, char val);
	int read_control_register_banked(uint8_t regno, char *val);
	
	// Methods for reading/writing data into SRAM
	int write_data(char *data, uint16_t len, uint16_t dst_addr);
	int read_data(char *data, uint16_t len, uint16_t src_addr);
};
