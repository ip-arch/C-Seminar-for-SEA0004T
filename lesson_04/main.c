// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 Naohiko Shimizu <nshimizu@ip-arch.jp>
/* lesson_04: LCD に文字を表示する (Elecrow L_4 LCD_display) */

#include <stdio.h>
#include "sea0004t.h"
#include "sea_time.h"
#include "sea_uart.h"
#include "sea_i2c.h"
#include "sea_lcd.h"

int main(void)
{
    uart_init();
    i2c_init();
    sea_button_init();

    while (!lcd_init()) {
        puts("Could not init backpack. Check wiring.");
        delay_ms(50);
    }
    puts("Backpack init'd.");

    lcd_print_at(0, 0, "HELLO WORLD");
    lcd_print_at(0, 1, "Push Button");
    while (!sea_button_pressed()) { }
    lcd_print_at(0, 1, "Bye Bye       ");
    delay_ms(1000);
    lcd_clear();
    lcd_set_backlight(0);

    for (;;) { }
}
