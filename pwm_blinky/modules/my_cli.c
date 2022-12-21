#include "my_cli.h"

#define READ_SIZE 1
//
static uint32_t count_rows;
static uint32_t count_cols;
static char prev_char = ' ';
static bool received_command;
//
static char m_rx_buffer[READ_SIZE];
static char m_cli_buffer[10][20];
//

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
        ret = app_usbd_cdc_acm_read(&usb_cdc_acm, m_rx_buffer, READ_SIZE);
        UNUSED_VARIABLE(ret);
        break;
    }
    case APP_USBD_CDC_ACM_USER_EVT_PORT_CLOSE:
    {
        break;
    }
    case APP_USBD_CDC_ACM_USER_EVT_TX_DONE:
    {
        // NRF_LOG_INFO("tx done");
        break;
    }
    case APP_USBD_CDC_ACM_USER_EVT_RX_DONE:
    {
        ret_code_t ret;
        do
        {
            /*Get amount of data transfered*/
            size_t size = app_usbd_cdc_acm_rx_size(&usb_cdc_acm);
            UNUSED_VARIABLE(size);
            // NRF_LOG_INFO("rx size: %d", size);

            /* It's the simple version of an echo. Note that writing doesn't
             * block execution, and if we have a lot of characters to read and
             * write, some characters can be missed.
             */
            if (m_rx_buffer[0] == '\r' || m_rx_buffer[0] == '\n')
            {
                received_command = true;
                m_cli_buffer[count_rows][count_cols] = '\0';
                if (prev_char != ' ')
                {
                    count_rows++;
                }
                ret = app_usbd_cdc_acm_write(&usb_cdc_acm, "\r\n", 2);
            }
            else
            {
                if (m_rx_buffer[0] == ' ')
                {
                    if (prev_char != ' ')
                    {
                        m_cli_buffer[count_rows++][count_cols] = '\0';
                        count_cols = 0;
                    }
                    prev_char = ' ';
                }
                else
                {
                    m_cli_buffer[count_rows][count_cols++] = m_rx_buffer[0];
                    prev_char = m_rx_buffer[0];
                }

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

void unknown_command(void)
{
    app_usbd_cdc_acm_write(&usb_cdc_acm, "Unknown command!\r\n", 19);
}

void help_command(void)
{
    app_usbd_cdc_acm_write(&usb_cdc_acm, "RGB <red> <green> <blue> - the device sets current color to specified one.\r\n", 77);
    app_usbd_cdc_acm_write(&usb_cdc_acm, "HSV <hur> <saturation> <value> - the same with RGB, but color is specified in HSV.\r\n", 85);
    app_usbd_cdc_acm_write(&usb_cdc_acm, "add_rgb_color <r> <g> <b> <color_name>\r\n", 41);
    app_usbd_cdc_acm_write(&usb_cdc_acm, "add_current_color <color_name>\r\n", 33);
    app_usbd_cdc_acm_write(&usb_cdc_acm, "del_color <color_name>\r\n", 25);
    app_usbd_cdc_acm_write(&usb_cdc_acm, "apply_color <color_name>\r\n", 27);
    app_usbd_cdc_acm_write(&usb_cdc_acm, "list_colors\r\n", 14);
    app_usbd_cdc_acm_write(&usb_cdc_acm, "help - print information about supported commands.\r\n", 53);
}

my_command_t determine_command(void)
{
    for (uint32_t i = 0; i < M_COMMANDS; i++)
    {
        if (strcmp(m_cli_buffer[0], commands[i]) == 0)
        {
            return i;
        }
    }
    return -1;
}

void hsv_command(const char argv[10][20], uint32_t argc, Color *color)
{
    if (argc != 4)
    {
        unknown_command();
        return;
    }

    uint32_t hue_ = atoi(argv[1]);
    uint32_t saturation_ = atoi(argv[2]);
    uint32_t value_ = atoi(argv[3]);

    if (hue_ > 360 || saturation_ > 100 || value_ > 100)
    {
        unknown_command();
        return;
    }

    color->modified = true;
    color->hue = hue_;
    color->saturation = saturation_ / 100.f;
    color->value = value_ / 100.f;
    hsv2rgb(color);

    NRF_LOG_INFO("hue = %u, saturation = %u, value = %u", hue_, saturation_, value_);
}

void rgb_command(const char argv[10][20], uint32_t argc, Color *color)
{
    if (argc != 4)
    {
        unknown_command();
        return;
    }

    uint32_t red_ = atoi(argv[1]);
    uint32_t green_ = atoi(argv[2]);
    uint32_t blue_ = atoi(argv[3]);

    if (red_ > 255 || green_ > 255 || blue_ > 255)
    {
        unknown_command();
        return;
    }

    color->modified = true;
    color->red = red_;

    color->green = green_;
    color->blue = blue_;
    rgb2hsv(color);

    NRF_LOG_INFO("red = %u, green = %u, blue = %u", red_, green_, blue_);
}

uint32_t determine_color(char str[8])
{
    for (uint32_t i = 0; i < n_elem; i++)
    {
        if (strcmp(str, colors_list[i].name) == 0)
        {
            NRF_LOG_INFO("%s", colors_list[i].name);
            return i;
        }
    }
    return -1;
}

void list_colors_command()
{
    if (n_elem < 1)
    {
        app_usbd_cdc_acm_write(&usb_cdc_acm, "no colors in list\r\n", 20);
        return;
    }
    for (uint32_t i = 0; i < n_elem; i++)
    {
        app_usbd_cdc_acm_write(&usb_cdc_acm, colors_list[i].name, strlen(colors_list[i].name));
        app_usbd_cdc_acm_write(&usb_cdc_acm, " ", 1);
        char buffer[4] = {'\0'};
        int a = snprintf(buffer, 4, "%lu", colors_list[i].red);
        app_usbd_cdc_acm_write(&usb_cdc_acm, buffer, a);
        app_usbd_cdc_acm_write(&usb_cdc_acm, " ", 1);
        a = snprintf(buffer, 4, "%lu", colors_list[i].green);
        app_usbd_cdc_acm_write(&usb_cdc_acm, buffer, a);
        app_usbd_cdc_acm_write(&usb_cdc_acm, " ", 1);
        a = snprintf(buffer, 4, "%lu\r\n", colors_list[i].blue);
        app_usbd_cdc_acm_write(&usb_cdc_acm, buffer, a);
    }
}

void add_rgb_color_command(char argv[10][20], uint32_t argc)
{
    if (argc != 5)
    {
        unknown_command();
        return;
    }

    save_rgb_color((uint32_t)atoi(argv[1]), (uint32_t)atoi(argv[2]), (uint32_t)atoi(argv[3]), argv[4]);
    list_colors_command();
}

void add_current_color_command(char argv[10][20], uint32_t argc, Color *color)
{
    if (argc != 2)
    {
        unknown_command();
        return;
    }
    save_rgb_color(color->red, color->green, color->blue, argv[1]);
    list_colors_command();
}

void del_color_command(char argv[10][20], uint32_t argc)
{
    if (argc != 2)
    {
        unknown_command();
        return;
    }
    uint32_t i = determine_color(argv[1]);
    if (i == (uint32_t)-1)
    {
        unknown_command();
        return;
    }
    colors_list[i] = colors_list[n_elem - 1];
    n_elem--;
    update_list();
    list_colors_command();
}

void apply_color_command(char argv[10][20], uint32_t argc, Color *color)
{
    if (argc != 2)
    {
        unknown_command();
        return;
    }
    uint32_t i = determine_color(argv[1]);
    NRF_LOG_INFO("%s", argv[1]);
    if (i == (uint32_t)-1)
    {
        unknown_command();
        return;
    }
    color->modified = true;
    color->red = colors_list[i].red;
    color->green = colors_list[i].green;
    color->blue = colors_list[i].blue;
    rgb2hsv(color);
}

void cli_process(Color *color)
{
    while (app_usbd_event_queue_process())
    {
    }

    if (received_command)
    {
        NRF_LOG_INFO("count_rows = %u", count_rows);

        for (uint32_t i = 0; i < count_rows; i++)
        {
            NRF_LOG_INFO("length of buffer[%u] = %d", i, strlen(m_cli_buffer[i]));
            NRF_LOG_INFO("buffer[%u] = %s", i, m_cli_buffer[i]);
        }

        switch (determine_command())
        {
        case help:
            NRF_LOG_INFO("help");
            help_command();
            break;
        case RGB:
            NRF_LOG_INFO("rgb");
            rgb_command(m_cli_buffer, count_rows, color);
            break;
        case HSV:
            NRF_LOG_INFO("hsv");
            hsv_command(m_cli_buffer, count_rows, color);
            break;
        case ADD_RGB_COLOR:
            NRF_LOG_INFO("add rgb color");
            add_rgb_color_command(m_cli_buffer, count_rows);
            break;
        case ADD_CURRENT_COLOR:
            NRF_LOG_INFO("add current color");
            add_current_color_command(m_cli_buffer, count_rows, color);
            break;
        case DEL_COLOR:
            NRF_LOG_INFO("del color");
            del_color_command(m_cli_buffer, count_rows);
            break;
        case APPLY_COLOR:
            NRF_LOG_INFO("apply color");
            apply_color_command(m_cli_buffer, count_rows, color);
            break;
        case LIST_COLORS:
            NRF_LOG_INFO("list colors");
            list_colors_command();
            break;
        default:
            NRF_LOG_INFO("unknown command");
            unknown_command();
            break;
        }

        count_rows = 0;
        count_cols = 0;
        prev_char = ' ';
        received_command = false;
    }
}

void cli_init(void)
{
    app_usbd_class_inst_t const *class_cdc_acm = app_usbd_cdc_acm_class_inst_get(&usb_cdc_acm);
    ret_code_t ret = app_usbd_class_append(class_cdc_acm);
    APP_ERROR_CHECK(ret);
}