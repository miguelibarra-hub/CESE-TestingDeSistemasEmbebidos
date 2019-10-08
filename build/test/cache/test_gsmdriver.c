#include "build/temp/_test_gsmdriver.c"
#include "gsmdriver.h"
#include "mock_uart.h"
#include "unity.h"




















char DatosEscritos[128];

char DatosFromGSM[128];

int Len;

int SecuenciaInit;









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

int simula_uart_write_bytes(uart_port_t uart_num, const char* src, size_t size)

{

    memcpy(DatosEscritos,src, size);

    return 0;

}

int simula_uart_read_bytes(uart_port_t uart_num, uint8_t* buf, uint32_t length, uint32_t ticks_to_wait)

{

    memcpy(buf,&DatosFromGSM, Len);

    return Len;

}









void clearBufferEscritos(void)

{

    for(int i = 0; i < 128 ; i++)

        DatosEscritos[i] = 0;

}











void clearBufferGSM(void)

{

    for(int i = 0; i < 128 ; i++)

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



    UnityAssertEqualNumber((UNITY_INT)((0x0123)), (UNITY_INT)((SecuenciaInit)), (

   ((void *)0)

   ), (UNITY_UINT)(96), UNITY_DISPLAY_STYLE_INT);

}



void test_deteccion_y_sincronizacion_inicial_modulo_gsm(void)

{



    uart_write_bytes_fake.custom_fake = simula_uart_write_bytes;

    uart_read_bytes_fake.custom_fake = simula_uart_read_bytes;



    UnityAssertEqualNumber((UNITY_INT)((GSM_IN_PROGRESS)), (UNITY_INT)((GSMDriverStartProcess())), (

   ((void *)0)

   ), (UNITY_UINT)(105), UNITY_DISPLAY_STYLE_INT);



    UnityAssertEqualString((const char*)(("AT\r")), (const char*)((DatosEscritos)), (

   ((void *)0)

   ), (UNITY_UINT)(107));



    sprintf((char*)&DatosFromGSM,"OK");

    Len = 2;



    for(int i = 0; i < (10 -1); i++){

        GSMDriverStartProcess();

    }



    UnityAssertEqualNumber((UNITY_INT)((GSM_OK)), (UNITY_INT)((GSMDriverStartProcess())), (

   ((void *)0)

   ), (UNITY_UINT)(116), UNITY_DISPLAY_STYLE_INT);

}



void test_configuracion_modulo_gsm(void)

{



    uart_write_bytes_fake.custom_fake = simula_uart_write_bytes;

    uart_read_bytes_fake.custom_fake = simula_uart_read_bytes;



    clearBufferEscritos();

    clearBufferGSM();



    UnityAssertEqualNumber((UNITY_INT)((GSM_IN_PROGRESS)), (UNITY_INT)((GSMDriverConfigureProcess())), (

   ((void *)0)

   ), (UNITY_UINT)(128), UNITY_DISPLAY_STYLE_INT);



    UnityAssertEqualString((const char*)(("ATE0\r")), (const char*)((DatosEscritos)), (

   ((void *)0)

   ), (UNITY_UINT)(130));



    sprintf((char*)&DatosFromGSM,"OK");

    Len = 2;



    for(int i = 0; i < (10 -1); i++)

        GSMDriverConfigureProcess();



    UnityAssertEqualNumber((UNITY_INT)((GSM_IN_PROGRESS)), (UNITY_INT)((GSMDriverConfigureProcess())), (

   ((void *)0)

   ), (UNITY_UINT)(138), UNITY_DISPLAY_STYLE_INT);



    clearBufferEscritos();

    clearBufferGSM();



    UnityAssertEqualNumber((UNITY_INT)((GSM_IN_PROGRESS)), (UNITY_INT)((GSMDriverConfigureProcess())), (

   ((void *)0)

   ), (UNITY_UINT)(143), UNITY_DISPLAY_STYLE_INT);



    UnityAssertEqualString((const char*)(("AT+CREG?\r")), (const char*)((DatosEscritos)), (

   ((void *)0)

   ), (UNITY_UINT)(145));



    sprintf((char*)&DatosFromGSM,"+CREG: 1,1");

    Len = 10;



    for(int i = 0; i < (10 -1); i++)

        GSMDriverConfigureProcess();



    UnityAssertEqualNumber((UNITY_INT)((GSM_OK)), (UNITY_INT)((GSMDriverConfigureProcess())), (

   ((void *)0)

   ), (UNITY_UINT)(153), UNITY_DISPLAY_STYLE_INT);

}



