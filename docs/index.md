# PiSniffer Documentation

Ethernet, despite being a relatively old connector/bus, is still ubiquitous in
modern computing. While Wi-Fi has grown in popularity and become faster and
more secure over time, Ethernet is still the networking connection of choice
when users want maximum speed and reliability. Additionally, from a security
perspective, Ethernet is much more desirable: there are no powerful observable
radio waves traveling freely through space that any regular device can see if
correctly configured. Instead, to "sniff" on Ethernet lines, you must be
somehow inserted in between two devices you want to listen in on. So, for our
ECE 4180 final project, we decided to learn a bit more about how you might
build such a device. Below is a documentation of our methodology, our process,
and our results.

## Ethernet

Ethernet evolved a lot since its inception. Over time, it has gotten
significantly faster, and some modern devices support Ethernet speeds in excess
of 10Gb/s. Additionally, Ethernet has moved away from the "bus" design towards
a store-and-forward packet switching strategy of connecting devices. In the
"good 'ole days" of Ethernet, a malicious device could actually listen in on
traffic going between devices on its network because devices were actually
electrically connected (usually using "hubs"). Old Ethernet specifications
include details on CMSA/CD, the protocol for arbitrating the bus. Nowadays,
this is not possible. High data rates have necessitated a more efficient
interconnect, and switches deal with connecting many devices together.
Switches, as opposed to hubs, do not electrically connect their clients.
Instead, they receive Ethernet frames from their clients and forward them to
other connected clients based on the Layer 2 routing information contained in
the Ethernet frame's header.

The other complicating factor in this project is Ethernet's ability to support
full-duplex communication. This means that devices can be receiving and
transmitting information simulatenously. This means that, if we wanted to
passively "tap" an Ethernet line, which simply consists of a few RJ-45 jacks
and basic wiring between them, we would actually have to have 4 jacks total: 1
for the first client, 1 for the second client, 1 to read data going from client
1 to client 2, and 1 to read data going from client 2 to client 1. Not only is
this solution space-inefficient, but it isn't nearly as flexible as the design
we wanted to implement. Thus, in the following explanation, you will see us
strive to design a device that acts much more like a malicious switch.

## Hardware

Since ECE 4180 is a class in embedded systems, we wanted to design a devices
that could be used with either the LPC1768 mbed device or the Raspberry Pi. Due
to its versatility and ability to run `tshark` and other analysis programs, we
chose to make the Raspberry Pi the target device for this project (more on that
later). While the Pi does have a single Ethernet port on-board, that doesn't
allow the sniffing we desired, and it suffers from extremely limited
documentation. Deciding, initially, that we wanted to design a HAT for the Pi
that would add two Ethernet ports, enabling a man-in-the-middle attack, we set
out to find the right hardware.

### Ethernet Controller: ENC424J600

