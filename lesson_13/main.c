// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 Naohiko Shimizu <nshimizu@ip-arch.jp>
/* lesson_13: リモコンで LED を操作する (Elecrow L_13 IR_control_LED)
 *   [1] 点灯  [2] 1 秒ごとに点滅 (タイマ割り込み)  [3] 消灯 */

#include <stdio.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include "sea0004t.h"
#include "sea_time.h"
#include "sea_uart.h"
#include "sea_ir.h"

static volatile uint8_t led_state = 1;
static uint8_t timer_running = 0;

/* Timer1: 1/1024 分周で 15625 カウント = 1 秒 */
ISR(TIMER1_COMPA_vect)
{
    led_state = !led_state;
    sea_led_write(led_state);
}

static void start_timer(void)
{
    TCCR1A = 0;
    TCNT1  = 0;
    OCR1A  = 15624;
    TIMSK1 = (1 << OCIE1A);
    TCCR1B = (1 << WGM12) | (1 << CS12) | (1 << CS10);   /* CTC、1/1024 */
    timer_running = 1;
}

static void stop_timer(void)
{
    TCCR1B = 0;
    TIMSK1 = 0;
    timer_running = 0;
}

int main(void)
{
    uart_init();
    time_init();
    ir_init();
    puts("init over");
    sea_led_init();

    for (;;) {
        if (ir_available()) {
            uint8_t key = ir_read();
            printf("- press -\t");
            switch (key) {
            case IR_KEY_1:
                puts("[1]");
                sea_led_on();
                if (timer_running) stop_timer();
                break;
            case IR_KEY_2:
                puts("[2]");
                if (!timer_running) start_timer();
                break;
            case IR_KEY_3:
                puts("[3]");
                sea_led_off();
                if (timer_running) stop_timer();
                break;
            default:
                printf("(0x%02X)\n", key);
                break;
            }
        }
    }
}
