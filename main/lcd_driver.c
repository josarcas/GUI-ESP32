/*
 * lcd_driver.c
 *
 *  Created on: 17 dic. 2021
 *      Author: JoseCarlos
 */

/*INCLUDES******************************************************************************/
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"

#include "driver/gpio.h"
#include "lcd_driver.h"

/*GLOBAL VARIABLES*********************************************************************/
static spi_device_handle_t spi;
static xSemaphoreHandle lcd_mutex;

/*PROTOTYPES***************************************************************************/
static void lcd_spi_pre_transfer_callback(spi_transaction_t *t);
static void lcd_write_reg(uint8_t reg);
static void lcd_write_data(uint8_t *data, int len);
static void lcd_write_data_byte(uint8_t data);
static void lcd_set_cursor(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t x2);

/*FUNCTIONS****************************************************************************/
static void lcd_spi_pre_transfer_callback(spi_transaction_t *t)
{
    int dc=(int)t->user;
    gpio_set_level(DC_PIN, dc);
}


static void lcd_write_reg(uint8_t reg)
{
    esp_err_t status;
    spi_transaction_t spi_transaction;
    memset(&spi_transaction, 0, sizeof(spi_transaction));
    spi_transaction.length=8;
    spi_transaction.tx_buffer=&reg;
    spi_transaction.user=(void*)0;
    status=spi_device_polling_transmit(spi, &spi_transaction);
    assert(status==ESP_OK);
}

static void lcd_write_data(uint8_t *data, int len)
{
    esp_err_t status;
    spi_transaction_t spi_transaction;
    if (len==0) return;
    memset(&spi_transaction, 0, sizeof(spi_transaction));
    spi_transaction.length=len*8;
    spi_transaction.tx_buffer=data;
    spi_transaction.user=(void*)1;
    status=spi_device_polling_transmit(spi, &spi_transaction);
    assert(status==ESP_OK);
}

static void lcd_write_data_byte(uint8_t data)
{
	uint8_t buffer[2] = {0x00, data};
    esp_err_t status;
    spi_transaction_t spi_transaction;
    memset(&spi_transaction, 0, sizeof(spi_transaction));
    spi_transaction.length=16;
    spi_transaction.tx_buffer=buffer;
    spi_transaction.user=(void*)1;
    status=spi_device_polling_transmit(spi, &spi_transaction);
    assert(status==ESP_OK);
}

static void lcd_set_cursor(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1)
{
    lcd_write_reg(0x2A);
    lcd_write_data_byte(x0>>8);
    lcd_write_data_byte(x0);
    lcd_write_data_byte((x1-1)>>8);
    lcd_write_data_byte((x1 - 1));

    lcd_write_reg(0x2B);
    lcd_write_data_byte(y0>>8);
    lcd_write_data_byte(y0);
    lcd_write_data_byte((y1-1)>>8);
    lcd_write_data_byte((y1-1));
    lcd_write_reg(0x2C);
}


esp_err_t spi_init_device(bool init_spi)
{
    esp_err_t status;

    if(init_spi)
    {
		spi_bus_config_t buscfg={
			.miso_io_num=SPI_MISO_PIN,
			.mosi_io_num=SPI_MOSI_PIN,
			.sclk_io_num=SPI_CLK_PIN,
			.quadwp_io_num=-1,
			.quadhd_io_num=-1,
			//.max_transfer_sz=10000
		};

	    status = spi_bus_initialize(LCD_SPI_HOST, &buscfg, DMA_CH);

	    if(status != ESP_OK)
	    	return status;
    }

    spi_device_interface_config_t devcfg={
        .clock_speed_hz=14000000,
        .mode=0,
        .spics_io_num=SPI_CS_PIN,
        .queue_size=20,
        .pre_cb=lcd_spi_pre_transfer_callback,
    };

    status = spi_bus_add_device(LCD_SPI_HOST, &devcfg, &spi);

    if(status != ESP_OK)
    	return status;

    return status;
}


