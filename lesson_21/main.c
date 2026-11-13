// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 Naohiko Shimizu <nshimizu@ip-arch.jp>
/* lesson_21: リモコンの電卓 (Elecrow L_21 simple_calculator)
 *   数字キーと [VOL+] (+)、[VOL-] (-)、[EQ] (=) で、たし算とひき算ができる */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sea_time.h"
#include "sea_uart.h"
#include "sea_i2c.h"
#include "sea_lcd.h"
#include "sea_ir.h"

#define MAX_INPUT  30
#define LIMIT      100000000L

static char input[MAX_INPUT + 2];          /* 入力中の式。例: "123+45" */
static uint8_t input_len = 0;
static int has_plus = 0;

static void add_char(char c)
{
    if (input_len < MAX_INPUT) {
        input[input_len++] = c;
        input[input_len] = '\0';
    }
}

static void clear_input(void)
{
    input_len = 0;
    input[0] = '\0';
}

static void show_result(const char *text)
{
    lcd_print_at(0, 1, "               ");
    lcd_print_at(0, 1, text);
}

/* "123+45=" を 123 と 45 に分ける。分けられたら 1 */
static int slice_string(const char *in, char *part1, char *part2, size_t size)
{
    const char *plus  = strchr(in, '+');
    const char *minus = strchr(in, '-');
    const char *equal = strchr(in, '=');

    has_plus = (plus != NULL);
    if (!((plus || minus) && equal)) {
        return 0;
    }

    const char *op = plus ? plus : minus;
    size_t p = (size_t)(op - in);
    size_t e = (size_t)(equal - in);

    snprintf(part1, size, "%.*s", (int)p, in);
    if (e > p) {
        snprintf(part2, size, "%.*s", (int)(e - p - 1), in + p + 1);
    } else {
        part2[0] = '\0';
    }
    return 1;
}

static long perform_operation(long a, long b, char op)
{
    switch (op) {
    case '+': return a + b;
    case '-': return a - b;
    default:  return a;
    }
}

/* リモコンのキーを数字に直す。数字でなければ -1 */
static int key_to_digit(uint8_t key)
{
    switch (key) {
    case IR_KEY_0: return 0;
    case IR_KEY_1: return 1;
    case IR_KEY_2: return 2;
    case IR_KEY_3: return 3;
    case IR_KEY_4: return 4;
    case IR_KEY_5: return 5;
    case IR_KEY_6: return 6;
    case IR_KEY_7: return 7;
    case IR_KEY_8: return 8;
    case IR_KEY_9: return 9;
    default:       return -1;
    }
}

int main(void)
{
    char part1[MAX_INPUT + 2], part2[MAX_INPUT + 2], text[17];

    uart_init();
    time_init();
    ir_init();
    puts("init over");
    i2c_init();
    while (!lcd_init()) {
        puts("Could not init backpack. Check wiring.");
        delay_ms(50);
    }

    clear_input();
    lcd_print_at(0, 0, "Equal to:");

    for (;;) {
        if (!ir_available()) {
            continue;
        }

        uint8_t key = ir_read();
        printf("- press -\t");

        int digit = key_to_digit(key);
        if (digit >= 0) {
            printf("[%d]\n", digit);
            add_char((char)('0' + digit));
        } else if (key == IR_KEY_VOL_DOWN) {
            puts("[VOL-]");
            add_char('-');
        } else if (key == IR_KEY_VOL_UP) {
            puts("[VOL+]");
            add_char('+');
        } else if (key == IR_KEY_EQ) {
            puts("[EQ]");
            add_char('=');

            if (slice_string(input, part1, part2, sizeof part1)) {
                long a = atol(part1);
                long b = atol(part2);

                if (a > LIMIT || b > LIMIT || a < 0 || b < 0) {
                    puts("Invalid data");
                    clear_input();
                    show_result("Out of range");
                } else {
                    long value = perform_operation(a, b, has_plus ? '+' : '-');
                    clear_input();
                    snprintf(text, sizeof text, "%ld", value);
                    show_result(text);
                    printf("value: %ld\n", value);
                }
            } else {
                clear_input();
                show_result("Error");
            }
        } else {
            printf("(0x%02X)\n", key);
        }

        /* 1 行目に入力中の式を表示 (長いときは最後の 14 文字) */
        lcd_print_at(0, 0, "                ");
        if (input_len > 14) {
            lcd_print_at(0, 0, &input[input_len - 14]);
        } else {
            lcd_print_at(0, 0, input);
        }
    }
}