While internet links have not universally reached gigabit speeds, almost all
modern Ethernet hardware supports the 1000Base-T specification, meaning that
devices connected through Ethernet can usually transmit data at 1Gb/s. In a
perfect world, our Pi HAT would support sniffing these extremely high-speed
lines; however, due to both processing overhead and the extreme complications
in PCB design that sniffing at such high speeds would introduce, we opted to
target the 100Base-T specification. After some searching, we settled on using
the [ENC424J600](https://www.microchip.com/wwwproducts/en/ENC424J600) Ethernet
controller IC from Microchip. The reasons for this were simple:

1. It is cheap. At just a few bucks per chip, it keeps board costs low.
2. It supports communication via the SPI interface. While this does limit the
   maximum rate at which data can be received and transmitted, it makes the PCB
   layout process much easier and makes it more compatible. It does also
   support a 16-bit parallel interface that would help saturate the theoretical
   maximum Ethernet speed.

### PCB Take One: Pi HAT

Being ambitious, we set out to make an all-in-one board that would slot right
onto a Pi, adding two Ethernet ports with a bit of coding effort on our part;
however, we found out quickly that time constraints would make this difficult.
After laying out our initial attempt and starting the fabrication process in
the Hive, we realized that our assumptions about wiring the Ethernet controller
were incorrect.

#### Ethernet Magnetics

To isolate devices and make the communication between devices more resistant to
EMI, most Ethernet devices or jacks include some form of "magnetics," or a
special configuration of transformers that simply allows the cable and the
device to communicate but be electrically isolated. Our design utilizes
"MagJacks" which incorporate the magnetics and the LEDs into the RJ-45 jack. We
had initially assumed that we would be able to leave the center taps of the TX
and RX transformers unconnected; however, after a more careful inspection of
the datasheet, we realized that the controller induces voltages in these
transformers by generating differential currents, not voltages. This meant that
the connections to ground/3.3V that we omitted in the failed design below would
make the device non-functional.

![Failed HAT PCB](https://github.com/isaac-webb/PiSniffer/raw/master/docs/failed_pcb.jpg)

#### Theoretically Correct HAT Design

Learning from our mistake, we corrected the HAT's schematic and board to
include connections that allow *differential* currents to be induced in the
Ethernet Magjack's included magnetics; however, we have **not** verified this
design, as we opted to manufacture the Ethernet breakouts described in the next
section instead.

![Theoretically functioning HAT](https://github.com/isaac-webb/PiSniffer/raw/master/docs/pisniffer.png)

### PCB Take Two: Ethernet Breakouts

To both reduce manufacturing time and make our design more versatile for
prototyping, we opted for the following breakout board design:

![Custom Ethernet modules](https://github.com/isaac-webb/PiSniffer/raw/master/docs/pisniffermodule_no_planes.png)

This breakout simply provides easy access to the chip's SPI interface and a few
other necessary pins. This design also contains far more resistors and
capacitors next to the Ethernet jack. These are the impedance matching and
filtering components that the chip's datasheet recommends. We included these in
hopes of our board working (adhering to the datasheet is often a safe way to
ensure proper operation).

### Mbed Ethernet Tap

Unforunately, these breakout boards did not work as expected, so it is
necessary to explain one more hardware configuration that we ended up using in
our final demos.

![Ethernet tapping with mbeds](https://github.com/isaac-webb/PiSniffer/raw/master/docs/ethernet_tapping.png)

Somewhat similar to the completely passive Ethernet tap described in the
introduction, this hardware configuration consists of two mbed devices
listening to traffic passing between two Ethernet jacks. These jacks are
transparent to the devices connected by them, but they allow the mbeds to
observe any and all data traveling on the line, which they then send to the
Raspberry Pi to process using `tshark`. This configuration is nothing more than
two Ethernet jacks, two mbeds, and some wiring in between, and it allowed us
to demonstrate the concept we were aiming to implement as closely as possible.

## Code

While our code would have been much more complicated if our hardware designs
had functioned properly, there was still a significant amount of effort
expended in developing code that provided the greatest flexibility.

### ENC424J600 Library/Driver

To facilitate interfacing with the Ethernet controller, we developed a library
that allowed us to treat the controller like a C++ object. Additionally, we
included several constants and bitmasks that made the code much more readable
for potential developers. An example of a member function we wrote and driver
using it are below (full source is included in the repository).

#### ENC424J600 SPI Interface

It is worth noting that this chip has a very unique way of transferring data.
To reduce SPI traffic, it allows addressing registers using a "banked" address.
To read from a register, you must first switch to the proper bank and then read
from the register's address. This may seem more complicated, but it allows only
1 byte to be transferred to the chip to tell it to perform a read operation and
where to read from.

Additionally, the chip uses an interesting system of "pointer" registers and
data registers that allow it to operate entirely in its own address space using
on-chip SRAM. To write data to the chip, you must first set the write pointer
to the desired address in SRAM, then write the data you wish to write into the
corresponding write register. Unfortunately, we were unable to get the chip to
respond to read/write operations dealing with SRAM, possibly due to a defect or
due to damage done to the chip while assembling/learning how it operates.

#### Library Member Function Example

This function reads a byte from the provided register and stores the result in
the character `*val` passed in as a parameter.

```c
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
```

#### Library Driver Example

This example uses the function above to write a value to a control register and
read it to verify that the SPI bus is functioning properly before the program
continues executing.

```c
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
```

### Ethernet Tap Code

While simpler, the Ethernet tapping code served two important purposes:

1. Getting Ethernet data from the mbeds to the Pi.
2. Taking hat Ethernet data and inserting it into the Linux networking stack
   for later use with `tshark`.

#### Mbed

This code segment receives an Ethernet frame, sends its length over the USB
serial interface so the host knows how much data to expect, and then sends the
data itself.

```c
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
        }
    }
}
```

#### Pi

This code segment reads in the data on the USB serial interface sent from the
above code segment and forwards the packets onto a loopback interface that
`tshark` can easily read from to process and analyze packets.

```python
device = serial.Serial('COM6', 115200)

interface = IFACES.dev_from_index(5)
socket = conf.L2socket(iface = interface)
print(interface)

while True:
    len_bit_1 = device.read()
    len_bit_2 = device.read()
    len_msg = int.from_bytes(len_bit_1,'big') + int.from_bytes(len_bit_2,'big')
    data = device.read(len_msg)
    socket.send(data)
```

## Results

After all was said and done, we wound up with a breadboard that contained both
the mbed side of our demo as well as the Pi side of our demo:

![Breadboard setup](https://github.com/isaac-webb/PiSniffer/raw/master/docs/setup.jpg)

Unfortunately, our breakout that we had intended to use for the entirety of the
project did not function properly. While we were able to interface with the
chip, the Ethernet jack did not respond to a cable being plugged into it,
indicating that we likely made an error in our PCB design or used a jack with
an incompatible magnetics configuration (there are different configurations
based on bus speed, whether the jack supports PoE, etc.). Thus, we were unable
to integrate the hardware and software pieces of our project. For this reason,
we have included demos/documentation of both parts:

1. Our custom Ethernet controller breakout board.
2. Our `tshark` sniffing demo that uses two Ethernet ports, two mbeds, and a
   Raspberry Pi to get data off of the line.

This indicates that, if we were able to overcome the aforementioned issues in
the future, we should be able to combine them resulting in our initial goal of
a flexible, elegant hardware man-in-the-middle device for the Pi.

### Hardware Demo

To prove that our library works (to an extent), we have included logic analyzer
captures showing a quick initialization sequence, followed by a read of the
chip's factory-programmed MAC address.

#### EUDAST Power-on Sequence

The datasheet recommends performing a few different steps to verify that the
chip is ready to operate after powering on. The first two are:

1. Write `0x1234` to `EUDAST`.
2. Read `EUDAST`, repeating step 1 until reading shows `0x1234`.

The following screen capture shows the Pi writing `0x12` and `0x34` to
`EUDASTH` and `EUDASTL`, respectively.

![Writing to EUDAST](https://github.com/isaac-webb/PiSniffer/raw/master/docs/eudast_write.png)

Shortly following this, the Pi reads those same two registers. The MISO line
shows that the Ethernet controller did indeed change those registers' values.

![Verifying EUDAST](https://github.com/isaac-webb/PiSniffer/raw/master/docs/eudast_read.png)

To show another set of registers functioning, we wrote a program that reads the
chip's factory-programmed MAC address. Here you will see 7 distinct transfer
groups (look at the SCK line). The first transfer tells the chip to select the
register bank which contains the MAADR, followed by 6 reads which get the
6-byte MAC address.

![Reading the MAC address, logic analyzer](https://github.com/isaac-webb/PiSniffer/raw/master/docs/mac_read.png)

The values in the capture above do indeed match the output that we see from the
program on the Pi:

![Reading the MAC address, terminal output](https://github.com/isaac-webb/PiSniffer/raw/master/docs/mac_read_terminal.jpg)

### Software Demo

The following two videos are examples of what sorts of data can be expected off
of the Ethernet tap. The first shows how insecure forms can reveal sensitive
data. The second shows that you can see where and what devices are requesting
on the Ethernet line, allowing activity analysis and pattern recognition later
on if desired.

<iframe width="560" height="315" src="https://www.youtube.com/embed/qo8h67pPoVU" frameborder="0" allow="accelerometer; autoplay; encrypted-media; gyroscope; picture-in-picture" allowfullscreen></iframe>
<iframe width="560" height="315" src="https://www.youtube.com/embed/-2y3PTK_6E8" frameborder="0" allow="accelerometer; autoplay; encrypted-media; gyroscope; picture-in-picture" allowfullscreen></iframe>

## Acknowledgements

Much of this work was done in Georgia Tech's ECE 4180 lab. The hardware and
software were developed by Isaac Webb and Elias Boyer.
