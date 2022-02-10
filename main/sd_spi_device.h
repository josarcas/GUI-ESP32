/*
 * sd_spi_device.h
 *
 *  Created on: 17 dic. 2021
 *      Author: JoseCarlos
 */

#ifndef MAIN_SD_SPI_DEVICE_H_
#define MAIN_SD_SPI_DEVICE_H_


/*INCLUDES*******************************************************************************/
#include "stdint.h"
#include "string.h"
#include "stdbool.h"

/*DEFINES********************************************************************************/
#define STORAGE_MOUNT	"/sd"
#define INIT_FILE_CONFIG_NAME "/sd/config.txt"
#define SPI_DMA_CHAN    2
#define MAX_LENGH_NAME_FILE 50

#ifndef	TYPE_FILE
#define FILE_TYPE_UNKOWN	0
#define FILE_TYPE_FOLDER	1
#define FILE_TYPE_TXT		10
#define FILE_TYPE_BMP		11
#define FILE_TYPE_WAV		12
#endif

#ifndef SPI_BUS_PORT
#define SPI_BUS_PORT
#define SPI_MOSI_PIN			23
#define SPI_MISO_PIN			19
#define SPI_CLK_PIN				18
#endif
#define PIN_NUM_CS_SD   5

/*TYPEDEF********************************************************************************/
struct file_linked_list{
#ifndef MAX_LENGH_NAME_FILE
#define MAX_LENGH_NAME_FILE 256
#endif
	char name[MAX_LENGH_NAME_FILE];
	uint8_t type;
	struct file_linked_list *next;
	struct file_linked_list *prev;
};
typedef struct file_linked_list file_linked_list_t;


/*PROTOTYPES*****************************************************************************/
int init_sd_spi_master(bool init_spi);
uint16_t scan_directory(char * dir_name, file_linked_list_t ** file_list);
void delete_file_list(file_linked_list_t **file_list, uint8_t size);


#endif /* MAIN_SD_SPI_DEVICE_H_ */
