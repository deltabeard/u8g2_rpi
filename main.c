/**
 * MIT License
 * Copyright (c) 2018 Mahyar Koshkouei
 *
 * Compile with:
 * `gcc -lwiringPi -lu8g2  main.c`
 */

#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include <u8g2.h>
#include <wiringPi.h>
#include <wiringPiSPI.h>

/* Defining ports (BCM pins) for SPI communication for the Raspberry Pi. */
#define LCD_CS		7	/* LCD chip select. */
#define SPI_MOSI	10	/* SPI MOSI pin. */
#define SPI_CLK		11	/* SPI CLK pin. */
#define RESET		2

#define pika_width 40
#define pika_height 40
static unsigned char pika_bits[] = {
   0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0x07, 0x00, 0x00, 0x00, 0x00, 0x07,
   0x00, 0x00, 0x00, 0x80, 0x07, 0x00, 0x00, 0x00, 0x80, 0x0b, 0x00, 0xf8,
   0x00, 0x80, 0x08, 0x00, 0xf7, 0x00, 0xc0, 0x08, 0xc0, 0x70, 0x00, 0x40,
   0xf0, 0x31, 0x30, 0x00, 0x40, 0x0c, 0x0e, 0x18, 0x00, 0x40, 0x01, 0x00,
   0x08, 0x06, 0x40, 0x00, 0x00, 0x04, 0x09, 0x20, 0x00, 0x00, 0x02, 0x31,
   0x60, 0x80, 0x01, 0x81, 0x40, 0x50, 0x40, 0xc3, 0x80, 0x80, 0x70, 0xc0,
   0x83, 0x40, 0x80, 0x28, 0x81, 0x81, 0x40, 0x80, 0x08, 0x00, 0x0c, 0x21,
   0x40, 0x98, 0x26, 0x1e, 0x21, 0x20, 0x18, 0x1f, 0x1e, 0x12, 0x10, 0x18,
   0x0e, 0x1c, 0x12, 0x08, 0x16, 0x06, 0x28, 0x16, 0x04, 0x21, 0x00, 0x46,
   0x27, 0x0c, 0x41, 0x00, 0x40, 0x46, 0x10, 0x02, 0x00, 0x20, 0xc8, 0x18,
   0x04, 0x00, 0x00, 0x28, 0x04, 0x18, 0x00, 0x38, 0x1e, 0x03, 0x20, 0x00,
   0x45, 0x2f, 0x01, 0x60, 0x80, 0x0a, 0x3e, 0x02, 0x40, 0x80, 0x08, 0xf8,
   0x01, 0x40, 0x80, 0x10, 0x7c, 0x00, 0x40, 0x00, 0x11, 0x1c, 0x00, 0x80,
   0x00, 0x11, 0x06, 0x00, 0x80, 0x00, 0x09, 0x01, 0x00, 0x00, 0x01, 0x86,
   0x00, 0x00, 0x00, 0x06, 0x70, 0x00, 0x00, 0x00, 0x18, 0x0c, 0x00, 0x00,
   0x00, 0x60, 0x07, 0x00, 0x00, 0x00, 0x40, 0x04, 0x00, 0x00, 0x00, 0x40,
   0x05, 0x00, 0x00, 0x00, 0x80, 0x02, 0x00, 0x00 };

uint8_t u8x8_byte_rpi_hw_spi(u8x8_t *u8x8, uint8_t msg,
		uint8_t arg_int, void *arg_ptr);

uint8_t u8x8_rpi_gpio_and_delay(u8x8_t *u8x8, uint8_t msg,
		uint8_t arg_int, void *arg_ptr);

int main(void)
{
	/* Display structure. */
	static u8g2_t u8g2;

	/* Initialise wiringPi. */
	wiringPiSetupGpio();
	
	/* Initialise graphics library. */
	u8g2_Setup_st7920_s_128x64_f(&u8g2, U8G2_R0, u8x8_byte_rpi_hw_spi,
			u8x8_rpi_gpio_and_delay);
	/* Send init sequence to the display, display will be in sleep mode. */
	u8g2_InitDisplay(&u8g2);
	/* Wake up Display. */
	u8g2_SetPowerSave(&u8g2, 0);

	u8g2_SetFont(&u8g2, u8g2_font_profont10_mr);
	u8g2_SetFontMode(&u8g2, 0);
	u8g2_DrawStr(&u8g2, 0,8, "Hello World!");
	u8g2_DrawStr(&u8g2, 0, 16, "Finally got this working");
	u8g2_DrawFilledEllipse(&u8g2, 50, 35, 15, 15, U8G2_DRAW_ALL);
	u8g2_SetDrawColor(&u8g2, 0);
	u8g2_DrawHLine(&u8g2, 25, 35, 60);
	u8g2_DrawFilledEllipse(&u8g2, 50, 35, 3, 3, U8G2_DRAW_ALL);
	u8g2_DrawXBM(&u8g2, 70, 17, pika_width, pika_height, pika_bits);
	u8g2_SetFont(&u8g2, u8g2_font_u8glib_4_hr);
	u8g2_SetDrawColor(&u8g2, 1);
	u8g2_DrawStr(&u8g2, 0, 62, "This is some tiny text.");

	while(1)
	{
		static time_t t;
		u8g2_SetFontMode(&u8g2, 0);
		time(&t);
		u8g2_DrawStr(&u8g2, 0, 24, ctime(&t));
		u8g2_SendBuffer(&u8g2);
		sleep(1);
	}

	return 0;
}

/**
 * U8G2 communication callback for the Raspberry Pi with wiringPi.
 */
