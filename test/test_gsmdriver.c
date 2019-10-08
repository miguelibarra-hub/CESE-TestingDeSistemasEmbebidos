/*
*       TP-2 Testing en Sistemas Embebidos
*               Miguel Ibarra
*/
#include "string.h"
#include "unity.h"
#include "mock_uart.h"
#include "gsmdriver.h"

#define BUFFER_TEST_SIZE        128
#define TIMEOUT_FOR_REPLY       10
#define INIT_UART_SECUENCE_OK   0x0123

/* ======== Definicion de variables internas ===============================================*/
char DatosEscritos[BUFFER_TEST_SIZE];    /*  buffer para capturar los datos escritos a la UART*/
char DatosFromGSM[BUFFER_TEST_SIZE];     /*  buffer para simular la respuesta de la UART*/
int Len;                    /*  variable para indicar la longuitud de los datos que vienen desde la UART*/
int SecuenciaInit;

/*========== Definicion de funciones internas ==============================================*/


esp_err_t simula_uart_param_config(uart_port_t uart_num, const uart_config_t *uart_config)
{
    SecuenciaInit = (SecuenciaInit << 4) | 1; 
    return 0;
}

esp_err_t simula_uart_driver_install(uart_port_t uart_num, int rx_buffer_size, int tx_buffer_size, int queue_size, QueueHandle_t* uart_queue, int intr_alloc_flags)
{
    SecuenciaInit = (SecuenciaInit << 4) | 3;
    return 0;
}

esp_err_t simula_uart_set_pin(uart_port_t uart_num, int tx_io_num, int rx_io_num, int rts_io_num, int cts_io_num)
{
    SecuenciaInit = (SecuenciaInit << 4) | 2;
    return 0;
}
/**
*   simula_uart_write_bytes:
*       Funcion para capturar los datos escritos en la UART.
*   Parametros:
*       uart_num    Puerto uart  
*       src         puntero a los datos a transmitir
*       size        longuitud de los datos          
*/
int simula_uart_write_bytes(uart_port_t uart_num, const char* src, size_t size)
{
    memcpy(DatosEscritos,src, size);
    return 0;
}
/**
*   simula_uart_read_bytes:
*       Funcion para simular la lectura de datos desde UART.
*   Parametros:
*       uart_num    Puerto uart  
*       buf         puntero al buffer donde se guardaran los datos
*       lenght      longuitud del buffer          
*/
int simula_uart_read_bytes(uart_port_t uart_num, uint8_t* buf, uint32_t length, uint32_t ticks_to_wait)
{
    memcpy(buf,&DatosFromGSM, Len);
    return Len;   
}	
/*
*   clearBufferEscritos:
*       Funcion para inicializar a cero el buffer de captura de datos a la uart 
*/
void clearBufferEscritos(void)
{
    for(int i = 0; i < BUFFER_TEST_SIZE ; i++)
        DatosEscritos[i] = 0;            
}
/*
*   clearBufferGSM:
*       Funcion para inicializar a cero el buffer para simular los datos que llegan desde uart 
*/

void clearBufferGSM(void)
{
    for(int i = 0; i < BUFFER_TEST_SIZE ; i++)
        DatosFromGSM[i] = 0;
    Len = 0;        
}


void test_inicializacion_uart_gsm(void)
{
	uart_param_config_fake.custom_fake = simula_uart_param_config;
    uart_set_pin_fake.custom_fake = simula_uart_set_pin;
    uart_driver_install_fake.custom_fake = simula_uart_driver_install;
    SecuenciaInit = 0;    
    GSMDriverInit();
    /* Verificacion de la inicializacion del puerto en el orden correcto*/
    TEST_ASSERT_EQUAL_INT(INIT_UART_SECUENCE_OK, SecuenciaInit); 
}

