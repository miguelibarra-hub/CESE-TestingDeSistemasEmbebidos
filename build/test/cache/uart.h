#include "esp_types.h"
#include "esp_intr_alloc.h"
#include "esp_err.h"
















typedef void * QueueHandle_t;













typedef enum {

    UART_MODE_UART = 0x00,

    UART_MODE_RS485_HALF_DUPLEX = 0x01,

    UART_MODE_IRDA = 0x02,

    UART_MODE_RS485_COLLISION_DETECT = 0x03,

    UART_MODE_RS485_APP_CTRL = 0x04,

} uart_mode_t;









typedef enum {

    UART_DATA_5_BITS = 0x0,

    UART_DATA_6_BITS = 0x1,

    UART_DATA_7_BITS = 0x2,

    UART_DATA_8_BITS = 0x3,

    UART_DATA_BITS_MAX = 0x4,

} uart_word_length_t;









typedef enum {

    UART_STOP_BITS_1 = 0x1,

    UART_STOP_BITS_1_5 = 0x2,

    UART_STOP_BITS_2 = 0x3,

    UART_STOP_BITS_MAX = 0x4,

} uart_stop_bits_t;









typedef enum {

    UART_NUM_0 = 0x0,

    UART_NUM_1 = 0x1,

    UART_NUM_2 = 0x2,

    UART_NUM_MAX,

} uart_port_t;









typedef enum {

    UART_PARITY_DISABLE = 0x0,

    UART_PARITY_EVEN = 0x2,

    UART_PARITY_ODD = 0x3

} uart_parity_t;









typedef enum {

    UART_HW_FLOWCTRL_DISABLE = 0x0,

    UART_HW_FLOWCTRL_RTS = 0x1,

    UART_HW_FLOWCTRL_CTS = 0x2,

    UART_HW_FLOWCTRL_CTS_RTS = 0x3,

    UART_HW_FLOWCTRL_MAX = 0x4,

} uart_hw_flowcontrol_t;









typedef struct {

    int baud_rate;

    uart_word_length_t data_bits;

    uart_parity_t parity;

    uart_stop_bits_t stop_bits;

    uart_hw_flowcontrol_t flow_ctrl;

    uint8_t rx_flow_ctrl_thresh;

    

   _Bool 

        use_ref_tick;

} uart_config_t;









typedef struct {

    uint32_t intr_enable_mask;

    uint8_t rx_timeout_thresh;

    uint8_t txfifo_empty_intr_thresh;

    uint8_t rxfifo_full_thresh;

} uart_intr_config_t;









typedef enum {

    UART_DATA,

    UART_BREAK,

    UART_BUFFER_FULL,

    UART_FIFO_OVF,

    UART_FRAME_ERR,

    UART_PARITY_ERR,

    UART_DATA_BREAK,

    UART_PATTERN_DET,

    UART_EVENT_MAX,

} uart_event_type_t;









typedef struct {

    uart_event_type_t type;

    size_t size;

} uart_event_t;

esp_err_t uart_set_pin(uart_port_t uart_num, int tx_io_num, int rx_io_num, int rts_io_num, int cts_io_num);

esp_err_t uart_set_rts(uart_port_t uart_num, int level);

esp_err_t uart_set_dtr(uart_port_t uart_num, int level);

esp_err_t uart_set_tx_idle_num(uart_port_t uart_num, uint16_t idle_num);

esp_err_t uart_param_config(uart_port_t uart_num, const uart_config_t *uart_config);

esp_err_t uart_driver_install(uart_port_t uart_num, int rx_buffer_size, int tx_buffer_size, int queue_size, QueueHandle_t* uart_queue, int intr_alloc_flags);

int uart_write_bytes(uart_port_t uart_num, const char* src, size_t size);

int uart_read_bytes(uart_port_t uart_num, uint8_t* buf, uint32_t length, uint32_t ticks_to_wait);

esp_err_t uart_set_mode(uart_port_t uart_num, uart_mode_t mode);
