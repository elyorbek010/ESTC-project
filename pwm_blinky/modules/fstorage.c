/*

page 3
/////////////////
/n_elem         /   page_3_addr + 3 * WORD_SIZE  |  current number of colors list
/cur_list_addr  /   page_3_addr + 2 * WORD_SIZE  |  current address of colors list
//cur addr     //   page_3_addr + 1 * WORD_SIZE  |  current address of rgb values
//validity key //   page_3_addr + 0 * WORD_SIZE
/////////////////

page 2
/////////////////
/      .        /
/      .        /
/colors list    /   page_2_addr + 0 * WORD_SIZE
/////////////////

page 1
/////////////////
/      .        /
/      .        /
//   blue      //   cur_addr + 2 * WORD_SIZE
//   green     //   cur_addr + 1 * WORD_SIZE
//   red       //   cur_addr + 0 * WORD_SIZE
/      .        /
/      .        /   page_1_addr
/////////////////

*/
#include "fstorage.h"
#include "hsv_rgb.h"

#include "nrf_fstorage.h"
#include "nrf_fstorage_sd.h"
#include "nrf_log.h"
#include "stddef.h"

#define BOOTLOADER_START_ADDR 0xE0000
#define PAGE_SIZE 4096
#define WORD_SIZE 4
#define NRF_DFU_APP_DATA_AREA_SIZE (3 * PAGE_SIZE)

#define PAGE_1_ADDR (BOOTLOADER_START_ADDR - NRF_DFU_APP_DATA_AREA_SIZE + 0 * PAGE_SIZE)
#define PAGE_2_ADDR (BOOTLOADER_START_ADDR - NRF_DFU_APP_DATA_AREA_SIZE + 1 * PAGE_SIZE)
#define PAGE_3_ADDR (BOOTLOADER_START_ADDR - NRF_DFU_APP_DATA_AREA_SIZE + 2 * PAGE_SIZE)

#define PAGES_TO_ERASE 1

static const uint32_t validity_key = 0x010101030;

static uint32_t color_cur_addr = 0;

static void retrieve_color(void);

NRF_FSTORAGE_DEF(nrf_fstorage_t my_instance) =
    {
        .evt_handler = NULL,
        .start_addr = PAGE_1_ADDR,
        .end_addr = PAGE_1_ADDR + 4 * PAGE_SIZE};

void save_cur_addr()
{
    nrf_fstorage_erase(
        &my_instance,   /* The instance to use. */
        PAGE_3_ADDR,    /* The address of the flash pages to erase. */
        PAGES_TO_ERASE, /* The number of pages to erase. */
        NULL            /* Optional parameter, backend-dependent. */
    );
    nrf_fstorage_write(
        &my_instance,                /* The instance to use. */
        PAGE_3_ADDR + 0 * WORD_SIZE, /* The address in flash where to store the data. */
        &validity_key,               /* A pointer to the data. */
        sizeof(validity_key),        /* Lenght of the data, in bytes. */
        NULL                         /* Optional parameter, backend-dependent. */
    );
    nrf_fstorage_write(
        &my_instance,                /* The instance to use. */
        PAGE_3_ADDR + 1 * WORD_SIZE, /* The address in flash where to store the data. */
        &(color_cur_addr),           /* A pointer to the data. */
        sizeof(color_cur_addr),      /* Lenght of the data, in bytes. */
        NULL                         /* Optional parameter, backend-dependent. */
    );
    nrf_fstorage_write(
        &my_instance,                /* The instance to use. */
        PAGE_3_ADDR + 2 * WORD_SIZE, /* The address in flash where to store the data. */
        &(list_cur_addr),            /* A pointer to the data. */
        sizeof(list_cur_addr),       /* Lenght of the data, in bytes. */
        NULL                         /* Optional parameter, backend-dependent. */
    );
    nrf_fstorage_write(
        &my_instance,                /* The instance to use. */
        PAGE_3_ADDR + 3 * WORD_SIZE, /* The address in flash where to store the data. */
        &(n_list_elem),              /* A pointer to the data. */
        sizeof(n_list_elem),         /* Lenght of the data, in bytes. */
        NULL                         /* Optional parameter, backend-dependent. */
    );
}

static void retrieve_color(void)
{
    uint32_t red = 0, green = 0, blue = 0;

    nrf_fstorage_read(
        &my_instance,                   /* The instance to use. */
        color_cur_addr + 0 * WORD_SIZE, /* The address in flash where to read data from. */
        &red,                           /* A buffer to copy the data into. */
        sizeof(red)                     /* Lenght of the data, in bytes. */
    );

    nrf_fstorage_read(
        &my_instance,                   /* The instance to use. */
        color_cur_addr + 1 * WORD_SIZE, /* The address in flash where to read data from. */
        &green,                         /* A buffer to copy the data into. */
        sizeof(green)                   /* Lenght of the data, in bytes. */
    );

    nrf_fstorage_read(
        &my_instance,                   /* The instance to use. */
        color_cur_addr + 2 * WORD_SIZE, /* The address in flash where to read data from. */
        &blue,                          /* A buffer to copy the data into. */
        sizeof(blue)                    /* Lenght of the data, in bytes. */
    );

    if (red > 255 || green > 255 || blue > 255)
    {
        NRF_LOG_INFO("ERROR: FSTORAGE INVALID COLOR");
        return;
    }

    set_rgb(red, green, blue);
}

