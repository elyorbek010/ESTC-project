#include <stdbool.h>
#include <stdint.h>

#include "logs.h"

#include "nordic_common.h"
#include "boards.h"

#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"

#include "nrf_log_backend_usb.h"

#include "app_usbd.h"
#include "app_usbd_serial_num.h"
#include "app_usbd_cdc_acm.h"

#define READ_SIZE 1
#define CLI_READ_SIZE 4096
static uint32_t count;
static bool received_command;

static char m_rx_buffer[READ_SIZE];
static char m_cli_buffer[CLI_READ_SIZE];
static char command[20][60];

static void usb_ev_handler(app_usbd_class_inst_t const *p_inst,
                           app_usbd_cdc_acm_user_event_t event);

/* Make sure that they don't intersect with LOG_BACKEND_USB_CDC_ACM */
#define CDC_ACM_COMM_INTERFACE 2
#define CDC_ACM_COMM_EPIN NRF_DRV_USBD_EPIN3

#define CDC_ACM_DATA_INTERFACE 3
#define CDC_ACM_DATA_EPIN NRF_DRV_USBD_EPIN4
#define CDC_ACM_DATA_EPOUT NRF_DRV_USBD_EPOUT4

APP_USBD_CDC_ACM_GLOBAL_DEF(usb_cdc_acm,
                            usb_ev_handler,
                            CDC_ACM_COMM_INTERFACE,
                            CDC_ACM_DATA_INTERFACE,
                            CDC_ACM_COMM_EPIN,
                            CDC_ACM_DATA_EPIN,
                            CDC_ACM_DATA_EPOUT,
                            APP_USBD_CDC_COMM_PROTOCOL_NONE);

static void usb_ev_handler(app_usbd_class_inst_t const *p_inst,
                           app_usbd_cdc_acm_user_event_t event)
{
    switch (event)
    {
    case APP_USBD_CDC_ACM_USER_EVT_PORT_OPEN:
    {
        ret_code_t ret;
        bsp_board_led_on(0);
        ret = app_usbd_cdc_acm_read(&usb_cdc_acm, m_rx_buffer, READ_SIZE);
        UNUSED_VARIABLE(ret);
        break;
    }
    case APP_USBD_CDC_ACM_USER_EVT_PORT_CLOSE:
    {
        bsp_board_led_off(0);
        break;
    }
    case APP_USBD_CDC_ACM_USER_EVT_TX_DONE:
    {
        NRF_LOG_INFO("tx done");
        bsp_board_led_invert(1);
        break;
    }
    case APP_USBD_CDC_ACM_USER_EVT_RX_DONE:
    {
        bsp_board_led_invert(2);
        ret_code_t ret;
        do
        {
            /*Get amount of data transfered*/
            size_t size = app_usbd_cdc_acm_rx_size(&usb_cdc_acm);
            NRF_LOG_INFO("rx size: %d", size);

            /* It's the simple version of an echo. Note that writing doesn't
             * block execution, and if we have a lot of characters to read and
             * write, some characters can be missed.
             */
            if (m_rx_buffer[0] == '\r' || m_rx_buffer[0] == '\n')
            {
                received_command = true;
                NRF_LOG_INFO("cli buffer is: %s", m_cli_buffer);
                ret = app_usbd_cdc_acm_write(&usb_cdc_acm, "\r\n", 2);
            }
            else
            {
                m_cli_buffer[count] = m_rx_buffer[0];
                count++;
                ret = app_usbd_cdc_acm_write(&usb_cdc_acm,
                                             m_rx_buffer,
                                             READ_SIZE);
            }

            /* Fetch data until internal buffer is empty */
            ret = app_usbd_cdc_acm_read(&usb_cdc_acm,
                                        m_rx_buffer,
                                        READ_SIZE);
        } while (ret == NRF_SUCCESS);
        break;
    }
    default:
        break;
    }
}

void usb_main(void)
{
    NRF_LOG_INFO("Starting up the test project with USB logging");

    bsp_board_init(BSP_INIT_LEDS);

    app_usbd_class_inst_t const *class_cdc_acm = app_usbd_cdc_acm_class_inst_get(&usb_cdc_acm);
    ret_code_t ret = app_usbd_class_append(class_cdc_acm);
    APP_ERROR_CHECK(ret);

    while (true)
    {
        while (app_usbd_event_queue_process())
        {
        }
        if (received_command)
        {
            determine_command();
        }
        send_log();
    }
}

void determine_command(void)
{ 
    idx = 0;
    end = count;
    for (int i = 0; i < 20; i++)
    {
        idx = next_char(idx, end);
        if (idx == end)
        {
            break;
        }

        for (int j = 0; j < 60; j++, idx++)
        {
            if (m_cli_buffer[idx] != ' ')
            {
                command[i][j] = m_cli_buffer[idx];
            }
            else
            {
                while (j < 60)
                {
                    command[i][j] = 0;
                }
                break;
            }
        }
    }
}

int next_char(int idx, int stop)
{
    for (idx; idx < stop; idx++)
    {
        if (m_cli_buffer[idx] != ' ')
        {
            break;
        }
    }
    return idx;
}

bool compare_strings(char string1[], char string2[])
{
    for (int i = 0; i < NRFX_ARRAY_SIZE(string1), i++)
    {
        if (string1[i] != string2[i])
        {
            return 0;
        }
    }
    return 1;
}

uint32_t string_to_int(char string1[])
{
    j = 1;
    uint32_t res = 0;
    for (int i = NRFX_ARRAY_SIZE(string1); i >= 0; i--)
    {
        res += uint32_t(string1[i] + 48) * j;
        j *= 10;
    }
}