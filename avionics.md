# Custom Avionics
## Overview
Flight computer:
- Power via battery.
- MCU: STM8.
- LoRa Radio.
- GPS.
- Altimeter.
- IMU.
- Pressure sensor.
- SD card.

Additional electronics:
- Separation charge (comes with motor).
- Thermocouple (being read by flight computer).

Pictured is the revA of the PCB. Next is designing
a mount, inside the airframe for flight. See more
details in the airframe section.

![screenshot](/img/flight_computer_pcb.jpeg)

## Design
KiCAD 7 was used for designing the electronics for the flight computer. The design process is pretty well illustrated
by the git graph, a nice view is shown with:

```
git log --pretty=oneline --graph --decorate --all
```

### Thermocouple
The thermocouple likely won't work due to the upper extreme
of K type thermocouples being lower than the expected stream
temperature of the motor. As a work-around, we plan to
attach the thermocouple to a high heat capacity metal.
This will let us capture temperature information without
losing the thermocouple itself.

### LoRa
The radio link was tested to check if the receiver would work
inside of the motor casing. We plan to try transferring in-
flight telemetry while the rocket flies, which will be further
detailed here. 

### MCU
The STM8 allows pin re-mapping for GPIO. This allowed
more flexibility when designing the PCB. The STM8 proprietary
software will be used for programming the STM, since its code
generation is more trouble than it is worth to set up in VS Code.

[Back to home site](https://shaunlowis.github.io/rocketeering/)