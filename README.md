# QHYCFW3 USB Mode Driver

This implements an [X2 Driver](http://www.bisque.com/x2standard/) for
the [QHYCFW3 filter wheel](https://www.qhyccd.com/index.php?m=content&c=index&a=show&catid=137&id=34),
running in USB mode. This enables you to control the filter wheel from
[The SkyX Pro](http://www.bisque.com/sc/pages/TheSkyX-Professional-Edition.aspx).

The built-in TSX driver for the filtwheel only works when tethered to
a QHY camera, and then appears to only work when run on Windows under
ASCOM. See [here](http://www.bisque.com/sc/forums/t/35821.aspx) for details.

This driver enables the use of the filter wheel  on Linux and OSX, and also
allows it to be used independent of a QHY camera, should you wish.

## Prereqs

The device must be set to USB mode in order to work with the driver.
See the [QHY Manual](https://www.qhyccd.com/index.php?m=content&c=index&a=show&catid=30&id=186)
for instructions on how to put the device into USB mode.

## Mac OSX installation

1. You need to install the last UART drivers from
   [silabs](https://www.silabs.com/products/development-tools/software/usb-to-uart-bridge-vcp-drivers).
   
2. Then install the TSX driver. You can download pre-built OSX binaries
   [here](https://github.com/scottwis/qhycfw3_usb/releases).

## Linux

This should work under Linux without any additional spport, but I have
not tested it yet (I don't have a Linux lisence for TSX). For now you
will have to build and install the driver your self.

## Windows

This may work under Windows, but I have not tested it yet. YMMV. You
will need to get the [virtual com port drivers](https://www.silabs.com/products/development-tools/software/usb-to-uart-bridge-vcp-drivers).
