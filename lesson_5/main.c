// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 Naohiko Shimizu <nshimizu@ip-arch.jp>
/* lesson_5: 土壌水分センサの値を表示する (Elecrow L_5 Moisture_Monitor) */

#include <stdio.h>
#include "sea0004t.h"
#include "sea_time.h"
#include "sea_uart.h"
#include "sea_adc.h"
#include "sea_i2c.h"
#include "sea_lcd.h"

int main(void)
{
    char line[17];

    uart_init();
    adc_init();
    i2c_init();
    while (!lcd_init()) {
        puts("Could not init backpack. Check wiring.");
        delay_ms(50);
    }
    puts("Backpack init'd.");

    for (;;) {
        uint16_t value = adc_read(ADC_MOISTURE);
        printf("sensor1 = %u\n", value);

        snprintf(line, sizeof line, "A3:%u", value);
        lcd_clear();
        lcd_print_at(0, 0, line);

        delay_ms(1000);
    }
}
