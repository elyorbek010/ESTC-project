#ifndef LOGS_H
#define LOGS_H

#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"
#include "nrf_log_backend_usb.h"
#include "app_usbd.h"
#include "app_usbd_serial_num.h"

/**
 * @brief Initialize logging over USB
 *
 * @param void no parameters needed
 */
void logs_init(void);

/**
 * @brief Send logs through USB
 *
 * @param void no parameters needed
 */
void send_log(void);

#endif //LOGS_H