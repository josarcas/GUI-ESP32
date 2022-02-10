/*INCLUDES******************************************************************************/
#include "stdio.h"
#include "string.h"
#include "stdlib.h"
#include "stdarg.h"

#include "time.h"
#include "sys/time.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/timers.h"

#include "esp_system.h"
#include "esp_event.h"
#include "esp_event_loop.h"
#include "nvs_flash.h"

#include "esp_wifi.h"

#include "esp_tls.h"
#include "esp_http_client.h"

#include "sd_spi_device.h"
#include "lcd_driver.h"
#include "eeprom_driver.h"
#include "inet_protocols_driver.h"

/*DEFINES******************************************************************************/
//SYSTEM
#define DEVICE_NAME				"ESPIOS"
#define DEVICE_VERSION			1

//EEPROM
#ifndef MEMORY_MAP
#define MEMORY_MAP
#define MAX_SIZE_VAR			32
#define SYSTEM_OFFSET			0x00
#define WIFI_SSID_OFFSET		MAX_SIZE_VAR*1
#define WIFI_PASS_OFFSET		MAX_SIZE_VAR*2

#endif

//WIFI
#define MAX_SSID_LEN_WIFI			MAX_SIZE_VAR
#define MAX_PASS_LEN_WIFI			MAX_SIZE_VAR

/*GLOBAL VARIABLES********************************************************************/
//char *ssid_wifi= NULL;
//char *pass_wifi= NULL;

/*PROTOTYPES**************************************************************************/
esp_err_t connect_wifi_sta(char *ssid, char *pass);
esp_err_t init_wifi_module();
void debug(int state, const char *format, ...);
bool show_file(char *file_name, uint8_t type_file);

/*
float toRadians(float grad)
{
	return (float)grad*M_PI/(float) 180;
}
*/

