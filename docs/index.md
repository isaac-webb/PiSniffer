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



## Code

## Results

After all was said and done, we wound up with a breadboard that contained both
the mbed side of our demo as well as the Pi side of our demo:

![Breadboard setup](https://github.com/isaac-webb/PiSniffer/raw/master/docs/setup.jpg)
