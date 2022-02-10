/*
 * eeprom_driver.c
 *
 *  Created on: 18 dic. 2021
 *      Author: JoseCarlos
 */

/*INCLUDES*******************************************************************************/
#include "eeprom_driver.h"

/*PROTOTYPES****************************************************************************/
static esp_err_t i2c_write_data(uint8_t *data, uint16_t len);
static esp_err_t i2c_read_data(uint8_t *data, uint16_t len);

/*FUNCTIONS*****************************************************************************/
static esp_err_t i2c_write_data(uint8_t *data, uint16_t len)
{
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();

    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, I2C_EEPROM_DIR<<1, 1);
    i2c_master_write(cmd, data, len, 1);
    i2c_master_stop(cmd);
    esp_err_t ret = i2c_master_cmd_begin(I2C_PORT, cmd, 1000);
    i2c_cmd_link_delete(cmd);
    look_write_eeprom(true);
    return ret;
}

static esp_err_t i2c_read_data(uint8_t *data, uint16_t len)
{

    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (I2C_EEPROM_DIR << 1) | 0x01, 1);
    if (len > 1) {
        i2c_master_read(cmd, data, len - 1, 0);
    }
    i2c_master_read_byte(cmd, data + len - 1, 1);
    i2c_master_stop(cmd);
    esp_err_t ret = i2c_master_cmd_begin(I2C_PORT, cmd, 1000);
    i2c_cmd_link_delete(cmd);
    return ret;
}


esp_err_t i2c_init_device()
{
    int i2c_master_port = I2C_PORT;
    i2c_config_t conf = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = I2C_SDA_PIN,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_io_num = I2C_SCL_PIN,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master.clk_speed = I2C_BAUD
    };

    esp_err_t err = i2c_param_config(i2c_master_port, &conf);
    if (err != ESP_OK)
    {
        return err;
    }
    return i2c_driver_install(i2c_master_port, conf.mode, 0, 0, 0);
}

bool init_eeprom()
{
	gpio_set_direction(WP_PIN, GPIO_MODE_OUTPUT);
	look_write_eeprom(true);
	return true;
}

void look_write_eeprom(bool look)
{
	gpio_set_level(WP_PIN, look);
}

esp_err_t write_byte_eeprom(uint16_t dir, uint8_t data)
{
    uint8_t buffer[3] = {dir>>8, dir, data};
    look_write_eeprom(false);
    esp_err_t status = i2c_write_data(buffer, 3);
    look_write_eeprom(true);
    return status;
}

esp_err_t write_page_eeprom(uint8_t dir, uint8_t data[32])
{
    uint8_t buffer[34];
    buffer[0] = dir>>8;
    buffer[1] = dir;

    for(uint8_t i=0; i<32; i++)
    	buffer[i+2] = data[i];

    look_write_eeprom(false);
    esp_err_t status = i2c_write_data(buffer, 34);
    look_write_eeprom(true);
    return status;

}

uint8_t read_byte_eeprom(uint16_t dir)
{
	uint8_t buffer[2] ={dir>>8, dir};
	uint8_t rd_byte;
	i2c_write_data(buffer, 2);
	i2c_read_data(&rd_byte, 1);
	return rd_byte;
}

uint8_t read_current_byte()
{
	uint8_t rd_byte;
	i2c_read_data(&rd_byte, 1);
	return rd_byte;
}

void sequential_read_eeprom(uint16_t dir, uint8_t *data, uint16_t size)
{
	uint8_t buffer[2] ={dir>>8, dir};
	//uint8_t rd_byte;
	i2c_write_data(buffer, 2);
	i2c_read_data(data, size);
}


