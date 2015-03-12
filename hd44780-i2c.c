#include "hd44780-i2c.h"

/*
 * Useful commands:
 * 0x1E - Scroll display one character right (every line): [  Dupa] -> [a  Dup]
 * 0x18 - Scroll display one character left (every line)
 * 0x10 - Move cursor one character left
 * 0x14 - Move cursor one character right
 * 0x02 - Go cursor home
 * 0x0E - Underlined cursor
 * 0x0F - Blinked block cursor
 * 0x0C - Invisible cursor
 * 0x08 - Hide display
 * 0x0C - Show display with invisible cursor
 * 0x01 - Clear display
 *
 */

u8 backlight = 1 << 3;
u8 en        = 1 << 2;
u8 rw        = 1 << 1;
u8 rs        = 1 << 0;
TIM_TypeDef *timer;
uint8_t data[4] = {0x00, 0x00, 0x00};

void hd44780_send(u8 cmd, bool set_rs) {
	u8 rs_ = 0;
	if (set_rs)
		rs_ = rs;

	data[0] = (cmd & 0xF0) | backlight | en | rs_;
	data[1] = (cmd & 0xF0) | backlight;
	data[2] = (cmd & 0x0F) << 4 | backlight | en | rs_;
	data[3] = (cmd & 0x0F) << 4 | backlight;
	I2C_Master_BufferWrite(I2C1, data, 4, Polling, hd44780_address << 1);
}

void hd44780_char(u8 cmd) {
	hd44780_send(cmd, true);
	delay_us(timer, 200);
}

void hd44780_cmd(u8 cmd) {
	hd44780_send(cmd, false);
	delay_us(timer, 5000);
}

void hd44780_print(char *string) {
	char* ptr = string;
	while (*(ptr) != 0) {
		hd44780_char(*ptr++);
	};
}

void hd44780_backlight(bool new_value) {
	backlight = new_value << 3;
	I2C_Master_BufferRead(I2C1, data, 1, Polling, hd44780_address << 1);
	data[0] |= backlight;
	I2C_Master_BufferWrite(I2C1, data, 1, Polling, hd44780_address << 1);
}

void hd44780_go_to_line(u8 line) {
	switch(line) {
	case 0:
		hd44780_cmd(0x80);
		break;
	case 1:
		hd44780_cmd(0xC0);
		break;
	case 2:
		hd44780_cmd(0x94);
		break;
	case 3:
		hd44780_cmd(0xD4);
		break;
	default:
		break;
	}
}

void hd44780_go_to(u8 row, u8 col) {
    hd44780_go_to_line(row);
    u8 i = 0;
    while (i++ < col)
        hd44780_cmd(0x14);
}

void hd44780_cgram_write(u8 pos, u8 data[8]) {
	if (pos > 7) {
		return;
	}
	pos = 64 + pos * 8;
	hd44780_cmd(pos);
	u8 i;
	for (i = 0; i < 8; ++i) {
		hd44780_send(data[i], true);
	}
}

void hd44780_init(TIM_TypeDef *t) {
	timer = t;
	// Setup clock
	if (timer == TIM2)
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
	else if (timer == TIM3)
	    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
	else
		while(1){} // not implemented
	TIM_TimeBaseInitTypeDef TIM_InitStructure;
	TIM_InitStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_InitStructure.TIM_Prescaler = 72 - 1;
	TIM_InitStructure.TIM_Period = 10000 - 1; // Update event every 10000 us / 10 ms
	TIM_InitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_InitStructure.TIM_RepetitionCounter = 0;
	TIM_TimeBaseInit(timer, &TIM_InitStructure);
	TIM_Cmd(timer, ENABLE);


	data[0] = 0x00 | backlight;
	data[1] = 0x00 | backlight;

//	GPIO_SetBits(GPIOC, GPIO_Pin_8);
//	delay_us(timer, 250);
//	GPIO_ResetBits(GPIOC, GPIO_Pin_8);

	// Reset all
	I2C_Master_BufferWrite(I2C1, data, 1, Polling, hd44780_address << 1);

	hd44780_cmd(0x03);
	delay_us(timer, 5000);
	hd44780_cmd(0x03);
	delay_us(timer, 100);
	hd44780_cmd(0x02);
	delay_us(timer, 200);

	hd44780_cmd(0x28); // 4 bit mode
	hd44780_cmd(0x06); // set direction of cursor to right
	hd44780_cmd(0x01); // clear display, go to 0x0
	hd44780_cmd(0x0E); // turn on display, set solid cursor
	hd44780_cmd(0x0C); // turn on display, set invisiblecursor

	hd44780_cgram_write(0, (u8[]){0,10,31,31,14,4,0,0});
	hd44780_cgram_write(1, (u8[]){0,10,21,17,10,4,0,0});

	hd44780_cmd(0x01); // clear display, go to 0x0

//	hd44780_backlight(true);
//	hd44780_print("Linia 0");
//	hd44780_go_to_line(1);
//	hd44780_print("Linia 1");
//	hd44780_go_to_line(2);
//	hd44780_print("Linia 2");
//	hd44780_go_to_line(3);
//	hd44780_print("Linia 3");

//	hd44780_char(0);
//	hd44780_char(1);
}