/*FUNCTIONS**************************************************************************/
void app_main()
{
	//ESP_ERROR_CHECK(nvs_flash_init());
	//ESP_ERROR_CHECK(esp_netif_init());
	//ESP_ERROR_CHECK(esp_event_loop_create_default());

	/*
	ESP_ERROR_CHECK(i2c_init_device());
	init_eeprom();
	//printf("Valor leido en 0x00 es : %d\n", read_byte_eeprom(0x00));
	ssid_wifi = (char *) pvPortMalloc(MAX_SSID_LEN_WIFI*sizeof(char));
	pass_wifi = (char *) pvPortMalloc(MAX_PASS_LEN_WIFI*sizeof(char));
*/
	/*
	memset(ssid_wifi, '\0', MAX_SSID_LEN_WIFI);
	memset(pass_wifi, '\0', MAX_PASS_LEN_WIFI);

	strcpy(ssid_wifi, "ae38ac");
	strcpy(pass_wifi, "273665629");

	vTaskDelay(1000);
	printf("Iniciando escritura\n\r");
	write_page_eeprom(WIFI_SSID_OFFSET, (uint8_t *)ssid_wifi);
	write_page_eeprom(WIFI_PASS_OFFSET, (uint8_t *)pass_wifi);
	printf("Escritura correcta\n\r");
	*/
/*
	vTaskDelay(1000);
	sequential_read_eeprom(WIFI_SSID_OFFSET, (uint8_t *)ssid_wifi, MAX_SSID_LEN_WIFI);
	sequential_read_eeprom(WIFI_PASS_OFFSET, (uint8_t *)pass_wifi, MAX_PASS_LEN_WIFI);

	printf("SSID = %s\n\r", ssid_wifi);
	printf("PASS = %s\n\r", pass_wifi);
*/
	/*
	inet_status_t status;
	ESP_ERROR_CHECK(init_wifi_module());

	char *ssid = (char *) pvPortMalloc(MAX_SSID_LEN_WIFI);
	char *pass = (char *) pvPortMalloc(MAX_PASS_LEN_WIFI);



	ESP_ERROR_CHECK(conect_wifi_sta());



	vTaskDelay(1000);
	SSL *ssl=NULL;
	int sock;
	//struct sockaddr_in dest_addr;
	printf("------------INICIANDO PRUEBA--------\n\r");
	status = connect_smtp(&ssl, "smtp.gmail.com", 465, 10000);
	status = open_smtp(&ssl, "esp32user@gmail.com", "#include<jossarr.h>", 100);
	status = send_email_smtp(&ssl, "esp32user@gmail.com", "thijoseph@hotmail.com",
			"prueba lunes 27 dic", "enviados desde innet_protocols", 100);
	close_smtp(&ssl, 100);
	printf("------------FIN DE PRUEBA PRUEBA--------\n\r");

	while(1)
		vTaskDelay(100);
		*/
	spi_init_device(true);
	lcd_init();
	lcd_clear(BLACK);

	/*
	int r=50;
	int xc=100;
	int yc=100;
	double theta = toRadians(0);
	int x = r;
	int y = 0;

	uint16_t color = WHITE;

	while (theta <= 2*M_PI) {
		lcd_draw_line(xc, yc, x + xc, y + yc, color);
		theta=theta+toRadians(1);
		double xd = r * cos(theta);
		x = (int) round(xd);
		double yd = r * sin(theta);
		y = (int) yd;
		color = convert_from_rgb_8(x, x*y, y);
	}

	while(1) vTaskDelay(100);
	*/



	debug(1, "%s", DEVICE_NAME);
	debug(1, "VERSION : %d", DEVICE_VERSION);
	debug(nvs_flash_init(), "NVS FLASH");
	debug(esp_netif_init(), "NETIF");
	debug(esp_event_loop_create_default(), "EVENT LOOP");
	debug(i2c_init_device(), "I2C DEVICE");

	if(read_byte_eeprom(0x00) == 0x31)
		debug(0, "INTERNAL MEMORY OK");
	else
		debug(-1, "INTERNAL MEMORY CORRUPTED");

	debug(init_sd_spi_master(false), "SD CARD");

	debug(init_wifi_module(), "WIFI MODULE");

	char *ssid = (char *) pvPortMalloc(MAX_SSID_LEN_WIFI);
	char *pass = (char *) pvPortMalloc(MAX_PASS_LEN_WIFI);
	sequential_read_eeprom(WIFI_SSID_OFFSET, (uint8_t *)ssid, MAX_SSID_LEN_WIFI);
	sequential_read_eeprom(WIFI_PASS_OFFSET, (uint8_t *)pass, MAX_PASS_LEN_WIFI);

	debug(connect_wifi_sta(ssid, pass), "WIFI CONNECTION");

	vPortFree(ssid);
	vPortFree(pass);

	/*
	lcd_draw_batery_widget(30, 200, 80);
	lcd_draw_wifi_signal_widget(30, 280, -100);
	lcd_draw_rect_load_widget(0, 300, 80, WHITE, GREEN, BLACK);
	lcd_draw_rect(200, 200, 10, 10, WHITE);
	lcd_draw_line(220, 220, 220, 230, WHITE);
	lcd_draw_line(300, 300, 310, 310, WHITE);
	*/

	show_file("windows.bmp", FILE_TYPE_BMP);



	while(1)
		vTaskDelay(100);

}

esp_err_t connect_wifi_sta(char *ssid, char *pass)
{
	esp_err_t status;
	esp_netif_inherent_config_t esp_netif_config = ESP_NETIF_INHERENT_DEFAULT_WIFI_STA();
    esp_netif_config.route_prio = 128;
    esp_netif_t *netif = esp_netif_create_wifi(WIFI_IF_STA, &esp_netif_config);
	esp_wifi_set_default_wifi_sta_handlers();
    wifi_config_t wifi_config_wifi; /*= {
        .sta = {
            .ssid = ssid,
            .password = pass,
        },
    };*/
    memset(&wifi_config_wifi, 0, sizeof(wifi_config_t));

    strcpy((char *)wifi_config_wifi.sta.ssid, ssid);
    strcpy((char *)wifi_config_wifi.sta.password, pass);



    status = esp_wifi_set_mode(WIFI_MODE_STA);
    if(status != ESP_OK)
    	return status;

    status = esp_wifi_set_config(WIFI_IF_STA, &wifi_config_wifi);
    if(status != ESP_OK)
    	return status;

    status = esp_wifi_start();
    if(status != ESP_OK)
    	return status;

    return esp_wifi_connect();
}

