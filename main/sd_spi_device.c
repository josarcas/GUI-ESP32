/*
 * sd_spi_device.c
 *
 *  Created on: 17 dic. 2021
 *      Author: JoseCarlos
 */

/*INCLUDES******************************************************************************/
#include "sd_spi_device.h"
#include <sys/unistd.h>
#include <sys/stat.h>
#include "esp_vfs_fat.h"
#include "driver/sdspi_host.h"
#include "driver/spi_common.h"
#include "sdmmc_cmd.h"
#include "driver/sdmmc_host.h"
#include <sys/dirent.h>
#include "esp_vfs.h"
#include "esp_spiffs.h"
#include "driver/spi_master.h"


/*	@brief	Initializate SPI bus and SD cofiguration.
 *
 */
int init_sd_spi_master(bool init_spi)
{
	   esp_vfs_fat_sdmmc_mount_config_t mount_config = {
			.format_if_mount_failed = false,
			.max_files = 5,
			.allocation_unit_size = 16 * 1024
		};
		sdmmc_card_t* card;
		const char mount_point[] = STORAGE_MOUNT;

		sdmmc_host_t host = SDSPI_HOST_DEFAULT();
		host.max_freq_khz = 20000;
		spi_bus_config_t bus_cfg = {
			.mosi_io_num = SPI_MOSI_PIN,
			.miso_io_num = SPI_MISO_PIN,
			.sclk_io_num = SPI_CLK_PIN,
			.quadwp_io_num = -1,
			.quadhd_io_num = -1,
		};

		if(init_spi)
		{
			esp_err_t status =spi_bus_initialize(host.slot, &bus_cfg, SPI_DMA_CHAN);
			if(status != ESP_OK) return status;
		}

		sdspi_device_config_t slot_config = SDSPI_DEVICE_CONFIG_DEFAULT();
		slot_config.gpio_cs = PIN_NUM_CS_SD;
		slot_config.host_id = host.slot;

		return esp_vfs_fat_sdspi_mount(mount_point, &host, &slot_config, &mount_config, &card);
}


/*	@brief	Scan directory of SD storage
 *
 * 	@param	dir_name		Name of directory for scan.
 * 	@param	file_list		Linked list for write name of files and type.
 *
 */
uint16_t scan_directory(char * dir_name, file_linked_list_t ** file_list)
{
	file_linked_list_t *aux = (*file_list);
	file_linked_list_t *aux_prev;

	static struct dirent *dir;
	uint16_t size_dir=0;
	char buffer[3];
	uint8_t i;

	void  *d = opendir(dir_name);

	while(aux != NULL)
	{
		(*file_list) = aux->next;
		vPortFree(aux);
		aux = (*file_list);
	}

	if(d)
	{
		while ((dir = readdir(d)) != NULL)
		{

			if((*file_list) == NULL)
			{
				(*file_list) = aux = (file_linked_list_t *)pvPortMalloc(sizeof(file_linked_list_t));
				aux_prev = NULL;
			}
			else
			{
				aux->next = (file_linked_list_t *)pvPortMalloc(sizeof(file_linked_list_t));
				aux_prev= aux;
				aux = aux->next;
			}
			//strcpy(aux->name, dir->d_name);
			aux->next = NULL;
			aux->prev = aux_prev;
			size_dir++;

			for(i=0; i<MAX_LENGH_NAME_FILE; i++)
			{
				if(aux->name[i] == '.')
					break;
			}

			if(i<MAX_LENGH_NAME_FILE)
			{
				buffer[0] = aux->name[i+1];
				buffer[1] = aux->name[i+2];
				buffer[2] = aux->name[i+3];

				if(strcmp("txt", buffer) == 0)
					aux->type = FILE_TYPE_TXT;
				else if(strcmp("bmp", buffer) == 0)
					aux->type = FILE_TYPE_BMP;
				else if(strcmp("wav", buffer) == 0)
					aux->type = FILE_TYPE_WAV;
				else
					aux->type = FILE_TYPE_UNKOWN;
			}
			else
			{
				strcat(aux->name, "/");
				aux->type = FILE_TYPE_FOLDER;
			}

		}

	closedir(d);
	}

	aux->next = (*file_list);
	aux_prev = aux;
	aux = aux->next;
	aux->prev = aux_prev;

	return size_dir;
}


/*	@brief	Free memory used by list file
 *
 * 	@param	file_list		Pointer to linked list for delete.
 * 	@param		 size		Size of list.
 *
 * 	@return	State of operation.
 *
 */
void delete_file_list(file_linked_list_t **file_list, uint8_t size)
{

	file_linked_list_t *aux = (*file_list);
	for(uint8_t i=0; i<size; i++)
	{
		(*file_list) = aux->next;
		vPortFree(aux);
		aux = (*file_list);
	}
}


