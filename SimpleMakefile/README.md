A simple Makefile calling `arduino` command line.

Example for a `my-project/Makefile`:
```
BOARD=arduino:avr:nano

# do not send reset
ADDITIONAL_STTY_FLAGS+=-hup

include /path/to/simple_arduino.make
```

Usage `my-project` example:
```
$> export ARDUINO_PORT=/dev/ttyACM1
$> make info        # prints build variables
$> make verify      # builds/verifies project
$> make flash tty   # flashes/uploads then setups and open the ARDUINO_PORT serial tty (read-only)
```
