/*Module containing common includes, defines, functions etc*/

#pragma once

#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "stm8s.h"
#include "delay.h"
#include "ebyte_radio.h"

#define DEBUG_ENABLE

#define F_CPU 16000000UL // 16 MHz 

void print_bits_of_byte(uint8_t byte);