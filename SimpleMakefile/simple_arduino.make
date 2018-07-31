BOARD?=arduino:avr:uno

BAUD_RATE?=9600
ADDITIONAL_STTY_FLAGS?=
STTY_FLAGS?=-icrnl

ifneq ("$(wildcard ${ARDUINO_PORT})","")
PORT?=${ARDUINO_PORT}
else
PORT?=$(error Invalid ARDUINO_PORT variable)
endif

BUILD_DIR?=$(abspath ./build)

PREFS?= --pref compiler.warning_level=all

ARDUINO_FLAGS=--verbose --board ${BOARD} --pref build.path=${BUILD_DIR} ${PREFS}

all: verify

builddir:
#	trying to fix the ctags_target_for_gcc_minus_e.cpp error:
	rm -rf ${BUILD_DIR}

	mkdir -p ${BUILD_DIR}

clean:
	rm -rf ${BUILD_DIR}

verify: info builddir
	arduino . --verify ${ARDUINO_FLAGS}

upload: info builddir
	arduino . --upload --port ${PORT} ${ARDUINO_FLAGS}
	sleep 0.1

# aliases
build: verify
flash: upload

info:
	@echo "pwd: ${shell pwd}"
	@echo "BOARD=${BOARD}"
	@echo "BUILD_DIR=${BUILD_DIR}"
	@echo "ARDUINO_PORT=${ARDUINO_PORT}"
	@echo "BAUD_RATE=${BAUD_RATE}"

tty:
#	csN : set character size to N bits, N in [5..8]
#	[-]icrnl : translate carriage return to newline
	stty -F ${PORT} cs8 ${STTY_FLAGS} ${ADDITIONAL_STTY_FLAGS} ${BAUD_RATE}
	cat ${PORT}

.PHONY: all builddir clean verify upload flash info tty