void lcd_init()
{
	gpio_set_direction(DC_PIN, GPIO_MODE_OUTPUT);
	gpio_set_direction(RST_PIN, GPIO_MODE_OUTPUT);

	lcd_reset();
	lcd_mutex = xSemaphoreCreateMutex();

	if(xSemaphoreTake(lcd_mutex, portMAX_DELAY) == pdTRUE)
	{
	    lcd_write_reg(0XF9);
	    lcd_write_data_byte(0x00);
	    lcd_write_data_byte(0x08);

	    lcd_write_reg(0xC0);
	    lcd_write_data_byte(0x19);
	    lcd_write_data_byte(0x1a);

	    lcd_write_reg(0xC1);
	    lcd_write_data_byte(0x45);
	    lcd_write_data_byte(0x00);

	    lcd_write_reg(0xC2);
	    lcd_write_data_byte(0x33);

	    lcd_write_reg(0XC5);
	    lcd_write_data_byte(0x00);
	    lcd_write_data_byte(0x28);

	    lcd_write_reg(0xB1);
	    lcd_write_data_byte(0xA0);
	    lcd_write_data_byte(0x11);

	    lcd_write_reg(0xB4);
	    lcd_write_data_byte(0x02);

	    lcd_write_reg(0xB6);
	    lcd_write_data_byte(0x00);
	    lcd_write_data_byte(0x42);
	    lcd_write_data_byte(0x3B);

	    lcd_write_reg(0xB7);
	    lcd_write_data_byte(0x07);

	    lcd_write_reg(0xE0);
	    lcd_write_data_byte(0x1F);
	    lcd_write_data_byte(0x25);
	    lcd_write_data_byte(0x22);
	    lcd_write_data_byte(0x0B);
	    lcd_write_data_byte(0x06);
	    lcd_write_data_byte(0x0A);
	    lcd_write_data_byte(0x4E);
	    lcd_write_data_byte(0xC6);
	    lcd_write_data_byte(0x39);
	    lcd_write_data_byte(0x00);
	    lcd_write_data_byte(0x00);
	    lcd_write_data_byte(0x00);
	    lcd_write_data_byte(0x00);
	    lcd_write_data_byte(0x00);
	    lcd_write_data_byte(0x00);

	    lcd_write_reg(0XE1);
	    lcd_write_data_byte(0x1F);
	    lcd_write_data_byte(0x3F);
	    lcd_write_data_byte(0x3F);
	    lcd_write_data_byte(0x0F);
	    lcd_write_data_byte(0x1F);
	    lcd_write_data_byte(0x0F);
	    lcd_write_data_byte(0x46);
	    lcd_write_data_byte(0x49);
	    lcd_write_data_byte(0x31);
	    lcd_write_data_byte(0x05);
	    lcd_write_data_byte(0x09);
	    lcd_write_data_byte(0x03);
	    lcd_write_data_byte(0x1C);
	    lcd_write_data_byte(0x1A);
	    lcd_write_data_byte(0x00);

	    lcd_write_reg(0XF1);
	    lcd_write_data_byte(0x36);
	    lcd_write_data_byte(0x04);
	    lcd_write_data_byte(0x00);
	    lcd_write_data_byte(0x3C);
	    lcd_write_data_byte(0x0F);
	    lcd_write_data_byte(0x0F);
	    lcd_write_data_byte(0xA4);
	    lcd_write_data_byte(0x02);

	    lcd_write_reg(0XF2);
	    lcd_write_data_byte(0x18);
	    lcd_write_data_byte(0xA3);
	    lcd_write_data_byte(0x12);
	    lcd_write_data_byte(0x02);
	    lcd_write_data_byte(0x32);
	    lcd_write_data_byte(0x12);
	    lcd_write_data_byte(0xFF);
	    lcd_write_data_byte(0x32);
	    lcd_write_data_byte(0x00);

	    lcd_write_reg(0XF4);
	    lcd_write_data_byte(0x40);
	    lcd_write_data_byte(0x00);
	    lcd_write_data_byte(0x08);
	    lcd_write_data_byte(0x91);
	    lcd_write_data_byte(0x04);

	    lcd_write_reg(0XF8);
	    lcd_write_data_byte(0x21);
	    lcd_write_data_byte(0x04);

	    lcd_write_reg(0X3A);
	    lcd_write_data_byte(0x55);

	    lcd_write_reg(0xB6);
	    lcd_write_reg(0X00);
	    lcd_write_data_byte(0x08);

	    lcd_write_reg(0x36);
	    lcd_write_data_byte(0x62);

	    vTaskDelay(200);

	    lcd_write_reg(0x11);
	    vTaskDelay(120);

	    lcd_write_reg(0x29);

		xSemaphoreGive(lcd_mutex);
	}
}

void lcd_reset(void)
{
	gpio_set_level(RST_PIN, 1);
	vTaskDelay(50);
	gpio_set_level(RST_PIN, 0);
	vTaskDelay(50);
	gpio_set_level(RST_PIN, 1);
	vTaskDelay(50);
}

