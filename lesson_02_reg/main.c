#include "sea0004t.h"
#include "sea_time.h"
int main(void)
{
	DDRB |= (1 << PB2);              /* PB2を出力に設定 */
	DDRD &= ~(1 << PD7);              /* PD7を入力に設定 */
	for (;;) {
		uint8_t sw = !(PIND & (1 << PD7));
		if (sw == 1)
			PORTB |= (1 << PB2);         /* LED点灯 */
		else
			PORTB &= ~(1 << PB2);        /* LED消灯 */
		delay_ms(100);
	}

}