#include "nvmc.h"

#define BOOTLOADER_START_ADDR 0xE0000
#define PAGE_SIZE 4096
#define WORD_SIZE 4
#define NRF_DFU_APP_DATA_AREA_SIZE (3 * PAGE_SIZE)
#define VALIDITY_KEY 0xAAAA0000

static uint32_t stack_top;
static const uint32_t my_flash_page_1_addr = BOOTLOADER_START_ADDR - NRF_DFU_APP_DATA_AREA_SIZE;
static const uint32_t my_flash_page_2_addr = BOOTLOADER_START_ADDR - NRF_DFU_APP_DATA_AREA_SIZE + PAGE_SIZE;
static uint32_t *my_stack;

bool nvmc_init(void)
{
    my_stack = (uint32_t *)my_flash_page_1_addr;
    if (*my_stack == VALIDITY_KEY)
    {
        stack_top = *(my_stack + 1);
        return stack_top > 2;
    }

    my_stack = (uint32_t *)my_flash_page_2_addr;
    if (*my_stack == VALIDITY_KEY)
    {
        stack_top = *(my_stack + 1);
        return stack_top > 2;
    }

    my_stack = (uint32_t *)my_flash_page_1_addr;
    stack_top = 2;
    nrf_nvmc_page_erase((uint32_t)my_stack);
    nrf_nvmc_write_word((uint32_t)my_stack, VALIDITY_KEY);
    nrf_nvmc_write_word((uint32_t)(my_stack + 1), stack_top);
    return stack_top > 2;
}

void reallocate_stack(void)
{
    if (my_stack == (uint32_t *)my_flash_page_1_addr)
    {
        nrf_nvmc_page_erase(my_flash_page_2_addr);

        nrf_nvmc_write_words(my_flash_page_2_addr + 8, my_stack + 2, stack_top - 2);
        my_stack = (uint32_t *)my_flash_page_2_addr;
        nrf_nvmc_write_word((uint32_t)my_stack, VALIDITY_KEY);

        nrf_nvmc_page_erase(my_flash_page_1_addr);
    }
    else if (my_stack == (uint32_t *)my_flash_page_2_addr)
    {
        nrf_nvmc_page_erase(my_flash_page_1_addr);

        nrf_nvmc_write_words(my_flash_page_1_addr + 8, my_stack + 2, stack_top - 2);
        my_stack = (uint32_t *)my_flash_page_1_addr;
        nrf_nvmc_write_word((uint32_t)my_stack, VALIDITY_KEY);

        nrf_nvmc_page_erase(my_flash_page_2_addr);
    }
}

void push_to_flash(uint32_t data)
{
    reallocate_stack();

    NRF_LOG_INFO("Value pushed is : %u to address: %p", data, my_stack + stack_top);
    nrf_nvmc_write_word((uint32_t)(my_stack + stack_top), data);
    stack_top++;
    nrf_nvmc_write_word((uint32_t)(my_stack + 1), stack_top);
}

uint32_t pop_from_flash(void)
{
    stack_top--;
    uint32_t data = *(my_stack + stack_top);
    NRF_LOG_INFO("Value popped is : %u from address: %p", data, my_stack + stack_top);
    reallocate_stack();
    nrf_nvmc_write_word((uint32_t)(my_stack + 1), stack_top);
    return data;
}
