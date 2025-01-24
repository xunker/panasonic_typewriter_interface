# Serial Configuration Terminal for Panasonic Typewriter Interface

https://github.com/xunker/panasonic_typewriter_interface

# Serial Configuration

There is an option to view and change the current configuration using the
serial port.

This is only available if the code was compiled with `#define ENABLE_CONSOLE`
uncommented in the main `.ino` file. Serial console support
**is enabled by default**.

## Connecting

Switch "RUN/HALT" to "HALT" and reboot the MCU, you can now connect using
whatever program you choose (the Arduino IDE Serial Console works well).

The default speed/baud is 57600.

If the serial config console is enabled, you should see this when you connect:

> Config console active.

## Commands

### `?` - Help

```
=========Help==========
Commands:
  baud   [rate]   get or set the Serial Baud rate
  config [value]  get or set the Serial configuration
  show            print current settings
  load            load config from EEPROM
  write           write current config to EEPROM
  reset           reset EEPROM to defaults
  info            system information
  ?               list commands (this screen)
```

### `baud` - get/set baud rate

```
======Serial Baud======
57600
```

If no argument passed, the current baud rate is returned.

If a valid baud rate is passed, that value is set in **memory** and returned.

Important: the baud rate change will not take effect until the next reboot.
Unless you write this change to EEPROM (see `write` command) the change will be
lost upon reboot.

### `config` - get/set serial config

```
=====Serial Config=====
8n1
```

If no argument passed, the current serial config is returned.

If a valid serial config is passed, that value is set in **memory** and
returned.

Important: the serial config change will not take effect until the next reboot.
Unless you write this change to EEPROM (see `write` command) the change will be
lost upon reboot.

### `show` - show current serial settings

```
=====Show Settings=====
	baud	57600
	config	8n1
```

Shows the current baud rate and config for the serial port.

### `load` - load config from EEPROM

Reloads curent config from EEPROM, overwriting what is currently in memory.

### `write` - write current config to EEPROM

Write the current in-memory configuration to EEPROM, which will survive reboots.

### `reset` - reset EEPROM to defaults

Reset EEPROM to defaults.

### `info` - show system information

```
======System Info======
Firmware version: 1.0.0
Licensed under GNU GPL V3

Compiled with:
	serial console
	eeprom support
Memory:
	Stack size:	4
	Free ram:	1458
	SRAM size:	2047
Uptime: 39785
```

Show firmware and system details.

## Returning

To return to _RUN_ mode (rememeber to `write` your changes first!), set the
"RUN/HALT" switch to "RUN" and reboot the microcontroller with the "reset"
button!