void test_deteccion_y_sincronizacion_inicial_modulo_gsm(void)
{
    /*  Definicion de las funciones de prueba */
    uart_write_bytes_fake.custom_fake = simula_uart_write_bytes;
    uart_read_bytes_fake.custom_fake = simula_uart_read_bytes;
    /*  Verificacion del estado inicial */
    TEST_ASSERT_EQUAL_INT(GSM_IN_PROGRESS, GSMDriverStartProcess());
    /*  Verificacion de la cadena enviada a la uart*/ 
    TEST_ASSERT_EQUAL_STRING("AT\r",DatosEscritos);
    /*  Inicializacion del buffer que simula la respuesta desde uart*/
    sprintf((char*)&DatosFromGSM,"OK");    
    Len = 2; 
    /*  Simulamos que la funcion es llamada varias veces para que se cumpla el timeout*/   
    for(int i = 0; i < (TIMEOUT_FOR_REPLY-1); i++){
        GSMDriverStartProcess();
    }
    /*  Verificacion de la sincronizacion con el modulo*/
    TEST_ASSERT_EQUAL_INT(GSM_OK, GSMDriverStartProcess()); 
}

void test_configuracion_modulo_gsm(void)
{
    /*  Definicion de las funciones de prueba */
    uart_write_bytes_fake.custom_fake = simula_uart_write_bytes;
    uart_read_bytes_fake.custom_fake = simula_uart_read_bytes;
    /*  Inicializacion los buffer de prueba*/    
    clearBufferEscritos();
    clearBufferGSM();
    /*  Verificacion del estado inicial */
    TEST_ASSERT_EQUAL_INT(GSM_IN_PROGRESS, GSMDriverConfigureProcess());
    /*  Verificacion de la cadena enviada a la uart*/  
    TEST_ASSERT_EQUAL_STRING("ATE0\r",DatosEscritos);
    /*  Inicializacion del buffer que simula la respuesta desde uart*/
    sprintf((char*)&DatosFromGSM,"OK");    
    Len = 2;  
    /*  Simulamos que la funcion es llamada varias veces para que se cumpla el timeout*/  
    for(int i = 0; i < (TIMEOUT_FOR_REPLY-1); i++)
        GSMDriverConfigureProcess();
    /*  Verificacion del estado y lectura de la respuesta */
    TEST_ASSERT_EQUAL_INT(GSM_IN_PROGRESS, GSMDriverConfigureProcess());
    /*  Inicializacion de los buffer de prueba*/ 
    clearBufferEscritos();
    clearBufferGSM();
    /*  Verificacion del estado y envio de comando de configuracion*/
    TEST_ASSERT_EQUAL_INT(GSM_IN_PROGRESS, GSMDriverConfigureProcess());
     /*  Verificacion de la cadena enviada a la uart*/   
    TEST_ASSERT_EQUAL_STRING("AT+CREG?\r",DatosEscritos);
     /*  Inicializacion del buffer que simula la respuesta desde uart*/
    sprintf((char*)&DatosFromGSM,"+CREG: 1,1");    
    Len = 10;    
    /*  Simulamos que la funcion es llamada varias veces para que se cumpla el timeout*/  
    for(int i = 0; i < (TIMEOUT_FOR_REPLY-1); i++)
        GSMDriverConfigureProcess();
     /*  Verificacion de la configuracion con el modulo*/
    TEST_ASSERT_EQUAL_INT(GSM_OK, GSMDriverConfigureProcess());
}

