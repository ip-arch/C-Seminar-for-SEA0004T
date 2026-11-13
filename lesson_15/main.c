// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 Naohiko Shimizu <nshimizu@ip-arch.jp>
/* lesson_15: リモコンでサーボの角度を決める (Elecrow L_15 Servo_angle_control)
 *   [NEXT] 180度  [PLAY/PAUSE] サーボを止める
 *   数字キーで角度を入力して [EQ] で決定 (0〜180) */

#include <stdio.h>
#include <stdlib.h>
#include "sea_time.h"
#include "sea_uart.h"
#include "sea_i2c.h"
#include "sea_lcd.h"
#include "sea_servo.h"
#include "sea_ir.h"

static char num[8];
static uint8_t num_len = 0;

static void show_top(const char *text)
{
    lcd_print_at(0, 0, "             ");
    lcd_print_at(0, 0, text);
}

static void add_digit(char c)
{
    printf("[%c]\n", c);
    if (num_len < sizeof num - 1) {
        num[num_len++] = c;
        num[num_len] = '\0';
    }
}

static void clear_num(void)
{
    num_len = 0;
    num[0] = '\0';
}

int main(void)
{
    char line[17];

    uart_init();
    time_init();
    i2c_init();
    while (!lcd_init()) {
        puts("Could not init backpack. Check wiring.");
        delay_ms(50);
    }
    puts("Backpack init'd.");
    ir_init();
    puts("init over");

    clear_num();
    lcd_print_at(0, 0, "Angle:");

    for (;;) {
        if (ir_available()) {
            uint8_t key = ir_read();
            printf("- press -\t");

            switch (key) {
            case IR_KEY_NEXT:
                puts("[NEXT]");
                servo_attach();
                servo_write(180);
                show_top("Servo Open");
                break;
            case IR_KEY_PLAY_PAUSE:
                puts("[PLAY/PAUSE]");
                servo_detach();
                show_top("Servo Close");
                break;
            case IR_KEY_EQ: {
                puts("[EQ]");
                long angle = atol(num);
                if (angle >= 0 && angle <= 180) {
                    printf("%s\n", num);
                    servo_write((uint8_t)angle);
                    clear_num();
                    snprintf(line, sizeof line, "Angle:%d", servo_read());
                    show_top(line);
                } else {
                    clear_num();
                    show_top("Angle:Error");
                }
                break;
            }
            case IR_KEY_0: add_digit('0'); break;
            case IR_KEY_1: add_digit('1'); break;
            case IR_KEY_2: add_digit('2'); break;
            case IR_KEY_3: add_digit('3'); break;
            case IR_KEY_4: add_digit('4'); break;
            case IR_KEY_5: add_digit('5'); break;
            case IR_KEY_6: add_digit('6'); break;
            case IR_KEY_7: add_digit('7'); break;
            case IR_KEY_8: add_digit('8'); break;
            case IR_KEY_9: add_digit('9'); break;
            default: break;
            }

            lcd_clear_row(1);
            lcd_print_at(0, 1, num);
        }
        delay_ms(100);
    }
}
