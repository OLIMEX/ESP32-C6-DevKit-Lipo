#include "uart.h"

static void console_uart_init(void)
{
    esp_vfs_dev_uart_use_driver(CONSOLE_UART);

    uart_driver_install(CONSOLE_UART, 256, 0, 0, NULL, 0);
}

static inline bool is_valid_key(int c)
{
    return (c == '\r' || c == '\n' || c == '\t' || c == 0x1B || c == 0x08 ||
            (c >= 32 && c <= 126));
}

static void drain_uart_input(void)
{
    uint8_t ch;
    while (uart_read_bytes(CONSOLE_UART, &ch, 1, 0) > 0) { /* drop */ }
}

char wait_for_key(void)
{
    drain_uart_input();
    vTaskDelay(pdMS_TO_TICKS(10));
    drain_uart_input();

    uint8_t ch;
    for (;;) {
        int n = uart_read_bytes(CONSOLE_UART, &ch, 1, portMAX_DELAY);
        if (n == 1) {
            if (!is_valid_key(ch)) {
                continue;
            }
            if (ch == '\r') ch = '\n';
            return (char)ch;
        }
        vTaskDelay(pdMS_TO_TICKS(1));
    }
}
