// Copyright 2015-2019 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

//#pragma once

#ifdef __cplusplus
extern "C" {
#endif

//#include "soc/uart_periph.h"
//#include "soc/uart_caps.h"
#include "esp_err.h"
#include "esp_intr_alloc.h"
//#include "driver/periph_ctrl.h"
//#include "freertos/FreeRTOS.h"
//#include "freertos/semphr.h"
//#include "freertos/xtensa_api.h"
//#include "freertos/task.h"
//#include "freertos/queue.h"
//#include "freertos/ringbuf.h"
#include <esp_types.h>


#define UART_FIFO_LEN           (128)        /*!< Length of the hardware FIFO buffers */
#define UART_INTR_MASK          0x1ff        /*!< Mask of all UART interrupts */
#define UART_LINE_INV_MASK      (0x3f << 19) /*!< TBD */
#define UART_BITRATE_MAX        5000000      /*!< Max bit rate supported by UART */
#define UART_PIN_NO_CHANGE      (-1)         /*!< Constant for uart_set_pin function which indicates that UART pin should not be changed */

#define UART_INVERSE_DISABLE  (0x0)            /*!< Disable UART signal inverse*/
#define UART_INVERSE_RXD   (UART_RXD_INV_M)    /*!< UART RXD input inverse*/
#define UART_INVERSE_CTS   (UART_CTS_INV_M)    /*!< UART CTS input inverse*/
#define UART_INVERSE_TXD   (UART_TXD_INV_M)    /*!< UART TXD output inverse*/
#define UART_INVERSE_RTS   (UART_RTS_INV_M)    /*!< UART RTS output inverse*/

typedef void * QueueHandle_t;
#define  TickType_t uint32_t;
#define portTICK_PERIOD_MS	1000

/**
 * @brief UART mode selection
 */
typedef enum {
    UART_MODE_UART = 0x00,                      /*!< mode: regular UART mode*/
    UART_MODE_RS485_HALF_DUPLEX = 0x01,         /*!< mode: half duplex RS485 UART mode control by RTS pin */
    UART_MODE_IRDA = 0x02,                      /*!< mode: IRDA  UART mode*/
    UART_MODE_RS485_COLLISION_DETECT = 0x03,    /*!< mode: RS485 collision detection UART mode (used for test purposes)*/
    UART_MODE_RS485_APP_CTRL = 0x04,            /*!< mode: application control RS485 UART mode (used for test purposes)*/
} uart_mode_t;

/**
 * @brief UART word length constants
 */
typedef enum {
    UART_DATA_5_BITS = 0x0,    /*!< word length: 5bits*/
    UART_DATA_6_BITS = 0x1,    /*!< word length: 6bits*/
    UART_DATA_7_BITS = 0x2,    /*!< word length: 7bits*/
    UART_DATA_8_BITS = 0x3,    /*!< word length: 8bits*/
    UART_DATA_BITS_MAX = 0x4,
} uart_word_length_t;

/**
 * @brief UART stop bits number
 */
typedef enum {
    UART_STOP_BITS_1   = 0x1,  /*!< stop bit: 1bit*/
    UART_STOP_BITS_1_5 = 0x2,  /*!< stop bit: 1.5bits*/
    UART_STOP_BITS_2   = 0x3,  /*!< stop bit: 2bits*/
    UART_STOP_BITS_MAX = 0x4,
} uart_stop_bits_t;

/**
 * @brief UART peripheral number
 */
typedef enum {
    UART_NUM_0 = 0x0,  /*!< UART base address 0x3ff40000*/
    UART_NUM_1 = 0x1,  /*!< UART base address 0x3ff50000*/
    UART_NUM_2 = 0x2,  /*!< UART base address 0x3ff6e000*/
    UART_NUM_MAX,
} uart_port_t;

/**
 * @brief UART parity constants
 */
typedef enum {
    UART_PARITY_DISABLE = 0x0,  /*!< Disable UART parity*/
    UART_PARITY_EVEN = 0x2,     /*!< Enable UART even parity*/
    UART_PARITY_ODD  = 0x3      /*!< Enable UART odd parity*/
} uart_parity_t;

/**
 * @brief UART hardware flow control modes
 */
typedef enum {
    UART_HW_FLOWCTRL_DISABLE = 0x0,   /*!< disable hardware flow control*/
    UART_HW_FLOWCTRL_RTS     = 0x1,   /*!< enable RX hardware flow control (rts)*/
    UART_HW_FLOWCTRL_CTS     = 0x2,   /*!< enable TX hardware flow control (cts)*/
    UART_HW_FLOWCTRL_CTS_RTS = 0x3,   /*!< enable hardware flow control*/
    UART_HW_FLOWCTRL_MAX     = 0x4,
} uart_hw_flowcontrol_t;

/**
 * @brief UART configuration parameters for uart_param_config function
 */
typedef struct {
    int baud_rate;                      /*!< UART baud rate*/
    uart_word_length_t data_bits;       /*!< UART byte size*/
    uart_parity_t parity;               /*!< UART parity mode*/
    uart_stop_bits_t stop_bits;         /*!< UART stop bits*/
    uart_hw_flowcontrol_t flow_ctrl;    /*!< UART HW flow control mode (cts/rts)*/
    uint8_t rx_flow_ctrl_thresh;        /*!< UART HW RTS threshold*/
    bool use_ref_tick;                  /*!< Set to true if UART should be clocked from REF_TICK */
} uart_config_t;

/**
 * @brief UART interrupt configuration parameters for uart_intr_config function
 */
typedef struct {
    uint32_t intr_enable_mask;          /*!< UART interrupt enable mask, choose from UART_XXXX_INT_ENA_M under UART_INT_ENA_REG(i), connect with bit-or operator*/
    uint8_t  rx_timeout_thresh;         /*!< UART timeout interrupt threshold (unit: time of sending one byte)*/
    uint8_t  txfifo_empty_intr_thresh;  /*!< UART TX empty interrupt threshold.*/
    uint8_t  rxfifo_full_thresh;        /*!< UART RX full interrupt threshold.*/
} uart_intr_config_t;

/**
 * @brief UART event types used in the ring buffer
 */
typedef enum {
    UART_DATA,              /*!< UART data event*/
    UART_BREAK,             /*!< UART break event*/
    UART_BUFFER_FULL,       /*!< UART RX buffer full event*/
    UART_FIFO_OVF,          /*!< UART FIFO overflow event*/
    UART_FRAME_ERR,         /*!< UART RX frame error event*/
    UART_PARITY_ERR,        /*!< UART RX parity event*/
    UART_DATA_BREAK,        /*!< UART TX data and break event*/
    UART_PATTERN_DET,       /*!< UART pattern detected */
    UART_EVENT_MAX,         /*!< UART event max index*/
} uart_event_type_t;

/**
 * @brief Event structure used in UART event queue
 */
typedef struct {
    uart_event_type_t type; /*!< UART event type */
    size_t size;            /*!< UART data size for UART_DATA event*/
} uart_event_t;







/**
 * @brief Set UART pin number
 *
 * @note Internal signal can be output to multiple GPIO pads.
 *       Only one GPIO pad can connect with input signal.
 *
 * @note Instead of GPIO number a macro 'UART_PIN_NO_CHANGE' may be provided
         to keep the currently allocated pin.
 *
 * @param uart_num   UART_NUM_0, UART_NUM_1 or UART_NUM_2
 * @param tx_io_num  UART TX pin GPIO number.
 * @param rx_io_num  UART RX pin GPIO number.
 * @param rts_io_num UART RTS pin GPIO number.
 * @param cts_io_num UART CTS pin GPIO number.
 *
 * @return
 *     - ESP_OK   Success
 *     - ESP_FAIL Parameter error
 */
esp_err_t uart_set_pin(uart_port_t uart_num, int tx_io_num, int rx_io_num, int rts_io_num, int cts_io_num);

/**
 * @brief Manually set the UART RTS pin level.
 * @note  UART must be configured with hardware flow control disabled.
 *
 * @param uart_num UART_NUM_0, UART_NUM_1 or UART_NUM_2
 * @param level    1: RTS output low (active); 0: RTS output high (block)
 *
 * @return
 *     - ESP_OK   Success
 *     - ESP_FAIL Parameter error
 */
esp_err_t uart_set_rts(uart_port_t uart_num, int level);

/**
 * @brief Manually set the UART DTR pin level.
 *
 * @param uart_num UART_NUM_0, UART_NUM_1 or UART_NUM_2
 * @param level    1: DTR output low; 0: DTR output high
 *
 * @return
 *     - ESP_OK   Success
 *     - ESP_FAIL Parameter error
 */
esp_err_t uart_set_dtr(uart_port_t uart_num, int level);

/**
 * @brief Set UART idle interval after tx FIFO is empty
 *
 * @param uart_num UART_NUM_0, UART_NUM_1 or UART_NUM_2
 * @param idle_num idle interval after tx FIFO is empty(unit: the time it takes to send one bit
 *        under current baudrate)
 *
 * @return
 *     - ESP_OK   Success
 *     - ESP_FAIL Parameter error
 */
esp_err_t uart_set_tx_idle_num(uart_port_t uart_num, uint16_t idle_num);

/**
 * @brief Set UART configuration parameters.
 *
 * @param uart_num    UART_NUM_0, UART_NUM_1 or UART_NUM_2
 * @param uart_config UART parameter settings
 *
 * @return
 *     - ESP_OK   Success
 *     - ESP_FAIL Parameter error
 */
esp_err_t uart_param_config(uart_port_t uart_num, const uart_config_t *uart_config);


/**
 * @brief Install UART driver.
 *
 * UART ISR handler will be attached to the same CPU core that this function is running on.
 *
 * @note  Rx_buffer_size should be greater than UART_FIFO_LEN. Tx_buffer_size should be either zero or greater than UART_FIFO_LEN.
 *
 * @param uart_num UART_NUM_0, UART_NUM_1 or UART_NUM_2
 * @param rx_buffer_size UART RX ring buffer size.
 * @param tx_buffer_size UART TX ring buffer size.
 *        If set to zero, driver will not use TX buffer, TX function will block task until all data have been sent out.
 * @param queue_size UART event queue size/depth.
 * @param uart_queue UART event queue handle (out param). On success, a new queue handle is written here to provide
 *        access to UART events. If set to NULL, driver will not use an event queue.
 * @param intr_alloc_flags Flags used to allocate the interrupt. One or multiple (ORred)
 *        ESP_INTR_FLAG_* values. See esp_intr_alloc.h for more info. Do not set ESP_INTR_FLAG_IRAM here
 *        (the driver's ISR handler is not located in IRAM)
 *
 * @return
 *     - ESP_OK   Success
 *     - ESP_FAIL Parameter error
 */
esp_err_t uart_driver_install(uart_port_t uart_num, int rx_buffer_size, int tx_buffer_size, int queue_size, QueueHandle_t* uart_queue, int intr_alloc_flags);


/**
 * @brief Send data to the UART port from a given buffer and length,
 *
 * If the UART driver's parameter 'tx_buffer_size' is set to zero:
 * This function will not return until all the data have been sent out, or at least pushed into TX FIFO.
 *
 * Otherwise, if the 'tx_buffer_size' > 0, this function will return after copying all the data to tx ring buffer,
 * UART ISR will then move data from the ring buffer to TX FIFO gradually.
 *
 * @param uart_num UART_NUM_0, UART_NUM_1 or UART_NUM_2
 * @param src   data buffer address
 * @param size  data length to send
 *
 * @return
 *     - (-1) Parameter error
 *     - OTHERS (>=0) The number of bytes pushed to the TX FIFO
 */
int uart_write_bytes(uart_port_t uart_num, const char* src, size_t size);


/**
 * @brief UART read bytes from UART buffer
 *
 * @param uart_num UART_NUM_0, UART_NUM_1 or UART_NUM_2
 * @param buf     pointer to the buffer.
 * @param length  data length
 * @param ticks_to_wait sTimeout, count in RTOS ticks
 *
 * @return
 *     - (-1) Error
 *     - OTHERS (>=0) The number of bytes read from UART FIFO
 */
int uart_read_bytes(uart_port_t uart_num, uint8_t* buf, uint32_t length, uint32_t ticks_to_wait);



/**
 * @brief UART set communication mode
 * @note  This function must be executed after uart_driver_install(), when the driver object is initialized.
 * @param uart_num     Uart number to configure
 * @param mode UART    UART mode to set
 *
 * @return
 *     - ESP_OK Success
 *     - ESP_ERR_INVALID_ARG Parameter error
 */
esp_err_t uart_set_mode(uart_port_t uart_num, uart_mode_t mode);






#ifdef __cplusplus
}
#endif

