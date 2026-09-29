BLIP Protocol

The Blind Lossy (i) Packet Protocol
This a a lightweight library written in CPP for use on Arduino/Esp32 boards along with Win10.

Packets are provided to the function via BLIP.tx(packet) and sent 'blindly' over 1 of 4 mediums 
(Bluetooth, LORA, WIFI and USB).The incoming packets register is done using BLIP.rx(). In the event
that a medium fails to transmit or receive data, it will automatically cycle to another medium, 
which it will repeat until either it exhausts all options or locates a functioning medium.

V0.0
