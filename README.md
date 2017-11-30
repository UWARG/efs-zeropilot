# ZeroPilot-SW

[![Build Status](https://travis-ci.org/UWARG/ZeroPilot-SW.svg?branch=devel)](https://travis-ci.org/UWARG/ZeroPilot-SW)

ZeroPilot is the next-generation autopilot currently in development at the Waterloo Aerial Robotics Group. It is powered by a high-performance STM32F7 ARM processor running at 216 MHz, and features two isolated on-board power supplies, 12 PWM outputs, and a seperate safety controller that supports direct manual takeover of the aircraft systems.

Like the [PICPilot](https://github.com/UWARG/PICPilot), it has been designed for both fixed-wind and multirotor aircraft, and is easily adapted to control other, more esoteric systems.

This repository contains the code that runs on the autopilot board. For the schematic and PCB design, see [ZeroPilot-HW](https://github.com/UWARG/ZeroPilot-HW).

## Documentation

Documentation for the ZeroPilot software is available [here](https://uwarg-docs.atlassian.net/wiki/spaces/ZP/overview).
ZeroPilot uses two STM32 microcontrollers to control a vehicle. The code for these MCUs is built on the STM32 HAL library.

### Autopilot (STM32F765ZG)

* [HAL Documentation](http://www.st.com/resource/en/user_manual/dm00189702.pdf)
* [Reference Manual](http://www.st.com/resource/en/reference_manual/dm00224583.pdf)
* [Chip Datasheet](http://www.st.com/resource/en/datasheet/stm32f765zg.pdf)

### Safety controller (STM32F030RC)

* [HAL Documentation](http://www.st.com/resource/en/user_manual/dm00122015.pdf)
* [Reference Manual](http://www.st.com/resource/en/reference_manual/dm00091010.pdf)
* [Chip Datasheet](http://www.st.com/resource/en/datasheet/stm32f030rc.pdf)
