/*
 * SPDX-FileCopyrightText: 2010-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: CC0-1.0
 */

#include <stdio.h>
#include <inttypes.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdbool.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_chip_info.h"
#include "esp_flash.h"
#include "esp_system.h"
#include "esp_err.h"
#include "driver/gpio.h"



void app_main(void)
{
	esp_rom_gpio_pad_select_gpio(GPIO_NUM_15);
	gpio_set_direction(GPIO_NUM_15, GPIO_MODE_OUTPUT);
	gpio_set_level(GPIO_NUM_15, 0);
	vTaskDelay(1000 / portTICK_PERIOD_MS);
	gpio_set_level(GPIO_NUM_15, 1);

	while (1) 
	{
		printf("Hello World!\n");
		gpio_set_level(GPIO_NUM_15, 0);
		vTaskDelay(1000 / portTICK_PERIOD_MS);
		gpio_set_level(GPIO_NUM_15, 1);
		vTaskDelay(1000 / portTICK_PERIOD_MS);
	}
	
}
