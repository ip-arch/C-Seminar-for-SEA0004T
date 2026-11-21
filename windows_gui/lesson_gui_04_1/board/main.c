// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 Naohiko Shimizu <nshimizu@ip-arch.jp>
/* lesson_gui_01_1: UARTから受け取った文字列をLCDに表示する */

#include <stdio.h>
#include <string.h>
#include "sea_time.h"
#include "sea_uart.h"
#include "sea_i2c.h"
#include "sea_lcd.h"
#include "../protocol.h"

int main(void)
{
    char buf[8];
    char line[32];


    uart_init();
    i2c_init();
    while (!lcd_init()) {
        puts("Could not init backpack. Check wiring.");
        delay_ms(50);
    }
    puts(PROTO_READY);


    for (;;) {
        if (fgets(line, sizeof line, stdin) == NULL) {
            continue;
        }
        line[strcspn(line, "\r\n")] = '\0';
        lcd_clear();
        lcd_print_at(0, 0, line);
        if(strlen(line) > 16)
            lcd_print_at(0, 1, &line[16]);
    }
}
