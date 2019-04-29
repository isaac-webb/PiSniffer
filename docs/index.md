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

![Custom Ethernet modules](https://github.com/isaac-webb/PiSniffer/raw/master/docs/pisniffermodule_no_planes.png)

### Mbed Ethernet Tap

![Ethernet tapping with mbeds](https://github.com/isaac-webb/PiSniffer/raw/master/docs/ethernet_tapping.png)

## Code

## Results

After all was said and done, we wound up with a breadboard that contained both
the mbed side of our demo as well as the Pi side of our demo:

![Breadboard setup](https://github.com/isaac-webb/PiSniffer/raw/master/docs/setup.jpg)

Unfortunately, due to the issues we struggled with above, we were unable to
integrate the two pieces of our project. For this reason, we have included
demos/documentation of both parts:

1. Our custom Ethernet controller breakout board.
2. Our `tshark` sniffing demo that uses two Ethernet ports, two mbeds, and a
   Raspberry Pi to get data off of the line.

This indicates that, if we were able to overcome the aforementioned issues in
the future, we should be able to combine them, resulting in our initial goal of
a flexible, elegant hardware man-in-the-middle device for the Pi.

### Hardware Demo

![Writing to EUDAST](https://github.com/isaac-webb/PiSniffer/raw/master/docs/eudast_write.png)
![Verifying EUDAST](https://github.com/isaac-webb/PiSniffer/raw/master/docs/eudast_read.png)
![Reading the MAC address, logic analyzer](https://github.com/isaac-webb/PiSniffer/raw/master/docs/mac_read.png)
![Reading the MAC address, terminal output](https://github.com/isaac-webb/PiSniffer/raw/master/docs/mac_read_terminal.jpg)

### Software Demo

<iframe width="1280" height="720" src="https://www.youtube.com/embed/qo8h67pPoVU" frameborder="0" allow="accelerometer; autoplay; encrypted-media; gyroscope; picture-in-picture" allowfullscreen></iframe>
<iframe width="1280" height="720" src="https://www.youtube.com/embed/-2y3PTK_6E8" frameborder="0" allow="accelerometer; autoplay; encrypted-media; gyroscope; picture-in-picture" allowfullscreen></iframe>