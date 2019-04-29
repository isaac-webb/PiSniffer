#include <cstdint>

using namespace std;

// ENC424J600 Opcodes
const uint8_t RCR = 0x00;
const uint8_t RGPDATA = 0x30;
const uint8_t WGPDATA = 0x32;
const uint8_t WCR = 0x40;
const uint8_t WGPRDPT = 0x68;
const uint8_t RGPRDPT = 0x6A;
const uint8_t WGPWRPT = 0x74;
const uint8_t RGPWRPT = 0x76;
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
const uint8_t EGPRDPTL = 0x06;
const uint8_t EGPWRPTL = 0x08;
const uint8_t MAADR3L = 0x00;
const uint8_t MAADR3H = 0x01;
const uint8_t MAADR2L = 0x02;
const uint8_t MAADR2H = 0x03;
const uint8_t MAADR1L = 0x04;
const uint8_t MAADR1H = 0x05;

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
