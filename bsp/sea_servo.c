// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 Naohiko Shimizu <nshimizu@ip-arch.jp>
#include "sea_servo.h"
#include <avr/io.h>

/* Elecrow のサンプルと同じ 600us 〜 2520us を 0〜180 度に割り当てる */
#define SERVO_MIN_US  600UL
#define SERVO_MAX_US  2520UL

static uint8_t angle = 90;
static uint8_t attached = 0;

static void update_pulse(void)
{
    uint32_t us = SERVO_MIN_US + (uint32_t)angle * (SERVO_MAX_US - SERVO_MIN_US) / 180UL;
    OCR1A = (uint16_t)(us * 2UL);          /* 1/8 分周: 1カウント = 0.5us */
}

void servo_attach(void)
{
    DDRB  |= (1 << PB1);
    /* 高速 PWM (モード14)、TOP=ICR1、1/8 分周、周期 20ms */
    ICR1   = 39999;
    update_pulse();
    TCCR1A = (1 << COM1A1) | (1 << WGM11);
    TCCR1B = (1 << WGM13) | (1 << WGM12) | (1 << CS11);
    attached = 1;
}

void servo_detach(void)
{
    TCCR1A &= ~(1 << COM1A1);
    PORTB  &= ~(1 << PB1);
    attached = 0;
}

void servo_write(uint8_t degrees)
{
    if (degrees > 180) degrees = 180;
    angle = degrees;
    if (attached) update_pulse();
}

uint8_t servo_read(void)
{
    return angle;
}
