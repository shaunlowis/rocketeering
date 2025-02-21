# Custom Avionics
## Overview

In depth documentation is available in the [FC_dev branch of this repository.](
    https://github.com/shaunlowis/rocketeering/tree/FC_dev
)

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

Pictured is the revA of the PCB.

![screenshot](/img/flight_computer_pcb.jpeg)

Here is the mostly assembled version, used for software
development.

![screenshot](/img/semi_assembled_FC.jpg)

## Design

KiCAD 7 was used for designing the electronics for the flight computer. The design process is pretty well illustrated
by the git graph, a nice view is shown with:

```bash
git log --pretty=oneline --graph --decorate --all
```

[Back to home site](https://shaunlowis.github.io/rocketeering/)