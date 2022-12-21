/*

page 3
/////////////////
/      .        /
/cur_list_addr  /   page_3_addr + 3 * WORD_SIZE  |  current address of colors list
/validity key   /   page_3_addr + 2 * WORD_SIZE  |  validity key of colors list
/cur addr       /   page_3_addr + 1 * WORD_SIZE  |  current address of rgb values
/validity key   /   page_3_addr + 0 * WORD_SIZE
/////////////////

page 2
/////////////////
/      .        /
/      .        /
/      .        /   page_2_addr + 0 * WORD_SIZE
/////////////////

page 1
/////////////////
/      .        /
/      .        /
/     blue      /   cur_addr + 2 * WORD_SIZE
/    green      /   cur_addr + 1 * WORD_SIZE
/     red       /   cur_addr + 0 * WORD_SIZE
/      .        /
/      .        /   page_2_addr
/////////////////



*/
#include "nvmc.h"

#define BOOTLOADER_START_ADDR 0xE0000
#define PAGE_SIZE 4096
#define WORD_SIZE 4
#define NRF_DFU_APP_DATA_AREA_SIZE (3 * PAGE_SIZE)
#define VALIDITY_KEY 0x01010101

static const uint32_t page_1_addr = BOOTLOADER_START_ADDR - NRF_DFU_APP_DATA_AREA_SIZE + 0 * PAGE_SIZE;
static const uint32_t page_2_addr = BOOTLOADER_START_ADDR - NRF_DFU_APP_DATA_AREA_SIZE + 1 * PAGE_SIZE;
static const uint32_t page_3_addr = BOOTLOADER_START_ADDR - NRF_DFU_APP_DATA_AREA_SIZE + 2 * PAGE_SIZE;

void save_cur_addr(uint32_t cur_addr)
{
    uint32_t validity_key = *(uint32_t *)(page_3_addr + 2 * WORD_SIZE);
    nrf_nvmc_page_erase(page_3_addr);
    nrf_nvmc_write_word(page_3_addr, VALIDITY_KEY);
    nrf_nvmc_write_word(page_3_addr + WORD_SIZE, cur_addr);
    if (validity_key == VALIDITY_KEY)
    {
        nrf_nvmc_write_word(page_3_addr + 2 * WORD_SIZE, VALIDITY_KEY);
        nrf_nvmc_write_word(page_3_addr + 3 * WORD_SIZE, cur_list_addr);
    }
}

void save_list_cur_addr(void)
{
    uint32_t rgb_cur_addr = *(uint32_t *)(page_3_addr + WORD_SIZE);
    nrf_nvmc_page_erase(page_3_addr);
    nrf_nvmc_write_word(page_3_addr, VALIDITY_KEY);
    nrf_nvmc_write_word(page_3_addr + WORD_SIZE, rgb_cur_addr);
    nrf_nvmc_write_word(page_3_addr + 2 * WORD_SIZE, VALIDITY_KEY);
    nrf_nvmc_write_word(page_3_addr + 3 * WORD_SIZE, cur_list_addr);
}

void nvmc_init(Color *color)
{
    if (*(uint32_t *)(page_3_addr) == VALIDITY_KEY) // check if flash is initialized
    {
        color->cur_addr = *((uint32_t *)(page_3_addr + WORD_SIZE)); // get current address of rgb values from memory
        read_color(color);
    }
    else // initialize flash
    {
        nrf_nvmc_page_erase(page_1_addr);
        color->cur_addr = page_1_addr - 3 * WORD_SIZE; // rgb takes 3 word fields
        save_cur_addr(color->cur_addr);
    }

    if (*(uint32_t *)(page_3_addr + 2 * WORD_SIZE) == VALIDITY_KEY)
    {
        cur_list_addr = *((uint32_t *)(page_3_addr + 3 * WORD_SIZE)); // get current address of colors list from memory
        memcpy(colors_list, (uint32_t *)cur_list_addr, sizeof(colors_list));
    }
    else
    {
        n_elem = 0;
        cur_list_addr = page_2_addr - sizeof(colors_list);
        save_list_cur_addr();
        update_list();
    }
}

void read_color(Color *color)
{
    color->modified = true;
    color->red = *(uint32_t *)(color->cur_addr + 0 * WORD_SIZE);
    color->green = *(uint32_t *)(color->cur_addr + 1 * WORD_SIZE);
    color->blue = *(uint32_t *)(color->cur_addr + 2 * WORD_SIZE);
    rgb2hsv(color);
}

void write_color(Color *color)
{
    color->cur_addr += 3 * WORD_SIZE;

    if (color->cur_addr + 3 * WORD_SIZE > page_2_addr)
    {
        color->cur_addr = page_1_addr;
        nrf_nvmc_page_erase(color->cur_addr);
        nrf_nvmc_write_words(color->cur_addr, (void *)color, 3);
    }
    else
    {
        nrf_nvmc_write_words(color->cur_addr, (void *)color, 3);
    }
    save_cur_addr(color->cur_addr);
    color->modified = false;
    rgb2hsv(color);
    NRF_LOG_INFO("hue: %u, saturation: %u, value: %u\nred: %u, green: %u, blue: %u\n",
                 (uint32_t)color->hue, (uint32_t)(color->saturation * 100), (uint32_t)(color->value * 100),
                 color->red, color->green, color->blue);
}

void update_list()
{
    cur_list_addr += sizeof(colors_list);
    if (cur_list_addr + sizeof(colors_list) > page_3_addr) // if overflow, erase page
    {
        cur_list_addr = page_2_addr;
        nrf_nvmc_page_erase(cur_list_addr);
        nrf_nvmc_write_words(cur_list_addr, (void *)colors_list, sizeof(colors_list));
    }
    else
    {
        nrf_nvmc_write_words(cur_list_addr, (void *)colors_list, sizeof(colors_list));
        NRF_LOG_INFO("UPDATE LIST");
    }
    char *buffer = (char*)cur_list_addr;
    NRF_LOG_INFO("size of buffer = %d", sizeof(buffer));
    NRF_LOG_PUSH(buffer);
    save_list_cur_addr();
}

void save_rgb_color(uint32_t red, uint32_t green, uint32_t blue, char color_name[8])
{
    if (n_elem == 10)
    {
        return;
    }
    else
    {
        strcpy(colors_list[n_elem].name, color_name);
        colors_list[n_elem].name[7] = '\0';
        colors_list[n_elem].red = red;
        colors_list[n_elem].green = green;
        colors_list[n_elem].blue = blue;
        n_elem++;
    }
    update_list();
}