static void retrieve_list(void)
{
    uint32_t object_size = COLOR_NAME_LENGTH + 3 * sizeof(uint32_t);

    for (uint32_t i = 0; i < n_list_elem; i++)
    {
        nrf_fstorage_read(
            &my_instance,                    /* The instance to use. */
            list_cur_addr + i * object_size, /* The address in flash where to store the data. */
            &(colors_list[i].name),          /* A pointer to the data. */
            COLOR_NAME_LENGTH                /* Lenght of the data, in bytes. */
        );
        nrf_fstorage_read(
            &my_instance,                                                               /* The instance to use. */
            list_cur_addr + COLOR_NAME_LENGTH + 0 * sizeof(uint32_t) + i * object_size, /* The address in flash where to store the data. */
            &(colors_list[i].red),                                                      /* A pointer to the data. */
            sizeof(uint32_t)                                                            /* Lenght of the data, in bytes. */
        );
        nrf_fstorage_read(
            &my_instance,                                                               /* The instance to use. */
            list_cur_addr + COLOR_NAME_LENGTH + 1 * sizeof(uint32_t) + i * object_size, /* The address in flash where to store the data. */
            &(colors_list[i].green),                                                    /* A pointer to the data. */
            sizeof(uint32_t)                                                            /* Lenght of the data, in bytes. */
        );
        nrf_fstorage_read(
            &my_instance,                                                               /* The instance to use. */
            list_cur_addr + COLOR_NAME_LENGTH + 2 * sizeof(uint32_t) + i * object_size, /* The address in flash where to store the data. */
            &(colors_list[i].blue),                                                     /* A pointer to the data. */
            sizeof(uint32_t)                                                            /* Lenght of the data, in bytes. */
        );
    }
}

void fstorage_init()
{
    uint32_t validity = 0;

    nrf_fstorage_init(
        &my_instance,     /* You fstorage instance, previously defined. */
        &nrf_fstorage_sd, /* Name of the backend. */
        NULL              /* Optional parameter, backend-dependant. */
    );

    nrf_fstorage_read(
        &my_instance,                /* The instance to use. */
        PAGE_3_ADDR + 0 * WORD_SIZE, /* The address in flash where to read data from. */
        &validity,                   /* A buffer to copy the data into. */
        sizeof(validity)             /* Lenght of the data, in bytes. */
    );

    if (validity == validity_key) // check if flash is initialized
    {
        nrf_fstorage_read(
            &my_instance,                /* The instance to use. */
            PAGE_3_ADDR + 1 * WORD_SIZE, /* The address in flash where to read data from. */
            &(color_cur_addr),           /* A buffer to copy the data into. */
            sizeof(color_cur_addr)       /* Lenght of the data, in bytes. */
        );
        nrf_fstorage_read(
            &my_instance,                /* The instance to use. */
            PAGE_3_ADDR + 2 * WORD_SIZE, /* The address in flash where to read data from. */
            &(list_cur_addr),            /* A buffer to copy the data into. */
            sizeof(list_cur_addr)        /* Lenght of the data, in bytes. */
        );
        nrf_fstorage_read(
            &my_instance,                /* The instance to use. */
            PAGE_3_ADDR + 3 * WORD_SIZE, /* The address in flash where to read data from. */
            &(n_list_elem),              /* A buffer to copy the data into. */
            sizeof(n_list_elem)          /* Lenght of the data, in bytes. */
        );
        retrieve_color();
        retrieve_list();
    }
    else // initialize flash
    {
        nrf_fstorage_erase(
            &my_instance,   /* The instance to use. */
            PAGE_1_ADDR,    /* The address of the flash pages to erase. */
            PAGES_TO_ERASE, /* The number of pages to erase. */
            NULL            /* Optional parameter, backend-dependent. */
        );
        nrf_fstorage_erase(
            &my_instance,   /* The instance to use. */
            PAGE_2_ADDR,    /* The address of the flash pages to erase. */
            PAGES_TO_ERASE, /* The number of pages to erase. */
            NULL            /* Optional parameter, backend-dependent. */
        );
        color_cur_addr = PAGE_1_ADDR - 3 * WORD_SIZE;                                  // rgb takes 3 word fields
        list_cur_addr = PAGE_2_ADDR - 10 * (COLOR_NAME_LENGTH + 3 * sizeof(uint32_t)); // 10 objects [name + red + green + blue]
        n_list_elem = 0;
    }
}

