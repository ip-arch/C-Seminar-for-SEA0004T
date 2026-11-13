// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 Naohiko Shimizu <nshimizu@ip-arch.jp>
/* sea_lcd.h -- 16x2 文字 LCD。I2C 経由 (MCP23008 → HD44780 の 4bit 接続)
 *   lcd_init() の前に i2c_init() を呼ぶこと */
#ifndef SEA_LCD_H
#define SEA_LCD_H
#include <stdint.h>

uint8_t lcd_init(void);                       /* 成功なら 1、LCD が応答しなければ 0 */
void    lcd_clear(void);
void    lcd_set_cursor(uint8_t col, uint8_t row);
void    lcd_puts(const char *s);
void    lcd_set_backlight(uint8_t on);

/* 補助: 指定位置に文字列を書く */
void    lcd_print_at(uint8_t col, uint8_t row, const char *s);
/* 補助: 1 行を空白で消す */
void    lcd_clear_row(uint8_t row);
/* 補助: 2 行まとめて書き換える (どちらも先に消す) */
void    lcd_print_lines(const char *line1, const char *line2);

#endif