void lcd_clear(uint16_t color)
{
	lcd_set_window_color(0, 0, LCD_WIDTH, LCD_HEIGHT, color);
}

void lcd_set_pixel(uint16_t x, uint16_t y, uint16_t color)
{
	uint8_t buffer[2] = {color>>8, color};
	lcd_set_cursor(x, y, x, y);
	lcd_write_data(buffer, 2);
}

void lcd_set_window_color(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1,
		uint16_t color)
{

	uint8_t buffer[2] = {color>>8, color};

    if(xSemaphoreTake(lcd_mutex, portMAX_DELAY) == pdTRUE)
    {
        lcd_set_cursor(x0, y0, x1, y1);
        for(uint16_t i=x0; i<x1; i++)
        {
        	for(uint16_t j=y0; j<y1; j++)
        		lcd_write_data(buffer, 2);
        }

        xSemaphoreGive(lcd_mutex);
    }

}

void lcd_draw_string(char *str, uint16_t x, uint16_t y,
		uint16_t color, font_t font)
{
	uint16_t page;
	uint8_t size = strlen(str);

	switch (font)
	{
		case font8:
		{
			for(uint8_t i=0; i<size; i++)
			{
				x+=8;
				page = (str[i]-32)*8;
				for(uint8_t j=0; j<8; j++)
				{
					for(uint8_t k=0; k<8; k++)
					{
						if((font8_table[page]>>k)& 0x01)
							lcd_set_pixel(x-k, j+y, color);
					}
					page++;
				}
			}

		}
		break;

		case font12:
		{
			for(uint8_t i=0; i<size; i++)
			{
				x+=8;
				page = (str[i]-32)*12;
				for(uint8_t j=0; j<12; j++)
				{
					for(uint8_t k=0; k<8; k++)
					{
						if((font12_table[page]>>k)& 0x01)
							lcd_set_pixel(x-k, j+y, color);
					}
					page++;
				}
			}
		}
		break;

		case font16:
		{
			for(uint8_t i=0; i<size; i++)
			{
				x+=13;
				page = (str[i]-32)*32;
				for(uint8_t j=0; j<16; j++)
				{
					page++;
					for(uint8_t k=0; k<8; k++)
					{
						if((font16_table[page]>>k)& 0x01)
							lcd_set_pixel(x-k, j+y, color);
						if((font16_table[page-1]>>k)& 0x01)
							lcd_set_pixel((x-8)-k, j+y, color);
					}
					page++;
				}
			}
		}
		break;

		case font20:
		{
			for(uint8_t i=0; i<size; i++)
			{
				x+=16;
				page = (str[i]-32)*40;
				for(uint8_t j=0; j<20; j++)
				{
					page++;
					for(uint8_t k=0; k<8; k++)
					{
						if((font20_table[page]>>k)& 0x01)
							lcd_set_pixel(x-k, j+y, color);
						if((font20_table[page-1]>>k)& 0x01)
							lcd_set_pixel((x-8)-k, j+y, color);
					}
					page++;
				}
			}
		}
		break;

		case font24:
		{
			for(uint8_t i=0; i<size; i++)
			{
				x+=24;
				page = (str[i]-32)*47;
				for(uint8_t j=0; j<24; j++)
				{
					page+=2;
					for(uint8_t k=0; k<8; k++)
					{
						if((font24_table[page]>>k)& 0x01)
							lcd_set_pixel(x-k, j+y, color);
						if((font24_table[page-1]>>k)& 0x01)
							lcd_set_pixel((x-8)-k, j+y, color);
						if((font24_table[page-2]>>k)& 0x01)
							lcd_set_pixel((x-16)-k, j+y, color);
					}
					page++;
				}
			}
		}
		break;

	}
}

void lcd_draw_image(const uint16_t *image, uint16_t x, uint16_t y, uint16_t width,
		uint16_t height)
{
	uint16_t page=0;

	lcd_set_cursor(x, y, x+width, y+height);
	uint8_t buffer[2];

	for(uint16_t i=0; i< height; i++)
	{
		for(uint16_t j= 0; j<width; j++)
		{
			buffer[0] = image[page+j]>>8;
			buffer[1] = image[page+j];
			lcd_write_data(buffer, 2);
		}
		page += width;
	}

}

