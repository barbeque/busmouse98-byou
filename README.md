# busmouse98-byou
A modified PS/2 or USB to PC-98 quadrature mouse converter.

This firmware is based off of [busmouse98](https://github.com/tyama501/ps2busmouse98) by tyama501. His original version is much more clever than this modified one. Thank you very much for your hard work!

The PS/2 mouse interaction uses [the PS2MouseHandler library](https://github.com/getis/Arduino-PS2-Mouse-Handler), which is included. This component is licensed under MIT and was originally developed by Bob Grant.

## History
Originally, tyama501 did this busmouse98 firmware, for the Akizuki Denshi AE-ATMEGA328-MINI board. His firmware works well, but it had trouble initializing the optical PS/2 mouse that I bought on Amazon.

This is because the PS/2 mouse contains a _USB-and-PS/2_ compatible controller chip, the PixArt PAW3515DB, which required additional initialization code to work properly. I ended up merging in [the `PS2MouseHandler` Arduino library](https://github.com/getis/Arduino-PS2-Mouse-Handler) to support initialization, and then rewrote some of the original busmouse98 code to not conflict with the use of this library.

## Instructions
### What You Need
 - Arduino Pro Mini, either official or one of the many floating around AliExpress
 - FTDI or CH430 USB-to-RS232 adapter
 - Edoardo's [PS/2 to PC-9801 D-sub adapter PCB](https://www.pcbway.com/project/shareproject/NEC_PC_98xx_serial_mouse_adapter_from_PS_2_Mini_DIN_6_USB_A_to_DE_9_Standard_32bc41de.html), the assembly of which requires:
  - A male DE9 D-sub connector
  - A female PS/2 connector
  - 10kΩ 7-SIP (6 resistors) bussed resistor network
  - Optionally an 0805 SMT LED and a 220Ω 1/4W resistor

### Assembly of Hardware
 - Open the ino file in the Arduino IDE.
 - Build the ino file, and program this firmware onto the Arduino Pro Mini using an FTDI or CH340 USB-to-RS232 adapter.
 - Assemble the mouse adapter according to Edoardo's instructions

### Use of Hardware
 - Turn PC-98 off.
 - Connect the DE-9 connector to the Mouse port of the PC-98.
 - Connect the PS/2 mouse to the PS/2 connector of the mouse adapter.
 - Use mouse as normal. If it seems twitchy or otherwise defective, alter the timing values in the ino file and reflash it.

## Tested With
### Mice
 - Perixx Perimice 209 3-button PS/2 optical mouse

### Computers
 - PC-9801RA2

### Software
 - Artdink's _Tokio_