esp_err_t init_wifi_module()
{
	wifi_init_config_t wifi_config = WIFI_INIT_CONFIG_DEFAULT();
	return esp_wifi_init(&wifi_config);

}

#define MAX_ARGS_SIZE			10
#define DEBUG_FONT				font16
void debug(int state, const char *format, ...)
{
	uint16_t color;

	switch (state){
	case -1:
		color = RED;
		break;
	case 0:
		color = GREEN;
		break;
	default:
		color = WHITE;
		break;
	}

	va_list args;
	char buffer[MAX_ARGS_SIZE];
	uint16_t len=0;
	int i;
	static uint16_t y=0;

	memset(buffer, 0, MAX_ARGS_SIZE);

	va_start(args, format);

	while(*format != '\0')
	{
		if(*format == '%')
		{
			format++;
			switch (*format)
			{
				case 'd':
				{
					i = va_arg(args, int);
					sprintf(buffer, "%d", i);
					lcd_draw_string(buffer, len, y, color, DEBUG_FONT);
					len = len + (strlen(buffer)*13);
				}
				break;

				case 'c':
				{
					i = va_arg(args, int);
					buffer[0] = i;
					buffer[1] = 0;
					lcd_draw_string(buffer, len, y, color, DEBUG_FONT);
					len+=13;
				}
				break;
				/*
				case 'f':
				{
					double d = va_arg(args, double);
					sprintf(buffer, "%f", d);
					strcat(wifi.buffer_tx, buffer);
					aux += strlen(buffer);
				}
				break;
				*/
				case 's':
				{
					char *ch = va_arg(args, char *);
					lcd_draw_string(ch, len, y, color, DEBUG_FONT);
					len = len+(strlen(ch)*13);
				}
				break;
				default:
				break;
			}
		}

		else
		{
			buffer[0] = *format;
			buffer[1] = 0;
			lcd_draw_string(buffer, len, y, color, DEBUG_FONT);
			len+=13;
		}

		format++;
	}
	va_end(args);

	y+=16;
}

FILE *operation_file = NULL;
bool show_file(char *file_name, uint8_t type_file)
{

	char file_dir[strlen(file_name)+strlen(STORAGE_MOUNT)];

	sprintf(file_dir, "%s/%s", STORAGE_MOUNT, file_name);

	if(operation_file != NULL)
		fclose(operation_file);

	switch(type_file)
	{
		case FILE_TYPE_TXT:
		{
			char buffer_file[21];
			uint8_t y_pos=10;

			operation_file = fopen(file_dir, "r");

			if(operation_file == NULL)
				return false;

			lcd_set_window_color(0, 10, LCD_WIDTH, LCD_HEIGHT, 0x0000);

			while(fgets(buffer_file, 21, operation_file) != NULL)
			{
				lcd_draw_string(buffer_file, 0, y_pos, 0xFFFF, font12);
				y_pos +=10;
			}

		}
		break;

		case FILE_TYPE_BMP:
		{
			uint8_t buffer_file[3];
			uint8_t size_type = sizeof(uint8_t);
			int i;
			operation_file = fopen(file_dir, "rb");

			if(operation_file == NULL)
				return false;

			for(i=0; i<18; i++)
			{
				if(fread(buffer_file, size_type, 3, operation_file) == 0)
					return false;
			}


			for(i= LCD_HEIGHT-1; i>=0; i--)
			{
				for(int j=0; j<LCD_WIDTH; j++)
				{
					fread(buffer_file, size_type,  3, operation_file);
					lcd_set_pixel(j, i, convert_from_rgb_8(buffer_file[0], buffer_file[1], buffer_file[2]));
				}
			}


		}
		break;

		default:
			return false;
	}

	fclose(operation_file);

	return true;
}
