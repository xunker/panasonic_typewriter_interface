# Panasonic Typewriter Interface

https://github.com/xunker/panasonic_typewriter_interface

Print text using the Interface Port of your compatible Panasonic KX-R/KX-W
Daisywheel or Thermal typewriter, or KX-WD55 printererer. Emulates the
serial/RS-232 function of the KX-R60, RP-K100, or RP-K105 interface adapters.

Use your 🏋️heavy🥌 and 🐢slow🐌 typewriter as a noisy, inconvenient, inflexible,
single-page printer!

[![Assembled Adapter](adapter_boards/assembled_small.jpg)](adapter_boards/README.md)

## Compatibility

### KX-R series with MiniDIN-8 port

Developed and tested with my KX-R435, and should be compatible with any other
Panasonic typewriter with the round, 8-pin MiniDIN port.

### KX-W series with DE-9 port

Untested, but should work with machines which have a DE-9 (DB-9) connector.

## Pinout

See [PINOUT.md](./PINOUT.md) for detailed information about the typewriter
connector pinout.

## Adapter Board

There's an [adapter board](adapter_boards/README.md) available to ease assembly,
and to provide additional features.

## Code

The included code will read data from the `Serial` device (usually USB Serial)
and relay that to the typewriter in the correct format.

### Required Libraries

The following libraries are required by default. They can either be installed
manually or via the Arduino IDE:

