// Read destination and source from every ethernet packet

#include "mbed.h"

Serial pc(USBTX, USBRX, 115200);
Ethernet eth;

int main() {
    char buf[0x600];
    uint8_t len[2];
    for (int i = 0; i < 0x600; ++i)
        buf[i] = 0;
    
    
    while(1) {
        int size = eth.receive();
        if(size > 0) {
            eth.read(buf, size);
            len[1] = ((uint16_t)size>>0x08) & 0xFF;
            len[0] = (uint16_t)size & 0xFF;
            pc.putc(len[0]);
            pc.putc(len[1]);
            pc.puts(buf);
            for (int i = 0; i < 0x600; ++i)
                buf[i] = 0;
            //for (int i = 0; i < size; ++i)
            //    pc.putc(buf[i]);
            //pc.puts(len,2);
            //pc.puts(buf, size);
        }
    }
}
