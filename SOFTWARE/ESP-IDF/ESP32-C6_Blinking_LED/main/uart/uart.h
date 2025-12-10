#include <stdio.h>
#include <ctype.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/uart.h"
#include "esp_vfs_dev.h"
#include "esp_system.h"

#define CONSOLE_UART UART_NUM_0

static void console_uart_init(void);
static inline bool is_valid_key(int c);
char wait_for_key(void);

