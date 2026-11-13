// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 Naohiko Shimizu <nshimizu@ip-arch.jp>
/* lesson_19: 加速度センサの値を LCD に表示する (Elecrow L_19 Calculation_of_acceleration)
 *   値の単位は m/s^2 */

#include <stdio.h>
#include "sea_time.h"
#include "sea_uart.h"
#include "sea_i2c.h"
#include "sea_lcd.h"
#include "sea_lsm6ds3.h"
#include "sea0004t.h"

int main(void)
{
    char x[12], y[12], z[12], buf[12];
    int32_t accel[3];                      /* 0.01 m/s^2 単位 */

    uart_init();
    i2c_init();
    if (!lsm6ds3_init()) {
        puts("LSM6DS3 not found");
    }
    while (!lcd_init()) {
        puts("Could not init backpack. Check wiring.");
        delay_ms(50);
    }

    lcd_print_at(0, 0, "ax:");
    lcd_print_at(8, 0, "ay:");
    lcd_print_at(0, 1, "az:");

    for (;;) {
        if (lsm6ds3_read_accel(accel) == 0) {
            sea_fixed_str(x, sizeof x, accel[0], 2);
            sea_fixed_str(y, sizeof y, accel[1], 2);
            sea_fixed_str(z, sizeof z, accel[2], 2);

            printf("\t\tAccel X: %s \tY: %s \tZ: %s m/s^2 \n", x, y, z);

            snprintf(buf, sizeof buf, "%-5.5s", x);
            lcd_print_at(3, 0, buf);
            snprintf(buf, sizeof buf, "%-5.5s", y);
            lcd_print_at(11, 0, buf);
            snprintf(buf, sizeof buf, "%-5.5s", z);
            lcd_print_at(3, 1, buf);
        }
        delay_ms(500);
    }
}
