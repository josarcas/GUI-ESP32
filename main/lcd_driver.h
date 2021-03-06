/*
 * lcd_driver.h
 *
 *  Created on: 17 dic. 2021
 *      Author: JoseCarlos
 */

#ifndef MAIN_LCD_DRIVER_H_
#define MAIN_LCD_DRIVER_H_

/*INCLUDES*******************************************************************************/
#include "stdint.h"
#include "string.h"
#include "stdio.h"
#include "stdbool.h"
#include "math.h"

#include "driver/spi_master.h"

#include "fonts.h"


/*DEFINES********************************************************************************/
#define LCD_SPI_HOST			SPI2_HOST
#define DMA_CH					2

#define SPI_CS_PIN				22
#ifndef SPI_BUS_PORT
#define SPI_BUS_PORT
#define SPI_MOSI_PIN			23
#define SPI_MISO_PIN			19
#define SPI_CLK_PIN				18
#endif
#define DC_PIN					17
#define RST_PIN					16

#define LCD_WIDTH				480
#define LCD_HEIGHT				320

#define SOFT_RST				0x01
#define SLEEP_IN				0x10
#define SLEEP_OUT				0X11
#define PARTIAL_ON				0x12
#define PARTIAL_OFF				0x13
#define INVERSION_ON			0X20
#define INVERSION_OFF			0X21
#define DISPLAY_OFF				0X28
#define DISPLAY_ON				0X29

#define WHITE					0xFFFF
#define BLACK					0x0000
#define BLUE 					0x001F
#define BRED 					0XF81F
#define GRED 					0XFFE0
#define GBLUE					0X07FF
#define RED  					0x001F
#define MAGENTA					0xF81F
#define GREEN					0x07E0
#define CYAN 					0x7FFF
#define YELLOW					0xFFE0
#define BROWN					0XBC40
#define BRRED					0XFC07
#define GRAY 					0X8430
#define DARKBLUE				0X01CF
#define LIGHTBLUE				0X7D7C
#define GRAYBLUE     	    	0X5458
#define LIGHTGREEN    			0X841F
#define LGRAY 			  		0XC618
#define LGRAYBLUE     			0XA651
#define LBBLUE        			0X2B12


/*PROTOTYPES******************************************************************/
esp_err_t spi_init_device(bool init_spi);
void lcd_init();
void lcd_reset();
void lcd_clear(uint16_t color);

void lcd_set_pixel(uint16_t x, uint16_t y, uint16_t color);
void lcd_set_window_color(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1,
		uint16_t color);
void lcd_draw_string(char *str, uint16_t x, uint16_t y,
		uint16_t color, font_t font);
void lcd_draw_image(const uint16_t *image, uint16_t x, uint16_t y, uint16_t width,
		uint16_t height);
void lcd_draw_line(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1,
		uint16_t color);
void lcd_draw_rect(uint16_t x, uint16_t y, uint16_t width, uint16_t height,
		uint16_t color);
void lcd_draw_circle(uint16_t x, uint16_t y, uint16_t rad, uint16_t color);
void lcd_draw_fill_circle(uint16_t x, uint16_t y, uint16_t rad, uint16_t color);
void lcd_draw_batery_widget(uint16_t x, uint16_t y, uint8_t level);
void lcd_draw_wifi_signal_widget(uint16_t x,uint16_t y, int8_t level);
void lcd_draw_circular_load_widget(uint16_t x, uint16_t y, uint16_t rad,
		uint8_t percent, uint16_t color, uint16_t background_color);
void lcd_draw_rect_load_widget(uint16_t x, uint16_t y, uint8_t percent,
		uint16_t color, uint16_t left_color, uint16_t right_color);
uint16_t convert_from_rgb_8(uint8_t r, uint8_t g, uint8_t b);
void lcd_draw_message_widget(uint16_t x, uint16_t y, uint8_t n_messages);



#endif /* MAIN_LCD_DRIVER_H_ */
