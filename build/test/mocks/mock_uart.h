#ifndef mock_uart_H
#define mock_uart_H

#include "fff.h"
#include "fff_unity_helper.h"
#include "uart.h"


DECLARE_FAKE_VALUE_FUNC5(esp_err_t, uart_set_pin, uart_port_t, int, int, int, int);
DECLARE_FAKE_VALUE_FUNC2(esp_err_t, uart_set_rts, uart_port_t, int);
DECLARE_FAKE_VALUE_FUNC2(esp_err_t, uart_set_dtr, uart_port_t, int);
DECLARE_FAKE_VALUE_FUNC2(esp_err_t, uart_set_tx_idle_num, uart_port_t, uint16_t);
DECLARE_FAKE_VALUE_FUNC2(esp_err_t, uart_param_config, uart_port_t, const const uart_config_t*);
DECLARE_FAKE_VALUE_FUNC6(esp_err_t, uart_driver_install, uart_port_t, int, int, int, QueueHandle_t*, int);
DECLARE_FAKE_VALUE_FUNC3(int, uart_write_bytes, uart_port_t, const const char*, size_t);
DECLARE_FAKE_VALUE_FUNC4(int, uart_read_bytes, uart_port_t, uint8_t*, uint32_t, uint32_t);
DECLARE_FAKE_VALUE_FUNC2(esp_err_t, uart_set_mode, uart_port_t, uart_mode_t);

void mock_uart_Init(void);
void mock_uart_Verify(void);
void mock_uart_Destroy(void);

#endif // mock_uart_H
