// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 Naohiko Shimizu <nshimizu@ip-arch.jp>
/* lesson_10: 可変抵抗の値を LCD に表示し、LED の明るさも変える (Elecrow L_10 Brightness_display) */

#include <stdio.h>
#include "sea0004t.h"
#include "sea_time.h"
#include "sea_uart.h"
#include "sea_adc.h"
#include "sea_pwm.h"
#include "sea_i2c.h"
#include "sea_lcd.h"

int main(void)
{
    char buf[8];

    uart_init();
    adc_init();
    pwm_init();
    i2c_init();
    while (!lcd_init()) {
        puts("Could not init backpack. Check wiring.");
        delay_ms(50);
    }
    puts("Backpack init'd.");

    lcd_print_at(0, 0, "Brightness check");

    for (;;) {
        uint16_t adc = adc_read(ADC_POT);
        pwm_write((uint8_t)sea_map(adc, 0, 1023, 0, 255));   /* LED の明るさ */

        int level = (int)sea_map(adc, 0, 1023, 0, 10);        /* 表示は 0〜10 */
        snprintf(buf, sizeof buf, "%d", level);
        delay_ms(100);
        lcd_print_at(0, 1, "    ");
        lcd_print_at(0, 1, buf);
    }
}
