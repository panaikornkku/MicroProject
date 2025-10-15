/*
 * light.h
 *
 *  Created on: Sep 30, 2025
 *      Author: panaikornmaneeratchakit
 */
#pragma once

/* Initialize anything needed for the light sensor (currently none) */
static inline void Light_Init(void) {}

/* Returns computed Lux from LDR on PA1/ADC1_IN1 */
float Light_ReadLux(void);
