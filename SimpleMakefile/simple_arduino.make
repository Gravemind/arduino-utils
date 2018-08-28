BOARD?=arduino:avr:uno

VERBOSE?=0
BAUD_RATE?=9600
ADDITIONAL_STTY_FLAGS?=
STTY_FLAGS?=-icrnl

ifneq ("$(wildcard ${ARDUINO_PORT})","")
PORT?=${ARDUINO_PORT}
else
PORT?=$(error Invalid ARDUINO_PORT variable)
endif

ARDUINO_CLI_COMPILE_FLAGS=--fqbn ${BOARD} --warnings all
ARDUINO_CLI_UPLOAD_FLAGS=--fqbn ${BOARD} --verify --port ${PORT}

ifeq (${VERBOSE},1)
ARDUINO_CLI_COMPILE_FLAGS+=--verbose
ARDUINO_CLI_UPLOAD_FLAGS+=--verbose
endif

all: compile

info:
	@echo "Info:"
	@echo "  pwd: ${shell pwd}"
	@echo "  arduino-cli: $(shell which arduino-cli)"
	@echo "  BOARD=${BOARD}"
	@echo "  ARDUINO_PORT=${ARDUINO_PORT}"
	@echo "  BAUD_RATE=${BAUD_RATE}"
	@echo

clean:
	echo "Clean not implemented !"

compile: info
	arduino-cli compile ${ARDUINO_CLI_COMPILE_FLAGS}

upload: compile info
	arduino-cli upload ${ARDUINO_CLI_UPLOAD_FLAGS}
	sleep 0.1

# aliases
build: compile
verify: compile
flash: upload

tty:
#	csN : set character size to N bits, N in [5..8]
#	[-]icrnl : translate carriage return to newline
	stty -F ${PORT} cs8 ${STTY_FLAGS} ${ADDITIONAL_STTY_FLAGS} ${BAUD_RATE}
	cat ${PORT}

.PHONY: all clean compile upload build verify flash info tty