void test_send_sms_message(void)
{
    char BytesEsperados[BUFFER_TEST_SIZE];
    char Escape = 0x1A;
    /*  Definicion de las funciones de prueba */
    uart_write_bytes_fake.custom_fake = simula_uart_write_bytes;
    uart_read_bytes_fake.custom_fake = simula_uart_read_bytes;
    /*  Inicializacion los buffer de prueba*/  
    clearBufferEscritos();
    clearBufferGSM();
    /*  Pasa un mensaje de prueba*/
    GSMDriverSetMessage("Test SMS");
    /*  Verificacion del estado inicial y envio de comando de inicio SMS*/
    TEST_ASSERT_EQUAL_INT(GSM_IN_PROGRESS, GSMDriverSendSMS());
    /*  Verificacion de la cadena enviada a la uart*/       
    TEST_ASSERT_EQUAL_STRING("AT+CMGF=1\r",DatosEscritos);
    /*  Inicializacion del buffer que simula la respuesta desde uart*/
    sprintf((char*)&DatosFromGSM,"OK");    
    Len = 2;    
    /*  Simulamos que la funcion es llamada varias veces para que se cumpla el timeout*/     
    for(int i = 0; i < (TIMEOUT_FOR_REPLY-1); i++)
        GSMDriverSendSMS();
    /*  Verificacion del estado y lectura de la respuesta*/
    TEST_ASSERT_EQUAL_INT(GSM_IN_PROGRESS, GSMDriverSendSMS());
    /*  Inicializacion los buffer de prueba*/  
    clearBufferEscritos();
    clearBufferGSM();
    /*  Verificacion del estado inicial y envio de comando de configuracion de Numero Telefonico*/
    TEST_ASSERT_EQUAL_INT(GSM_IN_PROGRESS, GSMDriverSendSMS()); 
    /*  Verificacion de la cadena enviada a la uart*/
    TEST_ASSERT_EQUAL_STRING("AT+CMGS=\"+543513024449\"\r",DatosEscritos);
    /*  Inicializacion del buffer que simula la respuesta desde uart*/
    sprintf((char*)&DatosFromGSM,">");    
    Len = 1;    
     /*  Simulamos que la funcion es llamada varias veces para que se cumpla el timeout*/     
    for(int i = 0; i < (TIMEOUT_FOR_REPLY-1); i++)
        GSMDriverSendSMS(); 
    /*  Verificacion del estado y lectura de la respuesta*/
    TEST_ASSERT_EQUAL_INT(GSM_IN_PROGRESS, GSMDriverSendSMS());
    /*  Inicializacion los buffer de prueba*/ 
    clearBufferEscritos();
    clearBufferGSM();
    /*  Verificacion del estado inicial y envio del SMS */    
    TEST_ASSERT_EQUAL_INT(GSM_IN_PROGRESS, GSMDriverSendSMS());
    /*  Inicializacion del buffer a comparar con lo que se espera recibir*/ 
    sprintf(&BytesEsperados[0],"Test SMS%x",Escape);
    /*  Verificacion del mensaje enviado*/
    TEST_ASSERT_EQUAL_INT8_ARRAY(BytesEsperados, DatosEscritos, 9);       
}

void test_send_sms_with_module_timeout(void)
{
    /*  Definicion de las funciones de prueba */
    uart_write_bytes_fake.custom_fake = simula_uart_write_bytes;
    uart_read_bytes_fake.custom_fake = simula_uart_read_bytes;
    /*  Inicializacion los buffer de prueba*/  
    clearBufferEscritos();
    clearBufferGSM();
    /*  Pasa un mensaje de prueba*/
    GSMDriverSetMessage("Test SMS");
    /*  Verificacion del estado inicial y envio de comando de inicio SMS*/
    TEST_ASSERT_EQUAL_INT(GSM_IN_PROGRESS, GSMDriverSendSMS());
    /*  Verificacion de la cadena enviada a la uart*/       
    TEST_ASSERT_EQUAL_STRING("AT+CMGF=1\r",DatosEscritos);
    /*  Inicializacion del buffer que simula que no se recibio nada*/
    sprintf((char*)&DatosFromGSM," ");    
    Len = 0;
     /*  Inicializacion los buffer de prueba*/  
    clearBufferEscritos();
    clearBufferGSM();    
    /*  Simulamos que la funcion es llamada varias veces para que se cumpla el timeout*/     
    for(int i = 0; i < TIMEOUT_FOR_REPLY; i++)
        GSMDriverSendSMS();
    /*  Verificacion del estado y retransmicion del comando*/
    TEST_ASSERT_EQUAL_INT(GSM_IN_PROGRESS, GSMDriverSendSMS());
    /*  Verificacion del re-envio del comando cuando hay timeout*/       
    TEST_ASSERT_EQUAL_STRING("AT+CMGF=1\r",DatosEscritos);    
}












