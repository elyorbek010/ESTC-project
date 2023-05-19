#include "my_cli.h"
#include "hsv_rgb.h"
#include "fstorage.h"

#include "app_usbd_cdc_acm.h"
#include "nrf_log.h"
#include "stdbool.h"
#include "stdint.h"
#include "stdlib.h"

#define MAX_ARGS 10
#define MAX_TOKEN_LENGTH 20

#define N_COMMANDS 8

typedef enum
{
    HELP,
    RGB,
    HSV,
    ADD_RGB_COLOR,
    ADD_CURRENT_COLOR,
    DEL_COLOR,
    APPLY_COLOR,
    LIST_COLORS
} my_command_t;

static const char commands[N_COMMANDS][MAX_TOKEN_LENGTH] =
    {"help", "RGB", "HSV", "add_rgb_color", "add_current_color", "del_color", "apply_color", "list_colors"};

#define READ_SIZE 1

#define DELIMITER ' ' // if appeared in input, divides tokens, means you have to start next string
#define DELETE 127    // ASCII code for delete (in Windows backspace is delete in ASCII)

static char m_rx_buffer[READ_SIZE];
static char m_cli_buffer[MAX_ARGS][MAX_TOKEN_LENGTH]; // 10 args including command name, max of 20 chars length

static void usb_ev_handler(app_usbd_class_inst_t const *p_inst,
                           app_usbd_cdc_acm_user_event_t event);

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

