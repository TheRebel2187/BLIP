BLIP Protocol

The Blind Lossy (i) Packet Protocol
This a a lightweight library written in CPP for use on Arduino/Esp32 boards along with Win10 for communicating.
This was primarily design for the MedullaOS software architecture.

Each instance of this library is able to communicate with another instance of this library, assuming 
both libraries run on a platform that supports at least one of the supported mediums and is sufficiently 
close (subject to the current medium).Instances of this library are 'blind', meaning they are, by default, 
unaware of the existence of their counterpart, thus missing packets are not re-transmitted - hence 'Lossy'. 
This library does support a response toggle, meaning it will wait on a reply, this is useful if a package is 
especially valuable and necessitates a greater degree of safety than the conventional checksum.

Packets are provided to the function via BLIP.tx(packet) and sent 'blindly' over 1 of 4 mediums 
(Bluetooth, LORA, WIFI and USB).The incoming packets register is done using BLIP.rx(). In the event
that a medium fails to transmit or receive data, it will automatically cycle to another medium, 
which it will repeat until either it exhausts all options or locates a functioning medium.

Packets consist of between 3 and 18 bytes. A packet consists of a header, type, length, payload (a subtype 
and data)and checksum. 