uint8_t u8x8_byte_rpi_hw_spi(u8x8_t *u8x8, uint8_t msg,
		uint8_t arg_int, void *arg_ptr)
{
	switch(msg)
	{
		case U8X8_MSG_BYTE_INIT:
			if(wiringPiSetup() == -1)
			{
				puts("WiringPi Error");
				exit(1);
			}

			if(wiringPiSPISetup(0, 100000) < 0)
			{
				printf("Unable to open SPI device 0: %s\n", strerror(errno));
				exit(1);
			}

			puts("Initialized Raspberry Pi SPI device.");
			break;

		case U8X8_MSG_BYTE_SET_DC:
			break;

		case U8X8_MSG_BYTE_START_TRANSFER:
			break;

		case U8X8_MSG_BYTE_SEND:
			wiringPiSPIDataRW(0, arg_ptr, arg_int);
			break;

		case U8X8_MSG_BYTE_END_TRANSFER:
			break;

		default:
			return 0;
	}

	return 1;
}

uint8_t u8x8_rpi_gpio_and_delay(u8x8_t *u8x8, uint8_t msg,
		uint8_t arg_int, void *arg_ptr)
{
	static struct timespec tv = {0};
	switch(msg)
	{
		case U8X8_MSG_GPIO_AND_DELAY_INIT:	// called once during init phase of u8g2/u8x8
			/* No need to initialise the pin mode of SPI pins. */
			/* Initialise LCD Chip Select pin as output. */
			pinMode(LCD_CS, OUTPUT);
			pinMode(RESET, OUTPUT);
			break;							// can be used to setup pins
		case U8X8_MSG_DELAY_NANO:			// delay arg_int * 1 nano second
			tv.tv_sec = 0;
			tv.tv_nsec = (long) arg_int;
			nanosleep(&tv, NULL);
			break;
		case U8X8_MSG_DELAY_100NANO:		// delay arg_int * 100 nano seconds
			tv.tv_sec = 0;
			tv.tv_nsec = ((long) arg_int) * 100L;
			nanosleep(&tv, NULL);
			break;
		case U8X8_MSG_DELAY_10MICRO:		// delay arg_int * 10 micro seconds
			tv.tv_sec = 0;
			tv.tv_nsec = ((long) arg_int) * 10000L;
			nanosleep(&tv, NULL);
			break;
		case U8X8_MSG_DELAY_MILLI:			// delay arg_int * 1 milli second
			tv.tv_sec = 0;
			tv.tv_nsec = ((long) arg_int) * 1000000L;
			nanosleep(&tv, NULL);
			break;
		case U8X8_MSG_DELAY_I2C:				// arg_int is the I2C speed in 100KHz, e.g. 4 = 400 KHz
			break;							// arg_int=1: delay by 5us, arg_int = 4: delay by 1.25us
		case U8X8_MSG_GPIO_D0:				// D0 or SPI clock pin: Output level in arg_int
			//case U8X8_MSG_GPIO_SPI_CLOCK:
			break;
		case U8X8_MSG_GPIO_D1:				// D1 or SPI data pin: Output level in arg_int
			//case U8X8_MSG_GPIO_SPI_DATA:
			break;
		case U8X8_MSG_GPIO_D2:				// D2 pin: Output level in arg_int
			break;
		case U8X8_MSG_GPIO_D3:				// D3 pin: Output level in arg_int
			break;
		case U8X8_MSG_GPIO_D4:				// D4 pin: Output level in arg_int
			break;
		case U8X8_MSG_GPIO_D5:				// D5 pin: Output level in arg_int
			break;
		case U8X8_MSG_GPIO_D6:				// D6 pin: Output level in arg_int
			break;
		case U8X8_MSG_GPIO_D7:				// D7 pin: Output level in arg_int
			break;
		case U8X8_MSG_GPIO_E:				// E/WR pin: Output level in arg_int
			break;
		case U8X8_MSG_GPIO_CS:				// CS (chip select) pin: Output level in arg_int
			digitalWrite(LCD_CS, (int) arg_int);
			break;
		case U8X8_MSG_GPIO_DC:				// DC (data/cmd, A0, register select) pin: Output level in arg_int
			break;
		case U8X8_MSG_GPIO_RESET:			// Reset pin: Output level in arg_int
			digitalWrite(RESET, (int) arg_int);
			break;
		case U8X8_MSG_GPIO_CS1:				// CS1 (chip select) pin: Output level in arg_int
			break;
		case U8X8_MSG_GPIO_CS2:				// CS2 (chip select) pin: Output level in arg_int
			break;
		case U8X8_MSG_GPIO_I2C_CLOCK:		// arg_int=0: Output low at I2C clock pin
			break;							// arg_int=1: Input dir with pullup high for I2C clock pin
		case U8X8_MSG_GPIO_I2C_DATA:			// arg_int=0: Output low at I2C data pin
			break;							// arg_int=1: Input dir with pullup high for I2C data pin
		case U8X8_MSG_GPIO_MENU_SELECT:
			u8x8_SetGPIOResult(u8x8, /* get menu select pin state */ 0);
			break;
		case U8X8_MSG_GPIO_MENU_NEXT:
			u8x8_SetGPIOResult(u8x8, /* get menu next pin state */ 0);
			break;
		case U8X8_MSG_GPIO_MENU_PREV:
			u8x8_SetGPIOResult(u8x8, /* get menu prev pin state */ 0);
			break;
		case U8X8_MSG_GPIO_MENU_HOME:
			u8x8_SetGPIOResult(u8x8, /* get menu home pin state */ 0);
			break;
		default:
			u8x8_SetGPIOResult(u8x8, 1);			// default return value
			break;
	}
	return 1;
}

