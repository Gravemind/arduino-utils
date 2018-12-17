A simple Makefile calling [arduino-cli](https://github.com/arduino/arduino-cli) command lines.

Example for a `my-project/Makefile`:
```Makefile
BOARD=arduino:avr:nano

# do not send reset
ADDITIONAL_STTY_FLAGS+=-hup

# resolved by wildcard
PORT?=/dev/serial/by-id/usb-*Serial*

include /path/to/simple_arduino.make
```

Usage of `my-project` example:
```bash
$> make info        # prints build variables
...
$> make compile     # compiles the project (aliases: make build, make verify)
...
$> export ARDUINO_PORT=/dev/ttyACM1
$> make upload      # uploads to ARDUINO_PORT  (aliases: make flash)
...
$> make tty         # setups and opens the ARDUINO_PORT serial tty (read-only)
...
```
