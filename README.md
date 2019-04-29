# PiSniffer

In this repository, you will find a few different things:

* Schematics and board designs for our Ethernet devices. This includes both our
  attempt at a Pi HAT and the modules which we actually ordered and assembled.
* A C++ library that utilizes the `pigpio` library to interface with the
  ENC424J600 Ethernet controller over the Pi's SPI interface.
* Code to run on the LPC1768 mbed microcontroller that sniffs Ethernet packets
  off of a tapped line.
* Python code to run on the Pi that waits for the mbed to send data over USB
  and forwards packets onto the loopback interface for analysis with `tshark`.

More detailed documentation can be found [here](https://iwebb.me/PiSniffer/)
