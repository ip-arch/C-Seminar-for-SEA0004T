// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 Naohiko Shimizu <nshimizu@ip-arch.jp>
#include "sea0004t.h"
#include <stdio.h>

/* ---- LED (D10 = PB2) ---- */
void sea_led_init(void)   { LED_DDR  |=  (1 << LED_BIT); }
void sea_led_on(void)     { LED_PORT |=  (1 << LED_BIT); }
void sea_led_off(void)    { LED_PORT &= ~(1 << LED_BIT); }
void sea_led_toggle(void) { LED_PIN   =  (1 << LED_BIT); }   /* PINx への 1 書き込みで反転 */
void sea_led_write(uint8_t on) { if (on) sea_led_on(); else sea_led_off(); }

/* ---- リレー (D4 = PD4) ---- */
void sea_relay_init(void) { RELAY_DDR |= (1 << RELAY_BIT); }
void sea_relay_write(uint8_t on)
{
    if (on) RELAY_PORT |=  (1 << RELAY_BIT);
    else    RELAY_PORT &= ~(1 << RELAY_BIT);
}

/* ---- ボタン (D7 = PD7, 押すと LOW) ---- */
void sea_button_init(void) { DDRD &= ~(1 << BUTTON_BIT); }
uint8_t sea_button_pressed(void) { return !(BUTTON_PIN & (1 << BUTTON_BIT)); }

/* ---- 人感センサ (A2 = PC2) ---- */
void sea_pir_init(void) { DDRC &= ~(1 << PIR_BIT); }
uint8_t sea_pir_detected(void) { return (PIR_PIN & (1 << PIR_BIT)) != 0; }

/* ---- 音センサをデジタル入力として読む (A1 = PC1) ---- */
void sea_sound_digital_init(void) { DDRC &= ~(1 << SOUND_BIT); }
uint8_t sea_sound_digital(void) { return (SOUND_PIN & (1 << SOUND_BIT)) != 0; }

/* ---- 補助関数 ---- */
long sea_map(long x, long in_min, long in_max, long out_min, long out_max)
{
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

void sea_fixed_str(char *buf, size_t n, int32_t value, uint8_t decimals)
{
    uint32_t scale = 1;
    for (uint8_t i = 0; i < decimals; i++) scale *= 10;

    const char *sign = "";
    uint32_t a;
    if (value < 0) { sign = "-"; a = (uint32_t)(-value); }
    else           { a = (uint32_t)value; }

    uint32_t ip = a / scale;
    uint32_t fp = a % scale;

    /* 小数部を 0 埋めで作る */
    char frac[8];
    for (int8_t i = (int8_t)decimals - 1; i >= 0; i--) {
        frac[i] = (char)('0' + fp % 10);
        fp /= 10;
    }
    frac[decimals] = '\0';

    if (decimals == 0) snprintf(buf, n, "%s%lu", sign, (unsigned long)ip);
    else               snprintf(buf, n, "%s%lu.%s", sign, (unsigned long)ip, frac);
}
