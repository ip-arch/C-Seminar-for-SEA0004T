// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 Naohiko Shimizu <nshimizu@ip-arch.jp>
/*
 * sea0004t.h  --  SEA0004T ボードの定義と、ごく基本的な入出力
 *
 * ピン割り当ては Elecrow 公式ユーザーマニュアルとサンプルコードに基づく。
 *
 *   D2  IR 受信 (INT0)     D3  ブザー (OC2B)     D4  リレー
 *   D5  超音波 ECHO        D6  超音波 TRIG       D7  ボタン (押すと LOW)
 *   D9  サーボ (OC1A)      D10 赤色 LED (OC1B)
 *   A0  可変抵抗           A1  音センサ          A2  人感センサ (PIR)
 *   A3  土壌水分センサ     A4/A5 I2C (SDA/SCL)
 */
#ifndef SEA0004T_H
#define SEA0004T_H

#include <avr/io.h>
#include <stdint.h>
#include <stddef.h>

/* ---- ATmega328P のポートとビット ---- */
#define LED_PORT     PORTB
#define LED_PIN      PINB
#define LED_DDR      DDRB
#define LED_BIT      PB2          /* D10 */

#define BUTTON_PIN   PIND
#define BUTTON_BIT   PD7          /* D7  */

#define RELAY_PORT   PORTD
#define RELAY_DDR    DDRD
#define RELAY_BIT    PD4          /* D4  */

#define PIR_PIN      PINC
#define PIR_BIT      PC2          /* A2  */

#define SOUND_PIN    PINC
#define SOUND_BIT    PC1          /* A1 をデジタル入力として読む */

/* ---- ADC チャンネル ---- */
#define ADC_POT      0            /* A0 可変抵抗   */
#define ADC_SOUND    1            /* A1 音センサ   */
#define ADC_MOISTURE 3            /* A3 土壌水分   */

/* ---- LED / リレー / ボタン / 人感センサ ---- */
void    sea_led_init(void);
void    sea_led_on(void);
void    sea_led_off(void);
void    sea_led_toggle(void);
void    sea_led_write(uint8_t on);

void    sea_relay_init(void);
void    sea_relay_write(uint8_t on);

void    sea_button_init(void);
uint8_t sea_button_pressed(void);      /* 押している間 1 */

void    sea_pir_init(void);
uint8_t sea_pir_detected(void);        /* 人を検知している間 1 */

void    sea_sound_digital_init(void);
uint8_t sea_sound_digital(void);       /* 音を検知している間 1 */

/* ---- 小さな補助関数 ---- */

/* 数値の範囲変換 (Arduino の map と同じ) */
long sea_map(long x, long in_min, long in_max, long out_min, long out_max);

/* 固定小数点の数値を文字列にする
 *   value は 10^decimals 倍した整数。例: 2431, decimals=2 → "24.31" */
void sea_fixed_str(char *buf, size_t n, int32_t value, uint8_t decimals);

#endif /* SEA0004T_H */
