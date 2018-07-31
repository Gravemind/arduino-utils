
An even simpler and very much smaller DHT code for DHT22.

Can provides full 0.1 temperature and humidity precision via "fixed point"
`uint16_t` (`value*10`) rather than very costly `float`.

And can fit on an digispark-tiny (attiny45) along side a `DigiUSB` or `DigiCDC`.

## Links

- http://akizukidenshi.com/download/ds/aosong/AM2302.pdf
- https://github.com/winlinvip/SimpleDHT/blob/master/SimpleDHT.cpp
- https://github.com/adafruit/DHT-sensor-library/blob/master/DHT.cpp