void test_send_sms_message(void)

{

    char BytesEsperados[128];

    char Escape = 0x1A;



    uart_write_bytes_fake.custom_fake = simula_uart_write_bytes;

    uart_read_bytes_fake.custom_fake = simula_uart_read_bytes;



    clearBufferEscritos();

    clearBufferGSM();



    GSMDriverSetMessage("Test SMS");



    UnityAssertEqualNumber((UNITY_INT)((GSM_IN_PROGRESS)), (UNITY_INT)((GSMDriverSendSMS())), (

   ((void *)0)

   ), (UNITY_UINT)(169), UNITY_DISPLAY_STYLE_INT);



    UnityAssertEqualString((const char*)(("AT+CMGF=1\r")), (const char*)((DatosEscritos)), (

   ((void *)0)

   ), (UNITY_UINT)(171));



    sprintf((char*)&DatosFromGSM,"OK");

    Len = 2;



    for(int i = 0; i < (10 -1); i++)

        GSMDriverSendSMS();



    UnityAssertEqualNumber((UNITY_INT)((GSM_IN_PROGRESS)), (UNITY_INT)((GSMDriverSendSMS())), (

   ((void *)0)

   ), (UNITY_UINT)(179), UNITY_DISPLAY_STYLE_INT);



    clearBufferEscritos();

    clearBufferGSM();



    UnityAssertEqualNumber((UNITY_INT)((GSM_IN_PROGRESS)), (UNITY_INT)((GSMDriverSendSMS())), (

   ((void *)0)

   ), (UNITY_UINT)(184), UNITY_DISPLAY_STYLE_INT);



    UnityAssertEqualString((const char*)(("AT+CMGS=\"+543513024449\"\r")), (const char*)((DatosEscritos)), (

   ((void *)0)

   ), (UNITY_UINT)(186));



    sprintf((char*)&DatosFromGSM,">");

    Len = 1;



    for(int i = 0; i < (10 -1); i++)

        GSMDriverSendSMS();



    UnityAssertEqualNumber((UNITY_INT)((GSM_IN_PROGRESS)), (UNITY_INT)((GSMDriverSendSMS())), (

   ((void *)0)

   ), (UNITY_UINT)(194), UNITY_DISPLAY_STYLE_INT);



    clearBufferEscritos();

    clearBufferGSM();



    UnityAssertEqualNumber((UNITY_INT)((GSM_IN_PROGRESS)), (UNITY_INT)((GSMDriverSendSMS())), (

   ((void *)0)

   ), (UNITY_UINT)(199), UNITY_DISPLAY_STYLE_INT);



    sprintf(&BytesEsperados[0],"Test SMS%x",Escape);



    UnityAssertEqualIntArray(( const void*)((BytesEsperados)), ( const void*)((DatosEscritos)), (UNITY_UINT32)((9)), (

   ((void *)0)

   ), (UNITY_UINT)(203), UNITY_DISPLAY_STYLE_INT8, UNITY_ARRAY_TO_ARRAY);

}



void test_send_sms_with_module_timeout(void)

{



    uart_write_bytes_fake.custom_fake = simula_uart_write_bytes;

    uart_read_bytes_fake.custom_fake = simula_uart_read_bytes;



    clearBufferEscritos();

    clearBufferGSM();



    GSMDriverSetMessage("Test SMS");



    UnityAssertEqualNumber((UNITY_INT)((GSM_IN_PROGRESS)), (UNITY_INT)((GSMDriverSendSMS())), (

   ((void *)0)

   ), (UNITY_UINT)(217), UNITY_DISPLAY_STYLE_INT);



    UnityAssertEqualString((const char*)(("AT+CMGF=1\r")), (const char*)((DatosEscritos)), (

   ((void *)0)

   ), (UNITY_UINT)(219));



    sprintf((char*)&DatosFromGSM," ");

    Len = 0;



    clearBufferEscritos();

    clearBufferGSM();



    for(int i = 0; i < 10; i++)

        GSMDriverSendSMS();



    UnityAssertEqualNumber((UNITY_INT)((GSM_IN_PROGRESS)), (UNITY_INT)((GSMDriverSendSMS())), (

   ((void *)0)

   ), (UNITY_UINT)(230), UNITY_DISPLAY_STYLE_INT);



    UnityAssertEqualString((const char*)(("AT+CMGF=1\r")), (const char*)((DatosEscritos)), (

   ((void *)0)

   ), (UNITY_UINT)(232));

}