void lcd_draw_line(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1,
		uint16_t color)
{
	/*
	int slope;
	int dx, incE, incNE, x, y;
	int dy, d;


	if(x0 > x1)
	{
		lcd_draw_line(x1, y1, x0, y0, color);
		return;
	}


	dx = x1 - x0;
	dy = y1 - y0;

	if(dy < 0)
	{
		slope = -1;
		dy = -dy;
	}

	else
		slope = 1;

	incE = 2 * dy;
	incNE = 2 * dy - 2 * dx;
	d = 2 * dy - dx;
	y = y0;

	for(x=x0; x<=x1; x++)
	{
		lcd_set_pixel(x, y, color);

		if(d <= 0)
			d += incE;

		else
		{
			d += incNE;
			y += slope;
		}
	}
	*/
	int x,y,dx,dy,dx1,dy1,px,py,xe,ye,i;

	dx=x1-x0;
	dy=y1-y0;

	dx1=fabs(dx);
	dy1=fabs(dy);
	px=2*dy1-dx1;
	py=2*dx1-dy1;

	if(dy1<=dx1)
	{
		if(dx>=0)
		{
			x=x0;
			y=y0;
			xe=x1;
		}
		else
		{
			x=x1;
			y=y1;
			xe=x0;
		}

		lcd_set_pixel(x, y, color);
		for(i=0;x<xe;i++)
		{
			x=x+1;
			if(px<0)
				px=px+2*dy1;

			else
			{
				if((dx<0 && dy<0) || (dx>0 && dy>0))
					y=y+1;
				else
					y=y-1;
				px=px+2*(dy1-dx1);
			}
			lcd_set_pixel(x, y, color);
		}
	}

	else
	{
		if(dy>=0)
		{
			x=x0;
			y=y0;
			ye=y1;
		}
		else
		{
			x=x1;
			y=y1;
			ye=y0;
		}

		lcd_set_pixel(x, y, color);

		for(i=0;y<ye;i++)
		{
			y=y+1;
			if(py<=0)
				py=py+2*dx1;
			else
			{
				if((dx<0 && dy<0) || (dx>0 && dy>0))
				{
				 x=x+1;
				}
				else
				{
				 x=x-1;
				}
				py=py+2*(dx1-dy1);
			}
			lcd_set_pixel(x, y, color);
		}
	}
}

void lcd_draw_rect(uint16_t x, uint16_t y, uint16_t width, uint16_t height,
		uint16_t color)
{
	lcd_draw_line(x, y, x+width, y, color);
	lcd_draw_line(x, y, x, y+height, color);

	lcd_draw_line(x+width, y, x+width, y+height, color);
	lcd_draw_line(x, y+height, x+width, y+height, color);
}

void lcd_draw_circle(uint16_t x, uint16_t y, uint16_t rad, uint16_t color)
{
	int f = 1-rad;
	int ddf_x = 1;
	int ddf_y = -2*rad;
	int x_pos = 0;
	int y_pos = rad;

	lcd_set_pixel(x, y+rad, color);
	lcd_set_pixel(x, y-rad, color);
	lcd_set_pixel(x-rad, y, color);
	lcd_set_pixel(x+rad, y, color);

	while(x_pos<y_pos)
	{
		if(f >= 0)
		{
			y_pos--;
			ddf_y += 2;
			f += ddf_y;
		}

		x_pos++;
		ddf_x += 2;
		f += ddf_x;

		lcd_set_pixel(x+x_pos, y+y_pos, color);
		lcd_set_pixel(x-x_pos, y+y_pos, color);
		lcd_set_pixel(x+x_pos, y-y_pos, color);
		lcd_set_pixel(x-x_pos, y-y_pos, color);
		lcd_set_pixel(x+y_pos, y+x_pos, color);
		lcd_set_pixel(x-y_pos, y+x_pos, color);
		lcd_set_pixel(x+y_pos, y-x_pos, color);
		lcd_set_pixel(x-y_pos, y-x_pos, color);

	}
}

void lcd_draw_fill_circle(uint16_t x, uint16_t y, uint16_t rad, uint16_t color)
{
    int16_t x_pos, y_pos;
    x_pos = 0;
    y_pos = rad;

    int16_t esp = 3 - (rad << 1 );

    int16_t s_count_y;

	while(x_pos <= y_pos )
	{
		for(s_count_y = x_pos; s_count_y <= y_pos; s_count_y ++ )
		{
			lcd_set_pixel(x + x_pos, y + s_count_y, color);
			lcd_set_pixel(x - x_pos, y + s_count_y, color);
			lcd_set_pixel(x - s_count_y, y + x_pos, color);
			lcd_set_pixel(x - s_count_y, y - x_pos, color);
			lcd_set_pixel(x - x_pos, y - s_count_y, color);
			lcd_set_pixel(x + x_pos, y - s_count_y, color);
			lcd_set_pixel(x + s_count_y, y - x_pos, color);
			lcd_set_pixel(x + s_count_y, y + x_pos, color);
		}
		if(esp < 0 )
			esp += 4 * x_pos + 6;
		else
		{
			esp += 10 + 4 * (x_pos - y_pos );
			y_pos --;
		}
		x_pos ++;
	}
}

