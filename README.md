# Teensy Sampler

Shows a multitude of different usages from periodic timers, ADCs, to ethernet while using interrupts.

## Requirements
Requires a Teensy 4.1 with the ethernet extension.

## Run Example
If you want to run this with serial monitor use
```
pio run -e debug --target upload
```
beware that this might actually be to slow to use at high sampling frequencies (not tested).
To run without any of the serial traces, simply use
```
pio run -e release --target upload
```
The two options are provided by the `platform.ini` file.
With the only difference being debug mode having an additional build flag `-DSERIAL_DEBUG`.