static void parser(char input);

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
        break;
    }
    case APP_USBD_CDC_ACM_USER_EVT_RX_DONE:
    {
        ret_code_t ret;
        do // echo
        {
            size_t size = app_usbd_cdc_acm_rx_size(&usb_cdc_acm);
            UNUSED_VARIABLE(size);

            parser(m_rx_buffer[0]);

            if (m_rx_buffer[0] == '\r' || m_rx_buffer[0] == '\n')
            {
                ret = app_usbd_cdc_acm_write(&usb_cdc_acm, "\r\n", 2);
            }
            else
            {
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

int token_idx = 0, char_idx = 0;
static char prev_char = DELIMITER;
static bool received_command = false;

// Create an array just like an argv
static void parser(char input)
{
    if (input == '\r' || input == '\n') // newline characters
    {
        received_command = true;
        m_cli_buffer[token_idx][char_idx] = '\0'; // put nullchar at the end of the line
        if (prev_char != DELIMITER)               // prev_char == DELIMITER only when current row(line) is empty
        {
            token_idx++; // convert idx to number of tokens, just like argc
        }
        return;
    }

    if (input == DELIMITER)
    {
        if (prev_char != DELIMITER) // if encountered DELIMITER start new line(end of token), but if already on new_line then skip(continue) char
        {
            m_cli_buffer[token_idx++][char_idx] = '\0'; // put nullchar at the end of the line
            char_idx = 0;                               // start new line
            prev_char = DELIMITER;
        }
        return;
    }

    if (input != DELETE) // if input is not a special character like NEWLINE or DELIMITER or DELETE
    {
        m_cli_buffer[token_idx][char_idx++] = input;
        prev_char = input;
        return;
    }

    // if input is DELETE
    if (char_idx > 0)
    {
        char_idx--;
        if (char_idx == 0)         // if on empty line
            prev_char = DELIMITER; // then prev_char must be DELIMITER
        else
            prev_char = DELETE; // if not on empty line, no need to know prev_char
        return;
    }

    // if input is DELETE and cursor on empty line
    if (token_idx > 0) // if there are more lines behind, jump back, ignore if no not empty lines left, cursor at the start
    {
        token_idx--;
        char_idx = strlen(m_cli_buffer[token_idx]); // put cursor at the end of the line you came back
        prev_char = DELETE;
    }
    return;
}

static void unknown_command(void)
{
    app_usbd_cdc_acm_write(&usb_cdc_acm, "Unknown command!\r\nPrint 'help' to get information about available commands\r\n", 79);
}

static void help_command(void)
{
    app_usbd_cdc_acm_write(&usb_cdc_acm, "RGB <red> <green> <blue> - the device sets current color to specified one.\r\n"
                                         "HSV <hur> <saturation> <value> - the same with RGB, but color is specified in HSV.\r\n"
                                         "add_rgb_color <r> <g> <b> <color_name>\r\n"
                                         "add_current_color <color_name>\r\n"
                                         "del_color <color_name>\r\n"
                                         "apply_color <color_name>\r\n"
                                         "list_colors\r\n"
                                         "help - print information about supported commands.\r\n",
                           348);
}

my_command_t determine_command(void)
{
    for (uint32_t i = 0; i < N_COMMANDS; i++)
    {
        if (strcmp(m_cli_buffer[0], commands[i]) == 0)
        {
            return i;
        }
    }
    return -1;
}

static void hsv_command(const char argv[MAX_ARGS][MAX_TOKEN_LENGTH], uint32_t argc)
{
    if (argc != 4)
    {
        unknown_command();
        return;
    }

    uint32_t hue = atoi(argv[1]);
    uint32_t saturation = atoi(argv[2]);
    uint32_t value = atoi(argv[3]);

    if (hue > 360 || saturation > 100 || value > 100)
    {
        unknown_command();
        return;
    }

    set_hsv(hue, saturation, value);
}

static void rgb_command(const char argv[MAX_ARGS][MAX_TOKEN_LENGTH], uint32_t argc)
{
    if (argc != 4)
    {
        unknown_command();
        return;
    }

    uint32_t red = atoi(argv[1]);
    uint32_t green = atoi(argv[2]);
    uint32_t blue = atoi(argv[3]);

    if (red > 255 || green > 255 || blue > 255)
    {
        unknown_command();
        return;
    }

    set_rgb(red, green, blue);
}

uint32_t determine_color(char str[COLOR_NAME_LENGTH])
{
    for (uint32_t i = 0; i < n_list_elem; i++)
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
    if (n_list_elem == 0)
    {
        app_usbd_cdc_acm_write(&usb_cdc_acm, "no colors in list\r\n", 20);
        return;
    }
    for (uint32_t i = 0; i < n_list_elem; i++)
    {
        // list all colors in format "name  red  green  blue"
        char buffer[34] = {0};

        int a = snprintf(buffer, sizeof(buffer), "%s %lu %lu %lu\r\n", colors_list[i].name, 
        colors_list[i].red, colors_list[i].green, colors_list[i].blue);
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

void add_current_color_command(char argv[10][20], uint32_t argc)
{
    if (argc != 2)
    {
        unknown_command();
        return;
    }

    uint32_t red = 0, green = 0, blue = 0;
    get_rgb(&red, &green, &blue);

    save_rgb_color(red, green, blue, argv[1]);
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

    if (i == -1)
    {
        unknown_command();
        return;
    }

    colors_list[i] = colors_list[n_list_elem - 1];
    n_list_elem--;
    update_list();
    list_colors_command();
}

void apply_color_command(char argv[10][20], uint32_t argc)
{
    if (argc != 2)
    {
        unknown_command();
        return;
    }

    uint32_t i = determine_color(argv[1]);

    if (i == -1)
    {
        unknown_command();
        return;
    }

    set_rgb(colors_list[i].red, colors_list[i].green, colors_list[i].blue);
}

void cli_process(void)
{
    uint32_t token_n = token_idx; // number of tokens

    if (received_command)
    {
        NRF_LOG_INFO("number of tokens = %u", token_n);

        for (uint32_t i = 0; i < token_n; i++)
        {
            NRF_LOG_INFO("length of buffer[%u] = %d", i, strlen(m_cli_buffer[i]));
            NRF_LOG_INFO("buffer[%u] = %s", i, m_cli_buffer[i]);
        }

        switch (determine_command())
        {
        case HELP:
            NRF_LOG_INFO("help");
            help_command();
            break;
        case RGB:
            NRF_LOG_INFO("rgb");
            rgb_command(m_cli_buffer, token_n);
            break;
        case HSV:
            NRF_LOG_INFO("hsv");
            hsv_command(m_cli_buffer, token_n);
            break;
        case ADD_RGB_COLOR:
            NRF_LOG_INFO("add rgb color");
            add_rgb_color_command(m_cli_buffer, token_n);
            break;
        case ADD_CURRENT_COLOR:
            NRF_LOG_INFO("add current color");
            add_current_color_command(m_cli_buffer, token_n);
            break;
        case DEL_COLOR:
            NRF_LOG_INFO("del color");
            del_color_command(m_cli_buffer, token_n);
            break;
        case APPLY_COLOR:
            NRF_LOG_INFO("apply color");
            apply_color_command(m_cli_buffer, token_n);
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

        token_idx = 0;
        char_idx = 0;
        prev_char = DELIMITER;
        received_command = false;
    }
}

void cli_init(void)
{
    app_usbd_class_inst_t const *class_cdc_acm = app_usbd_cdc_acm_class_inst_get(&usb_cdc_acm);
    ret_code_t ret = app_usbd_class_append(class_cdc_acm);
    APP_ERROR_CHECK(ret);

    token_idx = 0;
    char_idx = 0;
    prev_char = DELIMITER;
    received_command = false;
}