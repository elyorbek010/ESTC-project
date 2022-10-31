/** @file
 *
 * @defgroup blinky_example_main main.c
 * @{
 * @ingroup blinky_example
 * @brief Blinky Example Application main file.
 *
 * This file contains the source code for a sample application to blink LEDs.
 *
 */

#include <stdbool.h>
#include <stdint.h>
#include "nrf_delay.h"
#include "nrf_gpio.h"
#include "boards.h"

#define led1 8
#define led2 NRF_GPIO_PIN_MAP(1, 9)
#define led3 12
#define button NRF_GPIO_PIN_MAP(1, 6)
/**
 * @brief Function for application main entry.
 */
int main(void)
{
    nrf_gpio_cfg_input(button, NRF_GPIO_PIN_PULLUP); // Configure P1.6(SW1) as pull-up input
    nrf_gpio_cfg_output(led1);   // Configure P0.8(LED2_R) as output
    nrf_gpio_cfg_output(led2);   // Configure P1.9(LED2_G) as output
    nrf_gpio_cfg_output(led3);   // Configure P0.12(LED2_B) as output

    nrf_gpio_pin_write(led1, 1);
    nrf_gpio_pin_write(led2, 1);
    nrf_gpio_pin_write(led3, 1);

    int cnt = 0, arr[] = {led1, led1, led2, led2, led2, led3, led3}; // RRGGGBB
    int size = sizeof(arr) / sizeof(arr[0]);

    while(true){
        if(nrf_gpio_pin_read(button) == 0){
            nrf_gpio_pin_write(arr[cnt], 0);
            nrf_delay_ms(500);
            nrf_gpio_pin_write(arr[cnt], 1);
            cnt++;
            nrf_delay_ms(500);
        }
        if(cnt == size) {
            cnt = 0;
        }
    }
}