* [SerialCmd](https://github.com/gpb01/SerialCmd)
* [MemoryUsage](https://github.com/Locoduino/MemoryUsage)
* [AceButton](https://github.com/bxparks/AceButton/)

SerialCmd and MemoryUsage are only needed by the
[Serial Configuration Terminal](./SERIAL_CONFIG.md) (enabled default). AceButton
is only needed if the "Mode" button is enabled (enabled by default).

### Configure

NOTE: if you are using the included [adapter board](adapter_boards/README.md), you do not need to do any
additional configuration. Simply connect your cable and adjust the "Cable Type"
jumpers so they are both closest to whichever LED (D1 or D2) is lit up.

Edit [panasonic_typewriter_interface.ino](./panasonic_typewriter_interface.ino)
and change the following lines as appropriate:

```c
#define ON_LINE_PIN A0 // Output, active LOW
#define STB_PIN     A2 // Output, active LOW
#define ACK_PIN     A3 // Input, active LOW
#define TXD_PIN     A1 // Output; HIGH = 1, LOW = 0

#define GO_PIN      A6 // trigger printing to begin when this is pulled low

#define MODE_PIN    A7 // trigger printing to begin when this is pulled low
```

CHECK WHAT KIND OF CABLE YOU HAVE, and remember that a Macintosh-style printer
cable likely has several pins swapped.

Once you're done, upload it to your Arduino-compatible MCU.

There is also a "Demo Mode" that will just print text automatically and not
wait for input over serial; enable that by uncommenting `// #define TEST_MODE`
in panasonic_typewriter_interface.ino.

#### Extra LEDs (Optional, default)

By default, there are 4 addition LEDs broken out to show the status of each of
the signal lines (in addition to the built-in LED).

```c
#define ON_LINE_LED 8
#define STB_LED     6
#define ACK_LED     2
#define TXD_LED     3
#define LED_BUILTIN 13 // "Status" LED
```

These LEDs will reflect the _state_ of each respective signal.
Note, this is a _logic state_, not the _electrical state_. That means for
active-low pins like ON_LINE, STB, and ACT, a low signal will turn *on* the
appropriate LED.

However, if you do not require these additional LEDs you can comment-out the
line `#define ENABLE_MULTIPLE_LEDS`, and then only the built-in LED will be
used.

## Printerering (aka, how to use this)

Once the code is uploaded and your Arduino is properly connected to your
typewriter, turn on your typewriter and load paper as appropriate.

### Set Typewriter to "On-Line Mode"

Then, you must get in to "On-Line Mode". The command varies from model to model,
so consult the owners manual for more detail. In the case of the KX-R435, you
enter On-Line Mode by holding down the `CODE` key and then pressing `E`; the LCD
should now say "On-Line Mode". To exit On-Line mode, or to stop printing, press
CODE+E again.

### Set microcontroller to "RUN" Mode

Once in "On-Line Mode", you'll need to set your Arduino to "Run" mode. If you
are using the included [adapter board](adapter_boards/README.md), you do this by
switching the "RUN/HALT" switch to "RUN". If you are have wired your own
microcontroller, you will connect pin A7 (or whatever pin you assigned to
`GO_PIN`) to ground.

Once you switch to "RUN" mode, reset the microcontroller once to ensure it's now
in the correct mode.

As long as that pin is grounded, sentences will keep being sent to the
typewriter.

### Send text - SLOWLY

Finally, open a serial connection to your MCU (using the built-in Serial Console
in the Arduino IDE works well).

The default speed is 300 baud. Yes, you read correctly, **300 baud**. Because
it's a freakin' TYPEWRITER! It can _barely_ keep up with 30 char/sec. If you
want a laser printer, you know where to find one.

Out of the box, the Arduino toolchain only gives the atMega328 a 64 byte serial
buffer, which can hold about 2 seconds of data at 300 baud. I'd actually like
to set the default speed to 110 baud, but no operating systems have supported
that speed for at least 30 years. Yes, not even Linux despite the lies that
`stty` tells you.

Send some text! With luck, you'll see it appear in your paper! Eventually!

## Serial Configuration Terminal

See [SERIAL_CONFIG.md](./SERIAL_CONFIG.md).

## Known Problems

### Can't print more than 62 columns

On my KX-R435, I cannot print a line longer than 62 columns. This happens
regardless of the CPI setting, margins, or tabs.

I am able to _type_ the full-carriage width, but "ON-LINE MODE" won't allow
me to go beyond 62 characters.

If there is a special setting in the manual, I have missed it. If you know how
to overcome this problem please tell me how in [a new
Issue](https://github.com/xunker/panasonic_typewriter_interface/issues/new?template=Blank+issue).

Wait: is this because the default arduino serial buffer is
[only 64
bytes](https://docs.arduino.cc/language-reference/en/functions/communication/serial/available/)?
Maybe I need to employ some kind of interrupt-drive ring buffer.

## Theory of Operation

From [this page](./panasonic_rp-k100_interface_circuit.pdf) in the KX-W50TH/W60TH
service manual:

> 10.2.4 Interface Circuit
>
> The interface circuit handles the handshaking needed for communication with a
> I/F Adaptor (RP-K100). The RP-K100 allows interfacing with a host computer.
> The handshake method is described in the following steps.
>
> Process:
>
> (1) The RP-K100 changes the ON LINE signal from H to L indicating that data
> transmission has started . This ON LINE signal remains Low during the
> transmission of 1 byte.
>
> (2) The RP-K100 first sends the LSB (DO) of a transmitted byte to the TXD line
> and changes the STB signal from H to L. This STB signal is sent to P51 of the
> CPU which is the interruption.
>
> (3) In the interruption state, the CPU receives a TXD signal and changes the
> ACK signal from L to H. This ACK signal is sent to the RP-K100.
>
> (4) After the RP-K100 has received the ACK signal (L level), the STB signal
> changes from L to H.
>
> (5) When the STB signal (High) is sent from the RP-K100, the thermalwriter
> sends the ACK signal (High) to the RP-K100.
>
> (6) When the ACK signal is High, the RP-K100 starts to send the next bit of
> data.
>
> (7) Once the RP-K100 sends 1 byte of data (8 bits) to the CPU , the ON LINE
> signal changes from L to H.

## TODO

### Need

* Get special/accented character conversion functioning
* Convert special characters like tab ("\t") to spaces, to prevent typewriter
  from going crazy if no tabs have been set
  * ability to set tab-to-space count in serial console
* mode button support
  * long press triggers demo while in "run" mode
* Better serial buffering
  * Default 64 byte buffer doesn't even do 300 baud well
  * Implement a 1K ring buffer? We have the free SRAM
* Software flow control
  * [theoretically possible](https://forum.arduino.cc/t/xon-xoff-problems-with-ch340g-on-arduio-uno-clone/647752/2),
    would negate the need for a bigger serial buffer

### Want

* Hardware Flow control
  * The RTS/CTS pins on the FT232/CH9340 are not connected to anything, so it
    means rolling a new adapter board with our own USB Serial IC, or moving
    to an MCU that implements RTS/CTS or DTR/DSR via their USB stack. ESP32?
* I2C display?
* Automatically insert correct line-breaks, depending on CPI switch setting
  - May not be needed, Windows' generic/text only printer [automatically wraps
  at 80 characters](https://support.microsoft.com/en-us/topic/printing-wide-carriage-with-generic-text-only-print-driver-3bdb3c49-abdd-597d-6416-5d460efab182)
* Automatically pause printing to insert next sheet of paper, based on
  line-spacing switch setting
  * MODE button becomes "Continue" button
* add CTS/RTS/DTR/DSR pins to serial breakout
  * for old computers
  * for signaling buffer is full
  * for signaling Paper Out
* mode button cycles throughCPI/Line Space settings? Need display for this?

## License

[GNU GPL 3](LICENSE)