void lcd_draw_batery_widget(uint16_t x, uint16_t y, uint8_t level)
{
	uint16_t color;

	if(level<30)
		color = RED;
	else if(level< 60)
		color = YELLOW;
	else
		color = GREEN;

	lcd_draw_rect(x, y, 21, 12, WHITE);
	level = level*2/10;
	x++;
	y++;
	lcd_set_window_color(x, y, x+level, y+10, color);
	lcd_set_window_color(x+level, y, x+19 , y+10, BLACK);
	lcd_set_window_color(x+20, y+3, x+23, y+7, WHITE);

}

void lcd_draw_wifi_signal_widget(uint16_t x,uint16_t y, int8_t level)
{
	uint16_t color;

	int f;
	int ddf_x;
	int ddf_y;
	int x_pos;
	int y_pos;

	lcd_set_pixel(x, y, WHITE);

	int rad = 4;//primer radio

	while(rad<=12)//radio maximo
	{
		f = 1-rad;
		ddf_x = 1;
		ddf_y = -2*rad;
		x_pos = 0;
		y_pos = rad;

		if((level+=40)<0)
			color = WHITE;
		else
			color = 0x8410;

		lcd_set_pixel(x, y-rad, color);
		lcd_set_pixel(x-rad, y, color);

		while(x_pos<y_pos)
		{
			if(f >= 0)
			{
				y_pos--;
				ddf_y += 2;
				f += ddf_y;
			}

			x_pos++;
			ddf_x += 2;
			f += ddf_x;

			lcd_set_pixel(x-x_pos, y-y_pos, color);
			lcd_set_pixel(x-y_pos, y-x_pos, color);
		}

		rad+=4;//incremento del radio

	}
}


void lcd_draw_circular_load_widget(uint16_t x, uint16_t y, uint16_t rad,
		uint8_t percent, uint16_t color, uint16_t background_color)
{
    float theta = 0;
    int x_pos = rad;
    int y_pos = 0;
    float xd;
    float yd;
    float increment = (float)3*M_PI/(float)180;

    for(uint8_t i=0; i<120; i++)
    {
    	if(i<percent)
    		lcd_set_pixel(x + x_pos, y + y_pos, color);
    	else
    		lcd_set_pixel(x + x_pos, y + y_pos, background_color);

        theta = theta + increment;
        xd = rad * cos(theta);
        x_pos = round(xd);
        yd = rad * sin(theta);
        y_pos = yd;
    }
}

void lcd_draw_rect_load_widget(uint16_t x, uint16_t y, uint8_t percent,
		uint16_t color, uint16_t left_color, uint16_t right_color)
{
	lcd_draw_rect(x, y, 102, 21, color);

	x++;
	y++;

	char buffer[5];
	lcd_set_window_color(x, y, x+percent, y+19, left_color);
	lcd_set_window_color(x+percent, y, x+100, y+19, right_color);

	sprintf(buffer, "%d%%", percent);
	lcd_draw_string(buffer, (x+50)-((strlen(buffer)*7)/2), y+4, color, font12);

}

uint16_t convert_from_rgb_8(uint8_t r, uint8_t g, uint8_t b)
{
	uint8_t r_5 = r>>3;
	uint8_t g_6 = g>>2;
	uint8_t b_5 = b>>3;

	return (r_5<<11) | (g_6<<5) | (b_5);
}

void lcd_draw_message_widget(uint16_t x, uint16_t y, uint8_t n_messages)
{
	lcd_set_window_color(x, y, x+16, y+10, WHITE);

	lcd_draw_line(x, y, x+(16/2), y+(10/2), BLACK);
	lcd_draw_line(x+15, y, x+(16/2), y+(10/2), BLACK);

	lcd_draw_line(x, y+9, x+(16/2)-2, y+(10/2)-2, BLACK);
	lcd_draw_line(x+15, y+9, x+(16/2)+2, y+(10/2)-2, BLACK);

	if(n_messages)
		lcd_draw_fill_circle(x+15, y+2, 2, RED);

}



