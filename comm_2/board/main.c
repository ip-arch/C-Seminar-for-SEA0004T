// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 Naohiko Shimizu <nshimizu@ip-arch.jp>
/* comm_2 (ボード側): PC からの問い合わせに、センサの値を答える
 *   PC → ボード  "GET POT" / "GET SOUND"
 *   ボード → PC  "POT 355" / "SOUND 120" / "ERROR" */

#include <stdio.h>
#include <string.h>
#include "sea0004t.h"
#include "sea_uart.h"
#include "sea_adc.h"
#include "../protocol.h"

int main(void)
{
    char line[32];

    uart_init();
    adc_init();
    puts(PROTO_READY);

    for (;;) {
        if (fgets(line, sizeof line, stdin) == NULL) {
            continue;
        }
        line[strcspn(line, "\r\n")] = '\0';

        if (strcmp(line, CMD_GET_POT) == 0) {
            printf("%s %u\n", RES_POT, adc_read(ADC_POT));
        } else if (strcmp(line, CMD_GET_SOUND) == 0) {
            printf("%s %u\n", RES_SOUND, adc_read(ADC_SOUND));
        } else {
            puts(RES_ERROR);
        }
    }
}
