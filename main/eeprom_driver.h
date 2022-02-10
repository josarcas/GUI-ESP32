/*
 * eeprom_driver.h
 *
 *  Created on: 18 dic. 2021
 *      Author: JoseCarlos
 */

#ifndef MAIN_EEPROM_DRIVER_H_
#define MAIN_EEPROM_DRIVER_H_

#include "stdint.h"
#include "stdbool.h"
#include "driver/i2c.h"

#define I2C_PORT			1
#define I2C_BAUD			400000
#define I2C_EEPROM_DIR		0x50

#ifndef I2C_BUS_PORT
#define I2C_SDA_PIN			32
#define I2C_SCL_PIN			33
#endif
#define WP_PIN				25


esp_err_t i2c_init_device();
bool init_eeprom();
void look_write_eeprom(bool look);
esp_err_t write_byte_eeprom(uint16_t dir, uint8_t data);
esp_err_t write_page_eeprom(uint8_t dir, uint8_t data[32]);
uint8_t read_byte_eeprom(uint16_t dir);
uint8_t read_current_byte();
void sequential_read_eeprom(uint16_t dir, uint8_t *data, uint16_t size);



#endif /* MAIN_EEPROM_DRIVER_H_ */
