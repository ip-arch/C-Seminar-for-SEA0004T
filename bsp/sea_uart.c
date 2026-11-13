// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 Naohiko Shimizu <nshimizu@ip-arch.jp>
#include "sea_uart.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>

#define RX_BUF_SIZE  64                    /* 2 のべき乗でなくてもよい */

static volatile uint8_t rx_buf[RX_BUF_SIZE];
static volatile uint8_t rx_head;           /* 次に書く位置 (割り込みが進める) */
static volatile uint8_t rx_tail;           /* 次に読む位置 (uart_getc が進める) */

/* 1 文字受信するたびに呼ばれる。バッファが満杯のときは、その文字を捨てる */
ISR(USART_RX_vect)
{
    uint8_t data = UDR0;
    uint8_t next = (uint8_t)((rx_head + 1) % RX_BUF_SIZE);
    if (next != rx_tail) {
        rx_buf[rx_head] = data;
        rx_head = next;
    }
}

uint8_t uart_available(void)
{
    return (uint8_t)((rx_head + RX_BUF_SIZE - rx_tail) % RX_BUF_SIZE);
}

int uart_getc(void)
{
    while (rx_head == rx_tail) { }
    uint8_t data = rx_buf[rx_tail];
    rx_tail = (uint8_t)((rx_tail + 1) % RX_BUF_SIZE);
    return data;
}

static int uart_putchar(char c, FILE *stream)
{
    (void)stream;
    if (c == '\n') {
        uart_putchar('\r', stream);
    }
    while (!(UCSR0A & (1 << UDRE0))) { }
    UDR0 = (uint8_t)c;
    return 0;
}

/* 行の区切りは '\n'。Windows から来る "\r\n" の '\r' は読み捨てる */
static int uart_getchar(FILE *stream)
{
    (void)stream;
    int c;
    do {
        c = uart_getc();
    } while (c == '\r');
    return c;
}

static FILE uart_stream = FDEV_SETUP_STREAM(uart_putchar, uart_getchar, _FDEV_SETUP_RW);

void uart_init(void)
{
    UCSR0A = (1 << U2X0);                          /* 倍速モード */
    UBRR0  = (uint16_t)(F_CPU / (8UL * 115200UL) - 1);
    UCSR0B = (1 << TXEN0) | (1 << RXEN0) | (1 << RXCIE0);   /* 送信・受信・受信割り込み */
    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);        /* 8N1 */
    stdout = &uart_stream;
    stdin  = &uart_stream;
    sei();
}
