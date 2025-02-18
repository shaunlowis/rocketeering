#pragma once

#include "common.h"

#define GREEN_LED_PORT  GPIOG
#define GREEN_LED_PIN   GPIO_PIN_0 // This is pin PG0, or pin 35 on the schematic.

#define RED_LED_PORT  GPIOG
#define RED_LED_PIN   GPIO_PIN_1 // This is pin PG1, or pin 36 on the schematic.

#define M0_RADIO_PORT GPIOC
#define M0_RADIO_PIN GPIO_PIN_4

#define M1_RADIO_PORT GPIOC
#define M1_RADIO_PIN  GPIO_PIN_3

#define THERMO_CHIP_PORT GPIOD
#define THERMO_CHIP_PIN_SELECT  GPIO_PIN_2 // This is pin PD2, or pin 43 on the schematic.