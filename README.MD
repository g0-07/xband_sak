# XBAND Swiss Army knife (xband_sak)

xband_sak is an [Everdrive MD](https://segaretro.org/Everdrive_MD "Everdrive MD") (including clones) RAM application for the *Sega Mega Drive/Genesis* with the following features:

- backup/restore XBAND SRAM
- backup ROM
- flash ROM with modified XBAND hardware
- hex-viewer

![xband_sak](https://user-images.githubusercontent.com/37120278/167273668-0532d905-e863-443a-8aa6-c33402b1d98c.gif)

****
### How to use:
Usage should be pretty much self-explanatory.

Copy the binary to the FAT-formatted sd-card and start the application from the Everdrive menu.

To use the flash function you need to desolder the stock ROM and replace it with a flash ROM.
Currently AM29F400BB/BT is supported, adding support for other types should be fairly straightforward, as long as there's no special programming voltage needed.

Don't forget the two missing 10k SMD resistors!
****
### Based on work from:
Krikzz - https://github.com/krikzz

Stephane-D - https://github.com/Stephane-D/SGDK

### Using FatFs from:
ChaN - http://elm-chan.org/fsw/ff/00index_e.htmls
