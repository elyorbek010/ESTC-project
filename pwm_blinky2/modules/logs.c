#include "logs.h"

void logs_init(void)
{
    ret_code_t ret = NRF_LOG_INIT(NULL);
    APP_ERROR_CHECK(ret);

    NRF_LOG_DEFAULT_BACKENDS_INIT();

    NRF_LOG_INFO("Starting up the project");
}

void send_log(void){
    //NRF_LOG_INFO("log No: %lu", iter_cnt);
    LOG_BACKEND_USB_PROCESS();
    NRF_LOG_PROCESS();
    iter_cnt++;
}