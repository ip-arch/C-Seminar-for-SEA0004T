// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 Naohiko Shimizu <nshimizu@ip-arch.jp>
/* lesson_16: 自動ドア (Elecrow L_16 Polite_automatic_door)
 *   ボタンを押している間、サーボが 180 度に開き "Welcome" と表示する */

#include <stdio.h>
#include "sea0004t.h"
#include "sea_time.h"
#include "sea_uart.h"
#include "sea_i2c.h"
#include "sea_lcd.h"
#include "sea_servo.h"

int main(void)
{
    uart_init();
    sea_button_init();
    servo_attach();
    i2c_init();
    while (!lcd_init()) {
        puts("Could not init backpack. Check wiring.");
        delay_ms(50);
    }

    for (;;) {
        if (sea_button_pressed()) {
            servo_write(180);
            lcd_print_at(0, 0, "             ");
            lcd_print_at(0, 0, "Welcome");
        } else {
            servo_write(0);
            lcd_print_at(0, 0, "             ");
        }
        delay_ms(100);
    }
}