void save_color()
{
    static uint32_t red = 0, green = 0, blue = 0;
    get_rgb(&red, &green, &blue);

    color_cur_addr += 3 * WORD_SIZE; // since cur_addr contains data, skip over to untouched memory space

    if (color_cur_addr + 3 * WORD_SIZE > PAGE_2_ADDR) // if after saving color at current address overflow occurs
    {
        color_cur_addr = PAGE_1_ADDR; // reset current address

        nrf_fstorage_erase(
            &my_instance,   /* The instance to use. */
            PAGE_1_ADDR,    /* The address of the flash pages to erase. */
            PAGES_TO_ERASE, /* The number of pages to erase. */
            NULL            /* Optional parameter, backend-dependent. */
        );
    }

    save_cur_addr(); // since address is changed, save it to PAGE 3

    // Flash write RGB values

    nrf_fstorage_write(
        &my_instance,                   /* The instance to use. */
        color_cur_addr + 0 * WORD_SIZE, /* The address in flash where to store the data. */
        &(red),                         /* A pointer to the data. */
        sizeof(red),                    /* Lenght of the data, in bytes. */
        NULL                            /* Optional parameter, backend-dependent. */
    );

    nrf_fstorage_write(
        &my_instance,                   /* The instance to use. */
        color_cur_addr + 1 * WORD_SIZE, /* The address in flash where to store the data. */
        &(green),                       /* A pointer to the data. */
        sizeof(green),                  /* Lenght of the data, in bytes. */
        NULL                            /* Optional parameter, backend-dependent. */
    );

    nrf_fstorage_write(
        &my_instance,                   /* The instance to use. */
        color_cur_addr + 2 * WORD_SIZE, /* The address in flash where to store the data. */
        &(blue),                        /* A pointer to the data. */
        sizeof(blue),                   /* Lenght of the data, in bytes. */
        NULL                            /* Optional parameter, backend-dependent. */
    );

    set_color_saved();
}

void save_rgb_color(uint32_t red, uint32_t green, uint32_t blue, char color_name[COLOR_NAME_LENGTH])
{
    if (n_list_elem == 10)
    {
        return;
    }
    else
    {
        strcpy(colors_list[n_list_elem].name, color_name);
        colors_list[n_list_elem].name[COLOR_NAME_LENGTH - 1] = '\0';
        colors_list[n_list_elem].red = red;
        colors_list[n_list_elem].green = green;
        colors_list[n_list_elem].blue = blue;
        n_list_elem++;
    }
    update_list();
}

void update_list(void)
{
    uint32_t object_size = COLOR_NAME_LENGTH + 3 * sizeof(uint32_t);

    list_cur_addr += 10 * object_size; // 20 + 3 * 4 = 32 bytes

    if (list_cur_addr + 10 * object_size > PAGE_3_ADDR) // if overflow, reset page
    {
        list_cur_addr = PAGE_2_ADDR;
        nrf_fstorage_erase(
            &my_instance,   /* The instance to use. */
            PAGE_2_ADDR,    /* The address of the flash pages to erase. */
            PAGES_TO_ERASE, /* The number of pages to erase. */
            NULL            /* Optional parameter, backend-dependent. */
        );
    }

    save_cur_addr();

    for (uint32_t i = 0; i < n_list_elem; i++)
    {
        nrf_fstorage_write(
            &my_instance,                    /* The instance to use. */
            list_cur_addr + i * object_size, /* The address in flash where to store the data. */
            &(colors_list[i].name),          /* A pointer to the data. */
            COLOR_NAME_LENGTH,               /* Lenght of the data, in bytes. */
            NULL                             /* Optional parameter, backend-dependent. */
        );
        nrf_fstorage_write(
            &my_instance,                                                               /* The instance to use. */
            list_cur_addr + COLOR_NAME_LENGTH + 0 * sizeof(uint32_t) + i * object_size, /* The address in flash where to store the data. */
            &(colors_list[i].red),                                                      /* A pointer to the data. */
            sizeof(uint32_t),                                                           /* Lenght of the data, in bytes. */
            NULL                                                                        /* Optional parameter, backend-dependent. */
        );
        nrf_fstorage_write(
            &my_instance,                                                               /* The instance to use. */
            list_cur_addr + COLOR_NAME_LENGTH + 1 * sizeof(uint32_t) + i * object_size, /* The address in flash where to store the data. */
            &(colors_list[i].green),                                                    /* A pointer to the data. */
            sizeof(uint32_t),                                                           /* Lenght of the data, in bytes. */
            NULL                                                                        /* Optional parameter, backend-dependent. */
        );
        nrf_fstorage_write(
            &my_instance,                                                               /* The instance to use. */
            list_cur_addr + COLOR_NAME_LENGTH + 2 * sizeof(uint32_t) + i * object_size, /* The address in flash where to store the data. */
            &(colors_list[i].blue),                                                     /* A pointer to the data. */
            sizeof(uint32_t),                                                           /* Lenght of the data, in bytes. */
            NULL                                                                        /* Optional parameter, backend-dependent. */
        );
    }

    NRF_LOG_INFO("UPDATE LIST");
    // char *buffer = (char*)list_cur_addr;
    // NRF_LOG_INFO("size of buffer = %d", sizeof(buffer));
    // NRF_LOG_INFO("%s", NRF_LOG_PUSH(buffer));
}
